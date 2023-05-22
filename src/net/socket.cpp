#include <arpa/inet.h>

#include <net/socket.h>

Net::Socket::Socket()
{
	
}

std::string Net::Socket::GetIP() const
{
	char ip[INET_ADDRSTRLEN];
	
	inet_ntop(AF_INET, &(this->_address.sin_addr), ip, INET_ADDRSTRLEN);
	
	return std::string(ip);
}

int Net::Socket::GetPort() const
{
	return ntohs(this->_address.sin_port);
}

std::string Net::Socket::GetAddress() const
{
	return this->GetIP() + ":" + std::to_string(this->GetPort());
}

