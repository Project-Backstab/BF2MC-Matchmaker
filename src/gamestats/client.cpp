#include <unistd.h>
#include <iostream>

#include <settings.h>
#include <logger.h>
#include <server.h>
#include <globals.h>
#include <util.h>
#include <database.h>
#include <battlefield/player.h>
#include <battlefield/gamestat.h>

#include <gamestats/client.h>

typedef void (GameStats::Client::*RequestActionFunc)(const GameSpy::Parameter&);

static std::map<std::string, RequestActionFunc> mRequestActions = 
{
	{ "auth",      &GameStats::Client::requestAuth },
	{ "newgame",   &GameStats::Client::requestNewGame },
	{ "updgame",   &GameStats::Client::requestUpdateGame },
	//{ "ka",        &GameStats::Client::requestKa }
};

GameStats::Client::Client(int socket, struct sockaddr_in address)
{
	this->_socket = socket;
	this->_address = address;
}

GameStats::Client::~Client()
{
	this->Disconnect();
}

void GameStats::Client::Listen()
{
	// Initialize connection send challenge
	this->requestChallenge();
	
	while(true)
	{
		std::string request;
		std::vector<unsigned char> buffer(4096, 0);
		
		int v = read(this->_socket, &(buffer[0]), 4096);
		
		// If error or no data is recieved we end the connection
		if(v <= 0)
		{
			break;
		}
		
		// Resize buffer
		buffer.resize(v);
		
		request = Decrypt(buffer);
		
		this->_LogTransaction("-->", request);
		
		this->onRequest(request);
	}
	
	this->Disconnect();
}

void GameStats::Client::Disconnect()
{
	this->Close();
	g_gamestats_server->onClientDisconnect(*this);
}

/*
	Events
*/
void GameStats::Client::onRequest(const std::string& request)
{
	GameSpy::Parameter parameter = GameSpy::Request2Parameter(request);
	
	// Find function name
	std::string action = parameter[0];
	
	auto it = mRequestActions.find(action);
	if (it != mRequestActions.end())
	{
		// Get Function address
		RequestActionFunc func = it->second;
	
		// Execute action function with class object.
		(this->*(func))(parameter);
	}
	else
	{
		Logger::warning("action \"" + action + "\" not implemented!", Server::Type::GameStats);
		
		this->Disconnect();
	}
}

/*
	Response:
		\lc\1\challenge\GzlCt7q8sV\id\1\final\
*/
void GameStats::Client::requestChallenge()
{
	std::string challenge = Util::generateRandomChallenge();
	
	std::string response = GameSpy::Parameter2Response({
		"lc",        "1",
		"challenge", challenge,
		"id",        "1",
		"final"
	});
	
	this->Send(Encrypt(response));
	
	this->_LogTransaction("<--", response);
}

/*
	Request:
		\auth\\gamename\bfield1942ps2\response\f34cc66938c4b07c70ebff98d9d98561\port\0\id\1\final\
	Response:
		\lc\2\sesskey\1687554231\proof\0\id\1\final\
*/
void GameStats::Client::requestAuth(const GameSpy::Parameter& parameter)
{	
	std::string response = GameSpy::Parameter2Response({
		"lc",      "2",
		"sesskey", "1687554231",
		"proof",   "0",
		"id",      "1",
		"final"
	});
	
	this->Send(Encrypt(response));
	
	this->_LogTransaction("<--", response);
}

/*
	Request:
		\newgame\\connid\1687554231\sesskey\197854479\final\
*/
void GameStats::Client::requestNewGame(const GameSpy::Parameter& parameter)
{
	
}

