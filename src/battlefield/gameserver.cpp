#include <string>
#include <arpa/inet.h>

#include <browsing/constants.h>

#include <battlefield/gameserver.h>

void Battlefield::GameServer::useExample()
{
	this->SetIp("168.119.189.149");
	this->SetPort(3658);
	this->SetFlag(FLAG_UNSOLICITED_UDP | FLAG_PRIVATE_IP | FLAG_ICMP_IP | FLAG_NONSTANDARD_PORT | FLAG_NONSTANDARD_PRIVATE_PORT);
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

bool Battlefield::GameServer::SetNatNeg(uint8_t natneg)
{
	this->_natneg = natneg;
	return true;
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

bool Battlefield::GameServer::SetMapName(uint8_t mapname)
{
	this->_mapname = mapname;
	return true;
}

bool Battlefield::GameServer::SetGC(uint8_t gc)
{
	this->_gc = gc;
	return true;
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

bool Battlefield::GameServer::SetMaxPlayers(uint8_t maxplayers)
{
	this->_maxplayers = maxplayers;
	return true;
}

bool Battlefield::GameServer::SetNumTeams(uint8_t numteams)
{
	this->_numteams = numteams;
	return true;
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

bool Battlefield::GameServer::SetFlagLimit(uint8_t fraglimit)
{
	this->_fraglimit = fraglimit;
	return true;
}

bool Battlefield::GameServer::SetTeamFragLimit(uint8_t teamfraglimit)
{
	this->_teamfraglimit = teamfraglimit;
	return true;
}

bool Battlefield::GameServer::SetTimeLimit(uint8_t timelimit)
{
	this->_timelimit = timelimit;
	return true;
}

bool Battlefield::GameServer::SetTimeElapsed(uint8_t timeelapsed)
{
	this->_timeelapsed = timeelapsed;
	return true;
}

bool Battlefield::GameServer::SetPassword(uint8_t password)
{
	this->_password = password;
	return true;
}

bool Battlefield::GameServer::SetMinRank(int64_t nr)
{
	this->_nr = nr;
	return true;
}

bool Battlefield::GameServer::SetMaxRank(int64_t xr)
{
	this->_xr = xr;
	return true;
}

bool Battlefield::GameServer::SetFriendlyFire(uint8_t ff)
{
	this->_ff = ff;
	return true;
}

bool Battlefield::GameServer::SetStatsTracking(uint8_t sr)
{
	this->_sr = sr;
	return true;
}

bool Battlefield::GameServer::SetReconfigurable(uint8_t rc)
{
	this->_rc = rc;
	return true;
}

bool Battlefield::GameServer::SetMinIpRange(uint8_t ni)
{
	this->_ni = ni;
	return true;
}

bool Battlefield::GameServer::SetMaxIpRange(uint8_t xi)
{
	this->_xi = xi;
	return true;
}

bool Battlefield::GameServer::SetQM(uint8_t qm)
{
	this->_qm = qm;
	return true;
}

bool Battlefield::GameServer::SetRegion(uint8_t region)
{
	this->_region = region;
	return true;
}

// Clan
bool Battlefield::GameServer::SetClanIdTeam1(int8_t c0)
{
	this->_c0 = c0;
	return true;
}

bool Battlefield::GameServer::SetClanIdTeam2(int8_t c1)
{
	this->_c1 = c1;
	return true;
}

bool Battlefield::GameServer::SetClanNameTeam1(const std::string& n0)
{
	this->_n0 = n0;
	return true;
}

bool Battlefield::GameServer::SetClanNameTeam2(const std::string& n1)
{
	this->_n1 = n1;
	return true;
}

bool Battlefield::GameServer::SetClan1Claimed(uint8_t c0c)
{
	this->_c0c = c0c;
	return true;
}

bool Battlefield::GameServer::SetClan2Claimed(uint8_t c1c)
{
	this->_c1c = c1c;
	return true;
}

