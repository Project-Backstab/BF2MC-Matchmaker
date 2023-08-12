#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include <algorithm>
 
#include <logger.h>
#include <settings.h>
#include <gpsp/client.h>
#include <gpcm/client.h>
#include <webserver/client.h>
#include <browsing/client.h>
#include <gamestats/client.h>
#include <qr/client.h>

#include <server.h>

Server::Server(Server::Type type)
{
	std::shared_lock<std::shared_mutex> guard2(g_mutex_settings); // settings lock (read)
	
	int port = -1;
	int opt = 10; // After 10 seconds time out socket
	int socket_type = SOCK_STREAM;
	
	this->_type = type;
	
	// setting
	switch(type)
	{
		case Server::Type::QR:
			port = g_settings["qr"]["port"].asInt();
			opt = g_settings["qr"]["connection_time_out"].asInt();
			socket_type = SOCK_DGRAM;
		break;
		case Server::Type::GPSP:
			port = g_settings["gpsp"]["port"].asInt();
			opt = g_settings["gpsp"]["connection_time_out"].asInt();
		break;
		case Server::Type::GPCM:
			port = g_settings["gpcm"]["port"].asInt();
			opt = g_settings["gpcm"]["connection_time_out"].asInt();
		break;
		case Server::Type::Webserver:
			port = g_settings["webserver"]["port"].asInt();
			opt = g_settings["webserver"]["connection_time_out"].asInt();
		break;
		case Server::Type::Browsing:
			port = g_settings["browsing"]["port"].asInt();
			opt = g_settings["browsing"]["connection_time_out"].asInt();
		break;
		case Server::Type::GameStats:
			port = g_settings["gamestats"]["port"].asInt();
			opt = g_settings["gamestats"]["connection_time_out"].asInt();
		break;
	}
	
	if ((this->_socket = socket(AF_INET, socket_type, 0)) < 0)
	{
		Logger::error("Server::Server() at socket", this->_type);
		exit(EXIT_FAILURE);
	}
	
	if (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		Logger::error("Server::Server() at setsockopt", this->_type);
		exit(EXIT_FAILURE);
	}

	this->_address.sin_family = AF_INET;
	this->_address.sin_addr.s_addr = INADDR_ANY;
	this->_address.sin_port = htons(port);

	if (bind(this->_socket, (struct sockaddr*)&this->_address, sizeof(this->_address)) < 0)
	{
		Logger::error("Server::Server() at bind", this->_type);
		exit(EXIT_FAILURE);
	}
}

void Server::Listen()
{
	int client_socket;
	struct sockaddr_in client_address;
	socklen_t client_address_len = sizeof(client_address);
	
	if (listen(this->_socket, 3) < 0)
	{
		Logger::error("Server::Listen() on listen", this->_type);
		exit(EXIT_FAILURE);
	}
	
	this->onServerListen();
	
	while(true)
	{
		if ((client_socket = accept(this->_socket, (struct sockaddr*)&client_address, &client_address_len)) < 0)
		{	
			Logger::error("Server::Listen() on accept", this->_type);
			exit(EXIT_FAILURE);
		}
		
		switch(this->_type)
		{
			case Server::Type::GPSP:
			{
				Net::Socket* client = new GPSP::Client(client_socket, client_address);
				
				this->onClientConnect(*client);
				
				std::thread t(&GPSP::Client::Listen, (GPSP::Client*)client);
				t.detach();
				
				this->_clients.push_back(client);
			}	
			break;
			case Server::Type::GPCM:
			{
				Net::Socket* client = new GPCM::Client(client_socket, client_address);
				
				this->onClientConnect(*client);
				
				std::thread t(&GPCM::Client::Listen, (GPCM::Client*)client);
				t.detach();
				
				this->_clients.push_back(client);
			}
			break;
			case Server::Type::Browsing:
			{
				Net::Socket* client = new Browsing::Client(client_socket, client_address);
				
				this->onClientConnect(*client);
				
				std::thread t(&Browsing::Client::Listen, (Browsing::Client*)client);
				t.detach();
				
				this->_clients.push_back(client);
			}
			break;
			case Server::Type::Webserver:
			{
				Net::Socket* client = new Webserver::Client(client_socket, client_address);
				
				this->onClientConnect(*client);
				
				std::thread t(&Webserver::Client::Listen, (Webserver::Client*)client);
				t.detach();
				
				this->_clients.push_back(client);
			}
			break;
			case Server::Type::GameStats:
			{
				Net::Socket* client = new GameStats::Client(client_socket, client_address);
				
				this->onClientConnect(*client);
				
				std::thread t(&GameStats::Client::Listen, (GameStats::Client*)client);
				t.detach();
				
				this->_clients.push_back(client);
			}
			break;
		}
	}
}

void Server::UDPListen()
{
	int client_socket;
	struct sockaddr_in client_address;
	socklen_t client_address_len = sizeof(client_address);

	this->onServerListen();
	
	while (true)
	{
		std::vector<unsigned char> buffer(2048, 0);
		
		ssize_t recv_size = recvfrom(this->_socket, &(buffer[0]), 2048, 0, (struct sockaddr*)&client_address, &client_address_len);
		
		if (recv_size < 0)
		{
			Logger::error("Server::UDPListen() on recvfrom", this->_type);
			close(this->_socket);
			return;
		}
		
		//Logger::debug("recv_size = " + std::to_string(recv_size));
		
		buffer.resize(recv_size);
		
		switch(this->_type)
		{
			case Server::Type::QR:
				QR::Client client = QR::Client(this->_socket, client_address);
				
				client.onRequest(buffer);
			break;
		}
	}
}

void Server::DisconnectAllClients()
{
	for(Net::Socket* client : this->_clients)
	{
		switch(this->_type)
		{
			case Server::Type::GPSP:
				((GPSP::Client*)client)->Disconnect();
			break;
			case Server::Type::GPCM:
				((GPCM::Client*)client)->Disconnect();
			break;
			case Server::Type::Browsing:
				((Browsing::Client*)client)->Disconnect();
			break;
			case Server::Type::Webserver:
				((Webserver::Client*)client)->Disconnect();
			break;
			case Server::Type::GameStats:
				((GameStats::Client*)client)->Disconnect();
			break;
		}
	}
}

void Server::Close()
{
	shutdown(this->_socket, SHUT_RDWR);
	
	onServerShutdown();
}

/*
	Events
*/
void Server::onServerListen() const
{
	Logger::info("Server is now listening on " + this->GetAddress() + " " + this->GetSocketType(), this->_type);
}

void Server::onServerShutdown() const
{
	Logger::info("Server shutdown", this->_type);
}

void Server::onClientConnect(const Net::Socket& client) const
{
	std::shared_lock<std::shared_mutex> guard2(g_mutex_settings); // settings lock  (read)
	
	Logger::info("Client " + client.GetAddress() + " connected",
		this->_type, g_settings["show_client_connect"].asBool());
}

void Server::onClientDisconnect(const Net::Socket& client)
{
	std::shared_lock<std::shared_mutex> guard2(g_mutex_settings); // settings lock  (read)
	
	Logger::info("Client " + client.GetAddress() + " disconnected",
			this->_type, g_settings["show_client_disconnect"].asBool());
	
	auto it = std::find(this->_clients.begin(), this->_clients.end(), const_cast<Net::Socket*>(&client));
	if (it != this->_clients.end())
	{
		this->_clients.erase(it);
		
		delete &client;
	}
}