/*
	\updgame\\sesskey\898654156\done\1\gamedata\
		\gametype\2\gamver\V1.31a\hostname\[Server]IamLupo\mapid\2\numplayers\0\pplayers\0\tplayed\1141
		\final\
	
	\updgame\\sesskey\1216337440\done\1\gamedata\
		\gametype\1\gamver\V1.31a\hostname\[Server]IamLupo\mapid\2\numplayers\2\pplayers\2\tplayed\88
		\clanid_t0\0\country_t0\2\victory_t0\2\clanid_t1\0\country_t1\1
		\auth_0\1413c48006d6c3c7aca3cb558ca9ae07\bod_0\0\havd_0\0\hed_0\0\k1_0\0\k2_0\0\k3_0\0\k4_0\0\k5_0\0\kills_0\0
			\lavd_0\0\mavd_0\0\medals_0\1069580287\mv_0\0\ngp_0\0\pid_0\10036819\pld_0\0\pph_0\5239\rank_0\13
			\s1_0\0\s2_0\1\s3_0\0\s4_0\0\s5_0\0\score_0\3\suicides_0\0\time_0\71\tk_0\0\ttb_0\0
		\auth_1\1413c48006d6c3c7aca3cb558ca9ae07\bod_1\0\havd_1\0\hed_1\0\k1_1\0\k2_1\0\k3_1\0\k4_1\0\k5_1\0\kills_1\0
			\lavd_1\0\mavd_1\0\medals_1\1069580287\mv_1\0\ngp_1\1\pid_1\10037049\pld_1\0\pph_1\5175\rank_1\13
			\s1_1\1\s2_1\0\s3_1\0\s4_1\0\s5_1\0\score_1\0\suicides_1\0\time_1\88\tk_1\0\ttb_1\1
		\final\
*/
void GameStats::Client::requestUpdateGame(const GameSpy::Parameter& parameter)
{
	int offset = 8;
	std::string key, value;
	Battlefield::GameStat game_stat;
	Battlefield::GameServer game_server;
	
	// Set game server ip
	game_server.SetIp(this->GetIP());
	
	// Check game server information in database
	g_database->queryGameServerByIp(game_server);
	
	if(!game_server.isVerified())
	{
		Logger::warning("Server is not verified. Go to the database and verify the server.");
		return;
	}
	
	// Read Game stat information
	while(parameter.size() > offset + 1)
	{
		key = parameter[offset];
		value = parameter[offset + 1];
		
		if (key.find("auth_") != std::string::npos)
			break;
		
		// Update
		if(key == "gametype")        game_stat.SetGameType(value);
		else if(key == "gamver")     game_stat.SetGameVersion(value);
		else if(key == "hostname")   game_stat.SetHostName(value);
		else if(key == "mapid")      game_stat.SetMapId(value);
		else if(key == "numplayers") game_stat.SetNumPlayers(value);
		else if(key == "pplayers")   game_stat.SetPPlayers(value);
		else if(key == "tplayed")    game_stat.SetTimePlayed(value);
		else if(key == "clanid_t0")  game_stat.SetTeam1ClanId(value);
		else if(key == "clanid_t1")  game_stat.SetTeam2ClanId(value);
		else if(key == "country_t0") game_stat.SetTeam1Country(value);
		else if(key == "country_t1") game_stat.SetTeam2Country(value);
		else if(key == "victory_t0") game_stat.SetTeam1Victory(value);
		else if(key == "victory_t1") game_stat.SetTeam2Victory(value);
		
		// Debug
		//Logger::debug(key + " = " + value);
		
		offset += 2;
	}
	
	// Debug
	//game_stat.Debug();
	//Logger::debug("=============================");
	
	// Get player index out of key "auth_"
	uint8_t player_index = this->_GetPlayerIndex(key);
	
	// Read Game stat player information
	while(parameter.size() > offset + 1)
	{
		Battlefield::GameStatPlayer gsplayer;
		
		// Debug
		//Logger::debug("player_index = " + std::to_string(player_index));
		
		while(parameter.size() > offset + 1)
		{
			key = parameter[offset];
			value = parameter[offset + 1];
			
			// If we found the new auth_ key then we create new GameStatPlayer
			if (key.find("auth_") != std::string::npos && key != "auth_" + std::to_string(player_index))
				break;
			
			// Update
			if(     key == "auth_"     + std::to_string(player_index)) gsplayer.SetAuth(value);
			else if(key == "pid_"      + std::to_string(player_index)) gsplayer.SetProfileId(value);
			else if(key == "team_"     + std::to_string(player_index)) gsplayer.SetTeam(value);
			else if(key == "score_"    + std::to_string(player_index)) gsplayer.SetScore(value);
			else if(key == "rank_"     + std::to_string(player_index)) gsplayer.SetRank(value);
			else if(key == "pph_"      + std::to_string(player_index)) gsplayer.SetPPH(value);
			else if(key == "kills_"    + std::to_string(player_index)) gsplayer.SetKills(value);
			else if(key == "deaths_"   + std::to_string(player_index)) gsplayer.SetDeaths(value);
			else if(key == "suicides_" + std::to_string(player_index)) gsplayer.SetSuicides(value);
			else if(key == "time_"     + std::to_string(player_index)) gsplayer.SetTime(value);
			else if(key == "lavd_"     + std::to_string(player_index)) gsplayer.SetLAVsDestroyed(value);
			else if(key == "mavd_"     + std::to_string(player_index)) gsplayer.SetMAVsDestroyed(value);
			else if(key == "hed_"      + std::to_string(player_index)) gsplayer.SetHelicoptersDestroyed(value);
			else if(key == "pld_"      + std::to_string(player_index)) gsplayer.SetPlanesDestroyed(value);
			else if(key == "bod_"      + std::to_string(player_index)) gsplayer.SetBoatsDestroyed(value);
			else if(key == "k1_"       + std::to_string(player_index)) gsplayer.SetKillsAssualtKit(value);
			else if(key == "s1_"       + std::to_string(player_index)) gsplayer.SetDeathsAssualtKit(value);
			else if(key == "k2_"       + std::to_string(player_index)) gsplayer.SetKillsSniperKit(value);
			else if(key == "s2_"       + std::to_string(player_index)) gsplayer.SetDeathsSniperKit(value);
			else if(key == "k3_"       + std::to_string(player_index)) gsplayer.SetKillsSpecialOpKit(value);
			else if(key == "s3_"       + std::to_string(player_index)) gsplayer.SetDeathsSpecialOpKit(value);
			else if(key == "k4_"       + std::to_string(player_index)) gsplayer.SetKillsCombatEngineerKit(value);
			else if(key == "s4_"       + std::to_string(player_index)) gsplayer.SetDeathsCombatEngineerKit(value);
			else if(key == "k5_"       + std::to_string(player_index)) gsplayer.SetKillsSupportKit(value);
			else if(key == "s5_"       + std::to_string(player_index)) gsplayer.SetDeathsSupportKit(value);
			else if(key == "tk_"       + std::to_string(player_index)) gsplayer.SetTeamKills(value);
			else if(key == "medals_"   + std::to_string(player_index)) gsplayer.SetMedals(value);
			else if(key == "ttb_"      + std::to_string(player_index)) gsplayer.SetTotalTopPlayer(value);
			else if(key == "mv_"       + std::to_string(player_index)) gsplayer.SetTotalVictories(value);
			else if(key == "ngp_"      + std::to_string(player_index)) gsplayer.SetTotalGameSessions(value);
			
			// Debug
			//Logger::debug(key + " = " + value);
			
			offset += 2;
		}
		
		game_stat.AddPlayer(gsplayer);
		
		// Get new player index
		player_index = this->_GetPlayerIndex(key);
		
		// Debug
		//gsplayer.Debug();
		//Logger::debug("=============================");
	}
	
	// Insert GameStat in database
	g_database->insertGameStat(game_stat);
	//game_stat.Debug();
	
	for(Battlefield::GameStatPlayer gsplayer : game_stat.GetPlayers())
	{
		gsplayer.UpdatePlayer();
	}
	
	g_database->createLeaderboards();
}

/*
	Private functions
*/
void GameStats::Client::_LogTransaction(const std::string& direction, const std::string& response) const
{
	std::shared_lock<std::shared_mutex> guard2(g_mutex_settings); // settings lock  (read)
	
	bool show_console = (g_settings["gamestats"]["show_requests"].asBool() && direction == "-->") ||
						(g_settings["gamestats"]["show_responses"].asBool() && direction == "<--");
	
	Logger::info(this->GetAddress() + " " + direction + " " + response,
			Server::Type::GameStats, show_console);
}

uint8_t GameStats::Client::_GetPlayerIndex(std::string key)
{
	uint8_t player_index = 0;
	
	// Remove first 5 characters
	key.erase(0, 5);
	
	try
	{
		player_index = static_cast<uint8_t>(std::stoul(key));
	}
	catch(...) {};
	
	return player_index;
}

static std::vector<unsigned char> example_A_response = {
	0x1b, 0x0d, 0x0e, 0x39, 0x62, 0x2c, 0x1a, 0x5b, 0x25, 0x2b, 0x0d, 0x08, 0x0b, 0x34, 0x15, 0x25,
	0x74, 0x3e, 0x2b, 0x22, 0x19, 0x52, 0x22, 0x48, 0x0a, 0x65, 0x18, 0x2e, 0x05, 0x31, 0x54, 0x5c,
	0x66, 0x69, 0x6e, 0x61, 0x6c, 0x5c, 
};

static std::vector<unsigned char> example_B_request = {
	0x1b, 0x00, 0x18, 0x11, 0x3b, 0x2c, 0x25, 0x54, 0x25, 0x2a, 0x04, 0x03, 0x04, 0x3e, 0x15, 0x25,
	0x51, 0x22, 0x2e, 0x04, 0x01, 0x01, 0x62, 0x49, 0x4d, 0x01, 0x34, 0x34, 0x53, 0x31, 0x17, 0x36,
	0x03, 0x09, 0x5c, 0x2a, 0x34, 0x04, 0x31, 0x03, 0x60, 0x44, 0x1a, 0x50, 0x72, 0x71, 0x58, 0x5e,
	0x5d, 0x30, 0x44, 0x1b, 0x03, 0x73, 0x24, 0x56, 0x5d, 0x00, 0x31, 0x16, 0x1f, 0x0a, 0x7c, 0x23,
	0x58, 0x09, 0x5c, 0x6b, 0x45, 0x4f, 0x02, 0x18, 0x37, 0x0e, 0x1f, 0x11, 0x0f, 0x40, 0x25, 0x5a,
	0x20, 0x1b, 0x50, 0x5c, 0x66, 0x69, 0x6e, 0x61, 0x6c, 0x5c, 
};

