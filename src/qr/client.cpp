#include <map>
#include <vector>

#include <logger.h>
#include <util.h>
#include <settings.h>
#include <qr/constants.h>
#include <battlefield/gameserver.h>


#include <qr/client.h>

typedef void (QR::Client::*RequestActionFunc)(const std::vector<unsigned char>&) const;

static std::map<int, RequestActionFunc> mRequestActions = 
{
	{ REQUEST_CHALLENGE,   &QR::Client::requestChallenge   }, // Done
	{ REQUEST_AVAILABLE,   &QR::Client::requestAvailable   }, // Done
	{ REQUEST_HEARTBEAT,   &QR::Client::requestHeartbeat   }, // Done
	{ REQUEST_KEEPALIVE,   &QR::Client::requestKeepAlive   }, // Done
};

QR::Client::Client(int socket, struct sockaddr_in address)
{
	this->_socket = socket;
	this->_address = address;
}

QR::Client::~Client()
{
	
}

/*
	Events
*/
void QR::Client::onRequest(const std::vector<unsigned char>& request)
{
	// One close services it will send empty requests
	if(this->GetAddress() == "0.0.0.0:0")
	{
		return;
	}
	
	this->_LogTransaction("-->", Util::Buffer::ToString(request));
	
	int action = request[0];
	
	// Find function name
	auto it = mRequestActions.find(action);
	if (it != mRequestActions.end())
	{
		// Get Function address
		RequestActionFunc func = it->second;
	
		// Execute action function with class object.
		(this->*(func))(request);
	}
	else
	{
		Logger::warning("action \"" + std::to_string(action) + "\" not implemented!", Server::Type::QR);
	}
}

/*
	
*/
void QR::Client::requestChallenge(const std::vector<unsigned char>& request) const
{
	std::vector<unsigned char> response = {
		RESPONSE_MAGIC_1, RESPONSE_MAGIC_2,				// Magic
		RESPONSE_CHALLENGE,								// Challenge response action
		request[1], request[2], request[3], request[4],	// Instance key
	};
	
	this->UDPSend(response);
	
	this->_LogTransaction("<--", Util::Buffer::ToString(response));
}

/*
	
*/
void QR::Client::requestAvailable(const std::vector<unsigned char>& request) const
{
	std::vector<unsigned char> response = {
		RESPONSE_MAGIC_1, RESPONSE_MAGIC_2,
		RESPONSE_AVAILABLE,
		0x0, 0x0, 0x0, 0x0
	};
	
	this->UDPSend(response);
	
	this->_LogTransaction("<--", Util::Buffer::ToString(response));
}

