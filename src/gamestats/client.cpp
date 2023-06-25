#include <unistd.h>
#include <iostream>

#include <settings.h>
#include <logger.h>
#include <server.h>
#include <globals.h>
#include <util.h>

#include <gamestats/client.h>

typedef void (GameStats::Client::*RequestActionFunc)(const GameSpy::Parameter&);

static std::map<std::string, RequestActionFunc> mRequestActions = 
{
	{ "auth",      &GameStats::Client::requestAuth },
	{ "newgame",   &GameStats::Client::requestNewGame }
};

GameStats::Client::Client(int socket, struct sockaddr_in address)
{
	this->_socket = socket;
	this->_address = address;
}

GameStats::Client::~Client()
{
	this->Disconnect();
}

void GameStats::Client::Listen()
{
	while(true)
	{
		std::string request;
		std::vector<unsigned char> buffer(4096, 0);
		
		int v = read(this->_socket, &(buffer[0]), 4096);
		
		// If error or no data is recieved we end the connection
		if(v <= 0)
		{
			break;
		}
		
		// Resize buffer
		buffer.resize(v);
		
		request = Decrypt(buffer);
		
		this->_LogTransaction("-->", request);
		
		this->onRequest(request);
	}
	
	this->Disconnect();
}

void GameStats::Client::Disconnect()
{
	this->Close();
	g_gamestats_server->onClientDisconnect(*this);
}

/*
	Events
*/
void GameStats::Client::onRequest(const std::string& request)
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
		Logger::warning("action \"" + action + "\" not implemented!", Server::Type::GPCM);
		
		this->Disconnect();
	}
}

/*
	Response:
		\lc\1\challenge\GzlCt7q8sV\id\1\final\
	Request:
		\auth\\gamename\bfield1942ps2\response\f34cc66938c4b07c70ebff98d9d98561\port\0\id\1\final\
	Response:
		\lc\2\sesskey\1687554231\proof\0\id\1\final\
*/
void GameStats::Client::requestAuth(const GameSpy::Parameter& parameter)
{	
	
}

/*
	Request:
		\newgame\\connid\1687554231\sesskey\197854479\final\
*/
void GameStats::Client::requestNewGame(const GameSpy::Parameter& parameter)
{	
	
}

/*
	Private functions
*/
void GameStats::Client::_LogTransaction(const std::string& direction, const std::string& response) const
{
	std::shared_lock<std::shared_mutex> guard2(g_mutex_settings); // settings lock  (read)
	
	bool show_console = (g_settings["browsing"]["show_requests"].asBool() && direction == "-->") ||
						(g_settings["browsing"]["show_responses"].asBool() && direction == "<--");
	
	Logger::info(this->GetAddress() + " " + direction + " " + response,
			Server::Type::GameStats, show_console);
}

static std::vector<unsigned char> example_A_request = {
	0x1b, 0x0d, 0x0e, 0x39, 0x62, 0x2c, 0x1a, 0x5b, 0x25, 0x2b, 0x0d, 0x08, 0x0b, 0x34, 0x15, 0x25,
	0x74, 0x3e, 0x2b, 0x22, 0x19, 0x52, 0x22, 0x48, 0x0a, 0x65, 0x18, 0x2e, 0x05, 0x31, 0x54, 0x5c,
	0x66, 0x69, 0x6e, 0x61, 0x6c, 0x5c, 
};

static std::vector<unsigned char> example_A_response = {
	0x1b, 0x00, 0x18, 0x11, 0x3b, 0x2c, 0x25, 0x54, 0x25, 0x2a, 0x04, 0x03, 0x04, 0x3e, 0x15, 0x25,
	0x51, 0x22, 0x2e, 0x04, 0x01, 0x01, 0x62, 0x49, 0x4d, 0x01, 0x34, 0x34, 0x53, 0x31, 0x17, 0x36,
	0x03, 0x09, 0x5c, 0x2a, 0x34, 0x04, 0x31, 0x03, 0x60, 0x44, 0x1a, 0x50, 0x72, 0x71, 0x58, 0x5e,
	0x5d, 0x30, 0x44, 0x1b, 0x03, 0x73, 0x24, 0x56, 0x5d, 0x00, 0x31, 0x16, 0x1f, 0x0a, 0x7c, 0x23,
	0x58, 0x09, 0x5c, 0x6b, 0x45, 0x4f, 0x02, 0x18, 0x37, 0x0e, 0x1f, 0x11, 0x0f, 0x40, 0x25, 0x5a,
	0x20, 0x1b, 0x50, 0x5c, 0x66, 0x69, 0x6e, 0x61, 0x6c, 0x5c, 
};

static std::vector<unsigned char> example_B_request = {
	0x1b, 0x0d, 0x0e, 0x39, 0x61, 0x2c, 0x0a, 0x56, 0x37, 0x34, 0x0a, 0x08, 0x1c, 0x0f, 0x41, 0x4f,
	0x0b, 0x73, 0x72, 0x54, 0x59, 0x57, 0x60, 0x41, 0x25, 0x43, 0x36, 0x28, 0x0e, 0x0b, 0x39, 0x63,
	0x2c, 0x10, 0x57, 0x18, 0x76, 0x5c, 0x66, 0x69, 0x6e, 0x61, 0x6c, 0x5c, 
};

static std::vector<unsigned char> example_B_response = {
	0x1b, 0x0f, 0x08, 0x12, 0x34, 0x11, 0x14, 0x56, 0x18, 0x1b, 0x02, 0x02, 0x0b, 0x3d, 0x19, 0x1d,
	0x6f, 0x75, 0x71, 0x59, 0x5a, 0x50, 0x66, 0x44, 0x4b, 0x00, 0x75, 0x1b, 0x12, 0x08, 0x16, 0x20,
	0x1b, 0x1c, 0x4a, 0x18, 0x76, 0x58, 0x5a, 0x5d, 0x66, 0x44, 0x4d, 0x04, 0x7d, 0x5c, 0x66, 0x69,
	0x6e, 0x61, 0x6c, 0x5c, 
};

std::string GameStats::Client::Decrypt(const std::vector<unsigned char>& request)
{
	std::string msg;
	const char key[10] = "GameSpy3D";
	
	for(int i = 0; i < request.size() - 7; i++)
	{
		msg += key[(i % 9)] ^ request[i];
	}
	
	msg += "\\final\\";
	
	return msg;
}

std::vector<unsigned char> GameStats::Client::Encrypt(const std::string& response)
{
	std::vector<unsigned char> enc_response;
	const char key[10] = "GameSpy3D";
	
	for(int i = 0; i < response.size() - 7; i++)
	{
		enc_response.push_back(key[(i % 9)] ^ response[i]);
	}
	
	// Add final at the end
	enc_response.push_back(0x5c);
	enc_response.push_back(0x66);
	enc_response.push_back(0x69);
	enc_response.push_back(0x6e);
	enc_response.push_back(0x61);
	enc_response.push_back(0x6c);
	enc_response.push_back(0x5c);
	
	return enc_response;
}

void GameStats::Client::Test()
{
	Logger::debug(Decrypt(example_A_request));
	Logger::debug(Decrypt(Encrypt(Decrypt(example_A_request))));
	
	Logger::debug(Decrypt(example_A_response));
	Logger::debug(Decrypt(Encrypt(Decrypt(example_A_response))));
	
	Logger::debug(Decrypt(example_B_request));
	Logger::debug(Decrypt(Encrypt(Decrypt(example_B_request))));
	
	Logger::debug(Decrypt(example_B_response));
	Logger::debug(Decrypt(Encrypt(Decrypt(example_B_response))));
}