static std::vector<unsigned char> example_B_response = {
	0x1b, 0x0d, 0x0e, 0x39, 0x61, 0x2c, 0x0a, 0x56, 0x37, 0x34, 0x0a, 0x08, 0x1c, 0x0f, 0x41, 0x4f,
	0x0b, 0x73, 0x72, 0x54, 0x59, 0x57, 0x60, 0x41, 0x25, 0x43, 0x36, 0x28, 0x0e, 0x0b, 0x39, 0x63,
	0x2c, 0x10, 0x57, 0x18, 0x76, 0x5c, 0x66, 0x69, 0x6e, 0x61, 0x6c, 0x5c, 
};

static std::vector<unsigned char> example_C_request = {
	0x1b, 0x0f, 0x08, 0x12, 0x34, 0x11, 0x14, 0x56, 0x18, 0x1b, 0x02, 0x02, 0x0b, 0x3d, 0x19, 0x1d,
	0x6f, 0x75, 0x71, 0x59, 0x5a, 0x50, 0x66, 0x44, 0x4b, 0x00, 0x75, 0x1b, 0x12, 0x08, 0x16, 0x20,
	0x1b, 0x1c, 0x4a, 0x18, 0x76, 0x58, 0x5a, 0x5d, 0x66, 0x44, 0x4d, 0x04, 0x7d, 0x5c, 0x66, 0x69,
	0x6e, 0x61, 0x6c, 0x5c, 
};

/*
	\updgame\\sesskey\349913892\done\1\gamedata\
		\gametype\1\gamver\V1.31a\hostname\[CQ]BF2MC-SERVER1\mapid\0\numplayers\2\pplayers\2\tplayed\63\clanid_t0\0\country_t0\3\victory_t0\3\clanid_t1\0\country_t1\1\victory_t1\3
		\auth_0\0\bod_0\0\havd_0\0\hed_0\0\k1_0\0\k2_0\0\k3_0\0\k4_0\0\k5_0\0\kills_0\0\lavd_0\0\mavd_0\0\medals_0\0\mv_0\0\ngp_0\0\pid_0\0\pld_0\0\pph_0\0\rank_0\0\s1_0\0\s2_0\0\s3_0\0\s4_0\0\s5_0\0\score_0\0\suicides_0\0\time_0\0\tk_0\0\ttb_0\0
		\auth_1\0\bod_1\0\havd_1\0\hed_1\0\k1_1\0\k2_1\0\k3_1\0\k4_1\0\k5_1\0\kills_1\0\lavd_1\0\mavd_1\0\medals_1\0\mv_1\0\ngp_1\0\pid_1\0\pld_1\0\pph_1\0\rank_1\0\s1_1\0\s2_1\0\s3_1\0\s4_1\0\s5_1\0\score_1\0\suicides_1\0\time_1\0\tk_1\0\ttb_1\0
		\final\
*/
static std::vector<unsigned char> example_D_request = {
	0x1b, 0x14, 0x1d, 0x01, 0x34, 0x11, 0x14, 0x56, 0x18, 0x1b, 0x12, 0x08, 0x16, 0x20,
	0x1b, 0x1c, 0x4a, 0x18, 0x74, 0x55, 0x54, 0x5c, 0x62, 0x43, 0x41, 0x0a, 0x76, 0x1b, 0x05, 0x02,
	0x0b, 0x36, 0x2c, 0x48, 0x6f, 0x23, 0x26, 0x0c, 0x08, 0x01, 0x32, 0x04, 0x18, 0x6f, 0x45, 0x20,
	0x00, 0x00, 0x00, 0x27, 0x09, 0x09, 0x56, 0x45, 0x76, 0x60, 0x0a, 0x04, 0x3e, 0x06, 0x1c, 0x41,
	0x45, 0x11, 0x50, 0x43, 0x56, 0x62, 0x11, 0x78, 0x5b, 0x2b, 0x34, 0x15, 0x03, 0x04, 0x3e, 0x15,
	0x78, 0x68, 0x07, 0x16, 0x3c, 0x2f, 0x23, 0x61, 0x3d, 0x3a, 0x1e, 0x17, 0x02, 0x33, 0x3b, 0x20,
	0x01, 0x41, 0x78, 0x5e, 0x25, 0x37, 0x08, 0x09, 0x64, 0x63, 0x71, 0x17, 0x46, 0x29, 0x37, 0x0d,
	0x0c, 0x1c, 0x36, 0x02, 0x0a, 0x32, 0x76, 0x46, 0x11, 0x1d, 0x09, 0x32, 0x09, 0x1c, 0x41, 0x37,
	0x46, 0x53, 0x6c, 0x11, 0x23, 0x1c, 0x18, 0x4a, 0x21, 0x23, 0x60, 0x5b, 0x56, 0x52, 0x13, 0x15,
	0x52, 0x2a, 0x2e, 0x05, 0x32, 0x11, 0x63, 0x71, 0x49, 0x32, 0x27, 0x28, 0x14, 0x03, 0x11, 0x21,
	0x09, 0x26, 0x47, 0x74, 0x46, 0x52, 0x6c, 0x13, 0x3a, 0x13, 0x0d, 0x5c, 0x36, 0x3e, 0x3e, 0x19,
	0x55, 0x52, 0x43, 0x78, 0x50, 0x28, 0x26, 0x0f, 0x04, 0x01, 0x0c, 0x04, 0x48, 0x32, 0x74, 0x46,
	0x02, 0x02, 0x10, 0x3d, 0x04, 0x0b, 0x4a, 0x1b, 0x33, 0x50, 0x6c, 0x54, 0x52, 0x06, 0x10, 0x50,
	0x30, 0x28, 0x13, 0x14, 0x3a, 0x27, 0x41, 0x78, 0x00, 0x45, 0x26, 0x14, 0x19, 0x0d, 0x0c, 0x40,
	0x78, 0x03, 0x45, 0x25, 0x0e, 0x09, 0x3a, 0x63, 0x71, 0x49, 0x32, 0x2c, 0x26, 0x17, 0x09, 0x3a,
	0x63, 0x71, 0x49, 0x32, 0x2c, 0x22, 0x05, 0x32, 0x55, 0x52, 0x40, 0x78, 0x58, 0x75, 0x18, 0x51,
	0x6c, 0x55, 0x52, 0x1b, 0x4b, 0x6c, 0x74, 0x46, 0x51, 0x6c, 0x0e, 0x60, 0x2f, 0x49, 0x32, 0x74,
	0x46, 0x0a, 0x59, 0x3a, 0x63, 0x71, 0x49, 0x32, 0x2f, 0x72, 0x3e, 0x5d, 0x64, 0x63, 0x71, 0x12,
	0x5a, 0x28, 0x2b, 0x12, 0x32, 0x55, 0x52, 0x40, 0x78, 0x5f, 0x25, 0x31, 0x05, 0x32, 0x55, 0x52,
	0x40, 0x78, 0x5e, 0x25, 0x31, 0x05, 0x32, 0x55, 0x52, 0x40, 0x78, 0x5e, 0x21, 0x23, 0x00, 0x01,
	0x16, 0x0c, 0x40, 0x78, 0x03, 0x45, 0x2a, 0x17, 0x32, 0x55, 0x52, 0x40, 0x78, 0x5d, 0x23, 0x37,
	0x3e, 0x5d, 0x64, 0x63, 0x71, 0x09, 0x5a, 0x20, 0x18, 0x51, 0x6c, 0x55, 0x52, 0x00, 0x15, 0x57,
	0x1b, 0x77, 0x60, 0x5d, 0x64, 0x23, 0x00, 0x11, 0x6c, 0x74, 0x46, 0x51, 0x6c, 0x17, 0x32, 0x1e,
	0x12, 0x6c, 0x74, 0x46, 0x51, 0x6c, 0x16, 0x62, 0x2f, 0x49, 0x32, 0x74, 0x46, 0x12, 0x5f, 0x3a,
	0x63, 0x71, 0x49, 0x32, 0x37, 0x74, 0x3e, 0x5d, 0x64, 0x63, 0x71, 0x0a, 0x07, 0x1b, 0x77, 0x60,
	0x5d, 0x64, 0x20, 0x45, 0x26, 0x03, 0x45, 0x77, 0x60, 0x1e, 0x06, 0x3c, 0x02, 0x1c, 0x6c, 0x74,
	0x46, 0x51, 0x6c, 0x16, 0x26, 0x19, 0x1a, 0x5a, 0x20, 0x22, 0x12, 0x32, 0x55, 0x52, 0x40, 0x78,
	0x47, 0x2d, 0x2a, 0x04, 0x32, 0x55, 0x52, 0x40, 0x78, 0x47, 0x2f, 0x18, 0x51, 0x6c, 0x55, 0x52,
	0x04, 0x0d, 0x51, 0x1b, 0x77, 0x60, 0x5d, 0x64, 0x32, 0x05, 0x0d, 0x5b, 0x1b, 0x76, 0x60, 0x5d,
	0x64, 0x31, 0x1f, 0x1d, 0x6c, 0x75, 0x46, 0x51, 0x6c, 0x0d, 0x32, 0x06, 0x1d, 0x6c, 0x75, 0x46,
	0x51, 0x6c, 0x0d, 0x36, 0x14, 0x26, 0x02, 0x45, 0x77, 0x60, 0x06, 0x54, 0x0c, 0x41, 0x78, 0x03,
	0x45, 0x2c, 0x53, 0x32, 0x54, 0x52, 0x40, 0x78, 0x58, 0x77, 0x18, 0x50, 0x6c, 0x55, 0x52, 0x1b,
	0x4d, 0x6c, 0x75, 0x46, 0x51, 0x6c, 0x0e, 0x66, 0x2f, 0x48, 0x32, 0x74, 0x46, 0x0a, 0x04, 0x09,
	0x3f, 0x03, 0x26, 0x02, 0x45, 0x77, 0x60, 0x01, 0x04, 0x25, 0x14, 0x26, 0x02, 0x45, 0x77, 0x60,
	0x00, 0x04, 0x25, 0x14, 0x26, 0x02, 0x45, 0x77, 0x60, 0x00, 0x00, 0x37, 0x11, 0x15, 0x40, 0x1b,
	0x76, 0x60, 0x5d, 0x64, 0x3e, 0x06, 0x26, 0x02, 0x45, 0x77, 0x60, 0x03, 0x02, 0x23, 0x2f, 0x48,
	0x32, 0x74, 0x46, 0x11, 0x04, 0x01, 0x0c, 0x41, 0x78, 0x03, 0x45, 0x37, 0x0d, 0x09, 0x3a, 0x62,
	0x71, 0x49, 0x32, 0x34, 0x37, 0x09, 0x32, 0x54, 0x52, 0x40, 0x78, 0x41, 0x25, 0x29, 0x0a, 0x32,
	0x54, 0x52, 0x40, 0x78, 0x40, 0x75, 0x18, 0x50, 0x6c, 0x55, 0x52, 0x03, 0x4b, 0x6c, 0x75, 0x46,
	0x51, 0x6c, 0x16, 0x60, 0x2f, 0x48, 0x32, 0x74, 0x46, 0x12, 0x59, 0x3a, 0x62, 0x71, 0x49, 0x32,
	0x37, 0x72, 0x3e, 0x5c, 0x64, 0x63, 0x71, 0x0a, 0x50, 0x2b, 0x35, 0x04, 0x32, 0x54, 0x52, 0x40,
	0x78, 0x40, 0x31, 0x2e, 0x02, 0x04, 0x01, 0x36, 0x03, 0x26, 0x02, 0x45, 0x77, 0x60, 0x19, 0x0c,
	0x3e, 0x15, 0x26, 0x02, 0x45, 0x77, 0x60, 0x19, 0x0e, 0x0c, 0x41, 0x78, 0x03, 0x45, 0x33, 0x15,
	0x0f, 0x3a, 0x62, 0x71, 0x49, 0x5c, 0x66, 0x69, 0x6e, 0x61, 0x6c, 0x5c, 
};

