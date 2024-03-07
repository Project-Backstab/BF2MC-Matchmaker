#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include <settings.h>
#include <logger.h>
#include <server.h>
#include <globals.h>
#include <util.h>
#include <database.h>

#include <gpsp/client.h>

typedef void (GPSP::Client::*RequestActionFunc)(const GameSpy::Parameter&) const;

static std::map<std::string, RequestActionFunc> mRequestActions = 
{
	{ "nicks",   &GPSP::Client::requestNicks   }, // Done
	{ "valid",   &GPSP::Client::requestValid   }, // Done
	{ "newuser", &GPSP::Client::requestNewUser }, // Done
	{ "search",  &GPSP::Client::requestSearch  }, // Done
};

GPSP::Client::Client(int socket, struct sockaddr_in address)
{
	this->_socket = socket;
	this->_address = address;
	this->UpdateLastRecievedTime();
}

GPSP::Client::~Client()
{
	this->Disconnect();
}

void GPSP::Client::Listen()
{
	while(true)
	{
		std::string request;
		std::vector<char> buffer(4096, 0);
		
		int recv_size = read(this->_socket, &(buffer[0]), 4096);
		
		// If error or no data is recieved we end the connection
		if(recv_size <= 0)
		{
			break;
		}
		
		// Resize buffer
		buffer.resize(recv_size);
		
		this->UpdateLastRecievedTime();

		request = Util::Buffer::ToString(buffer);
		
		this->_LogTransaction("-->", request);
		
		this->onRequest(request);
	}
	
	this->Disconnect();
}

void GPSP::Client::Disconnect()
{
	this->Close();
	g_gpsp_server->onClientDisconnect(*this);
}

/*
	Events
*/
void GPSP::Client::onRequest(const std::string& request)
{
	GameSpy::Parameter parameter = GameSpy::Request2Parameter(request);
	
	// Find function name
	std::string action = parameter[0];
	
	auto it = mRequestActions.find(action);
	if (it != mRequestActions.end())
	{
		// Get Function address
		RequestActionFunc func = it->second;
	
		// Execute action function with class object.
		(this->*(func))(parameter);
	}
	else
	{
		Logger::warning("action \"" + action + "\" not implemented!", Server::Type::GPSP);
		
		this->Disconnect();
	}
}

/*
	Request:  \nicks\\email\help0001@gmail.com\pass\12345\namespaceid\13\gamename\bfield1942ps2\final\
	Response: \nr\0\nick\IamLupo@6507BAD7\uniquenick\IamLupo\ndone\\final\
	
	Request:
		email: (string)
		pass: (string)
		namespaceid: (?integer?)
		gamename: (string)
	
	Return:
		nr: (integer)
		nick: (string)
		uniquenick: (string)
*/
void GPSP::Client::requestNicks(const GameSpy::Parameter& parameter) const
{
	if(parameter.size() != 11)
	{
		return;
	}
	
	std::string email = parameter[3];
	std::string password = parameter[5];
	
	// Filter email
	if(email.size() > 50)
	{
		email = email.substr(0, 49);
	}
	
	Battlefield::Players players;
	
	// Get players from database by email
	g_database->queryPlayersByEmail(players, email);
	
	// Check password
	bool correct_password = false;
	std::string md5password = Util::MD5hash(password);
	for(Battlefield::Player player : players)
	{
		if(player.GetPassword() == md5password)
		{
			correct_password = true;
		}
	}
	
	// If no database user exist or password isn't correct
	if(players.size() == 0 || !correct_password)
	{
		// Convert parameter to string response
		std::string response = GameSpy::Parameter2Response({"nr", "260", "ndone", "", "final"});
	
		// Send
		this->Send(response);
		
		// Log
		this->_LogTransaction("<--", response);
		
		return;
	}
	
	GameSpy::Parameter response_parameter = { "nr", "0" };
	
	for(Battlefield::Player player : players)
	{
		std::string uniquenick = player.GetUniquenick();

		// Update unique nick if is in clan
		Battlefield::Clan clan;
		g_database->queryClanByProfileId(clan, player);
		if(clan.GetClanId() != -1)
		{
			uniquenick = clan.GetTag() + " " + uniquenick;
		}

		response_parameter.push_back("nick");
		response_parameter.push_back(player.GetNick());
		
		response_parameter.push_back("uniquenick");
		response_parameter.push_back(uniquenick);
	}
	
	response_parameter.push_back("ndone");
	response_parameter.push_back("");
	response_parameter.push_back("final");
	
	// Convert parameter to string response
	std::string response = GameSpy::Parameter2Response(response_parameter);
	
	// Send
	this->Send(response);
	
	// Log
	this->_LogTransaction("<--", response);
}

/*
	Request:  \valid\\email\help0001@gmail.com\gamename\bfield1942ps2\final\
	Response: \vr\1\final\
	
	Request:
		email: (string)
		gamename: (string)
		
	Response:
		vr: (integer):
			1 = good
			0 = bad
	
*/
void GPSP::Client::requestValid(const GameSpy::Parameter& parameter) const
{
	if(parameter.size() != 7)
	{
		return;
	}
	
	std::string email = parameter[3];
	// Convert email to lowercase
	std::transform(email.begin(), email.end(), email.begin(), [](unsigned char c){ return std::tolower(c); });
	
	Battlefield::Players players;
	std::string response;
	
	// Get players from database by email
	g_database->queryPlayersByEmail(players, email);
	
	if(players.size() >= 1)
	{
		response = GameSpy::Parameter2Response({
			"vr", "1", "final"
		});
	}
	else
	{
		response = GameSpy::Parameter2Response({
			"vr", "0", "final"
		});
	}
	
	this->Send(response);
	
	this->_LogTransaction("<--", response);
}

