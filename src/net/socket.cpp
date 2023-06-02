#include <vector>
#include <arpa/inet.h>
#include <unistd.h>

#include <net/socket.h>

Net::Socket::Socket()
{
	
}

void Net::Socket::Close()
{
	close(this->_socket);
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

void Net::Socket::Send(const std::string &msg) const
{
	std::lock_guard<std::mutex> guard(this->_mutex); // socket lock (read/write)
	
	send(this->_socket, msg.c_str(), msg.size(), 0);
}

void Net::Socket::Send(const std::vector<unsigned char> &msg) const
{
	std::lock_guard<std::mutex> guard(this->_mutex); // socket lock (read/write)
	
	send(this->_socket, &(msg[0]), msg.size(), 0);
}

