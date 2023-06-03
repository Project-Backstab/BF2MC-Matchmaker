#ifndef BATTLEFIELD_GAMESERVER_H
#define BATTLEFIELD_GAMESERVER_H

#include <cstdint>
#include <vector>

namespace Battlefield
{
	class GameServer;
	
	typedef std::vector<GameServer> GameServers;
	
	/*
		Game server information
	*/
	class GameServer
	{	
		private:
			uint8_t  _ip[4];
			uint16_t _port;
			uint8_t  _flag;
		
		public:
			void useExample();
			
			const uint8_t* GetIp() const           { return this->_ip;   }
			uint16_t GetPort() const               { return this->_port; }
			uint8_t GetFlag() const                { return this->_flag; }
			
			bool SetIp(uint8_t* ip);
			bool SetIp(const std::string& str_ip);
			bool SetPort(uint16_t port);
			bool SetFlag(uint8_t flag);
	};
}

#endif // BATTLEFIELD_GAMESERVER_H
