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
	class GameServerPlayer;
	
	/*
		Type definitions
	*/
	typedef std::vector<GameServer> GameServers;
	typedef std::vector<GameServerPlayer> GameServerPlayers;
	
	/*
		Game server information
	*/
	class GameServer
	{
		public:
			enum Regions
			{
				America        = 1,
				Asia           = 2048,
				Europe         = 65536,
				Unknown_Region = -1,
			};
	
		private:
			int			_id            = -1;
			std::string _ip            = "";
			uint16_t    _port          = 3658;
			uint8_t     _flag          = 59;
			
			// See reference:
			//   https://documentation.help/GameSpy-SDK/Gamespy%20Query%20and%20Reporting%202%20SDK.html
			//
			std::string _localip0      = "";   // Local ip
			uint16_t    _localport     = 3658; // Local port
			uint8_t     _natneg        = 0;    // Enable client-to-client connections (1: true, 0: false)                                       ??? Unsure ???
			std::string _gamename      = "";   // Game name
			std::string _hostname      = "";   // Host name
			uint16_t    _hostport      = 3658; // Host port
			
			std::string _gamever       = "V1.31a"; // Game version
			std::string _cl            = "";   // Client version
			std::string _rv            = "";   // ??? (retail)                              ??? Unsure ???
			
			std::string _map           = "";   // Map name
			uint8_t     _mc            = 0;    // Map cycling (1: levelmap with mapname)
			                                   //             (2: All Maps)
			                                   //             (3: Assualt Maps)
			                                   //             (4: Incursion Maps)
			                                   //             (5: Domination Maps)
			uint8_t     _mapname       = 0;    // Map id
			
			uint8_t     _gc            = 0;    // Game type                                 ??? Unsure ???
			std::string _gametype      = "";   // String which specifies the type of game, or the mod being played.
			                                   // (conquest, capturetheflag)
			
			std::string _gamevariant   = "";    // Game code name
			uint8_t     _numplayers    = 0;    // Number of players
			uint8_t     _maxplayers    = 0;    // Max players
			uint8_t     _numteams      = 0;    // Number of teams
			std::string _gamemode      = "";   // Game status (openwaiting,
			                                   //              closedwaiting,
			                                   //              closedplaying,
			                                   //              openplaying,
			                                   //              openstaging,
			                                   //              closedstaging,
			                                   //              exiting)
			uint8_t     _teamplay      = 0;    // number which defines the type of teamplay in use, or 0 for no teamplay.
			                                   // Values > 0 are up to the developer
			uint8_t     _fraglimit     = 0;    // Number of total kills or points before a level change or game restart
			uint8_t     _teamfraglimit = 0;    // Number of total kills or points for a team before a level change or
			                                   // game restart.
			uint16_t    _timelimit     = 0;    // Amount of total time before a level change or game restart occurs in seconds
			uint16_t    _timeelapsed   = 0;    // Amount of time (in seconds) since the current level or game started
			uint8_t     _password      = 0;    // Has password requirement to join (1: true, 0: false)
			uint8_t     _nr            = 0;    // Minimum Rank number filter
			uint8_t     _xr            = 0;    // Maximum Rank number filter
			uint8_t     _ff            = 0;    // Friendly fire (1: true, 0: false)
			uint8_t     _sr            = 0;    // Stats tracking (1: true, 0: false)
			uint8_t     _rc            = 0;    // Server is reconfigurable (1: true, 0: false)
			int64_t     _ni            = 0;    // Minimum IP range
			int64_t     _xi            = 0;    // Maximum IP range
			uint8_t     _qm            = 0;    // 0 = Resurved/User for clan matches
			                                   // 5 = Unclaimed server free of use. Should be visable for everyone.
			uint64_t    _region        = 1;    // Server region (America: 1, Asia: 2048, Europe: 65536)
			
			// Clan
			int8_t      _c0            = -1;   // Clanid team 1
			int8_t      _c1            = -1;   // Clanid team 2
			std::string _n0            = "";   // Clan name team 1
			std::string _n1            = "";   // Clan name team 2
			uint8_t     _c0c           = 0;    // Team 1 is claimed (1: true, 0: false)
			uint8_t     _c1c           = 0;    // Team 2 is claimed (1: true, 0: false)
			
			// Team
			std::string _team0         = "";   // Team 1 name
			std::string _team1         = "";   // Team 2 name
			int16_t     _score0        = 0;    // Team 1 score
			int16_t     _score1        = 0;    // Team 2 score
			bool        _verified      = false; // Database check that a server is verified
			
			std::vector<GameServerPlayer> _players;
		
		public:
			void useExample();
			
			int            GetId() const             { return this->_id;            }
			std::string    GetIp() const             { return this->_ip;            }
			void GetIpArray(uint8_t* ip) const;
			uint16_t       GetPort() const           { return this->_port;          }
			uint8_t        GetFlag() const           { return this->_flag;          }
			std::string    GetLocalIp() const        { return this->_localip0;      }
			uint16_t       GetLocalPort() const      { return this->_localport;     }
			uint8_t        GetNatNeg() const         { return this->_natneg;        }
			std::string    GetGameName() const       { return this->_gamename;      }
			std::string    GetHostName() const       { return this->_hostname;      }
			uint16_t       GetHostPort() const       { return this->_hostport;      }
			std::string    GetGameVersion() const    { return this->_gamever;       }
			std::string    GetClientVersion() const  { return this->_cl;            }
			std::string    GetRV() const             { return this->_rv;            }
			std::string    GetMap() const            { return this->_map;           }
			uint8_t        GetMapCycling() const     { return this->_mc;            }
			uint8_t        GetMapName() const        { return this->_mapname;       }
			uint8_t        GetGC() const             { return this->_gc;            }
			std::string    GetGameType() const       { return this->_gametype;      }
			std::string    GetGameVariant() const    { return this->_gamevariant;   }
			uint8_t        GetNumPlayers() const     { return this->_numplayers;    }
			uint8_t        GetMaxPlayers() const     { return this->_maxplayers;    }
			uint8_t        GetNumTeams() const       { return this->_numteams;      }
			std::string    GetGameMode() const       { return this->_gamemode;      }
			uint8_t        GetTeamplay() const       { return this->_teamplay;      }
			uint8_t        GetFlagLimit() const      { return this->_fraglimit;     }
			uint8_t        GetTeamFragLimit() const  { return this->_teamfraglimit; }
			uint16_t       GetTimeLimit() const      { return this->_timelimit;     }
			uint16_t       GetTimeElapsed() const    { return this->_timeelapsed;   }
			uint8_t        GetPassword() const       { return this->_password;      }
			uint8_t        GetMinRank() const        { return this->_nr;            }
			uint8_t        GetMaxRank() const        { return this->_xr;            }
			uint8_t        GetFriendlyFire() const   { return this->_ff;            }
			uint8_t        GetStatsTracking() const  { return this->_sr;            }
			uint8_t        GetReconfigurable() const { return this->_rc;            }
			int64_t        GetMinIpRange() const     { return this->_ni;            }
			int64_t        GetMaxIpRange() const     { return this->_xi;            }
			uint8_t        GetQM() const             { return this->_qm;            }
			uint64_t       GetRegion() const         { return this->_region;        }
			// Clan
			int32_t        GetClan1Id() const        { return this->_c0;            }
			int32_t        GetClan2Id() const        { return this->_c1;            }
			std::string    GetClan1Name() const      { return this->_n0;            }
			std::string    GetClan2Name() const      { return this->_n1;            }
			uint8_t        GetClan1Claimed() const   { return this->_c0c;           }
			uint8_t        GetClan2Claimed() const   { return this->_c1c;           }
			// Team
			std::string       GetTeam1Name() const   { return this->_team0;         }
			std::string       GetTeam2Name() const   { return this->_team1;         }
			int16_t           GetTeam1Score() const  { return this->_score0;        }
			int16_t           GetTeam2Score() const  { return this->_score1;        }
			bool              isVerified() const     { return this->_verified;      }
			
			GameServerPlayers GetPlayers() const     { return this->_players;       }
			
			bool SetId(int id);
			bool SetIp(const std::string& ip);
			bool SetPort(uint16_t port);
			bool SetFlag(uint8_t flag);
			bool SetLocalIp(const std::string& localip);
			bool SetLocalPort(uint16_t localport);
			bool SetLocalPort(const std::string& str_localport);
			bool SetNatNeg(uint8_t natneg);
			bool SetNatNeg(const std::string& str_natneg);
			bool SetGameName(const std::string& gamename);
			bool SetHostName(const std::string& hostname);
			bool SetHostPort(uint16_t hostport);
			bool SetHostPort(const std::string& str_hostport);
			bool SetGameVersion(const std::string& gamever);
			bool SetClientVersion(const std::string& clientver);
			bool SetRV(const std::string& rv);
			bool SetMap(const std::string& map);
			bool SetMapCycling(uint8_t mc);
			bool SetMapCycling(const std::string& str_mc);
			bool SetMapName(uint8_t mapname);
			bool SetMapName(const std::string& str_mapname);
			bool SetGC(uint8_t gc);
			bool SetGC(const std::string& str_gc);
			bool SetGameType(const std::string& gametype);
			bool SetGameVariant(const std::string& gamevariant);
			bool SetNumPlayers(uint8_t numplayers);
			bool SetNumPlayers(const std::string& str_numplayers);
			bool SetMaxPlayers(uint8_t maxplayers);
			bool SetMaxPlayers(const std::string& str_maxplayers);
			bool SetNumTeams(uint8_t numteams);
			bool SetNumTeams(const std::string& str_numteams);
			bool SetGameMode(const std::string& gamemode);
			bool SetTeamplay(uint8_t teamplay);
			bool SetTeamplay(const std::string& str_teamplay);
			bool SetFlagLimit(uint8_t fraglimit);
			bool SetFlagLimit(const std::string& str_fraglimit);
			bool SetTeamFragLimit(uint8_t teamfraglimit);
			bool SetTeamFragLimit(const std::string& str_teamfraglimit);
			bool SetTimeLimit(uint16_t timelimit);
			bool SetTimeLimit(const std::string& str_timelimit);
			bool SetTimeElapsed(uint16_t timeelapsed);
			bool SetTimeElapsed(const std::string& str_timeelapsed);
			bool SetPassword(uint8_t password);
			bool SetPassword(const std::string& str_password);
			bool SetMinRank(uint8_t nr);
			bool SetMinRank(const std::string& str_nr);
			bool SetMaxRank(uint8_t xr);
			bool SetMaxRank(const std::string& str_xr);
			bool SetFriendlyFire(uint8_t ff);
			bool SetFriendlyFire(const std::string& str_ff);
			bool SetStatsTracking(uint8_t sr);
			bool SetStatsTracking(const std::string& str_sr);
			bool SetReconfigurable(uint8_t rc);
			bool SetReconfigurable(const std::string& str_rc);
			bool SetMinIpRange(int64_t ni);
			bool SetMinIpRange(const std::string& str_ni);
			bool SetMaxIpRange(int64_t xi);
			bool SetMaxIpRange(const std::string& str_xi);
			bool SetQM(uint8_t qm);
			bool SetQM(const std::string& str_qm);
			bool SetRegion(uint64_t region);
			bool SetRegion(const std::string& str_region);
			
			// Clan
			bool SetClan1Id(int c0);
			bool SetClan1Id(const std::string& str_c0);
			bool SetClan2Id(int c1);
			bool SetClan2Id(const std::string& str_c1);
			bool SetClan1Name(const std::string& n0);
			bool SetClan2Name(const std::string& n1);
			bool SetClan1Claimed(uint8_t c0c);
			bool SetClan1Claimed(const std::string& str_c0c);
			bool SetClan2Claimed(uint8_t c1c);
			bool SetClan2Claimed(const std::string& str_c1c);
			
			// Team
			bool SetTeam1Name(const std::string& team0);
			bool SetTeam2Name(const std::string& team1);
			bool SetTeam1Score(int16_t score0);
			bool SetTeam1Score(const std::string& str_score0);
			bool SetTeam2Score(int16_t score1);
			bool SetTeam2Score(const std::string& str_score1);
			
			bool SetVerified(bool verified);
			
			void AddPlayer(const GameServerPlayer& gsplayer);
			
			void Debug();
	};
	
	class GameServerPlayer
	{
		private:
			int         _id        = -1;
			std::string _name      = "";
			int16_t     _score     = 0;
			std::string _skill     = "";
			uint8_t     _ping      = 0;
			uint8_t     _team      = 0;
			uint16_t    _deaths    = 0;
			int         _profileid = -1;
		
		public:
			int         GetId() const        { return this->_id;        }
			std::string GetName() const      { return this->_name;      }
			int16_t     GetScore() const     { return this->_score;     }
			std::string GetSkill() const     { return this->_skill;     }
			uint8_t     GetPing() const      { return this->_ping;      }
			uint8_t     GetTeam() const      { return this->_team;      }
			uint16_t    GetDeaths() const    { return this->_deaths;    }
			int         GetProfileId() const { return this->_profileid; }
			
			bool SetId(int id);
			bool SetName(const std::string name);
			bool SetScore(int16_t score);
			bool SetScore(const std::string str_score);
			bool SetSkill(const std::string skill);
			bool SetPing(uint8_t ping);
			bool SetPing(const std::string str_ping);
			bool SetTeam(uint8_t team);
			bool SetTeam(const std::string str_team);
			bool SetDeaths(uint16_t deaths);
			bool SetDeaths(const std::string str_deaths);
			bool SetProfileId(int profileid);
			bool SetProfileId(const std::string str_profileid);
	};
}

#endif // BATTLEFIELD_GAMESERVER_H
