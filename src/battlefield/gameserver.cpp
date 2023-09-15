#include <string>
#include <arpa/inet.h>
#include <mysql/mysql_time.h>

#include <util.h>
#include <logger.h>
#include <browsing/constants.h>

#include <battlefield/gameserver.h>

void Battlefield::GameServer::useExample()
{
	this->SetIp("168.119.189.149");
	this->SetPort(3658);
	this->SetFlag(FLAG_UNSOLICITED_UDP | FLAG_PRIVATE_IP | FLAG_ICMP_IP | FLAG_NONSTANDARD_PORT | FLAG_NONSTANDARD_PRIVATE_PORT);
}

bool Battlefield::GameServer::SetId(int id)
{
	this->_id = id;
	return true;
}

void Battlefield::GameServer::GetIpArray(uint8_t* ip) const
{	
	inet_pton(AF_INET, this->_ip.c_str(), ip);
}

bool Battlefield::GameServer::SetIp(const std::string& ip)
{
	this->_ip = ip;
	return true;
}

bool Battlefield::GameServer::SetPort(uint16_t port)
{
	this->_port = port;
	return true;
}

bool Battlefield::GameServer::SetFlag(uint8_t flag)
{
	this->_flag = flag;
	return true;
}

bool Battlefield::GameServer::SetLocalIp(const std::string& localip)
{
	this->_localip0 = localip;
	return true;
}

bool Battlefield::GameServer::SetLocalPort(uint16_t localport)
{
	this->_localport = localport;
	return true;
}

