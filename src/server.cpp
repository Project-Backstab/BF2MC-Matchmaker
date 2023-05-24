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
 
#include <server.h>
#include <globals.h>
#include <gpsp/client.h>
#include <gpcm/client.h>
#include <browsing/client.h>
#include <easports/client.h>

Server::Server(Server::Type type, int port)
{
	int opt = 1;
	
	this->_type = type;
	
	if ((this->_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("[Error] Server::Server() at socket");
		exit(EXIT_FAILURE);
	}
	
	if (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("[Error] Server::Server() at setsockopt");
		exit(EXIT_FAILURE);
	}

	this->_address.sin_family = AF_INET;
	this->_address.sin_addr.s_addr = INADDR_ANY;
	this->_address.sin_port = htons(port);

	if (bind(this->_socket, (struct sockaddr*)&this->_address, sizeof(this->_address)) < 0)
	{
		perror("[Error] Server::Server() at bind");
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
		perror("[Error] Server::Listen() on listen");
		exit(EXIT_FAILURE);
	}
	
	this->onServerListen();
	
	while(true)
	{
		if ((client_socket = accept(this->_socket, (struct sockaddr*)&client_address, &client_address_len)) < 0)
		{
			perror("[Error] Server::Listen() on accept");
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
			case Server::Type::EASports:
			{
				Net::Socket* client = new EASports::Client(client_socket, client_address);
				
				this->onClientConnect(*client);
				
				std::thread t(&EASports::Client::Listen, (EASports::Client*)client);
				t.detach();
				
				this->_clients.push_back(client);
			}
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
	std::lock_guard<std::mutex> guard(g_mutex_io);
	
	std::cout << "Server is now listening on " << this->GetAddress() << std::endl;
}

void Server::onServerShutdown() const
{
	std::lock_guard<std::mutex> guard(g_mutex_io);
	
	std::cout << "Server shutdown" << std::endl;
}

void Server::onClientConnect(const Net::Socket &client) const
{
	std::lock_guard<std::mutex> guard(g_mutex_io);
	
	//std::cout << "Client " << client.GetAddress() << " connected" << std::endl;
}

void Server::onClientDisconnect(const Net::Socket &client)
{
	std::lock_guard<std::mutex> guard(g_mutex_io);
	
	std::vector<Net::Socket*>::iterator position;

	position = std::find(this->_clients.begin(), this->_clients.end(), const_cast<Net::Socket*>(&client));
	if (position != this->_clients.end()) // == myVector.end() means the element was not found
		this->_clients.erase(position);
	
	//std::cout << "Client " << client.GetAddress() << " disconnected" << std::endl;
	
	//std::cout << "Total Clients = " << this->_clients.size() << std::endl;
}

