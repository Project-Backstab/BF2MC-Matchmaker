#ifndef BATTLEFIELD_GAMESTAT_H
#define BATTLEFIELD_GAMESTAT_H

#include <cstdint>
#include <vector>
#include <string>

/*
	Forward declarations
*/
struct MYSQL_TIME;

namespace Battlefield
{
	/*
		Forward declarations
	*/
	class GameStat;
	class GameStatPlayer;
	
	/*
		Type definitions
	*/
	typedef std::vector<GameStat> GameStats;
	typedef std::vector<GameStatPlayer> GameStatPlayers;

	/*
		Game stat information
	*/
	class GameStat
	{
		public:
			enum class VictoryState
			{
				Unknown   = -1,
				Lost      = 0,
				Major     = 1,
				Minor     = 2,
				Draw      = 3
			};
			
		private:
			int             _id         = -1;
			uint8_t         _gametype   = 0;  // Game type (see: enum Battlefield::GameType)
			std::string     _gamver     = "";
			std::string     _hostname   = "";
			uint8_t         _mapid      = 0;
			uint8_t         _numplayers = 0;  // Total players in the whole game
			uint8_t         _pplayers   = 0;  // present players at the end of the game 
			uint16_t        _tplayed    = 0;  // Time played in seconds
			int             _clanid_t0  = 0;
			int             _clanid_t1  = 0;
			uint8_t         _country_t0 = 0;
			uint8_t         _country_t1 = 0;
			
			// Victory state for each team (see: enum Battlefield::GameStat::VictoryState)
			uint8_t         _victory_t0 = 0;
			uint8_t         _victory_t1 = 0;

			std::string     _created_at = "";
			GameStatPlayers _players;
			
		public:
			int         GetId() const            { return this->_id;         }
			uint8_t     GetGameType() const      { return this->_gametype;   }
			std::string GetGameVersion() const   { return this->_gamver;     }
			std::string GetHostName() const      { return this->_hostname;   }
			uint8_t     GetMapId() const         { return this->_mapid;      }
			uint8_t     GetNumPlayers() const    { return this->_numplayers; }
			uint8_t     GetPPlayers() const      { return this->_pplayers;   }
			uint16_t    GetTimePlayed() const    { return this->_tplayed;    }
			
			// Team
			int         GetTeam1ClanId() const   { return this->_clanid_t0;  }
			int         GetTeam2ClanId() const   { return this->_clanid_t1;  }
			uint8_t     GetTeam1Country() const  { return this->_country_t0; }
			uint8_t     GetTeam2Country() const  { return this->_country_t1; }
			uint8_t     GetTeam1Victory() const  { return this->_victory_t0; }
			uint8_t     GetTeam2Victory() const  { return this->_victory_t1; }
			
			std::string     GetCreatedAt() const { return this->_created_at; }
			GameStatPlayers GetPlayers() const   { return this->_players;    }
			
			bool SetId(int id);
			bool SetId(const std::string str_id);
			bool SetGameType(uint8_t gametype);
			bool SetGameType(const std::string str_gametype);
			bool SetGameVersion(const std::string gamver);
			bool SetHostName(const std::string hostname);
			bool SetMapId(uint8_t mapid);
			bool SetMapId(const std::string str_mapid);
			bool SetNumPlayers(uint8_t numplayers);
			bool SetNumPlayers(const std::string str_numplayers);
			bool SetPPlayers(uint8_t pplayers);
			bool SetPPlayers(const std::string str_pplayers);
			bool SetTimePlayed(uint16_t tplayed);
			bool SetTimePlayed(const std::string str_tplayed);
			
			// Team
			bool SetTeam1ClanId(int clanid_t0);
			bool SetTeam1ClanId(const std::string str_clanid_t0);
			bool SetTeam2ClanId(int clanid_t1);
			bool SetTeam2ClanId(const std::string str_clanid_t1);
			bool SetTeam1Country(uint8_t country_t0);
			bool SetTeam1Country(const std::string str_country_t0);
			bool SetTeam2Country(uint8_t country_t1);
			bool SetTeam2Country(const std::string str_country_t1);
			bool SetTeam1Victory(uint8_t victory_t0);
			bool SetTeam1Victory(const std::string str_victory_t0);
			bool SetTeam2Victory(uint8_t victory_t1);
			bool SetTeam2Victory(const std::string str_victory_t1);
			
