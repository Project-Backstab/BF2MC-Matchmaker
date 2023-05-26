#ifndef BATTLEFIELD_PLAYERSTATS_H
#define BATTLEFIELD_PLAYERSTATS_H

namespace Battlefield
{
	/*
		Player status information
	*/
	class PlayerStats
	{
		private:
			uint32_t score     = 0; // Total score
			uint32_t ran       = 0; // Rank
			uint32_t pph       = 0; // Point per Hour
			uint32_t kills     = 0; // Total kills
			uint32_t suicides  = 0; // Total suicides
			uint32_t time      = 0; // Total time played in seconds
			uint32_t lavd      = 0; // Total LAV's destroyed
			uint32_t mavd      = 0; // Total MAV's destroyed
			uint32_t havd      = 0; // Total HAV's destroyed
			uint32_t hed       = 0; // Total Helicopters destroyed
			uint32_t pld       = 0; // ?? Some other vehicle ??                    (Not used in game)
			uint32_t bod       = 0; // Total Boats destoyed
			uint32_t k1        = 0; // Total kills assualt kit
			uint32_t s1        = 0; // ??                                          (Not used in game)
			uint32_t k2        = 0; // Total kills sniper kit 
			uint32_t s2        = 0; // ??                                          (Not used in game)
			uint32_t k3        = 0; // Total kills special Op. kit
			uint32_t s3        = 0; // ??                                          (Not used in game)
			uint32_t k4        = 0; // Total kills Combat engineer kit 
			uint32_t s4        = 0; // ??                                          (Not used in game)
			uint32_t k5        = 0; // Total kills Support kit  
			uint32_t s5        = 0; // ??                                          (Not used in game)
			uint32_t tk        = 0; //                                             (Not used in game)
			uint32_t medals    = 0; // Earned medals (See: enum PlayerStatsMedals)
			uint32_t ttb       = 0; // Total times been the top player in the game
			uint32_t mv        = 0; // Total mayor victories
			uint32_t ngp       = 0; // Total Parcipated game sessions
		
		public:
			void useExample();
			
			bool SetScore(uint32_t score);
			bool SetRank(uint32_t ran);
			bool SetPPH(uint32_t pph);
			bool SetKills(uint32_t kills);
			bool SetSuicides(uint32_t suicides);
			bool SetTime(uint32_t time);
			bool SetLAVsDestroyed(uint32_t lavd);
			bool SetMAVsDestroyed(uint32_t mavd);
			bool SetHAVsDestroyed(uint32_t havd);
			bool SetHelicoptersDestroyed(uint32_t hed);
			bool SetBoatsDestroyed(uint32_t bod);
			bool SetKillsAssualtKit(uint32_t kills);
			bool SetKillsSniperKit(uint32_t kills);
			bool SetKillsSpecialOpKit(uint32_t kills);
			bool SetKillsCombatEngineerKit(uint32_t kills);
			bool SetKillsSupportKit(uint32_t kills);
			bool SetMedals(uint32_t medals);
			bool SetTotalTopPlayer(uint32_t total);
			bool SetTotalVictories(uint32_t total);
			bool SetTotalGameSessions(uint32_t total);
			
			std::string ToString();
	};
}

#endif // BATTLEFIELD_PLAYERSTATS_H