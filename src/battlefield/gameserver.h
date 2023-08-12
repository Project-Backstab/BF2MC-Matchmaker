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
			uint8_t     _ip[4];
			uint16_t    _port;
			uint8_t     _flag;
			
			std::string _localip0;        // Local ip
			std::string _localport;       // Local port
			uint8_t     _natneg;          // Enable client-to-client connections (1: true, 0: false)                                       ??? Unsure ???
			std::string _gamename;        // Game name
			std::string _hostname;        // Host name
			std::string _hostport;        // Host port
			
			std::string _gamever;         // Game version
			std::string _cl;              // Client version
			std::string _rv;              // ??? (retail)                              ??? Unsure ???
			
			std::string _map;             // Map name
			uint8_t     _mc;              // Map type (1: levelmap with mapname)
			                              //          (2: All Maps)
			                              //          (3: Assualt Maps)
			                              //          (4: Incursion Maps)
			                              //          (5: Domination Maps)
			uint8_t     _mapname;         // Map id
			
			uint8_t     _gc;              // Game type                                 ??? Unsure ???
			std::string _gametype;        // Game id (conquest, capturetheflag)
			
			std::string _gamevariant;     // Game code name
			uint8_t     _numplayers;      // Number of players
			uint8_t     _maxplayers;      // Max players
			uint8_t     _numteams;        // Number of teams
			std::string _gamemode;        // Game status (openplaying)                 ??? Unsure ???
			uint8_t     _teamplay;        // Is teamplay? (1: true, 0: false)          ??? Unsure ???
			uint8_t     _fraglimit;       //                                           ??? Unsure ???
			uint8_t     _teamfraglimit;   //                                           ??? Unsure ???
			uint8_t     _timelimit;       // Total time in seconds
			uint8_t     _timeelapsed;     // Total time elapsed in seconds
			uint8_t     _password;        // Has password (1: true, 0: false)
			uint8_t     _nr;              // Minimum Rank number filter
			uint8_t     _xr;              // Maximum Rank number filter
			uint8_t     _ff;              // Friendly fire (1: true, 0: false)
			uint8_t     _sr;              // Stats tracking (1: true, 0: false)
			uint8_t     _rc;              //                                           ??? Unsure ???
			int64_t     _ni;              //                                           ??? Unsure ???
			int64_t     _xi;              //                                           ??? Unsure ???
			uint8_t     _qm;              //                                           ??? Unsure ???
			uint8_t     _region;          // Server region (65536: Europe, 1: Us, 2048: Japan)
			
			// Clan
			int8_t      _c0;              // Clanid team 1
			int8_t      _c1;              // Clanid team 2
			std::string _n0;              //                                           ??? Unsure ???
			std::string _n1;              //                                           ??? Unsure ???
			uint8_t     _c0c;             // Team 1 is claimed (1: true, 0: false)
			uint8_t     _c1c;             // Team 2 is claimed (1: true, 0: false)
		
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
