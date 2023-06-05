#ifndef BATTLEFIELD_PLAYERSTATS_H
#define BATTLEFIELD_PLAYERSTATS_H

#include <cstdint>
#include <vector>

namespace Battlefield
{
	/*
		Player status information
	*/
	class PlayerStats
	{
		enum Ranks
		{
			Private                     = 1,
			Private_1st_Class           = 2,
			Corporal                    = 3,
			Sergeant                    = 4,
			Sergeant_1st_Class          = 5,
			Master_Sergeant             = 6,
			Sgt_Major                   = 7,
			Command_Sgt_Major           = 8,
			Warrant_Officer             = 9,
			Chief_Warrant_Officer       = 10,
			Second_Lieutenant           = 11,
			First_Lieutenant            = 12,
			Captian                     = 13,
			Major                       = 14,
			Lieutenant_Colonel          = 15,
			Colonel                     = 16,
			Brigadier_General           = 17,
			Major_General               = 18,
			Lieutenant_General          = 19,
			Five_Star_General           = 20,
		};
		
		enum Medals
		{
			Service_Cross               = 1 << 0,
			The_Bronse_Star             = 1 << 1,
			Air_Forse_Cross             = 1 << 2,
			The_Silver_star             = 1 << 3,
			The_Service_Cross_1st_Class = 1 << 4,
			The_Bronse_Star_1st_Class   = 1 << 5,
			Air_Force_Cross_1st_Class   = 1 << 6,
			Expert_Killing              = 1 << 7,
			Expert_Shooting             = 1 << 8,
			Expert_Demolition           = 1 << 9,
			Expert_Repair               = 1 << 10,
			Expert_Healer               = 1 << 11,
			Navy_Cross                  = 1 << 12,
			Legion_of_Merit             = 1 << 13,
			Legion_of_Merit_1st_Class   = 1 << 14,
			All_Medals                  = (1 << 15) - 1,
		};
		
		private:
			uint32_t  _score     = 0; // Total score
			uint32_t  _ran       = 0; // Rank
			uint32_t  _pph       = 0; // Point per Hour
			uint32_t  _kills     = 0; // Total kills
			uint32_t  _deaths    = 0; // Total Deaths
			uint32_t  _suicides  = 0; // Total suicides
			uint32_t  _time      = 0; // Total time played in seconds
			uint32_t  _vehicles  = 0; // Total Vehicle's destroyed
			uint32_t  _lavd      = 0; // Total LAV's destroyed, Light Armored Vehicle  (such as a Humvee or similar)
			uint32_t  _mavd      = 0; // Total MAV's destroyed, Medium Armored Vehicle (such as a Tank or similar)
			uint32_t  _havd      = 0; // Total HAV's destroyed, Heavy Armored Vehicle  (such as an APC or similar)
			uint32_t  _hed       = 0; // Total Helicopters destroyed
			uint32_t  _pld       = 0; // Total Planes destroyed                        (unimplemented vehicle)
			uint32_t  _bod       = 0; // Total Boats destoyed
			uint32_t  _k1        = 0; // Total kills assualt kit
			uint32_t  _s1        = 0; // Total Deaths assualt kit
			uint32_t  _k2        = 0; // Total kills sniper kit 
			uint32_t  _s2        = 0; // Total Deaths sniper kit 
			uint32_t  _k3        = 0; // Total kills special Op. kit
			uint32_t  _s3        = 0; // Total Deaths special Op. kit
			uint32_t  _k4        = 0; // Total kills Combat engineer kit 
			uint32_t  _s4        = 0; // Total Deaths Combat engineer kit
			uint32_t  _k5        = 0; // Total kills Support kit  
			uint32_t  _s5        = 0; // Total Deaths Support kit
			uint32_t  _tk        = 0; // Team kills                                   (Not used in game)
			uint32_t  _medals    = 0; // Earned medals (See: enum PlayerStatsMedals)
			uint32_t  _ttb       = 0; // Total times been the top player in the game
			uint32_t  _mv        = 0; // Total mayor victories
			uint32_t  _ngp       = 0; // Total Parcipated game sessions
		
		public:
			void useExample();
			
			int GetScore() const                   { return this->_score;    }
			int GetRank() const                    { return this->_ran;      }
			int GetPPH() const                     { return this->_pph;      }
			int GetKills() const                   { return this->_kills;    }
			int GetDeaths() const                  { return this->_deaths;   }
			int GetSuicides() const                { return this->_suicides; }
			int GetTime() const                    { return this->_time;     }
			int GetVehiclesDestroyed() const       { return this->_vehicles; }
			int GetLAVsDestroyed() const           { return this->_lavd;     }
			int GetMAVsDestroyed() const           { return this->_mavd;     } 
			int GetHAVsDestroyed() const           { return this->_havd;     }
			int GetHelicoptersDestroyed() const    { return this->_hed;      }
			int GetBoatsDestroyed() const          { return this->_bod;      }
			int GetKillsAssualtKit() const         { return this->_k1;       }
			int GetDeathsAssualtKit() const        { return this->_s1;       }
			int GetKillsSniperKit() const          { return this->_k2;       }
			int GetDeathsSniperKit() const         { return this->_s2;       }
			int GetKillsSpecialOpKit() const       { return this->_k3;       }
			int GetDeathsSpecialOpKit() const      { return this->_s3;       }
			int GetKillsCombatEngineerKit() const  { return this->_k4;       }
			int GetDeathsCombatEngineerKit() const { return this->_s4;       }
			int GetKillsSupportKit() const         { return this->_k5;       }
			int GetDeathsSupportKit() const        { return this->_s5;       }
			int GetMedals() const                  { return this->_medals;   }
			int GetTotalTopPlayer() const          { return this->_ttb;      }
			int GetTotalVictories() const          { return this->_mv;       }
			int GetTotalGameSessions() const       { return this->_ngp;      }
			
			double GetRatio() const                  { return static_cast<double>(this->_kills * 100) / this->_deaths; }
			double GetRatioAssualtKit() const        { return static_cast<double>(this->_k1 * 100) / this->_s1; }
			double GetRatioSniperKit() const         { return static_cast<double>(this->_k2 * 100) / this->_s2; }
			double GetRatioSpecialOpKit() const      { return static_cast<double>(this->_k3 * 100) / this->_s3; }
			double GetRatioCombatEngineerKit() const { return static_cast<double>(this->_k4 * 100) / this->_s4; }
			double GetRatioSupportKit() const        { return static_cast<double>(this->_k5 * 100) / this->_s5; }
			
			bool SetScore(uint32_t score);
			bool SetRank(uint32_t ran);
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
			bool SetTotalTopPlayer(uint32_t total);
			bool SetTotalVictories(uint32_t total);
			bool SetTotalGameSessions(uint32_t total);
			
			std::vector<int> GetStatsVector();
	};
}

#endif // BATTLEFIELD_PLAYERSTATS_H