#ifndef BATTLEFIELD_GAMESERVER_H
#define BATTLEFIELD_GAMESERVER_H

#include <cstdint>
#include <vector>

namespace Battlefield
{
	/*
		Forward declair
	*/
	class GameServer;
	struct GameServerPlayer;
	
	/*
		Type definitions
	*/
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
			uint16_t    _localport;       // Local port
			uint8_t     _natneg;          // Enable client-to-client connections (1: true, 0: false)                                       ??? Unsure ???
			std::string _gamename;        // Game name
			std::string _hostname;        // Host name
			uint16_t    _hostport;        // Host port
			
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
			
			//std::vector<GameServerPlayer> _players;
		
		public:
			void useExample();
			
			const uint8_t* GetIp() const         { return this->_ip;   }
			uint16_t       GetPort() const       { return this->_port; }
			uint8_t        GetFlag() const       { return this->_flag; }
			
			std::string GetLocalIp() const        { return this->_localip0; }
			uint16_t    GetLocalPort() const      { return this->_localport; }
			uint8_t     GetNatNeg() const         { return this->_natneg; }
			std::string GetGameName() const       { return this->_gamename; }
			std::string GetHostName() const       { return this->_hostname; }
			uint16_t    GetHostPort() const       { return this->_hostport; }
			std::string GetGameVersion() const    { return this->_gamever; }
			std::string GetClientVersion() const  { return this->_cl; }
			std::string GetRV() const             { return this->_rv; }
			std::string GetMap() const            { return this->_map; }
			uint8_t     GetMapCycling() const     { return this->_mc; }
			uint8_t     GetMapName() const        { return this->_mapname; }
			uint8_t     GetGC() const             { return this->_gc; }
			std::string GetGameType() const       { return this->_gametype; }
			std::string GetGameVariant() const    { return this->_gamevariant; }
			uint8_t     GetNumPlayers() const     { return this->_numplayers; }
			uint8_t     GetMaxPlayers() const     { return this->_maxplayers; }
			uint8_t     GetNumTeams() const       { return this->_numteams; }
			std::string GetGameMode() const       { return this->_gamemode; }
			uint8_t     GetTeamplay() const       { return this->_teamplay; }
			uint8_t     GetFlagLimit() const      { return this->_fraglimit; }
			uint8_t     GetTeamFragLimit() const  { return this->_teamfraglimit; }
			uint8_t     GetTimeLimit() const      { return this->_timelimit; }
			uint8_t     GetTimeElapsed() const    { return this->_timeelapsed; }
			uint8_t     GetPassword() const       { return this->_password; }
			int64_t     GetMinRank() const        { return this->_nr; }
			int64_t     GetMaxRank() const        { return this->_xr; }
			uint8_t     GetFriendlyFire() const   { return this->_ff; }
			uint8_t     GetStatsTracking() const  { return this->_sr; }
			uint8_t     GetReconfigurable() const { return this->_rc; }
			uint8_t     GetMinIpRange() const     { return this->_ni; }
			uint8_t     GetMaxIpRange() const     { return this->_xi; }
			uint8_t     GetQM() const             { return this->_qm; }
			uint8_t     GetRegion() const         { return this->_region; }
			// Clan
			int8_t      GetClanIdTeam1() const   { return this->_c0; }
			int8_t      GetClanIdTeam2() const   { return this->_c1; }
			std::string GetClanNameTeam1() const { return this->_n0; }
			std::string GetClanNameTeam2() const { return this->_n1; }
			uint8_t     GetClan1Claimed() const  { return this->_c0c; }
			uint8_t     GetClan2Claimed() const  { return this->_c1c; }
			
			bool SetIp(uint8_t* ip);
			bool SetIp(const std::string& str_ip);
			bool SetPort(uint16_t port);
			bool SetFlag(uint8_t flag);
			bool SetLocalIp(const std::string& str_ip);
			bool SetLocalPort(uint16_t port);
			bool SetNatNeg(uint8_t natneg);
			bool SetGameName(const std::string& gamename);
			bool SetHostName(const std::string& hostname);
			bool SetHostPort(uint16_t hostport);
			bool SetGameVersion(const std::string& gamever);
			bool SetClientVersion(const std::string& clientver);
			bool SetRV(const std::string& rv);
			bool SetMap(const std::string& map);
			bool SetMapCycling(uint8_t map_cycling);
			bool SetMapName(uint8_t mapname);
			bool SetGC(uint8_t gc);
			bool SetGameType(const std::string& gametype);
			bool SetGameVariant(const std::string& gamevariant);
			bool SetNumPlayers(uint8_t numplayers);
			bool SetMaxPlayers(uint8_t maxplayers);
			bool SetNumTeams(uint8_t numteams);
			bool SetGameMode(const std::string& gamemode);
			bool SetTeamplay(uint8_t teamplay);
			bool SetFlagLimit(uint8_t fraglimit);
			bool SetTeamFragLimit(uint8_t teamfraglimit);
			bool SetTimeLimit(uint8_t timelimit);
			bool SetTimeElapsed(uint8_t timeelapsed);
			bool SetPassword(uint8_t timeelapsed);
			bool SetMinRank(int64_t nr);
			bool SetMaxRank(int64_t xr);
			bool SetFriendlyFire(uint8_t ff);
			bool SetStatsTracking(uint8_t sr);
			bool SetReconfigurable(uint8_t rc);
			bool SetMinIpRange(uint8_t ni);
			bool SetMaxIpRange(uint8_t xi);
			bool SetQM(uint8_t qm);
			bool SetRegion(uint8_t region);
			// Clan
			bool SetClanIdTeam1(int8_t c0);
			bool SetClanIdTeam2(int8_t c1);
			bool SetClanNameTeam1(const std::string& n0);
			bool SetClanNameTeam2(const std::string& n1);
			bool SetClan1Claimed(uint8_t c0c);
			bool SetClan2Claimed(uint8_t c1c);
	};
	
	/*
	struct GameServerPlayer
	{
		uint16_t    _score     = 0;
		std::string _skill     = "";
		uint8_t     _ping      = 0;
		uint8_t     _team      = 0;
		uint16_t    _deaths    = 0;
		int         _profileid = -1;
	};
	*/
}

#endif // BATTLEFIELD_GAMESERVER_H
