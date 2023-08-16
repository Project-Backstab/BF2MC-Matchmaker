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
			
			// See reference:
			//   https://documentation.help/GameSpy-SDK/Gamespy%20Query%20and%20Reporting%202%20SDK.html
			//
			
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
			uint8_t     _mc;              // Map cycling (1: levelmap with mapname)
			                              //             (2: All Maps)
			                              //             (3: Assualt Maps)
			                              //             (4: Incursion Maps)
			                              //             (5: Domination Maps)
			uint8_t     _mapname;         // Map id
			
			uint8_t     _gc;              // Game type                                 ??? Unsure ???
			std::string _gametype;        // String which specifies the type of game, or the mod being played.
			                              // (conquest, capturetheflag)
			
			std::string _gamevariant;     // Game code name
			uint8_t     _numplayers;      // Number of players
			uint8_t     _maxplayers;      // Max players
			uint8_t     _numteams;        // Number of teams
			std::string _gamemode;        // Game status (openwaiting,
			                              //              closedwaiting,
			                              //              closedplaying,
			                              //              openplaying,
			                              //              openstaging,
			                              //              closedstaging,
			                              //              exiting)
			uint8_t     _teamplay;        // number which defines the type of teamplay in use, or 0 for no teamplay.
			                              // Values > 0 are up to the developer
			uint8_t     _fraglimit;       // Number of total kills or points before a level change or game restart
			uint8_t     _teamfraglimit;   // Number of total kills or points for a team before a level change or
			                              // game restart.
			uint8_t     _timelimit;       // Amount of total time before a level change or game restart occurs in seconds
			uint8_t     _timeelapsed;     // Amount of time (in seconds) since the current level or game started
			uint8_t     _password;        // Has password requirement to join (1: true, 0: false)
			uint8_t     _nr;              // Minimum Rank number filter
			uint8_t     _xr;              // Maximum Rank number filter
			uint8_t     _ff;              // Friendly fire (1: true, 0: false)
			uint8_t     _sr;              // Stats tracking (1: true, 0: false)
			uint8_t     _rc;              // Server is reconfigurable (1: true, 0: false)
			int64_t     _ni;              // Minimum IP range
			int64_t     _xi;              // Maximum IP range
			uint8_t     _qm;              //                                           ??? Unsure ???
			uint8_t     _region;          // Server region (65536: Europe, 1: Us, 2048: Japan)
			
			// Clan
			int8_t      _c0;              // Clanid team 1
			int8_t      _c1;              // Clanid team 2
			std::string _n0;              // Clan name team 1
			std::string _n1;              // Clan name team 2
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
