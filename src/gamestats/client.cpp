#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <thread>

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
	this->UpdateLastRecievedTime();
}

GameStats::Client::~Client()
{
	this->Disconnect();
}

void GameStats::Client::Listen()
{
	bool isDisconnected = false;

	// Initialize connection send challenge
	this->requestChallenge();
	
	while(!isDisconnected)
	{
		std::vector<unsigned char> combined_buffer;
		std::string last_seven_chars = "";

		do
		{
			std::vector<unsigned char> buffer(16384, 0);

			int v = read(this->_socket, &(buffer[0]), 16384);
		
			// If error or no data is recieved we end the connection
			if(v <= 0)
			{
				isDisconnected = true;
				break;
			}
			
			// Resize buffer
			buffer.resize(v);

			this->UpdateLastRecievedTime();

			// Debug
			//std::stringstream ss;
			//for(int i = 0; i < buffer.size(); i++)
			//{
			//	ss << std::hex << std::setfill('0') << std::setw(2) << (int)(buffer[i]);
			//}
			//Logger::info("buffer = " + ss.str());

			combined_buffer.insert(combined_buffer.end(), buffer.begin(), buffer.end());

			if(combined_buffer.size() > 7)
			{
				last_seven_chars.assign(combined_buffer.end() - 7, combined_buffer.end());
			}
		} while (last_seven_chars != "\\final\\" && combined_buffer.size() < 32768);

		if(combined_buffer.size() > 7)
		{
			std::string request = Decrypt(combined_buffer);

			this->_LogTransaction("-->", request);
		
			this->onRequest(request);
		}
	}
	
	this->Disconnect();
}

void GameStats::Client::Disconnect()
{
	this->Close();
	g_gamestats_server->onClientDisconnect(*this);
}


// Events

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
	
	Logger::info(this->GetAddress() + " --> Challenge", Server::Type::GameStats);

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
	Logger::info(this->GetAddress() + " --> Auth", Server::Type::GameStats);

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
	Logger::info(this->GetAddress() + " --> NewGame", Server::Type::GameStats);
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
	
	Logger::info(this->GetAddress() + " --> UpdateGame", Server::Type::GameStats);

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
		else if(key == "score0")     game_stat.SetTeam1Score(value);
		else if(key == "score1")     game_stat.SetTeam2Score(value);
		
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
			if(     key == "auth_"       + std::to_string(player_index)) gsplayer.SetAuth(value);
			else if(key == "pid_"        + std::to_string(player_index)) gsplayer.SetProfileId(value);
			else if(key == "machine_id_" + std::to_string(player_index)) gsplayer.SetMachineId(value);
			else if(key == "team_"       + std::to_string(player_index)) gsplayer.SetTeam(value);
			else if(key == "score_"      + std::to_string(player_index)) gsplayer.SetScore(value);
			else if(key == "rank_"       + std::to_string(player_index)) gsplayer.SetRank(value);
			else if(key == "pph_"        + std::to_string(player_index)) gsplayer.SetPPH(value);
			else if(key == "kills_"      + std::to_string(player_index)) gsplayer.SetKills(value);
			else if(key == "deaths_"     + std::to_string(player_index)) gsplayer.SetDeaths(value);
			else if(key == "suicides_"   + std::to_string(player_index)) gsplayer.SetSuicides(value);
			else if(key == "time_"       + std::to_string(player_index)) gsplayer.SetTime(value);
			else if(key == "lavd_"       + std::to_string(player_index)) gsplayer.SetLAVsDestroyed(value);
			else if(key == "mavd_"       + std::to_string(player_index)) gsplayer.SetMAVsDestroyed(value);
			else if(key == "havd_"       + std::to_string(player_index)) gsplayer.SetHAVsDestroyed(value);
			else if(key == "hed_"        + std::to_string(player_index)) gsplayer.SetHelicoptersDestroyed(value);
			else if(key == "pld_"        + std::to_string(player_index)) gsplayer.SetPlanesDestroyed(value);
			else if(key == "bod_"        + std::to_string(player_index)) gsplayer.SetBoatsDestroyed(value);
			else if(key == "k1_"         + std::to_string(player_index)) gsplayer.SetKillsAssualtKit(value);
			else if(key == "s1_"         + std::to_string(player_index)) gsplayer.SetDeathsAssualtKit(value);
			else if(key == "k2_"         + std::to_string(player_index)) gsplayer.SetKillsSniperKit(value);
			else if(key == "s2_"         + std::to_string(player_index)) gsplayer.SetDeathsSniperKit(value);
			else if(key == "k3_"         + std::to_string(player_index)) gsplayer.SetKillsSpecialOpKit(value);
			else if(key == "s3_"         + std::to_string(player_index)) gsplayer.SetDeathsSpecialOpKit(value);
			else if(key == "k4_"         + std::to_string(player_index)) gsplayer.SetKillsCombatEngineerKit(value);
			else if(key == "s4_"         + std::to_string(player_index)) gsplayer.SetDeathsCombatEngineerKit(value);
			else if(key == "k5_"         + std::to_string(player_index)) gsplayer.SetKillsSupportKit(value);
			else if(key == "s5_"         + std::to_string(player_index)) gsplayer.SetDeathsSupportKit(value);
			else if(key == "tk_"         + std::to_string(player_index)) gsplayer.SetTeamKills(value);
			else if(key == "medals_"     + std::to_string(player_index)) gsplayer.SetMedals(value);
			else if(key == "ttb_"        + std::to_string(player_index)) gsplayer.SetTotalTopPlayer(value);
			else if(key == "mv_"         + std::to_string(player_index)) gsplayer.SetTotalVictories(value);
			else if(key == "ngp_"        + std::to_string(player_index)) gsplayer.SetTotalGameSessions(value);
			
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
	
	// Update players stats
	for(Battlefield::GameStatPlayer gsplayer : game_stat.GetPlayers())
	{
		gsplayer.UpdatePlayerStats();
	}

	// Update Clan stats
	if(game_stat.GetTeam1ClanId() != 0 && game_stat.GetTeam2ClanId() != 0)
	{
		game_stat.UpdateClanStats();
	}
	
	g_database->createLeaderboards();
}

//	Private functions

void GameStats::Client::_LogTransaction(const std::string& direction, const std::string& response) const
{
	std::shared_lock<std::shared_mutex> guard2(g_settings_mutex); // settings lock (read)
	
	if ((g_logger_mode & Logger::Mode::Development) == 0)
	{
		return;
	}
	
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

// Static functions

void GameStats::Client::Heartbeat()
{
	Logger::info("Heartbeat started", Server::GameStats);

	while(true)
	{
		std::this_thread::sleep_for (std::chrono::seconds(60));

		for(std::shared_ptr<Net::Socket> client : g_gamestats_server->GetClients())
		{
			std::shared_ptr<GameStats::Client> gamestats_client = std::dynamic_pointer_cast<GameStats::Client>(client);

			std::string response = GameSpy::Parameter2Response({
				"ka",  "",
				"final"
			});
			
			gamestats_client.get()->Send(Encrypt(response));
		
			gamestats_client.get()->_LogTransaction("<--", response);
		}
	}
}

std::string GameStats::Client::Decrypt(const std::vector<unsigned char>& request)
{
	std::string msg;
	const char key[10] = "GameSpy3D";
	
	for(int i = 0; i < static_cast<int>(request.size()) - 7; i++)
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

