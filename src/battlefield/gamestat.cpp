#include <string>
#include <logger.h>
#include <globals.h>
#include <database.h>
#include <battlefield/player.h>
#include <battlefield/playerstats.h>

#include <battlefield/gamestat.h>

bool Battlefield::GameStat::SetId(int id)
{
	this->_id = id;
	return true;
}

bool Battlefield::GameStat::SetGameType(uint8_t gametype)
{
	this->_gametype = gametype;
	return true;
}

bool Battlefield::GameStat::SetGameType(const std::string str_gametype)
{
	try
	{
		uint8_t gametype = static_cast<uint8_t>(std::stoul(str_gametype));
		
		return this->SetGameType(gametype);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStat::SetGameVersion(const std::string gamver)
{
	this->_gamver = gamver;
	return true;
}

bool Battlefield::GameStat::SetHostName(const std::string hostname)
{
	this->_hostname = hostname;
	return true;
}

bool Battlefield::GameStat::SetMapId(uint8_t mapid)
{
	this->_mapid = mapid;
	return true;
}

bool Battlefield::GameStat::SetMapId(const std::string str_mapid)
{
	try
	{
		uint8_t mapid = static_cast<uint8_t>(std::stoul(str_mapid));
		
		return this->SetMapId(mapid);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStat::SetNumPlayers(uint8_t numplayers)
{
	this->_numplayers = numplayers;
	return true;
}

bool Battlefield::GameStat::SetNumPlayers(const std::string str_numplayers)
{
	try
	{
		uint8_t numplayers = static_cast<uint8_t>(std::stoul(str_numplayers));
		
		return this->SetNumPlayers(numplayers);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStat::SetPPlayers(uint8_t pplayers)
{
	this->_pplayers = pplayers;
	return true;
}

bool Battlefield::GameStat::SetPPlayers(const std::string str_pplayers)
{
	try
	{
		uint8_t pplayers = static_cast<uint8_t>(std::stoul(str_pplayers));
		
		return this->SetPPlayers(pplayers);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStat::SetTimePlayed(uint16_t tplayed)
{
	this->_tplayed = tplayed;
	return true;
}

bool Battlefield::GameStat::SetTimePlayed(const std::string str_tplayed)
{
	try
	{
		uint16_t tplayed = static_cast<uint16_t>(std::stoul(str_tplayed));
		
		return this->SetTimePlayed(tplayed);
	}
	catch(...) {};
	
	return false;
}

// Team
bool Battlefield::GameStat::SetTeam1ClanId(uint8_t clanid_t0)
{
	this->_clanid_t0 = clanid_t0;
	return true;
}

bool Battlefield::GameStat::SetTeam1ClanId(const std::string str_clanid_t0)
{
	try
	{
		uint8_t clanid_t0 = static_cast<uint8_t>(std::stoul(str_clanid_t0));
		
		return this->SetTeam1ClanId(clanid_t0);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStat::SetTeam2ClanId(uint8_t clanid_t1)
{
	this->_clanid_t1 = clanid_t1;
	return true;
}

bool Battlefield::GameStat::SetTeam2ClanId(const std::string str_clanid_t1)
{
	try
	{
		uint8_t clanid_t1 = static_cast<uint8_t>(std::stoul(str_clanid_t1));
		
		return this->SetTeam2ClanId(clanid_t1);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStat::SetTeam1Country(uint8_t country_t0)
{
	this->_country_t0 = country_t0;
	return true;
}

bool Battlefield::GameStat::SetTeam1Country(const std::string str_country_t0)
{
	try
	{
		uint8_t country_t0 = static_cast<uint8_t>(std::stoul(str_country_t0));
		
		return this->SetTeam1Country(country_t0);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStat::SetTeam2Country(uint8_t country_t1)
{
	this->_country_t1 = country_t1;
	return true;
}

bool Battlefield::GameStat::SetTeam2Country(const std::string str_country_t1)
{
	try
	{
		uint8_t country_t1 = static_cast<uint8_t>(std::stoul(str_country_t1));
		
		return this->SetTeam2Country(country_t1);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStat::SetTeam1Victory(uint8_t victory_t0)
{
	this->_victory_t0 = victory_t0;
	return true;
}

bool Battlefield::GameStat::SetTeam1Victory(const std::string str_victory_t0)
{
	try
	{
		uint8_t victory_t0 = static_cast<uint8_t>(std::stoul(str_victory_t0));
		
		return this->SetTeam1Victory(victory_t0);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStat::SetTeam2Victory(uint8_t victory_t1)
{
	this->_victory_t1 = victory_t1;
	return true;
}

bool Battlefield::GameStat::SetTeam2Victory(const std::string str_victory_t1)
{
	try
	{
		uint8_t victory_t1 = static_cast<uint8_t>(std::stoul(str_victory_t1));
		
		return this->SetTeam2Victory(victory_t1);
	}
	catch(...) {};
	
	return false;
}

void Battlefield::GameStat::AddPlayer(const GameStatPlayer& gsplayer)
{
	this->_players.push_back(gsplayer);
}

void Battlefield::GameStat::Debug()
{
	Logger::debug("id = "         + std::to_string(this->GetId()));
	Logger::debug("gametype = "   + std::to_string(this->GetGameType()));
	Logger::debug("gamver = "     + this->GetGameVersion());
	Logger::debug("hostname = "   + this->GetHostName());
	Logger::debug("mapid = "      + std::to_string(this->GetMapId()));
	Logger::debug("numplayers = " + std::to_string(this->GetNumPlayers()));
	Logger::debug("pplayers = "   + std::to_string(this->GetPPlayers()));
	Logger::debug("tplayed = "    + std::to_string(this->GetTimePlayed()));
	Logger::debug("clanid_t0 = "  + std::to_string(this->GetTeam1ClanId()));
	Logger::debug("clanid_t1 = "  + std::to_string(this->GetTeam2ClanId()));
	Logger::debug("country_t0 = " + std::to_string(this->GetTeam1Country()));
	Logger::debug("country_t1 = " + std::to_string(this->GetTeam2Country()));
	Logger::debug("victory_t0 = " + std::to_string(this->GetTeam1Victory()));
	Logger::debug("victory_t1 = " + std::to_string(this->GetTeam2Victory()));
}

// Class GameStatPlayer
bool Battlefield::GameStatPlayer::SetId(int id)
{
	this->_id = id;
	return true;
}

bool Battlefield::GameStatPlayer::SetAuth(const std::string& auth)
{
	this->_auth = auth;
	return true;
}

bool Battlefield::GameStatPlayer::SetProfileId(int pid)
{
	if(pid >= 0)
	{
		this->_pid = pid;
		return true;
	}
	
	return false;
}

bool Battlefield::GameStatPlayer::SetProfileId(const std::string& str_pid)
{
	try
	{
		int pid = std::stoi(str_pid);
		
		return this->SetProfileId(pid);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetScore(int32_t score)
{
	this->_score = score;
	return true;
}

bool Battlefield::GameStatPlayer::SetScore(const std::string& str_score)
{
	try
	{
		int32_t score = std::stoi(str_score);
		
		return this->SetScore(score);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetRank(uint32_t rank)
{
	if(rank >= 1 && rank <= 20)
	{
		this->_rank = rank;
		
		return true;
	}
	
	return false;
}

bool Battlefield::GameStatPlayer::SetRank(const std::string& str_rank)
{
	try
	{
		uint32_t rank = std::stoul(str_rank);
		
		return this->SetRank(rank);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetPPH(uint32_t pph)
{
	this->_pph = pph;
	return true;
}

bool Battlefield::GameStatPlayer::SetPPH(const std::string& str_pph)
{
	try
	{
		uint32_t pph = std::stoul(str_pph);
		
		return this->SetPPH(pph);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetKills(uint32_t kills)
{
	this->_kills = kills;
	return true;
}

bool Battlefield::GameStatPlayer::SetKills(const std::string& str_kills)
{
	try
	{
		uint32_t kills = std::stoul(str_kills);
		
		return this->SetKills(kills);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetSuicides(uint32_t suicides)
{
	this->_suicides = suicides;
	return true;
}

bool Battlefield::GameStatPlayer::SetSuicides(const std::string& str_suicides)
{
	try
	{
		uint32_t suicides = std::stoul(str_suicides);
		
		return this->SetSuicides(suicides);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetTime(uint32_t time)
{
	this->_time = time;
	return true;
}

bool Battlefield::GameStatPlayer::SetTime(const std::string& str_time)
{
	try
	{
		uint32_t time = std::stoul(str_time);
		
		return this->SetTime(time);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetLAVsDestroyed(uint32_t lavd)
{
	this->_lavd = lavd;
	return true;
}

bool Battlefield::GameStatPlayer::SetLAVsDestroyed(const std::string& str_lavd)
{
	try
	{
		uint32_t lavd = std::stoul(str_lavd);
		
		return this->SetLAVsDestroyed(lavd);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetMAVsDestroyed(uint32_t mavd)
{
	this->_mavd = mavd;
	return true;
}

bool Battlefield::GameStatPlayer::SetMAVsDestroyed(const std::string& str_mavd)
{
	try
	{
		uint32_t mavd = std::stoul(str_mavd);
		
		return this->SetMAVsDestroyed(mavd);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetHAVsDestroyed(uint32_t havd)
{
	this->_havd = havd;
	return true;
}

bool Battlefield::GameStatPlayer::SetHAVsDestroyed(const std::string& str_havd)
{
	try
	{
		uint32_t havd = std::stoul(str_havd);
		
		return this->SetHAVsDestroyed(havd);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetHelicoptersDestroyed(uint32_t hed)
{
	this->_hed = hed;
	return true;
}

bool Battlefield::GameStatPlayer::SetHelicoptersDestroyed(const std::string& str_hed)
{
	try
	{
		uint32_t hed = std::stoul(str_hed);
		
		return this->SetHelicoptersDestroyed(hed);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetPlanesDestroyed(uint32_t pld)
{
	this->_pld = pld;
	return true;
}

bool Battlefield::GameStatPlayer::SetPlanesDestroyed(const std::string& str_pld)
{
	try
	{
		uint32_t pld = std::stoul(str_pld);
		
		return this->SetPlanesDestroyed(pld);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetBoatsDestroyed(uint32_t bod)
{
	this->_bod = bod;
	return true;
}

bool Battlefield::GameStatPlayer::SetBoatsDestroyed(const std::string& str_bod)
{
	try
	{
		uint32_t bod = std::stoul(str_bod);
		
		return this->SetBoatsDestroyed(bod);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetKillsAssualtKit(uint32_t kills)
{
	this->_k1 = kills;
	return true;
}

bool Battlefield::GameStatPlayer::SetKillsAssualtKit(const std::string& str_kills)
{
	try
	{
		uint32_t kills = std::stoul(str_kills);
		
		return this->SetKillsAssualtKit(kills);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetDeathsAssualtKit(uint32_t deaths)
{
	this->_s1 = deaths;
	return true;
}

bool Battlefield::GameStatPlayer::SetDeathsAssualtKit(const std::string& str_deaths)
{
	try
	{
		uint32_t deaths = std::stoul(str_deaths);
		
		return this->SetDeathsAssualtKit(deaths);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetKillsSniperKit(uint32_t kills)
{
	this->_k2 = kills;
	return true;
}

bool Battlefield::GameStatPlayer::SetKillsSniperKit(const std::string& str_kills)
{
	try
	{
		uint32_t kills = std::stoul(str_kills);
		
		return this->SetKillsSniperKit(kills);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetDeathsSniperKit(uint32_t deaths)
{
	this->_s2 = deaths;
	return true;
}

bool Battlefield::GameStatPlayer::SetDeathsSniperKit(const std::string& str_deaths)
{
	try
	{
		uint32_t deaths = std::stoul(str_deaths);
		
		return this->SetDeathsSniperKit(deaths);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetKillsSpecialOpKit(uint32_t kills)
{
	this->_k3 = kills;
	return true;
}

bool Battlefield::GameStatPlayer::SetKillsSpecialOpKit(const std::string& str_kills)
{
	try
	{
		uint32_t kills = std::stoul(str_kills);
		
		return this->SetKillsSpecialOpKit(kills);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetDeathsSpecialOpKit(uint32_t deaths)
{
	this->_s3 = deaths;
	return true;
}

bool Battlefield::GameStatPlayer::SetDeathsSpecialOpKit(const std::string& str_deaths)
{
	try
	{
		uint32_t deaths = std::stoul(str_deaths);
		
		return this->SetDeathsSpecialOpKit(deaths);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetKillsCombatEngineerKit(uint32_t kills)
{
	this->_k4 = kills;
	return true;
}

bool Battlefield::GameStatPlayer::SetKillsCombatEngineerKit(const std::string& str_kills)
{
	try
	{
		uint32_t kills = std::stoul(str_kills);
		
		return this->SetKillsCombatEngineerKit(kills);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetDeathsCombatEngineerKit(uint32_t deaths)
{
	this->_s4 = deaths;
	return true;
}

bool Battlefield::GameStatPlayer::SetDeathsCombatEngineerKit(const std::string& str_deaths)
{
	try
	{
		uint32_t deaths = std::stoul(str_deaths);
		
		return this->SetDeathsCombatEngineerKit(deaths);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetKillsSupportKit(uint32_t kills)
{
	this->_k5 = kills;
	return true;
}

bool Battlefield::GameStatPlayer::SetKillsSupportKit(const std::string& str_kills)
{
	try
	{
		uint32_t kills = std::stoul(str_kills);
		
		return this->SetKillsSupportKit(kills);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetDeathsSupportKit(uint32_t deaths)
{
	this->_s5 = deaths;
	return true;
}

bool Battlefield::GameStatPlayer::SetDeathsSupportKit(const std::string& str_deaths)
{
	try
	{
		uint32_t deaths = std::stoul(str_deaths);
		
		return this->SetDeathsSupportKit(deaths);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetTeamKills(uint32_t tk)
{
	this->_tk = tk;
	return true;
}

bool Battlefield::GameStatPlayer::SetTeamKills(const std::string& str_tk)
{
	try
	{
		uint32_t tk = std::stoul(str_tk);
		
		return this->SetTeamKills(tk);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetMedals(uint32_t medals)
{
	this->_medals = medals & Battlefield::PlayerStats::Medals::All_Medals;
	return true;
}

bool Battlefield::GameStatPlayer::SetMedals(const std::string& str_medals)
{
	try
	{
		uint32_t medals = std::stoul(str_medals);
		
		return this->SetMedals(medals);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetTotalTopPlayer(uint32_t ttb)
{
	this->_ttb = ttb;
	return true;
}

bool Battlefield::GameStatPlayer::SetTotalTopPlayer(const std::string& str_ttb)
{
	try
	{
		uint32_t ttb = std::stoul(str_ttb);
		
		return this->SetTotalTopPlayer(ttb);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetTotalVictories(uint32_t mv)
{
	this->_mv = mv;
	return true;
}

bool Battlefield::GameStatPlayer::SetTotalVictories(const std::string& str_mv)
{
	try
	{
		uint32_t mv = std::stoul(str_mv);
		
		return this->SetTotalVictories(mv);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameStatPlayer::SetTotalGameSessions(uint32_t ngp)
{
	this->_ngp = ngp;
	return true;
}

bool Battlefield::GameStatPlayer::SetTotalGameSessions(const std::string& str_ngp)
{
	try
	{
		uint32_t ngp = std::stoul(str_ngp);
		
		return this->SetTotalGameSessions(ngp);
	}
	catch(...) {};
	
	return false;
}

void Battlefield::GameStatPlayer::UpdatePlayer()
{
	Battlefield::Player player;
		
	// Get player stats from database
	player.SetProfileId(this->GetProfileId());
	g_database->queryPlayerStats(player);

	player.SetBoatsDestroyed(          player.GetBoatsDestroyed()          + this->GetBoatsDestroyed()          ); // bod
	player.SetHAVsDestroyed(           player.GetHAVsDestroyed()           + this->GetHAVsDestroyed()           ); // havd
	player.SetHelicoptersDestroyed(    player.GetHelicoptersDestroyed()    + this->GetHelicoptersDestroyed()    ); // hed
	player.SetKillsAssualtKit(         player.GetKillsAssualtKit()         + this->GetKillsAssualtKit()         ); // k1
	player.SetKillsSniperKit(          player.GetKillsSniperKit()          + this->GetKillsSniperKit()          ); // k2
	player.SetKillsSpecialOpKit(       player.GetKillsSpecialOpKit()       + this->GetKillsSpecialOpKit()       ); // k3
	player.SetKillsCombatEngineerKit(  player.GetKillsCombatEngineerKit()  + this->GetKillsCombatEngineerKit()  ); // k4
	player.SetKillsSupportKit(         player.GetKillsSupportKit()         + this->GetKillsSupportKit()         ); // k5
	player.SetKills(                   player.GetKills()                   + this->GetKills()                   ); // kills
	player.SetLAVsDestroyed(           player.GetLAVsDestroyed()           + this->GetLAVsDestroyed()           ); // lavd
	player.SetMAVsDestroyed(           player.GetMAVsDestroyed()           + this->GetMAVsDestroyed()           ); // mavd
	player.SetTotalVictories(          player.GetTotalVictories()          + this->GetTotalVictories()          ); // mv
	player.SetTotalGameSessions(       player.GetTotalGameSessions()       + this->GetTotalGameSessions()       ); // ngp
	player.SetDeathsAssualtKit(        player.GetDeathsAssualtKit()        + this->GetDeathsAssualtKit()        ); // s1
	player.SetDeathsSniperKit(         player.GetDeathsSniperKit()         + this->GetDeathsSniperKit()         ); // s2
	player.SetDeathsSpecialOpKit(      player.GetDeathsSpecialOpKit()      + this->GetDeathsSpecialOpKit()      ); // s3
	player.SetDeathsCombatEngineerKit( player.GetDeathsCombatEngineerKit() + this->GetDeathsCombatEngineerKit() ); // s4
	player.SetDeathsSupportKit(        player.GetDeathsSupportKit()        + this->GetDeathsSupportKit()        ); // s5
	player.SetScore(                   player.GetScore()                   + this->GetScore()                   ); // score
	player.SetSuicides(                player.GetSuicides()                + this->GetSuicides()                ); // suicides
	player.SetTime(                    player.GetTime()                    + this->GetTime()                    ); // time
	player.SetTotalTopPlayer(          player.GetTotalTopPlayer()          + this->GetTotalTopPlayer()          ); // ttb
	
	player.SetMedals(this->GetMedals()); // medals
	player.SetPPH(this->GetPPH());       // pph
	player.SetRank(this->GetRank());     // rank
	
	// Update player stats on database
	g_database->updatePlayerStats(player);
}

void Battlefield::GameStatPlayer::Debug()
{
	Logger::debug("id = "       + std::to_string(this->GetId()));
	Logger::debug("auth = "     + this->GetAuth());
	Logger::debug("pid = "      + std::to_string(this->GetProfileId()));
	Logger::debug("score = "    + std::to_string(this->GetScore()));
	Logger::debug("rank = "     + std::to_string(this->GetRank()));
	Logger::debug("pph = "      + std::to_string(this->GetPPH()));
	Logger::debug("kills = "    + std::to_string(this->GetKills()));
	Logger::debug("suicides = " + std::to_string(this->GetSuicides()));
	Logger::debug("time = "     + std::to_string(this->GetTime()));
	Logger::debug("lavd = "     + std::to_string(this->GetLAVsDestroyed()));
	Logger::debug("mavd = "     + std::to_string(this->GetMAVsDestroyed()));
	Logger::debug("havd = "     + std::to_string(this->GetHAVsDestroyed()));
	Logger::debug("hed = "      + std::to_string(this->GetHelicoptersDestroyed()));
	Logger::debug("pld = "      + std::to_string(this->GetPlanesDestroyed()));
	Logger::debug("bod = "      + std::to_string(this->GetBoatsDestroyed()));
	Logger::debug("k1 = "       + std::to_string(this->GetKillsAssualtKit()));
	Logger::debug("s1 = "       + std::to_string(this->GetDeathsAssualtKit()));
	Logger::debug("k2 = "       + std::to_string(this->GetKillsSniperKit()));
	Logger::debug("s2 = "       + std::to_string(this->GetDeathsSniperKit()));
	Logger::debug("k3 = "       + std::to_string(this->GetKillsSpecialOpKit()));
	Logger::debug("s3 = "       + std::to_string(this->GetDeathsSpecialOpKit()));
	Logger::debug("k4 = "       + std::to_string(this->GetKillsCombatEngineerKit()));
	Logger::debug("s4 = "       + std::to_string(this->GetDeathsCombatEngineerKit()));
	Logger::debug("k5 = "       + std::to_string(this->GetKillsSupportKit()));
	Logger::debug("s5 = "       + std::to_string(this->GetDeathsSupportKit()));
	Logger::debug("tk = "       + std::to_string(this->GetTeamKills()));
	Logger::debug("medals = "   + std::to_string(this->GetMedals()));
	Logger::debug("ttb = "      + std::to_string(this->GetTotalTopPlayer()));
	Logger::debug("mv = "       + std::to_string(this->GetTotalVictories()));
	Logger::debug("ngp = "      + std::to_string(this->GetTotalGameSessions()));
}