			bool SetCreatedAt(MYSQL_TIME created_at);
			void AddPlayer(const GameStatPlayer& gsplayer);
			
			void UpdateClanStats();
			void Debug();
	};
	
	/*
		Game stat player information
	*/
	class GameStatPlayer
	{
		private:
			int         _id         = -1;
			
			// Player reference
			std::string _auth     = "";
			int         _pid      = -1;
			int         _team     = -1;
			
			// Player stats
			int32_t     _score    = 0;
			uint32_t    _rank     = 1;
			uint32_t    _pph      = 0;
			uint32_t    _kills    = 0;
			uint32_t    _deaths   = 0;
			uint32_t    _suicides = 0;
			uint32_t    _time     = 0;
			uint32_t    _lavd     = 0;
			uint32_t    _mavd     = 0;
			uint32_t    _havd     = 0;
			uint32_t    _hed      = 0;
			uint32_t    _pld      = 0;
			uint32_t    _bod      = 0;
			uint32_t    _k1       = 0;
			uint32_t    _s1       = 0;
			uint32_t    _k2       = 0;
			uint32_t    _s2       = 0;
			uint32_t    _k3       = 0;
			uint32_t    _s3       = 0;
			uint32_t    _k4       = 0;
			uint32_t    _s4       = 0;
			uint32_t    _k5       = 0;
			uint32_t    _s5       = 0;
			uint32_t    _tk       = 0;
			uint32_t    _medals   = 0;
			uint32_t    _ttb      = 0;
			uint32_t    _mv       = 0;
			uint32_t    _ngp      = 0;
		
		public:
			int         GetId() const                      { return this->_id;       }
			std::string GetAuth() const                    { return this->_auth;     }
			int         GetProfileId() const               { return this->_pid;      }
			int         GetTeam() const                    { return this->_team;     }
			int32_t     GetScore() const                   { return this->_score;    }
			uint32_t    GetRank() const                    { return this->_rank;     }
			uint32_t    GetPPH() const                     { return this->_pph;      }
			uint32_t    GetKills() const                   { return this->_kills;    }
			uint32_t    GetDeaths() const                  { return this->_deaths;   }
			uint32_t    GetSuicides() const                { return this->_suicides; }
			uint32_t    GetTime() const                    { return this->_time;     }
			uint32_t    GetLAVsDestroyed() const           { return this->_lavd;     }
			uint32_t    GetMAVsDestroyed() const           { return this->_mavd;     }
			uint32_t    GetHAVsDestroyed() const           { return this->_havd;     }
			uint32_t    GetHelicoptersDestroyed() const    { return this->_hed;      }
			uint32_t    GetPlanesDestroyed() const         { return this->_pld;      }
			uint32_t    GetBoatsDestroyed() const          { return this->_bod;      }
			uint32_t    GetKillsAssualtKit() const         { return this->_k1;       }
			uint32_t    GetSpawnsAssualtKit() const        { return this->_s1;       }
			uint32_t    GetKillsSniperKit() const          { return this->_k2;       }
			uint32_t    GetSpawnsSniperKit() const         { return this->_s2;       }
			uint32_t    GetKillsSpecialOpKit() const       { return this->_k3;       }
			uint32_t    GetSpawnsSpecialOpKit() const      { return this->_s3;       }
			uint32_t    GetKillsCombatEngineerKit() const  { return this->_k4;       }
			uint32_t    GetSpawnsCombatEngineerKit() const { return this->_s4;       }
			uint32_t    GetKillsSupportKit() const         { return this->_k5;       }
			uint32_t    GetSpawnsSupportKit() const        { return this->_s5;       }
			uint32_t    GetTeamKills() const               { return this->_tk;       }
			uint32_t    GetMedals() const                  { return this->_medals;   }
			uint32_t    GetTotalTopPlayer() const          { return this->_ttb;      }
			uint32_t    GetTotalVictories() const          { return this->_mv;       }
			uint32_t    GetTotalGameSessions() const       { return this->_ngp;      }
			