/*
	\updgame\\sesskey\349913882\done\1\gamedata\\gametype\1\gamver\V1.31a\hostname\[CQ]BF2MC-SERVER1\mapid\0\numplayers\6\pplayers\6\tplayed\1200\clanid_t0\0\country_t0\3\victory_t0\1\clanid_t1\0\country_t1\1
		\auth_0\0\bod_0\0\havd_0\0\hed_0\0\k1_0\0\k2_0\0\k3_0\0\k4_0\0\k5_0\0\kills_0\0\lavd_0\0\mavd_0\0\medals_0\0\mv_0\0\ngp_0\0\pid_0\0\pld_0\0\pph_0\0\rank_0\0\s1_0\0\s2_0\0\s3_0\0\s4_0\0\s5_0\0\score_0\0\suicides_0\0\time_0\0\tk_0\0\ttb_0\0
		\auth_1\0\bod_1\0\havd_1\0\hed_1\0\k1_1\0\k2_1\0\k3_1\0\k4_1\0\k5_1\0\kills_1\0\lavd_1\0\mavd_1\0\medals_1\0\mv_1\0\ngp_1\0\pid_1\0\pld_1\0\pph_1\0\rank_1\0\s1_1\0\s2_1\0\s3_1\0\s4_1\0\s5_1\0\score_1\0\suicides_1\0\time_1\0\tk_1\0\ttb_1\0
		\auth_2\0\bod_2\0\havd_2\0\hed_2\0\k1_2\0\k2_2\0\k3_2\0\k4_2\0\k5_2\0\kills_2\0\lavd_2\0\mavd_2\0\medals_2\0\mv_2\0\ngp_2\0\pid_2\0\pld_2\0\pph_2\0\rank_2\0\s1_2\0\s2_2\0\s3_2\0\s4_2\0\s5_2\0\score_2\0\suicides_2\0\time_2\0\tk_2\0\ttb_2\0
		\auth_3\0\bod_3\0\havd_3\0\hed_3\0\k1_3\0\k2_3\0\k3_3\0\k4_3\0\k5_3\0\kills_3\0\lavd_3\0\mavd_3\0\medals_3\0\mv_3\0\ngp_3\0\pid_3\0\pld_3\0\pph_3\0\rank_3\0\s1_3\0\s2_3\0\s3_3\0\s4_3\0\s5_3\0\score_3\0\suicides_3\0\time_3\0\tk_3\0\ttb_3\0
		\auth_4\0\bod_4\0\havd_4\0\hed_4\0\k1_4\0\k2_4\0\k3_4\0\k4_4\0\k5_4\0\kills_4\0\lavd_4\0\mavd_4\0\medals_4\0\mv_4\0\ngp_4\0\pid_4\0\pld_4\0\pph_4\0\rank_4\0\s1_4\0\s2_4\0\s3_4\0\s4_4\0\s5_4\0\score_4\0\suicides_4\0\time_4\0\tk_4\0\ttb_4\0
		\auth_5\0\bod_5\0\havd_5\0\hed_5\0\k1_5\0\k2_5\0\k3_5\0\k4_5\0\k5_5\0\kills_5\0\lavd_5\0\mavd_5\0\medals_5\0\mv_5\0\ngp_5\0\pid_5\0\pld_5\0\pph_5\0\rank_5\0\s1_5\0\s2_5\0\s3_5\0\s4_5\0\s5_5\0\score_5\0\suicides_5\0\time_5\0\tk_5\0\ttb_5\0
		\final\
*/
static std::vector<unsigned char> example_E_request = {
	0x1b, 0x14, 0x1d, 0x01, 0x34, 0x11, 0x14, 0x56, 0x18, 0x1b, 0x12, 0x08, 0x16, 0x20,
	0x1b, 0x1c, 0x4a, 0x18, 0x74, 0x55, 0x54, 0x5c, 0x62, 0x43, 0x41, 0x0b, 0x76, 0x1b, 0x05, 0x02,
	0x0b, 0x36, 0x2c, 0x48, 0x6f, 0x23, 0x26, 0x0c, 0x08, 0x01, 0x32, 0x04, 0x18, 0x6f, 0x45, 0x20,
	0x00, 0x00, 0x00, 0x27, 0x09, 0x09, 0x56, 0x45, 0x76, 0x60, 0x0a, 0x04, 0x3e, 0x06, 0x1c, 0x41,
	0x45, 0x11, 0x50, 0x43, 0x56, 0x62, 0x11, 0x78, 0x5b, 0x2b, 0x34, 0x15, 0x03, 0x04, 0x3e, 0x15,
	0x78, 0x68, 0x07, 0x16, 0x3c, 0x2f, 0x23, 0x61, 0x3d, 0x3a, 0x1e, 0x17, 0x02, 0x33, 0x3b, 0x20,
	0x01, 0x41, 0x78, 0x5e, 0x25, 0x37, 0x08, 0x09, 0x64, 0x63, 0x71, 0x17, 0x46, 0x29, 0x37, 0x0d,
	0x0c, 0x1c, 0x36, 0x02, 0x0a, 0x32, 0x72, 0x46, 0x11, 0x1d, 0x09, 0x32, 0x09, 0x1c, 0x41, 0x37,
	0x46, 0x57, 0x6c, 0x11, 0x23, 0x1c, 0x18, 0x4a, 0x21, 0x23, 0x60, 0x5c, 0x57, 0x63, 0x40, 0x78,
	0x50, 0x28, 0x26, 0x0f, 0x04, 0x01, 0x0c, 0x04, 0x49, 0x32, 0x74, 0x46, 0x02, 0x02, 0x10, 0x3d,
	0x04, 0x0b, 0x4a, 0x1b, 0x33, 0x51, 0x6c, 0x56, 0x52, 0x06, 0x10, 0x50, 0x30, 0x28, 0x13, 0x14,
	0x3a, 0x27, 0x40, 0x78, 0x02, 0x45, 0x24, 0x0d, 0x0c, 0x0b, 0x3a, 0x14, 0x26, 0x47, 0x75, 0x46,
	0x51, 0x6c, 0x06, 0x3c, 0x05, 0x17, 0x47, 0x36, 0x3e, 0x3e, 0x19, 0x54, 0x52, 0x41, 0x78, 0x52,
	0x31, 0x33, 0x09, 0x32, 0x55, 0x52, 0x40, 0x78, 0x51, 0x2b, 0x23, 0x3e, 0x5d, 0x64, 0x63, 0x71,
	0x11, 0x52, 0x32, 0x23, 0x3e, 0x5d, 0x64, 0x63, 0x71, 0x11, 0x56, 0x20, 0x18, 0x51, 0x6c, 0x55,
	0x52, 0x1b, 0x48, 0x6c, 0x74, 0x46, 0x51, 0x6c, 0x0e, 0x61, 0x2f, 0x49, 0x32, 0x74, 0x46, 0x0a,
	0x5e, 0x3a, 0x63, 0x71, 0x49, 0x32, 0x2f, 0x73, 0x3e, 0x5d, 0x64, 0x63, 0x71, 0x12, 0x06, 0x1b,
	0x77, 0x60, 0x5d, 0x64, 0x38, 0x19, 0x15, 0x5f, 0x37, 0x18, 0x51, 0x6c, 0x55, 0x52, 0x1c, 0x18,
	0x45, 0x20, 0x18, 0x51, 0x6c, 0x55, 0x52, 0x1d, 0x18, 0x45, 0x20, 0x18, 0x51, 0x6c, 0x55, 0x52,
	0x1d, 0x1c, 0x57, 0x25, 0x2b, 0x12, 0x32, 0x55, 0x52, 0x40, 0x78, 0x5e, 0x32, 0x18, 0x51, 0x6c,
	0x55, 0x52, 0x1e, 0x1e, 0x43, 0x1b, 0x77, 0x60, 0x5d, 0x64, 0x23, 0x19, 0x1d, 0x6c, 0x74, 0x46,
	0x51, 0x6c, 0x15, 0x3f, 0x14, 0x26, 0x03, 0x45, 0x77, 0x60, 0x1d, 0x15, 0x3b, 0x2f, 0x49, 0x32,
	0x74, 0x46, 0x13, 0x0c, 0x0b, 0x38, 0x2f, 0x49, 0x32, 0x74, 0x46, 0x12, 0x5c, 0x3a, 0x63, 0x71,
	0x49, 0x32, 0x37, 0x75, 0x3e, 0x5d, 0x64, 0x63, 0x71, 0x0a, 0x00, 0x1b, 0x77, 0x60, 0x5d, 0x64,
	0x20, 0x44, 0x26, 0x03, 0x45, 0x77, 0x60, 0x1e, 0x50, 0x0c, 0x40, 0x78, 0x03, 0x45, 0x34, 0x02,
	0x02, 0x17, 0x36, 0x2f, 0x49, 0x32, 0x74, 0x46, 0x12, 0x18, 0x0c, 0x30, 0x19, 0x1d, 0x56, 0x37,
	0x18, 0x51, 0x6c, 0x55, 0x52, 0x04, 0x10, 0x5e, 0x21, 0x18, 0x51, 0x6c, 0x55, 0x52, 0x04, 0x12,
	0x6c, 0x74, 0x46, 0x51, 0x6c, 0x11, 0x27, 0x12, 0x26, 0x03, 0x45, 0x77, 0x60, 0x0c, 0x10, 0x27,
	0x18, 0x26, 0x02, 0x45, 0x77, 0x60, 0x0f, 0x0a, 0x37, 0x2f, 0x48, 0x32, 0x74, 0x46, 0x09, 0x0c,
	0x13, 0x37, 0x2f, 0x48, 0x32, 0x74, 0x46, 0x09, 0x08, 0x01, 0x0c, 0x41, 0x78, 0x03, 0x45, 0x2c,
	0x50, 0x32, 0x54, 0x52, 0x40, 0x78, 0x58, 0x76, 0x18, 0x50, 0x6c, 0x55, 0x52, 0x1b, 0x4a, 0x6c,
	0x75, 0x46, 0x51, 0x6c, 0x0e, 0x67, 0x2f, 0x48, 0x32, 0x74, 0x46, 0x0a, 0x58, 0x3a, 0x62, 0x71,
	0x49, 0x32, 0x2f, 0x2e, 0x0d, 0x01, 0x16, 0x0c, 0x41, 0x78, 0x03, 0x45, 0x2b, 0x00, 0x1b, 0x01,
	0x0c, 0x41, 0x78, 0x03, 0x45, 0x2a, 0x00, 0x1b, 0x01, 0x0c, 0x41, 0x78, 0x03, 0x45, 0x2a, 0x04,
	0x09, 0x04, 0x3f, 0x03, 0x26, 0x02, 0x45, 0x77, 0x60, 0x00, 0x13, 0x0c, 0x41, 0x78, 0x03, 0x45,
	0x29, 0x06, 0x1d, 0x3a, 0x62, 0x71, 0x49, 0x32, 0x34, 0x2e, 0x05, 0x32, 0x54, 0x52, 0x40, 0x78,
	0x43, 0x28, 0x23, 0x3e, 0x5c, 0x64, 0x63, 0x71, 0x09, 0x43, 0x2c, 0x18, 0x50, 0x6c, 0x55, 0x52,
	0x02, 0x18, 0x5d, 0x2f, 0x18, 0x50, 0x6c, 0x55, 0x52, 0x03, 0x48, 0x6c, 0x75, 0x46, 0x51, 0x6c,
	0x16, 0x61, 0x2f, 0x48, 0x32, 0x74, 0x46, 0x12, 0x5e, 0x3a, 0x62, 0x71, 0x49, 0x32, 0x37, 0x73,
	0x3e, 0x5c, 0x64, 0x63, 0x71, 0x0a, 0x06, 0x1b, 0x76, 0x60, 0x5d, 0x64, 0x20, 0x13, 0x16, 0x41,
	0x21, 0x18, 0x50, 0x6c, 0x55, 0x52, 0x03, 0x0c, 0x5a, 0x27, 0x2e, 0x05, 0x08, 0x16, 0x0c, 0x41,
	0x78, 0x03, 0x45, 0x33, 0x08, 0x00, 0x00, 0x0c, 0x41, 0x78, 0x03, 0x45, 0x33, 0x0a, 0x32, 0x54,
	0x52, 0x40, 0x78, 0x47, 0x30, 0x25, 0x3e, 0x5c, 0x64, 0x63, 0x71, 0x18, 0x46, 0x30, 0x2f, 0x3e,
	0x5f, 0x64, 0x63, 0x71, 0x1b, 0x5c, 0x20, 0x18, 0x53, 0x6c, 0x55, 0x52, 0x18, 0x18, 0x45, 0x20,
	0x18, 0x53, 0x6c, 0x55, 0x52, 0x18, 0x1c, 0x57, 0x1b, 0x75, 0x60, 0x5d, 0x64, 0x38, 0x41, 0x26,
	0x01, 0x45, 0x77, 0x60, 0x06, 0x57, 0x0c, 0x42, 0x78, 0x03, 0x45, 0x2c, 0x52, 0x32, 0x57, 0x52,
	0x40, 0x78, 0x58, 0x70, 0x18, 0x53, 0x6c, 0x55, 0x52, 0x1b, 0x4c, 0x6c, 0x76, 0x46, 0x51, 0x6c,
	0x0e, 0x3a, 0x1c, 0x15, 0x40, 0x1b, 0x75, 0x60, 0x5d, 0x64, 0x3f, 0x11, 0x0f, 0x57, 0x1b, 0x75,
	0x60, 0x5d, 0x64, 0x3e, 0x11, 0x0f, 0x57, 0x1b, 0x75, 0x60, 0x5d, 0x64, 0x3e, 0x15, 0x1d, 0x52,
	0x28, 0x34, 0x3e, 0x5f, 0x64, 0x63, 0x71, 0x14, 0x45, 0x1b, 0x75, 0x60, 0x5d, 0x64, 0x3d, 0x17,
	0x09, 0x6c, 0x76, 0x46, 0x51, 0x6c, 0x15, 0x3a, 0x14, 0x26, 0x01, 0x45, 0x77, 0x60, 0x1d, 0x09,
	0x37, 0x2f, 0x4b, 0x32, 0x74, 0x46, 0x11, 0x1d, 0x0d, 0x0c, 0x42, 0x78, 0x03, 0x45, 0x35, 0x00,
	0x03, 0x0e, 0x0c, 0x42, 0x78, 0x03, 0x45, 0x34, 0x50, 0x32, 0x57, 0x52, 0x40, 0x78, 0x40, 0x76,
	0x18, 0x53, 0x6c, 0x55, 0x52, 0x03, 0x4a, 0x6c, 0x76, 0x46, 0x51, 0x6c, 0x16, 0x67, 0x2f, 0x4b,
	0x32, 0x74, 0x46, 0x12, 0x58, 0x3a, 0x61, 0x71, 0x49, 0x32, 0x37, 0x24, 0x0e, 0x1f, 0x00, 0x0c,
	0x42, 0x78, 0x03, 0x45, 0x34, 0x14, 0x04, 0x06, 0x3a, 0x14, 0x1c, 0x40, 0x1b, 0x75, 0x60, 0x5d,
	0x64, 0x27, 0x19, 0x14, 0x56, 0x1b, 0x75, 0x60, 0x5d, 0x64, 0x27, 0x1b, 0x26, 0x01, 0x45, 0x77,
	0x60, 0x19, 0x11, 0x31, 0x2f, 0x4b, 0x32, 0x74, 0x46, 0x00, 0x18, 0x11, 0x3b, 0x2f, 0x4a, 0x32,
	0x74, 0x46, 0x03, 0x02, 0x01, 0x0c, 0x43, 0x78, 0x03, 0x45, 0x2f, 0x00, 0x1b, 0x01, 0x0c, 0x43,
	0x78, 0x03, 0x45, 0x2f, 0x04, 0x09, 0x3a, 0x60, 0x71, 0x49, 0x32, 0x2f, 0x76, 0x3e, 0x5e, 0x64,
	0x63, 0x71, 0x12, 0x01, 0x1b, 0x74, 0x60, 0x5d, 0x64, 0x38, 0x43, 0x26, 0x00, 0x45, 0x77, 0x60,
	0x06, 0x51, 0x0c, 0x43, 0x78, 0x03, 0x45, 0x2c, 0x54, 0x32, 0x56, 0x52, 0x40, 0x78, 0x58, 0x2d,
	0x2b, 0x0d, 0x1e, 0x3a, 0x60, 0x71, 0x49, 0x32, 0x28, 0x26, 0x17, 0x09, 0x3a, 0x60, 0x71, 0x49,
	0x32, 0x29, 0x26, 0x17, 0x09, 0x3a, 0x60, 0x71, 0x49, 0x32, 0x29, 0x22, 0x05, 0x0c, 0x09, 0x20,
	0x2f, 0x4a, 0x32, 0x74, 0x46, 0x0c, 0x1b, 0x3a, 0x60, 0x71, 0x49, 0x32, 0x2a, 0x20, 0x11, 0x32,
	0x56, 0x52, 0x40, 0x78, 0x43, 0x2d, 0x23, 0x3e, 0x5e, 0x64, 0x63, 0x71, 0x09, 0x5f, 0x20, 0x18,
	0x52, 0x6c, 0x55, 0x52, 0x00, 0x09, 0x5b, 0x1b, 0x74, 0x60, 0x5d, 0x64, 0x21, 0x11, 0x17, 0x58,
	0x1b, 0x74, 0x60, 0x5d, 0x64, 0x20, 0x41, 0x26, 0x00, 0x45, 0x77, 0x60, 0x1e, 0x57, 0x0c, 0x43,
	0x78, 0x03, 0x45, 0x34, 0x52, 0x32, 0x56, 0x52, 0x40, 0x78, 0x40, 0x70, 0x18, 0x52, 0x6c, 0x55,
	0x52, 0x03, 0x4c, 0x6c, 0x77, 0x46, 0x51, 0x6c, 0x16, 0x30, 0x1f, 0x0b, 0x56, 0x1b, 0x74, 0x60,
	0x5d, 0x64, 0x20, 0x05, 0x10, 0x50, 0x2d, 0x23, 0x04, 0x1e, 0x3a, 0x60, 0x71, 0x49, 0x32, 0x30,
	0x2e, 0x0c, 0x08, 0x3a, 0x60, 0x71, 0x49, 0x32, 0x30, 0x2c, 0x3e, 0x5e, 0x64, 0x63, 0x71, 0x0d,
	0x47, 0x26, 0x18, 0x52, 0x6c, 0x55, 0x52, 0x11, 0x0c, 0x47, 0x2c, 0x18, 0x55, 0x6c, 0x55, 0x52,
	0x12, 0x16, 0x57, 0x1b, 0x73, 0x60, 0x5d, 0x64, 0x3b, 0x11, 0x0f, 0x57, 0x1b, 0x73, 0x60, 0x5d,
	0x64, 0x3b, 0x15, 0x1d, 0x6c, 0x70, 0x46, 0x51, 0x6c, 0x0e, 0x62, 0x2f, 0x4d, 0x32, 0x74, 0x46,
	0x0a, 0x5f, 0x3a, 0x67, 0x71, 0x49, 0x32, 0x2f, 0x74, 0x3e, 0x59, 0x64, 0x63, 0x71, 0x12, 0x07,
	0x1b, 0x73, 0x60, 0x5d, 0x64, 0x38, 0x45, 0x26, 0x07, 0x45, 0x77, 0x60, 0x06, 0x0c, 0x3f, 0x1c,
	0x0a, 0x6c, 0x70, 0x46, 0x51, 0x6c, 0x09, 0x32, 0x06, 0x1d, 0x6c, 0x70, 0x46, 0x51, 0x6c, 0x08,
	0x32, 0x06, 0x1d, 0x6c, 0x70, 0x46, 0x51, 0x6c, 0x08, 0x36, 0x14, 0x18, 0x5f, 0x37, 0x18, 0x55,
	0x6c, 0x55, 0x52, 0x1d, 0x0f, 0x6c, 0x70, 0x46, 0x51, 0x6c, 0x0b, 0x34, 0x00, 0x26, 0x07, 0x45,
	0x77, 0x60, 0x1d, 0x0c, 0x37, 0x2f, 0x4d, 0x32, 0x74, 0x46, 0x11, 0x01, 0x01, 0x0c, 0x44, 0x78,
	0x03, 0x45, 0x37, 0x11, 0x05, 0x3a, 0x67, 0x71, 0x49, 0x32, 0x36, 0x26, 0x0f, 0x06, 0x3a, 0x67,
	0x71, 0x49, 0x32, 0x37, 0x76, 0x3e, 0x59, 0x64, 0x63, 0x71, 0x0a, 0x01, 0x1b, 0x73, 0x60, 0x5d,
	0x64, 0x20, 0x43, 0x26, 0x07, 0x45, 0x77, 0x60, 0x1e, 0x51, 0x0c, 0x44, 0x78, 0x03, 0x45, 0x34,
	0x54, 0x32, 0x51, 0x52, 0x40, 0x78, 0x40, 0x27, 0x28, 0x13, 0x08, 0x3a, 0x67, 0x71, 0x49, 0x32,
	0x37, 0x32, 0x08, 0x0e, 0x0c, 0x37, 0x15, 0x0a, 0x6c, 0x70, 0x46, 0x51, 0x6c, 0x11, 0x3a, 0x1d,
	0x1c, 0x6c, 0x70, 0x46, 0x51, 0x6c, 0x11, 0x38, 0x2f, 0x4d, 0x32, 0x74, 0x46, 0x15, 0x19, 0x07,
	0x0c, 0x44, 0x78, 0x03, 0x45, 0x26, 0x14, 0x19, 0x0d, 0x0c, 0x45, 0x78, 0x03, 0x45, 0x25, 0x0e,
	0x09, 0x3a, 0x66, 0x71, 0x49, 0x32, 0x2c, 0x26, 0x17, 0x09, 0x3a, 0x66, 0x71, 0x49, 0x32, 0x2c,
	0x22, 0x05, 0x32, 0x50, 0x52, 0x40, 0x78, 0x58, 0x75, 0x18, 0x54, 0x6c, 0x55, 0x52, 0x1b, 0x4b,
	0x6c, 0x71, 0x46, 0x51, 0x6c, 0x0e, 0x60, 0x2f, 0x4c, 0x32, 0x74, 0x46, 0x0a, 0x59, 0x3a, 0x66,
	0x71, 0x49, 0x32, 0x2f, 0x72, 0x3e, 0x58, 0x64, 0x63, 0x71, 0x12, 0x5a, 0x28, 0x2b, 0x12, 0x32,
	0x50, 0x52, 0x40, 0x78, 0x5f, 0x25, 0x31, 0x05, 0x32, 0x50, 0x52, 0x40, 0x78, 0x5e, 0x25, 0x31,
	0x05, 0x32, 0x50, 0x52, 0x40, 0x78, 0x5e, 0x21, 0x23, 0x00, 0x01, 0x16, 0x0c, 0x45, 0x78, 0x03,
	0x45, 0x2a, 0x17, 0x32, 0x50, 0x52, 0x40, 0x78, 0x5d, 0x23, 0x37, 0x3e, 0x58, 0x64, 0x63, 0x71,
	0x09, 0x5a, 0x20, 0x18, 0x54, 0x6c, 0x55, 0x52, 0x00, 0x15, 0x57, 0x1b, 0x72, 0x60, 0x5d, 0x64,
	0x23, 0x00, 0x11, 0x6c, 0x71, 0x46, 0x51, 0x6c, 0x17, 0x32, 0x1e, 0x12, 0x6c, 0x71, 0x46, 0x51,
	0x6c, 0x16, 0x62, 0x2f, 0x4c, 0x32, 0x74, 0x46, 0x12, 0x5f, 0x3a, 0x66, 0x71, 0x49, 0x32, 0x37,
	0x74, 0x3e, 0x58, 0x64, 0x63, 0x71, 0x0a, 0x07, 0x1b, 0x72, 0x60, 0x5d, 0x64, 0x20, 0x45, 0x26,
	0x06, 0x45, 0x77, 0x60, 0x1e, 0x06, 0x3c, 0x02, 0x1c, 0x6c, 0x71, 0x46, 0x51, 0x6c, 0x16, 0x26,
	0x19, 0x1a, 0x5a, 0x20, 0x22, 0x12, 0x32, 0x50, 0x52, 0x40, 0x78, 0x47, 0x2d, 0x2a, 0x04, 0x32,
	0x50, 0x52, 0x40, 0x78, 0x47, 0x2f, 0x18, 0x54, 0x6c, 0x55, 0x52, 0x04, 0x0d, 0x51, 0x1b, 0x72,
	0x60, 0x5d, 0x5c, 0x66, 0x69, 0x6e, 0x61, 0x6c, 0x5c, 
};

