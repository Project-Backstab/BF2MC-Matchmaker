#include <string>
#include <arpa/inet.h>

#include <browsing/constants.h>

#include <battlefield/gameserver.h>

void Battlefield::GameServer::useExample()
{
	this->SetIp("168.119.189.149");
	this->SetPort(3658);
	this->SetFlag(FLAG_UNSOLICITED_UDP | FLAG_PRIVATE_IP | FLAG_ICMP_IP | FLAG_NONSTANDARD_PORT | FLAG_NONSTANDARD_PRIVATE_PORT);
}

bool Battlefield::GameServer::SetIp(uint8_t* ip)
{
	this->_ip[0] = ip[0];
	this->_ip[1] = ip[1];
	this->_ip[2] = ip[2];
	this->_ip[3] = ip[3];
	
	return true;
}

bool Battlefield::GameServer::SetIp(const std::string& str_ip)
{
	uint8_t ip[4];
	
	if(inet_pton(AF_INET, str_ip.c_str(), ip) != 1)
	{
		return false;
	}
	
	return this->SetIp(ip);
}

bool Battlefield::GameServer::SetPort(uint16_t port)
{
	this->_port = port;
	return true;
}

bool Battlefield::GameServer::SetFlag(uint8_t flag)
{
	this->_flag = flag;
	return true;
}