bool Battlefield::GameServer::SetLocalPort(const std::string& str_localport)
{
	try
	{
		uint16_t localport = static_cast<uint16_t>(std::stoul(str_localport));
		
		return this->SetLocalPort(localport);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetNatNeg(uint8_t natneg)
{
	this->_natneg = natneg;
	return true;
}

bool Battlefield::GameServer::SetNatNeg(const std::string& str_natneg)
{
	try
	{
		uint8_t natneg = static_cast<uint8_t>(std::stoul(str_natneg));
		
		return this->SetNatNeg(natneg);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetGameName(const std::string& gamename)
{
	this->_gamename = gamename;
	return true;
}

bool Battlefield::GameServer::SetHostName(const std::string& hostname)
{
	this->_hostname = hostname;
	return true;
}

bool Battlefield::GameServer::SetHostPort(uint16_t hostport)
{
	this->_hostport = hostport;
	return true;
}

bool Battlefield::GameServer::SetHostPort(const std::string& str_hostport)
{
	try
	{
		uint16_t hostport = static_cast<uint16_t>(std::stoul(str_hostport));
		
		return this->SetHostPort(hostport);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetGameVersion(const std::string& gamever)
{
	this->_gamever = gamever;
	return true;
}

bool Battlefield::GameServer::SetClientVersion(const std::string& clientver)
{
	this->_cl = clientver;
	return true;
}

bool Battlefield::GameServer::SetRV(const std::string& rv)
{
	this->_rv = rv;
	return true;
}

bool Battlefield::GameServer::SetMap(const std::string& map)
{
	this->_map = map;
	return true;
}

bool Battlefield::GameServer::SetMapCycling(uint8_t mc)
{
	this->_mc = mc;
	return true;
}

bool Battlefield::GameServer::SetMapCycling(const std::string& str_mc)
{
	try
	{
		uint8_t mc = static_cast<uint8_t>(std::stoul(str_mc));
		
		return this->SetMapCycling(mc);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetMapName(uint8_t mapname)
{
	this->_mapname = mapname;
	return true;
}

bool Battlefield::GameServer::SetMapName(const std::string& str_mapname)
{
	try
	{
		uint8_t mapname = static_cast<uint8_t>(std::stoul(str_mapname));
		
		return this->SetMapName(mapname);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetGC(uint8_t gc)
{
	this->_gc = gc;
	return true;
}

bool Battlefield::GameServer::SetGC(const std::string& str_gc)
{
	try
	{
		uint8_t gc = static_cast<uint8_t>(std::stoul(str_gc));
		
		return this->SetGC(gc);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetGameType(const std::string& gametype)
{
	this->_gametype = gametype;
	return true;
}

bool Battlefield::GameServer::SetGameVariant(const std::string& gamevariant)
{
	this->_gamevariant = gamevariant;
	return true;
}

bool Battlefield::GameServer::SetNumPlayers(uint8_t numplayers)
{
	this->_numplayers = numplayers;
	return true;
}

bool Battlefield::GameServer::SetNumPlayers(const std::string& str_numplayers)
{
	try
	{
		uint8_t numplayers = static_cast<uint8_t>(std::stoul(str_numplayers));
		
		return this->SetNumPlayers(numplayers);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetMaxPlayers(uint8_t maxplayers)
{
	this->_maxplayers = maxplayers;
	return true;
}

bool Battlefield::GameServer::SetMaxPlayers(const std::string& str_maxplayers)
{
	try
	{
		uint8_t maxplayers = static_cast<uint8_t>(std::stoul(str_maxplayers));
		
		return this->SetMaxPlayers(maxplayers);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetNumTeams(uint8_t numteams)
{
	this->_numteams = numteams;
	return true;
}

bool Battlefield::GameServer::SetNumTeams(const std::string& str_numteams)
{
	try
	{
		uint8_t numteams = static_cast<uint8_t>(std::stoul(str_numteams));
		
		return this->SetNumTeams(numteams);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetGameMode(const std::string& gamemode)
{
	this->_gamemode = gamemode;
	return true;
}

bool Battlefield::GameServer::SetTeamplay(uint8_t teamplay)
{
	this->_teamplay = teamplay;
	return true;
}

bool Battlefield::GameServer::SetTeamplay(const std::string& str_teamplay)
{
	try
	{
		uint8_t teamplay = static_cast<uint8_t>(std::stoul(str_teamplay));
		
		return this->SetTeamplay(teamplay);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetFlagLimit(uint8_t fraglimit)
{
	this->_fraglimit = fraglimit;
	return true;
}

bool Battlefield::GameServer::SetFlagLimit(const std::string& str_fraglimit)
{
	try
	{
		uint8_t fraglimit = static_cast<uint8_t>(std::stoul(str_fraglimit));
		
		return this->SetFlagLimit(fraglimit);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetTeamFragLimit(uint8_t teamfraglimit)
{
	this->_teamfraglimit = teamfraglimit;
	return true;
}

bool Battlefield::GameServer::SetTeamFragLimit(const std::string& str_teamfraglimit)
{
	try
	{
		uint8_t teamfraglimit = static_cast<uint8_t>(std::stoul(str_teamfraglimit));
		
		return this->SetTeamFragLimit(teamfraglimit);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetTimeLimit(uint16_t timelimit)
{
	this->_timelimit = timelimit;
	return true;
}

bool Battlefield::GameServer::SetTimeLimit(const std::string& str_timelimit)
{
	try
	{
		uint16_t timelimit = static_cast<uint16_t>(std::stoul(str_timelimit));
		
		return this->SetTimeLimit(timelimit);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetTimeElapsed(uint16_t timeelapsed)
{
	this->_timeelapsed = timeelapsed;
	return true;
}

bool Battlefield::GameServer::SetTimeElapsed(const std::string& str_timeelapsed)
{
	try
	{
		uint16_t timeelapsed = static_cast<uint16_t>(std::stoul(str_timeelapsed));
		
		return this->SetTimeElapsed(timeelapsed);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetPassword(uint8_t password)
{
	this->_password = password;
	return true;
}

bool Battlefield::GameServer::SetPassword(const std::string& str_password)
{
	try
	{
		uint8_t password = static_cast<uint8_t>(std::stoul(str_password));
		
		return this->SetPassword(password);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetMinRank(uint8_t nr)
{
	this->_nr = nr;
	return true;
}

bool Battlefield::GameServer::SetMinRank(const std::string& str_nr)
{
	try
	{
		uint8_t nr = static_cast<uint8_t>(std::stoul(str_nr));
		
		return this->SetMinRank(nr);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetMaxRank(uint8_t xr)
{
	this->_xr = xr;
	return true;
}

bool Battlefield::GameServer::SetMaxRank(const std::string& str_xr)
{
	try
	{
		uint8_t xr = static_cast<uint8_t>(std::stoul(str_xr));
		
		return this->SetMaxRank(xr);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetFriendlyFire(uint8_t ff)
{
	this->_ff = ff;
	return true;
}

bool Battlefield::GameServer::SetFriendlyFire(const std::string& str_ff)
{
	try
	{
		uint8_t ff = static_cast<uint8_t>(std::stoul(str_ff));
		
		return this->SetFriendlyFire(ff);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetStatsTracking(uint8_t sr)
{
	this->_sr = sr;
	return true;
}

bool Battlefield::GameServer::SetStatsTracking(const std::string& str_sr)
{
	try
	{
		uint8_t sr = static_cast<uint8_t>(std::stoul(str_sr));
		
		return this->SetStatsTracking(sr);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetReconfigurable(uint8_t rc)
{
	this->_rc = rc;
	return true;
}

bool Battlefield::GameServer::SetReconfigurable(const std::string& str_rc)
{
	try
	{
		uint8_t rc = static_cast<uint8_t>(std::stoul(str_rc));
		
		return this->SetReconfigurable(rc);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetMinIpRange(int64_t ni)
{
	this->_ni = ni;
	return true;
}

bool Battlefield::GameServer::SetMinIpRange(const std::string& str_ni)
{
	try
	{
		int64_t ni = static_cast<int64_t>(std::stoll(str_ni));
	
		return this->SetMinIpRange(ni);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetMaxIpRange(int64_t xi)
{
	this->_xi = xi;
	return true;
}

bool Battlefield::GameServer::SetMaxIpRange(const std::string& str_xi)
{
	try
	{
		int64_t xi = static_cast<int64_t>(std::stoll(str_xi));
		
		return this->SetMaxIpRange(xi);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetQM(uint8_t qm)
{
	this->_qm = qm;
	return true;
}

bool Battlefield::GameServer::SetQM(const std::string& str_qm)
{
	try
	{
		uint8_t qm = static_cast<uint8_t>(std::stoul(str_qm));
		
		return this->SetQM(qm);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetRegion(uint64_t region)
{
	this->_region = region;
	return true;
}

bool Battlefield::GameServer::SetRegion(const std::string& str_region)
{
	try
	{
		uint64_t region = static_cast<uint64_t>(std::stoull(str_region));
		
		return this->SetRegion(region);
	}
	catch(...) {};
	
	return false;
}

// Clan
bool Battlefield::GameServer::SetClan1Id(int32_t c0)
{
	this->_c0 = c0;
	return true;
}

bool Battlefield::GameServer::SetClan1Id(const std::string& str_c0)
{
	try
	{
		int32_t c0 = static_cast<int32_t>(std::stoi(str_c0));
		
		return this->SetClan1Id(c0);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetClan2Id(int32_t c1)
{
	this->_c1 = c1;
	return true;
}

bool Battlefield::GameServer::SetClan2Id(const std::string& str_c1)
{
	try
	{
		int32_t c1 = static_cast<int32_t>(std::stoi(str_c1));
		
		return this->SetClan2Id(c1);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetClan1Name(const std::string& n0)
{
	this->_n0 = n0;
	return true;
}

bool Battlefield::GameServer::SetClan2Name(const std::string& n1)
{
	this->_n1 = n1;
	return true;
}

bool Battlefield::GameServer::SetClan1Claimed(uint8_t c0c)
{
	this->_c0c = c0c;
	return true;
}

bool Battlefield::GameServer::SetClan1Claimed(const std::string& str_c0c)
{
	try
	{
		uint8_t c0c = static_cast<uint8_t>(std::stoul(str_c0c));
		
		return this->SetClan1Claimed(c0c);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetClan2Claimed(uint8_t c1c)
{
	this->_c1c = c1c;
	return true;
}

bool Battlefield::GameServer::SetClan2Claimed(const std::string& str_c1c)
{
	try
	{
		uint8_t c1c = static_cast<uint8_t>(std::stoul(str_c1c));
		
		return this->SetClan2Claimed(c1c);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetTeam1Name(const std::string& team0)
{
	this->_team0 = team0;
	return true;
}

bool Battlefield::GameServer::SetTeam2Name(const std::string& team1)
{
	this->_team1 = team1;
	return true;
}

bool Battlefield::GameServer::SetTeam1Score(int16_t score0)
{
	this->_score0 = score0;
	return true;
}

bool Battlefield::GameServer::SetTeam1Score(const std::string& str_score0)
{
	try
	{
		int16_t score0 = static_cast<int16_t>(std::stoi(str_score0));
		
		return this->SetTeam1Score(score0);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetTeam2Score(int16_t score1)
{
	this->_score1 = score1;
	return true;
}

bool Battlefield::GameServer::SetTeam2Score(const std::string& str_score1)
{
	try
	{
		int16_t score1 = static_cast<int16_t>(std::stoi(str_score1));
		
		return this->SetTeam2Score(score1);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServer::SetUpdatedAt(MYSQL_TIME updated_at)
{
    this->_updated_at = Util::Time::GetDateTime(updated_at);
	
    return true;
}

bool Battlefield::GameServer::SetVerified(bool verified)
{
	this->_verified = verified;
	return true;
}

void Battlefield::GameServer::AddPlayer(const GameServerPlayer& gsplayer)
{
	this->_players.push_back(gsplayer);
}

bool Battlefield::GameServer::IsAlive()
{
	struct tm timeinfo;
	
	if (strptime(this->GetUpdatedAt().c_str(), "%Y-%m-%d %H:%M:%S %Z", &timeinfo) != nullptr)
	{
		std::chrono::system_clock::time_point current_time_point = std::chrono::system_clock::now();
		std::chrono::system_clock::time_point two_minutes_ago = current_time_point - std::chrono::minutes(2);

		// get time_t
		std::time_t current_time = std::chrono::system_clock::to_time_t(current_time_point);
		std::time_t two_minutes_ago_time = std::chrono::system_clock::to_time_t(two_minutes_ago);
		std::time_t target_time = std::mktime(&timeinfo);

		//Logger::debug("target_time: " + std::string(std::ctime(&target_time)));
		//Logger::debug("current_time: " + std::string(std::ctime(&current_time)));
		//Logger::debug("two_minutes_ago_time: " + std::string(std::ctime(&two_minutes_ago_time)));
		
		return (std::difftime(target_time, current_time) <= 0 && std::difftime(target_time, two_minutes_ago_time) >= 0);
	}
	
	return false;
}

void Battlefield::GameServer::Debug()
{
	Logger::debug("============================");
	Logger::debug("id = "            + std::to_string(this->GetId()));
	Logger::debug("ip = "            + this->GetIp());
	Logger::debug("port = "          + std::to_string(this->GetPort()));
	Logger::debug("flag = "          + std::to_string(this->GetFlag()));
	Logger::debug("localip0 = "      + this->GetLocalIp());
	Logger::debug("localport = "     + std::to_string(this->GetLocalPort()));
	Logger::debug("natneg = "        + std::to_string(this->GetNatNeg()));
	Logger::debug("gamename = "      + this->GetGameName());
	Logger::debug("hostname = "      + this->GetHostName());
	Logger::debug("hostport = "      + std::to_string(this->GetHostPort()));
	Logger::debug("gamever = "       + this->GetGameVersion());
	Logger::debug("cl = "            + this->GetClientVersion());
	Logger::debug("rv = "            + this->GetRV());
	Logger::debug("map = "           + this->GetMap());
	Logger::debug("mc = "            + std::to_string(this->GetMapCycling()));
	Logger::debug("mapname = "       + std::to_string(this->GetMapName()));
	Logger::debug("gc = "            + std::to_string(this->GetGC()));
	Logger::debug("gametype = "      + this->GetGameType());
	Logger::debug("gamevariant = "   + this->GetGameVariant());
	Logger::debug("numplayers = "    + std::to_string(this->GetNumPlayers()));
	Logger::debug("maxplayers = "    + std::to_string(this->GetMaxPlayers()));
	Logger::debug("numteams = "      + std::to_string(this->GetNumTeams()));
	Logger::debug("gamemode = "      + this->GetGameMode());
	Logger::debug("teamplay = "      + std::to_string(this->GetTeamplay()));
	Logger::debug("fraglimit = "     + std::to_string(this->GetFlagLimit()));
	Logger::debug("teamfraglimit = " + std::to_string(this->GetTeamFragLimit()));
	Logger::debug("timelimit = "     + std::to_string(this->GetTimeLimit()));
	Logger::debug("timeelapsed = "   + std::to_string(this->GetTimeElapsed()));
	Logger::debug("password = "      + std::to_string(this->GetPassword()));
	Logger::debug("nr = "            + std::to_string(this->GetMinRank()));
	Logger::debug("xr = "            + std::to_string(this->GetMaxRank()));
	Logger::debug("ff = "            + std::to_string(this->GetFriendlyFire()));
	Logger::debug("sr = "            + std::to_string(this->GetStatsTracking()));
	Logger::debug("rc = "            + std::to_string(this->GetReconfigurable()));
	Logger::debug("ni = "            + std::to_string(this->GetMinIpRange()));
	Logger::debug("xi = "            + std::to_string(this->GetMaxIpRange()));
	Logger::debug("qm = "            + std::to_string(this->GetQM()));
	Logger::debug("region = "        + std::to_string(this->GetRegion()));
	Logger::debug("c0 = "            + std::to_string(this->GetClan1Id()));
	Logger::debug("c1 = "            + std::to_string(this->GetClan2Id()));
	Logger::debug("n0 = "            + this->GetClan1Name());
	Logger::debug("n1 = "            + this->GetClan2Name());
	Logger::debug("c0c = "           + std::to_string(this->GetClan1Claimed()));
	Logger::debug("c1c = "           + std::to_string(this->GetClan2Claimed()));
	Logger::debug("team0 = "         + this->GetTeam1Name());
	Logger::debug("score0 = "        + std::to_string(this->GetTeam1Score()));
	Logger::debug("team1 = "         + this->GetTeam2Name());
	Logger::debug("score1 = "        + std::to_string(this->GetTeam2Score()));
}

// GameServerPlayer
bool Battlefield::GameServerPlayer::SetId(int id)
{
	this->_id = id;
	return true;
}

bool Battlefield::GameServerPlayer::SetName(const std::string name)
{
	this->_name = name;
	return true;
}

bool Battlefield::GameServerPlayer::SetScore(int16_t score)
{
	this->_score = score;
	return true;
}

bool Battlefield::GameServerPlayer::SetScore(const std::string str_score)
{
	try
	{
		int16_t score = static_cast<int16_t>(std::stoi(str_score));
		
		return this->SetScore(score);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServerPlayer::SetSkill(const std::string skill)
{
	this->_skill = skill;
	return true;
}

bool Battlefield::GameServerPlayer::SetPing(uint8_t ping)
{
	this->_ping = ping;
	return true;
}

bool Battlefield::GameServerPlayer::SetPing(const std::string str_ping)
{
	try
	{
		uint8_t ping = static_cast<uint8_t>(std::stoul(str_ping));
	
		return this->SetPing(ping);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServerPlayer::SetTeam(uint8_t team)
{
	this->_team = team;
	return true;
}

bool Battlefield::GameServerPlayer::SetTeam(const std::string str_team)
{
	try
	{
		uint8_t team = static_cast<uint8_t>(std::stoul(str_team));
		
		return this->SetTeam(team);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServerPlayer::SetDeaths(uint16_t deaths)
{
	this->_deaths = deaths;
	return true;
}

bool Battlefield::GameServerPlayer::SetDeaths(const std::string str_deaths)
{
	try
	{
		uint16_t deaths = static_cast<uint16_t>(std::stoul(str_deaths));
		
		return this->SetDeaths(deaths);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::GameServerPlayer::SetProfileId(int profileid)
{
	this->_profileid = profileid;
	return true;
}

bool Battlefield::GameServerPlayer::SetProfileId(const std::string str_profileid)
{
	try
	{
		int profileid = std::stoi(str_profileid);
		
		return this->SetProfileId(profileid);
	}
	catch(...) {};
	
	return false;
}

