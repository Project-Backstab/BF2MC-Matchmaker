#include <unistd.h>
#include <iostream>

#include <settings.h>
#include <logger.h>
#include <server.h>
#include <globals.h>
#include <util.h>

#include <gamestats/client.h>

typedef void (GameStats::Client::*RequestActionFunc)(const std::vector<unsigned char>&);

static std::map<uint8_t, RequestActionFunc> mRequestActions = 
{
	
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
		
		std::vector<char> buffer2(buffer.begin(), buffer.end());
		request = Util::Buffer2String(buffer2);
		
		this->_LogTransaction("-->", request);
		
		this->onRequest(buffer);
	}
	
	this->Disconnect();
}

void GameStats::Client::Disconnect()
{
	this->Close();
	g_browsing_server->onClientDisconnect(*this);
}

/*
	Events
*/
void GameStats::Client::onRequest(const std::vector<unsigned char>& request)
{	
	uint8_t action = static_cast<uint8_t>(request[2]);
	
	// Find function
	auto it = mRequestActions.find(action);
	if (it != mRequestActions.end())
	{
		// Get Function address
		RequestActionFunc func = it->second;
	
		// Execute action function with class object.
		(this->*(func))(request);
	}
	else
	{
		Logger::warning("action \"" + std::to_string(action) + "\"not implemented!");
		
		this->Disconnect();
	}
}

/*
void Browsing::Client::requestServerList(const std::vector<unsigned char>& request)
{	
	
}
*/

/*
	Private functions
*/
void GameStats::Client::_LogTransaction(const std::string& direction, const std::string& response) const
{
	std::shared_lock<std::shared_mutex> guard2(g_mutex_settings); // settings lock  (read)
	
	bool show_console = (g_settings["browsing"]["show_requests"].asBool() && direction == "-->") ||
						(g_settings["browsing"]["show_responses"].asBool() && direction == "<--");
	
	Logger::info(this->GetAddress() + " " + direction + " " + response, show_console);
}

