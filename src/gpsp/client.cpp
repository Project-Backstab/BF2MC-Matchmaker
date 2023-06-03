#include <unistd.h>
#include <iostream>
#include <iomanip>

#include <settings.h>
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
		
		int v = read(this->_socket, &(buffer[0]), 4096);
		
		// If error or no data is recieved we end the connection
		if(v <= 0)
		{
			break;
		}
		
		// Resize buffer
		buffer.resize(v);
		
		request = Util::Buffer2String(buffer);
		
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
		std::unique_lock<std::mutex> guard(g_mutex_io); // io lock (read/write)
		
		std::cout << "action \"" << action << "\" not implemented!" << std::endl;
		
		guard.unlock();
		
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
		response_parameter.push_back("nick");
		response_parameter.push_back(player.GetNick());
		
		response_parameter.push_back("uniquenick");
		response_parameter.push_back(player.GetUniquenick());
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
	
	std::string response = GameSpy::Parameter2Response({
		"vr", "1", "final"
	});
	
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
	std::string email = parameter[5];
	std::string password = parameter[7];
	std::string uniquenick = parameter[13];
	
	// Filter email
	if(email.size() > 50)
	{
		email = email.substr(0, 49);
	}
	
	Battlefield::Players players;
	
	// Get players by email
	g_database->queryPlayersByEmail(players, email);
	
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
	
	// Get missing profileid
	g_database->queryPlayerByUniquenick(new_player);
	
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
	std::lock_guard<std::mutex>         guard(g_mutex_io);        // io lock        (read/write)
	std::shared_lock<std::shared_mutex> guard2(g_mutex_settings); // settings lock  (read)
	
	if(
		(g_settings["gpsp"]["show_requests"].asBool() && direction == "-->") ||
		(g_settings["gpsp"]["show_responses"].asBool() && direction == "<--")
	)
	{
		std::cout << std::setfill(' ') << std::setw(21) << this->GetAddress() << " " << direction << " " << response << std::endl;
	}
}

