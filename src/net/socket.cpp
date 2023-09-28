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

void Net::Socket::GetIpArray(uint8_t* ip)
{
	*ip = this->_address.sin_addr.s_addr;
}

uint16_t Net::Socket::GetPort() const
{
	return ntohs(this->_address.sin_port);
}

std::string Net::Socket::GetAddress() const
{
	return this->GetIP() + ":" + std::to_string(this->GetPort());
}

std::string Net::Socket::GetSocketType() const
{
	int socket_type;
    socklen_t optlen = sizeof(socket_type);
    
	// Get socket type
	getsockopt(this->_socket, SOL_SOCKET, SO_TYPE, &socket_type, &optlen);
	
	switch(socket_type)
	{
		case SOCK_STREAM:
			return "tcp";
		break;
		
		case SOCK_DGRAM:
			return "udp";
		break;
		
		default:
			return "unknown";
		break;
	}
}

void Net::Socket::Send(const std::string& msg) const
{
	std::lock_guard<std::mutex> guard(this->_mutex); // socket lock (read/write)
	
	send(this->_socket, msg.c_str(), msg.size(), 0);
}

void Net::Socket::Send(const std::vector<unsigned char>& msg) const
{
	std::lock_guard<std::mutex> guard(this->_mutex); // socket lock (read/write)
	
	send(this->_socket, &(msg[0]), msg.size(), 0);
}

void Net::Socket::UDPSend(const std::string& msg) const
{
	std::lock_guard<std::mutex> guard(this->_mutex); // socket lock (read/write)
	
	socklen_t address_len = sizeof(this->_address);
	
	sendto(this->_socket, msg.c_str(), msg.size(), 0, (struct sockaddr*)&this->_address, address_len);
}

void Net::Socket::UDPSend(const std::vector<unsigned char>& msg) const
{
	std::lock_guard<std::mutex> guard(this->_mutex); // socket lock (read/write)
	
	socklen_t address_len = sizeof(this->_address);
	
	sendto(this->_socket, &(msg[0]), msg.size(), 0, (struct sockaddr*)&this->_address, address_len);
}