			bool SetId(int id);
			bool SetAuth(const std::string& auth);
			bool SetProfileId(int pid);
			bool SetProfileId(const std::string& str_pid);
			bool SetTeam(int team);
			bool SetTeam(const std::string& str_team);
			bool SetScore(int32_t score);
			bool SetScore(const std::string& str_score);
			bool SetRank(uint32_t rank);
			bool SetRank(const std::string& str_rank);
			bool SetPPH(uint32_t pph);
			bool SetPPH(const std::string& str_pph);
			bool SetKills(uint32_t kills);
			bool SetKills(const std::string& str_kills);
			bool SetDeaths(uint32_t deaths);
			bool SetDeaths(const std::string& str_deaths);
			bool SetSuicides(uint32_t suicides);
			bool SetSuicides(const std::string& str_suicides);
			bool SetTime(uint32_t time);
			bool SetTime(const std::string& str_time);
			bool SetLAVsDestroyed(uint32_t lavd);
			bool SetLAVsDestroyed(const std::string& str_lavd);
			bool SetMAVsDestroyed(uint32_t mavd);
			bool SetMAVsDestroyed(const std::string& str_mavd);
			bool SetHAVsDestroyed(uint32_t havd);
			bool SetHAVsDestroyed(const std::string& str_havd);
			bool SetHelicoptersDestroyed(uint32_t hed);
			bool SetHelicoptersDestroyed(const std::string& str_hed);
			bool SetPlanesDestroyed(uint32_t pld);
			bool SetPlanesDestroyed(const std::string& str_pld);
			bool SetBoatsDestroyed(uint32_t bod);
			bool SetBoatsDestroyed(const std::string& str_bod);
			bool SetKillsAssualtKit(uint32_t kills);
			bool SetKillsAssualtKit(const std::string& str_kills);
			bool SetDeathsAssualtKit(uint32_t deaths);
			bool SetDeathsAssualtKit(const std::string& str_deaths);
			bool SetKillsSniperKit(uint32_t kills);
			bool SetKillsSniperKit(const std::string& str_kills);
			bool SetDeathsSniperKit(uint32_t deaths);
			bool SetDeathsSniperKit(const std::string& str_deaths);
			bool SetKillsSpecialOpKit(uint32_t kills);
			bool SetKillsSpecialOpKit(const std::string& str_kills);
			bool SetDeathsSpecialOpKit(uint32_t deaths);
			bool SetDeathsSpecialOpKit(const std::string& str_deaths);
			bool SetKillsCombatEngineerKit(uint32_t kills);
			bool SetKillsCombatEngineerKit(const std::string& str_kills);
			bool SetDeathsCombatEngineerKit(uint32_t deaths);
			bool SetDeathsCombatEngineerKit(const std::string& str_deaths);
			bool SetKillsSupportKit(uint32_t kills);
			bool SetKillsSupportKit(const std::string& str_kills);
			bool SetDeathsSupportKit(uint32_t deaths);
			bool SetDeathsSupportKit(const std::string& str_deaths);
			bool SetTeamKills(uint32_t tk);
			bool SetTeamKills(const std::string& str_tk);
			bool SetMedals(uint32_t medals);
			bool SetMedals(const std::string& str_medals);
			bool SetTotalTopPlayer(uint32_t ttb);
			bool SetTotalTopPlayer(const std::string& str_ttb);
			bool SetTotalVictories(uint32_t mv);
			bool SetTotalVictories(const std::string& str_mv);
			bool SetTotalGameSessions(uint32_t ngp);
			bool SetTotalGameSessions(const std::string& str_ngp);
			
			void UpdatePlayerStats();
			void Debug();
	};
}

#endif // BATTLEFIELD_GAMESTAT_H
