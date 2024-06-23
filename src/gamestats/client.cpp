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
		
		// Update game stat
		auto it = Battlefield::GameStat::SetterMap.find(key);
		if (it != Battlefield::GameStat::SetterMap.end()) {
			(game_stat.*(it->second))(value);
		}
		
		// Debug
		//Logger::debug(key + " = " + value);
		
		offset += 2;
	}
	
	// Debug
	//game_stat.Debug();
	//Logger::debug("=============================");
	
	// Get player index out of key
	int player_index;
	this->_GetKeyAndPlayerIndex(parameter[offset], key, player_index);

	// Copy player index 
	int previous_player_index = player_index;

	// Read Game stat player information
	while(parameter.size() > offset + 1)
	{
		Battlefield::GameStatPlayer gsplayer;
		
		// Debug
		//Logger::debug("player_index = " + std::to_string(player_index));
		
		while(parameter.size() > offset + 1)
		{
			// Get key and player index
			this->_GetKeyAndPlayerIndex(parameter[offset], key, player_index);

			// Get value
			value = parameter[offset + 1];
			
			// If we found the new player_index then we create new GameStatPlayer
			if (previous_player_index != player_index)
				break;
			
			// Update game stat player
			auto it = Battlefield::GameStatPlayer::SetterMap.find(key);
			if (it != Battlefield::GameStatPlayer::SetterMap.end()) {
				(gsplayer.*(it->second))(value);
			}
			
			// Debug
			//Logger::debug(key + " = " + value);
			
			offset += 2;
		}
		
		game_stat.AddPlayer(gsplayer);
		
		// Save previous player index
		previous_player_index = player_index;
		
		// Debug
		//gsplayer.Debug();
		//Logger::debug("=============================");
	}
	
	// Update players stats
	for(Battlefield::GameStatPlayer gsplayer : game_stat.GetPlayers())
	{
		gsplayer.UpdatePlayerStats(game_stat);
	}

	// Insert GameStat in database
	g_database->insertGameStat(game_stat);

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

bool GameStats::Client::_GetKeyAndPlayerIndex(const std::string& input, std::string& key, int& player_index)
{
	player_index = -1;
	key = "";

	size_t pos = input.find('_');
	if (pos == std::string::npos)
	{
		return false;
	}

	try
	{
		key = input.substr(0, pos);
		player_index = std::stoi(input.substr(pos + 1));
	}
	catch(...)
	{
		return false;
	};

	return true;
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

