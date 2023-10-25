#ifndef BATTLEFIELD_PLAYERSTATS_H
#define BATTLEFIELD_PLAYERSTATS_H

#include <cstdint>
#include <vector>
#include <battlefield.h>

namespace Battlefield
{
	/*
		Player status information
	*/
	class PlayerStats
	{
		private:
			int32_t   _score     = 0; // Total score
			uint32_t  _ran       = 1; // Rank                                          (See: enum Battlefield::Ranks)
			uint32_t  _pph       = 0; // Points per Hour
			uint32_t  _kills     = 0; // Total Kills
			uint32_t  _deaths    = 0; // Total Deaths
			uint32_t  _suicides  = 0; // Total Suicides
			uint32_t  _time      = 0; // Total time played in seconds
			uint32_t  _vehicles  = 0; // Total Vehicles destroyed
			uint32_t  _lavd      = 0; // Total LAV's destroyed, Light Armored Vehicle  (such as a Humvee or similar)
			uint32_t  _mavd      = 0; // Total MAV's destroyed, Medium Armored Vehicle (such as a Tank or similar)
			uint32_t  _havd      = 0; // Total HAV's destroyed, Heavy Armored Vehicle  (such as an APC or similar)
			uint32_t  _hed       = 0; // Total Helicopters destroyed
			uint32_t  _pld       = 0; // Total Planes destroyed                        (unimplemented vehicle)
			uint32_t  _bod       = 0; // Total Boats destoyed
			uint32_t  _k1        = 0; // Total kills Assualt kit
			uint32_t  _s1        = 0; // Total spawns Assualt kit
			uint32_t  _k2        = 0; // Total kills Sniper kit 
			uint32_t  _s2        = 0; // Total spawns Sniper kit 
			uint32_t  _k3        = 0; // Total kills Special Op. kit
			uint32_t  _s3        = 0; // Total spawns Special Op. kit
			uint32_t  _k4        = 0; // Total kills Combat Engineer kit 
			uint32_t  _s4        = 0; // Total spawns Combat Engineer kit
			uint32_t  _k5        = 0; // Total kills Support kit  
			uint32_t  _s5        = 0; // Total spawns Support kit
			uint32_t  _tk        = 0; // Team kills                                    (Not used in game)
			uint32_t  _medals    = 0; // Earned medals                                 (See: enum Battlefield::Medals)
			uint32_t  _ttb       = 0; // Total times top player
			uint32_t  _mv        = 0; // Total major victories
			uint32_t  _ngp       = 0; // Total participated game sessions
		
		public:
			void useExample();
			
			int32_t  GetScore() const                   { return this->_score;    }
			uint32_t GetRank() const                    { return this->_ran;      }
			uint32_t GetPPH() const                     { return this->_pph;      }
			uint32_t GetKills() const                   { return this->_kills;    }
			uint32_t GetDeaths() const                  { return this->_deaths;   }
			uint32_t GetSuicides() const                { return this->_suicides; }
			uint32_t GetTime() const                    { return this->_time;     }
			uint32_t GetVehiclesDestroyed() const       { return this->_vehicles; }
			uint32_t GetLAVsDestroyed() const           { return this->_lavd;     }
			uint32_t GetMAVsDestroyed() const           { return this->_mavd;     } 
			uint32_t GetHAVsDestroyed() const           { return this->_havd;     }
			uint32_t GetHelicoptersDestroyed() const    { return this->_hed;      }
			uint32_t GetPlanesDestroyed() const         { return this->_pld;      }
			uint32_t GetBoatsDestroyed() const          { return this->_bod;      }
			uint32_t GetKillsAssualtKit() const         { return this->_k1;       }
			uint32_t GetSpawnsAssualtKit() const        { return this->_s1;       }
			uint32_t GetKillsSniperKit() const          { return this->_k2;       }
			uint32_t GetSpawnsSniperKit() const         { return this->_s2;       }
			uint32_t GetKillsSpecialOpKit() const       { return this->_k3;       }
			uint32_t GetSpawnsSpecialOpKit() const      { return this->_s3;       }
			uint32_t GetKillsCombatEngineerKit() const  { return this->_k4;       }
			uint32_t GetSpawnsCombatEngineerKit() const { return this->_s4;       }
			uint32_t GetKillsSupportKit() const         { return this->_k5;       }
			uint32_t GetSpawnsSupportKit() const        { return this->_s5;       }
			uint32_t GetTeamKills() const               { return this->_tk;       }
			uint32_t GetMedals() const                  { return this->_medals;   }
			uint32_t GetTotalTopPlayer() const          { return this->_ttb;      }
			uint32_t GetTotalVictories() const          { return this->_mv;       }
			uint32_t GetTotalGameSessions() const       { return this->_ngp;      }
			
			double GetRatio() const;
			double GetRatioAssualtKit() const;
			double GetRatioSniperKit() const;
			double GetRatioSpecialOpKit() const;
			double GetRatioCombatEngineerKit() const;
			double GetRatioSupportKit() const;
			
			bool SetScore(int32_t score);
			bool SetRank(uint32_t ran);
			bool SetRank(Ranks ran);
			bool SetPPH(uint32_t pph);
			bool SetKills(uint32_t kills);
			bool SetDeaths(uint32_t kills);
			bool SetSuicides(uint32_t suicides);
			bool SetTime(uint32_t time);
			bool SetVehiclesDestroyed(uint32_t vehicles);
			bool SetLAVsDestroyed(uint32_t lavd);
			bool SetMAVsDestroyed(uint32_t mavd);
			bool SetHAVsDestroyed(uint32_t havd);
			bool SetHelicoptersDestroyed(uint32_t hed);
			bool SetBoatsDestroyed(uint32_t bod);
			bool SetKillsAssualtKit(uint32_t kills);
			bool SetDeathsAssualtKit(uint32_t deaths);
			bool SetKillsSniperKit(uint32_t kills);
			bool SetDeathsSniperKit(uint32_t deaths);
			bool SetKillsSpecialOpKit(uint32_t kills);
			bool SetDeathsSpecialOpKit(uint32_t deaths);
			bool SetKillsCombatEngineerKit(uint32_t kills);
			bool SetDeathsCombatEngineerKit(uint32_t deaths);
			bool SetKillsSupportKit(uint32_t kills);
			bool SetDeathsSupportKit(uint32_t deaths);
			bool SetMedals(uint32_t medals);
			bool SetMedals(Medals medals);
			bool SetTotalTopPlayer(uint32_t total);
			bool SetTotalVictories(uint32_t total);
			bool SetTotalGameSessions(uint32_t total);
		
		private:
			void _calcRank();
	};
}

#endif // BATTLEFIELD_PLAYERSTATS_H