/*
	Request:  \newuser\\nick\IamLupo@6507BAD7\email\help000@gmail.com\pass\12345\productID
	          \10307\namespaceid\13\uniquenick\IamLupo\gamename\bfield1942ps2\final\
	Response: \nur\0\pid\10037049\final\
	
	Request:
		nick: (string)
		email: (string)
		pass: (string)
		productID: (integer)
		namespaceid: (integer)
		uniquenick: (string)
		gamename: (string)
	
	Response:
		nur: (integer)
			0 = good
			516 = bad username
		pid: (integer)
*/
void GPSP::Client::requestNewUser(const GameSpy::Parameter& parameter) const
{
	if(parameter.size() != 17)
	{
		return;
	}
	
	std::string nick = parameter[3];
	
	// Fix: Else account with capital email address is stored in database.
	// If this happends then he cant login anymore because magicly the ps2 desides that it cant send capital email address xD
	std::string email = parameter[5];
	// Convert email to lowercase
	std::transform(email.begin(), email.end(), email.begin(), [](unsigned char c){ return std::tolower(c); });
	
	std::string password = parameter[7];
	std::string uniquenick = parameter[13];
	
	// Filter email
	if(email.size() > 50)
	{
		email = email.substr(0, 49);
	}

	// Remove clan name out of uniquenick
	Battlefield::Player player;
	player.SetUniquenickWithoutClanTag(uniquenick);
	uniquenick = player.GetUniquenick();
	
	// Check nick and uniquenick
	if(nick.size() < 3 or uniquenick.size() < 3)
	{
		std::string response = GameSpy::Parameter2Response({
			"nur", "516",
			"pid", "-1",
			"final"
		});
		
		this->Send(response);
		
		this->_LogTransaction("<--", response);
		
		return;
	}
	
	// Get players by email
	Battlefield::Players players;
	g_database->queryPlayersByEmailAndUniquenick(players, email, uniquenick);
	
	// Check Player exist with same email and uniquename already
	for(Battlefield::Player player : players)
	{
		// Check uniquenick
		if(player.GetUniquenick() == uniquenick)
		{
			std::string response = GameSpy::Parameter2Response({
				"nur", "516",
				"pid", std::to_string(player.GetProfileId()),
				"final"
			});
			
			this->Send(response);
			
			this->_LogTransaction("<--", response);
			
			return;
		}
	}
	
	// Create new dbuser
	Battlefield::Player new_player;
	
	new_player.SetNick(nick);
	new_player.SetEmail(email);
	new_player.SetUniquenick(uniquenick);
	new_player.SetPassword(password);
	
	// Check if player exist with same email
	if(players.size() >= 1)
	{
		// Copy there userid
		new_player.SetUserId(players[0].GetUserId());
	}
	else
	{
		// New userid
		g_database->queryPlayerNewUserID(new_player);
	}
	
	// Insert player in database
	g_database->insertPlayer(new_player);
	
	// Insert player status
	g_database->insertPlayerStats(new_player);
	
	std::string response = GameSpy::Parameter2Response({
		"nur", "0",
		"pid", std::to_string(new_player.GetProfileId()),
		"final"
	});
	
	this->Send(response);
	
	this->_LogTransaction("<--", response);
}

/*
	Request:
		\search\\sesskey\1\profileid\10036819\namespaceid\13\uniquenick\IamLupo\gamename\bfield1942ps2\final\
	Response:
		\bsr\10036819\nick\IamLupo@6507BAD7\firstname\\lastname\\email\[hidden]\uniquenick\IamLupo\namespaceid\13\bsrdone\\final\

		\bsrdone\\final\
*/
void GPSP::Client::requestSearch(const GameSpy::Parameter& parameter) const
{
	if(parameter.size() != 13)
	{
		return;
	}
	
	std::string uniquenick = parameter[9];
	
	Battlefield::Player player;
	player.SetUniquenick(uniquenick);
	
	// Get player information
	g_database->queryPlayerByUniquenick(player);
		
	std::string response;
	GameSpy::Parameter response_parameter;
	
	// User found
	if(player.GetProfileId() != -1)
	{
		response_parameter = {
			"bsr", std::to_string(player.GetProfileId()),
			"nick", player.GetNick(),
			"firstname", "",
			"lastname", "",
			"email", "[hidden]",
			"uniquenick", player.GetUniquenick(),
			"namespaceid", "13",
			"bsrdone", "",
			"final"
		};
	}
	
	response_parameter.push_back("bsrdone");
	response_parameter.push_back("");
	response_parameter.push_back("final");
	
	response = GameSpy::Parameter2Response(response_parameter);
	
	this->Send(response);
	
	this->_LogTransaction("<--", response);
}

/*
	Private functions
*/
void GPSP::Client::_LogTransaction(const std::string& direction, const std::string& response) const
{
	std::shared_lock<std::shared_mutex> guard(g_settings_mutex); // settings lock  (read)
	
	if ((g_logger_mode & Logger::Mode::Development) == 0)
	{
		return;
	}
	
	bool show_console = (g_settings["gpsp"]["show_requests"].asBool() && direction == "-->") ||
						(g_settings["gpsp"]["show_responses"].asBool() && direction == "<--");
	
	Logger::info(this->GetAddress() + " " + direction + " " + response,
			Server::Type::GPSP, show_console);
}

