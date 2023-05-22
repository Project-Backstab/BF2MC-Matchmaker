#include <unistd.h>
#include <iostream>
#include <thread>
#include <iomanip>
#include <map>

#include <server.h>
#include <gamespy.h>
#include <globals.h>
#include <util.h>
#include <database.h>

#include <gpsp/client.h>

typedef void (GPSP::Client::*RequestActionFunc)(const GameSpy::Parameter&) const;

static std::map<std::string, RequestActionFunc> mRequestActions = 
{
	{ "nicks",   &GPSP::Client::requestNicks   },
	{ "valid",   &GPSP::Client::requestValid   },
	{ "newuser", &GPSP::Client::requestNewUser },
	{ "search",  &GPSP::Client::requestSearch },
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
	close(this->_socket);
	g_gpsp_server->onClientDisconnect(*this);
}

void GPSP::Client::Send(const std::string &msg) const
{
	send(this->_socket, msg.c_str(), msg.size(), 0);
}

/*
	Events
*/
void GPSP::Client::onRequest(const std::string &request)
{
	GameSpy::Parameter parameter = GameSpy::Request2Parameter(request);
	
	// Find function name
	std::string action = parameter[0];
	
	if (mRequestActions.find(action) != mRequestActions.end())
	{
		// Get Function address
		RequestActionFunc func = mRequestActions[action];
	
		// Execute action function with class object.
		(this->*(func))(parameter);
	}
	else
	{
		std::unique_lock<std::mutex> guard(g_mutex_io);
		
		std::cout << "action \"" << action << "\"not implemented!" << std::endl;
		
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
	GameSpy::Parameter response_parameter;
	
	std::vector<DBUser> dbusers;
	
	if(!g_database->queryDBUsersByEmail(dbusers, email))
	{
		return; // No Database user found with uniquenick
	}
	
	std::cout << dbusers.size() << std::endl;
	
	response_parameter = { "nr", "0" };
	
	for(DBUser dbuser : dbusers)
	{
		response_parameter.push_back("nick");
		response_parameter.push_back(dbuser.nick);
		
		response_parameter.push_back("uniquenick");
		response_parameter.push_back(dbuser.uniquenick);
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
	
	std::string password = parameter[7];
	std::string uniquenick = parameter[13];
	int profileid;
	
	DBUser dbuser;
	
	dbuser.nick = parameter[3];
	dbuser.email = parameter[5];
	dbuser.uniquenick = uniquenick;
	dbuser.password = Util::MD5hash(password);
	
	// Insert user in database
	if(!g_database->insertDBUser(dbuser, profileid))
	{
		return; // Oeps something went wrong?!
	}
	
	if(!g_database->queryDBUserByUniquenick(dbuser, uniquenick))
	{
		return; // No Database user found with uniquenick
	}
	
	std::string response = GameSpy::Parameter2Response({
		"nur", "0",
		"pid", std::to_string(dbuser.profileid),
		"final"
	});
	
	this->Send(response);
	
	this->_LogTransaction("<--", response);
}

/*
	Request:
		\search\\sesskey\1\profileid\10036819\namespaceid\13\uniquenick\rank_name\gamename\bfield1942ps2\final\
	Response:
		???
*/
void GPSP::Client::requestSearch(const GameSpy::Parameter& parameter) const
{	
	std::string response = GameSpy::Parameter2Response({
		"bsr", "1",
		"nick", "LooooL",
		"firstname", "rank_name",
		"lastname", "lol",
		"email", "lol@lol.lol",
		"uniquenick", "rank_name",
		"namespaceid", "13",
		"bsrdone", "final"
	});
	
	this->Send(response);
	
	this->_LogTransaction("<--", response);
}

/*
	Private functions
*/
void GPSP::Client::_LogTransaction(const std::string &direction, const std::string &response) const
{
	std::lock_guard<std::mutex> guard(g_mutex_io);
	
	std::cout << std::setfill(' ') << std::setw(21) << this->GetAddress() << " " << direction << " " << response << std::endl;
}