/*
	\ka\\final\
*/
static std::vector<unsigned char> example_F_request = {
	0x1b, 0x0a, 0x0c, 0x39, 0x5c, 0x66, 0x69, 0x6e, 0x61, 0x6c, 0x5c, 
};

/*
	\newgame\\connid\1687902592\sesskey\349913882\final\
*/
static std::vector<unsigned char> example_G_request = {
	0x1b, 0x0f, 0x08, 0x12, 0x34, 0x11, 0x14, 0x56, 0x18, 0x1b, 0x02, 0x02, 0x0b, 0x3d, 0x19, 0x1d,
	0x6f, 0x75, 0x71, 0x59, 0x5a, 0x5c, 0x63, 0x42, 0x4c, 0x0a, 0x76, 0x1b, 0x12, 0x08, 0x16, 0x20,
	0x1b, 0x1c, 0x4a, 0x18, 0x74, 0x55, 0x54, 0x5c, 0x62, 0x43, 0x41, 0x0b, 0x76, 0x5c, 0x66, 0x69,
	0x6e, 0x61, 0x6c, 0x5c, 
};

/*
	\updgame\\sesskey\349913881\done\1\gamedata\\gametype\1\gamver\V1.31a\hostname\[CQ]BF2MC-SERVER1\mapid\0\numplayers\0\pplayers\0\tplayed\18\final\
*/
static std::vector<unsigned char> example_H_request = {
	0x1b, 0x14, 0x1d, 0x01, 0x34, 0x11, 0x14, 0x56, 0x18, 0x1b, 0x12, 0x08, 0x16, 0x20, 0x1b, 0x1c,
	0x4a, 0x18, 0x74, 0x55, 0x54, 0x5c, 0x62, 0x43, 0x41, 0x0b, 0x75, 0x1b, 0x05, 0x02, 0x0b, 0x36,
	0x2c, 0x48, 0x6f, 0x23, 0x26, 0x0c, 0x08, 0x01, 0x32, 0x04, 0x18, 0x6f, 0x45, 0x20, 0x00, 0x00,
	0x00, 0x27, 0x09, 0x09, 0x56, 0x45, 0x76, 0x60, 0x0a, 0x04, 0x3e, 0x06, 0x1c, 0x41, 0x45, 0x11,
	0x50, 0x43, 0x56, 0x62, 0x11, 0x78, 0x5b, 0x2b, 0x34, 0x15, 0x03, 0x04, 0x3e, 0x15, 0x78, 0x68,
	0x07, 0x16, 0x3c, 0x2f, 0x23, 0x61, 0x3d, 0x3a, 0x1e, 0x17, 0x02, 0x33, 0x3b, 0x20, 0x01, 0x41,
	0x78, 0x5e, 0x25, 0x37, 0x08, 0x09, 0x64, 0x63, 0x71, 0x17, 0x46, 0x29, 0x37, 0x0d, 0x0c, 0x1c,
	0x36, 0x02, 0x0a, 0x32, 0x74, 0x46, 0x11, 0x1d, 0x09, 0x32, 0x09, 0x1c, 0x41, 0x37, 0x46, 0x51,
	0x6c, 0x11, 0x23, 0x1c, 0x18, 0x4a, 0x21, 0x23, 0x60, 0x5c, 0x5d, 0x5c, 0x66, 0x69, 0x6e, 0x61,
	0x6c, 0x5c, 
};