/*
	Request:
		0030   03 e0 dd 81 84 6c 6f 63 61 6c 69 70 30 00 31 30   .....localip0.10
		0040   2e 31 30 2e 31 30 2e 31 31 30 00 6c 6f 63 61 6c   .10.10.110.local
		0050   70 6f 72 74 00 33 36 35 38 00 6e 61 74 6e 65 67   port.3658.natneg
		0060   00 31 00 67 61 6d 65 6e 61 6d 65 00 62 66 69 65   .1.gamename.bfie
		0070   6c 64 31 39 34 32 70 73 32 00 68 6f 73 74 6e 61   ld1942ps2.hostna
		0080   6d 65 00 5b 43 51 5d 42 46 32 4d 43 2d 49 61 6d   me.[CQ]BF2MC-Iam
		0090   4c 75 70 6f 00 68 6f 73 74 70 6f 72 74 00 33 36   Lupo.hostport.36
		00a0   35 38 00 67 61 6d 65 76 65 72 00 56 31 2e 33 31   58.gamever.V1.31
		00b0   61 00 6d 61 70 6e 61 6d 65 00 32 00 67 61 6d 65   a.mapname.2.game
		00c0   74 79 70 65 00 63 6f 6e 71 75 65 73 74 00 67 61   type.conquest.ga
		00d0   6d 65 76 61 72 69 61 6e 74 00 42 6f 72 64 65 72   mevariant.Border
		00e0   00 6e 75 6d 70 6c 61 79 65 72 73 00 31 00 6e 75   .numplayers.1.nu
		00f0   6d 74 65 61 6d 73 00 32 00 6d 61 78 70 6c 61 79   mteams.2.maxplay
		0100   65 72 73 00 32 34 00 67 61 6d 65 6d 6f 64 65 00   ers.24.gamemode.
		0110   6f 70 65 6e 70 6c 61 79 69 6e 67 00 74 65 61 6d   openplaying.team
		0120   70 6c 61 79 00 30 00 66 72 61 67 6c 69 6d 69 74   play.0.fraglimit
		0130   00 30 00 74 65 61 6d 66 72 61 67 6c 69 6d 69 74   .0.teamfraglimit
		0140   00 30 00 74 69 6d 65 6c 69 6d 69 74 00 31 32 30   .0.timelimit.120
		0150   30 00 74 69 6d 65 65 6c 61 70 73 65 64 00 39 30   0.timeelapsed.90
		0160   30 00 70 61 73 73 77 6f 72 64 00 30 00 6e 72 00   0.password.0.nr.
		0170   31 00 78 72 00 32 30 00 66 66 00 31 00 6d 63 00   1.xr.20.ff.1.mc.
		0180   31 00 72 76 00 72 65 74 61 69 6c 00 67 63 00 30   1.rv.retail.gc.0
		0190   00 72 63 00 31 00 73 72 00 31 00 6e 69 00 2d 32   .rc.1.sr.1.ni.-2
		01a0   31 34 37 34 38 33 36 34 38 00 78 69 00 32 31 34   147483648.xi.214
		01b0   37 34 38 33 36 34 37 00 71 6d 00 30 00 63 6c 00   7483647.qm.0.cl.
		01c0   31 30 30 2e 32 31 36 33 33 35 2e 61 6c 70 68 61   100.216335.alpha
		01d0   00 72 65 67 69 6f 6e 00 36 35 35 33 36 00 6d 61   .region.65536.ma
		01e0   70 00 62 72 69 64 67 65 74 6f 6f 66 61 72 00 00   p.bridgetoofar..
		01f0   00 01 70 6c 61 79 65 72 5f 00 73 63 6f 72 65 5f   ..player_.score_
		0200   00 73 6b 69 6c 6c 5f 00 70 69 6e 67 5f 00 74 65   .skill_.ping_.te
		0210   61 6d 5f 00 64 65 61 74 68 73 5f 00 70 69 64 5f   am_.deaths_.pid_
		0220   00 00 49 61 6d 4c 75 70 6f 00 30 00 72 6f 6f 6b   ..IamLupo.0.rook
		0230   69 65 00 30 00 30 00 30 00 31 30 30 33 36 38 31   ie.0.0.0.1003681
		0240   39 00 00 02 74 65 61 6d 5f 74 00 73 63 6f 72 65   9...team_t.score
		0250   5f 74 00 00 43 48 00 34 35 30 00 55 53 00 34 35   _t..CH.450.US.45
		0260   30 00                                             0.

	Response:
		0030   fe fd 01 e0 dd 81 84 30 30 30 30 30 30 30 30 30   .......000000000
		0040   30 30 30 30 30 30 30 30 30 30 30 00               00000000000.

*/
void QR::Client::requestHeartbeat(const std::vector<unsigned char>& request) const
{
	size_t offset = 5;
	std::string key, value;
	Battlefield::GameServer game_server;
	
	// Read request game server values
	while(Util::Buffer::ReadString(request, offset, key) && key.size() > 0)
	{
		// Read value
		Util::Buffer::ReadString(request, offset, value);
		
		if(value.size() > 0)
		{
			if(key == "localip0")           game_server.SetLocalIp(value);
			else if(key == "localport")     game_server.SetLocalPort(value);
			else if(key == "natneg")        game_server.SetNatNeg(value);
			else if(key == "gamename")      game_server.SetGameName(value);
			else if(key == "hostname")      game_server.SetHostName(value);
			else if(key == "hostport")      game_server.SetHostPort(value);
			else if(key == "gamever")       game_server.SetGameVersion(value);
			else if(key == "cl")            game_server.SetClientVersion(value);
			else if(key == "rv")            game_server.SetRV(value);
			else if(key == "map")           game_server.SetMap(value);
			else if(key == "mc")            game_server.SetMapCycling(value);
			else if(key == "mapname")       game_server.SetMapName(value);
			else if(key == "gc")            game_server.SetGC(value);
			else if(key == "gametype")      game_server.SetGameType(value);
			else if(key == "gamevariant")   game_server.SetGameVariant(value);
			else if(key == "numplayers")    game_server.SetNumPlayers(value);
			else if(key == "maxplayers")    game_server.SetMaxPlayers(value);
			else if(key == "numteams")      game_server.SetNumTeams(value);
			else if(key == "gamemode")      game_server.SetGameMode(value);
			else if(key == "teamplay")      game_server.SetTeamplay(value);
			else if(key == "fraglimit")     game_server.SetFlagLimit(value);
			else if(key == "teamfraglimit") game_server.SetTeamFragLimit(value);
			else if(key == "timelimit")     game_server.SetTimeLimit(value);
			else if(key == "timeelapsed")   game_server.SetTimeElapsed(value);
			else if(key == "password")      game_server.SetPassword(value);
			else if(key == "nr")            game_server.SetMinRank(value);
			else if(key == "xr")            game_server.SetMaxRank(value);
			else if(key == "ff")            game_server.SetFriendlyFire(value);
			else if(key == "sr")            game_server.SetStatsTracking(value);
			else if(key == "rc")            game_server.SetReconfigurable(value);
			else if(key == "ni")            game_server.SetMinIpRange(value);
			else if(key == "xi")            game_server.SetMaxIpRange(value);
			else if(key == "qm")            game_server.SetQM(value);
			else if(key == "region")        game_server.SetRegion(value);
			else if(key == "c0")            game_server.SetClanIdTeam1(value);
			else if(key == "c1")            game_server.SetClanIdTeam2(value);
			else if(key == "n0")            game_server.SetClanNameTeam1(value);
			else if(key == "n1")            game_server.SetClanNameTeam2(value);
			else if(key == "c0c")           game_server.SetClan1Claimed(value);
			else if(key == "c1c")           game_server.SetClan2Claimed(value);
			
			// Debug
			Logger::debug(key + " = " + value);
		}
	}
	
	// Debug
	game_server.Debug();
	
	// Send response
	std::vector<unsigned char> response = {
		RESPONSE_MAGIC_1, RESPONSE_MAGIC_2,				// Magic
		RESPONSE_HEARTBEAT,								// Challenge response action
		request[1], request[2], request[3], request[4],	// Instance key
		0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, //   - 8 random characters
		0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,	//   - client ip in hex
		0x30, 0x30, 0x30, 0x30,							//   - client port in hex
		0x00
	};
	
	this->UDPSend(response);
	
	this->_LogTransaction("<--", Util::Buffer::ToString(response));
}

/*

*/
void QR::Client::requestKeepAlive(const std::vector<unsigned char>& request) const
{
	std::vector<unsigned char> response = {
		RESPONSE_MAGIC_1, RESPONSE_MAGIC_2,				// Magic
		RESPONSE_KEEPALIVE,								// Challenge response action
		request[1], request[2], request[3], request[4],	// Instance key
		0x00, 0x00, 0x00, 0x00,							// time
	};
	
	this->UDPSend(response);
	
	this->_LogTransaction("<--", Util::Buffer::ToString(response));
}

/*
	Private functions
*/
void QR::Client::_LogTransaction(const std::string& direction, const std::string& response) const
{
	std::shared_lock<std::shared_mutex> guard(g_mutex_settings); // settings lock  (read)
	
	bool show_console = (g_settings["qr"]["show_requests"].asBool() && direction == "-->") ||
						(g_settings["qr"]["show_responses"].asBool() && direction == "<--");
	
	Logger::info(this->GetAddress() + " " + direction + " " + response,
			Server::Type::QR, show_console);
}