std::string GameStats::Client::Decrypt(const std::vector<unsigned char>& request)
{
	std::string msg;
	const char key[10] = "GameSpy3D";
	
	for(int i = 0; i < request.size() - 7; i++)
	{
		char v = key[(i % 9)] ^ request[i];
		
		if(v > 0x20 && v < 0x7F)
			msg += v;
		else
			msg += "\\";
	}
	
	msg += "\\final\\";
	
	return msg;
}

std::vector<unsigned char> GameStats::Client::Encrypt(const std::string& response)
{
	std::vector<unsigned char> enc_response;
	const char key[10] = "GameSpy3D";
	
	for(int i = 0; i < response.size() - 7; i++)
	{
		enc_response.push_back(key[(i % 9)] ^ response[i]);
	}
	
	// Add final at the end
	enc_response.push_back(0x5c);
	enc_response.push_back(0x66);
	enc_response.push_back(0x69);
	enc_response.push_back(0x6e);
	enc_response.push_back(0x61);
	enc_response.push_back(0x6c);
	enc_response.push_back(0x5c);
	
	return enc_response;
}

void GameStats::Client::Test()
{
	Logger::debug(Decrypt(example_A_response));
	
	Logger::debug(Decrypt(example_B_request));
	Logger::debug(Decrypt(example_B_response));
	
	Logger::debug(Decrypt(example_C_request));
	
	Logger::debug(Decrypt(example_D_request));
	
	Logger::debug(Decrypt(example_E_request));
	
	Logger::debug(Decrypt(example_F_request));
	
	Logger::debug(Decrypt(example_G_request));
	
	Logger::debug(Decrypt(example_H_request));
}

