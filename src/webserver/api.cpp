#include <atomizes.hpp>
#include <json/json.h>

#include <logger.h>
#include <globals.h>
#include <database.h>

#include <webserver/client.h>

void Webserver::Client::Send(const Json::Value &value) const
{
	// Create a JSON writer
	Json::StreamWriterBuilder writer;
	std::string jsonString = Json::writeString(writer, value);

	// Create http response
	atomizes::HTTPMessage http_response = this->_defaultResponseHeader();
	http_response.SetStatusCode(200);
	http_response.SetMessageBody(jsonString);

	//Logger::debug("json = " + jsonString);

	// Send http response
	this->Send(http_response);
}

void Webserver::Client::requestAPIServersLive(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	Json::Value json_game_servers(Json::arrayValue);
	Battlefield::GameServers game_servers;

	// Get game servers from database
	g_database->queryGameServers(game_servers);

	for(Battlefield::GameServer game_server : game_servers)
	{
		Json::Value json_game_server;
		
		// Get game server players from database
		g_database->queryGameServerPlayers(game_server);
		
		// Game Server information
		json_game_server["id"] = game_server.GetId();
		json_game_server["flag"] = game_server.GetFlag();
		json_game_server["natneg"] = game_server.GetNatNeg();
		json_game_server["gamename"] = game_server.GetGameName();
		json_game_server["hostname"] = game_server.GetHostName();
		json_game_server["gamever"] = game_server.GetGameVersion();
		json_game_server["cl"] = game_server.GetClientVersion();
		json_game_server["rv"] = game_server.GetRV();
		json_game_server["map"] = game_server.GetMap();
		json_game_server["mc"] = game_server.GetMapCycling();
		json_game_server["mapname"] = game_server.GetMapName();
		json_game_server["gc"] = game_server.GetGC();
		json_game_server["gametype"] = game_server.GetGameType();
		json_game_server["numplayers"] = game_server.GetNumPlayers();
		json_game_server["maxplayers"] = game_server.GetMaxPlayers();
		json_game_server["numteams"] = game_server.GetNumTeams();
		json_game_server["gamemode"] = game_server.GetGameMode();
		json_game_server["teamplay"] = game_server.GetTeamplay();
		json_game_server["fraglimit"] = game_server.GetFlagLimit();
		json_game_server["teamfraglimit"] = game_server.GetTeamFragLimit();
		json_game_server["timelimit"] = game_server.GetTimeLimit();
		json_game_server["timeelapsed"] = game_server.GetTimeElapsed();
		json_game_server["password"] = game_server.GetPassword();
		json_game_server["nr"] = game_server.GetMinRank();
		json_game_server["xr"] = game_server.GetMaxRank();
		json_game_server["ff"] = game_server.GetFriendlyFire();
		json_game_server["sr"] = game_server.GetStatsTracking();
		json_game_server["rc"] = game_server.GetReconfigurable();
		json_game_server["ni"] = game_server.GetMinIpRange();
		json_game_server["xi"] = game_server.GetMaxIpRange();
		json_game_server["qm"] = game_server.GetQM();
		json_game_server["region"] = game_server.GetRegion();
		json_game_server["c0"] = game_server.GetClan1Id();
		json_game_server["c1"] = game_server.GetClan2Id();
		json_game_server["n0"] = game_server.GetClan1Name();
		json_game_server["n1"] = game_server.GetClan2Name();
		json_game_server["c0c"] = game_server.GetClan1Claimed();
		json_game_server["c1c"] = game_server.GetClan2Claimed();
		json_game_server["team0"] = game_server.GetTeam1Name();
		json_game_server["team1"] = game_server.GetTeam2Name();
		json_game_server["score0"] = game_server.GetTeam1Score();
		json_game_server["score1"] = game_server.GetTeam2Score();
		json_game_server["updated_at"] = game_server.GetUpdatedAt();
		json_game_server["verified"] = game_server.isVerified();
		json_game_server["is_alive"] = game_server.IsAlive();
		
		// Secret
		//json_game_server["ip"] = game_server.GetIp();
		//json_game_server["port"] = game_server.GetPort();
		//json_game_server["localip0"] = game_server.GetLocalIp();
		//json_game_server["localport"] = game_server.GetLocalPort();
		//json_game_server["hostport"] = game_server.GetHostPort();
		
		// Game server player information
		Json::Value json_players(Json::arrayValue);
		for(const Battlefield::GameServerPlayer gsplayer : game_server.GetPlayers())
		{
			Json::Value json_player;
			
			json_player["name"] = gsplayer.GetName();
			json_player["score"] = gsplayer.GetScore();
			json_player["skill"] = gsplayer.GetSkill();
			json_player["ping"] = gsplayer.GetPing();
			json_player["team"] = gsplayer.GetTeam();
			json_player["deaths"] = gsplayer.GetDeaths();
			json_player["profileid"] = gsplayer.GetProfileId();
			
			json_players.append(json_player);
		}
		json_game_server["players"] = json_players;
		
		json_game_servers.append(json_game_server);
	}
	
	this->Send(json_game_servers);

	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestAPIGame(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	Json::Value json_game_stat;
	Battlefield::GameStat game_stat;
	
	// Get Game Stats
	auto it = url_variables.find("id");
	if (it != url_variables.end())
	{
		game_stat.SetId(it->second);
		
		// Get Game Stat information from database
		g_database->queryGameStatById(game_stat);
	}
	
	// Get Game stat player information from database
	g_database->queryGameStatPlayers(game_stat);
	
	// Game Stat information
	json_game_stat["id"]         = game_stat.GetId();
	json_game_stat["gametype"]   = game_stat.GetGameType();
	json_game_stat["gamver"]     = game_stat.GetGameVersion();
	json_game_stat["hostname"]   = game_stat.GetHostName();
	json_game_stat["mapid"]      = game_stat.GetMapId();
	json_game_stat["numplayers"] = game_stat.GetNumPlayers();
	json_game_stat["pplayers"]   = game_stat.GetPPlayers();
	json_game_stat["tplayed"]    = game_stat.GetTimePlayed();
	json_game_stat["clanid_t0"]  = game_stat.GetTeam1ClanId();
	json_game_stat["clanid_t1"]  = game_stat.GetTeam2ClanId();
	json_game_stat["country_t0"] = game_stat.GetTeam1Country();
	json_game_stat["country_t1"] = game_stat.GetTeam2Country();
	json_game_stat["victory_t0"] = game_stat.GetTeam1Victory();
	json_game_stat["victory_t1"] = game_stat.GetTeam2Victory();
	json_game_stat["created_at"] = game_stat.GetCreatedAt();
	
	// Game server player information
	Json::Value json_players(Json::arrayValue);
	for(const Battlefield::GameStatPlayer gsplayer : game_stat.GetPlayers())
	{
		Json::Value json_player;
		
		json_player["id"]       = gsplayer.GetId();
		json_player["auth"]     = gsplayer.GetAuth();
		json_player["pid"]      = gsplayer.GetProfileId();
		json_player["team"]     = gsplayer.GetTeam();
		json_player["score"]    = gsplayer.GetScore();
		json_player["rank"]     = gsplayer.GetRank();
		json_player["pph"]      = gsplayer.GetPPH();
		json_player["kills"]    = gsplayer.GetKills();
		json_player["deaths"]   = gsplayer.GetDeaths();
		json_player["suicides"] = gsplayer.GetSuicides();
		json_player["time"]     = gsplayer.GetTime();
		json_player["lavd"]     = gsplayer.GetLAVsDestroyed();
		json_player["mavd"]     = gsplayer.GetMAVsDestroyed();
		json_player["havd"]     = gsplayer.GetHAVsDestroyed();
		json_player["hed"]      = gsplayer.GetHelicoptersDestroyed();
		json_player["pld"]      = gsplayer.GetPlanesDestroyed();
		json_player["bod"]      = gsplayer.GetBoatsDestroyed();
		json_player["k1"]       = gsplayer.GetKillsAssualtKit();
		json_player["s1"]       = gsplayer.GetSpawnsAssualtKit();
		json_player["k2"]       = gsplayer.GetKillsSniperKit();
		json_player["s2"]       = gsplayer.GetSpawnsSniperKit();
		json_player["k3"]       = gsplayer.GetKillsSpecialOpKit();
		json_player["s3"]       = gsplayer.GetSpawnsSpecialOpKit();
		json_player["k4"]       = gsplayer.GetKillsCombatEngineerKit();
		json_player["s4"]       = gsplayer.GetSpawnsCombatEngineerKit();
		json_player["k5"]       = gsplayer.GetKillsSupportKit();
		json_player["s5"]       = gsplayer.GetSpawnsSupportKit();
		json_player["tk"]       = gsplayer.GetTeamKills();
		json_player["medals"]   = gsplayer.GetMedals();
		json_player["ttb"]      = gsplayer.GetTotalTopPlayer();
		json_player["mv"]       = gsplayer.GetTotalVictories();
		json_player["ngp"]      = gsplayer.GetTotalGameSessions();
		
		json_players.append(json_player);
	}
	json_game_stat["players"] = json_players;
	
	this->Send(json_game_stat);

	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestAPIGames(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	Json::Value json_game_stats(Json::arrayValue);
	Battlefield::GameStats game_stats;
	
	// Get Game Stats
	auto it = url_variables.find("date");
	if (it != url_variables.end())
	{
		std::string date = it->second;
		
		// Get Game Stat information from database
		g_database->queryGameStatsByDate(game_stats, date);
	}
	
	for(Battlefield::GameStat game_stat : game_stats)
	{
		json_game_stats.append(game_stat.GetId());
	}
	
	
	this->Send(json_game_stats);

	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestAPIPlayer(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	Json::Value json_player;
	Battlefield::Player player;
	Battlefield::Clan clan;
	
	// Get player profileid
	auto it = url_variables.find("profileid");
	if (it != url_variables.end())
	{
		player.SetProfileId(it->second);
		
		// Get player information from database
		g_database->queryPlayerByProfileid(player);
	}
	
	it = url_variables.find("uniquenick");
	if (it != url_variables.end())
	{
		player.SetUniquenick(it->second);
		
		// Get player information from database
		g_database->queryPlayerByUniquenick(player);
	}
	
	// Get player Stats information from database
	g_database->queryPlayerStats(player);
	
	// Get player Stats information from database
	g_database->queryClanByPlayer(clan, player);
	
	// Player information
	json_player["profileid"]  = player.GetProfileId();
	json_player["userid"]     = player.GetUserId();
	json_player["nick"]       = player.GetNick();
	json_player["uniquenick"] = player.GetUniquenick();
	json_player["last_login"] = player.GetLastLogin();
	json_player["created_at"] = player.GetCreatedAt();
	
	// Secret
	//json_player["email"]         = player.GetEmail();
	//json_player["password"]      = player.GetPassword();
	//json_player["last_login_ip"] = player.GetLastLoginIp();
	
	// Player stats informaton
	json_player["score"] = player.GetScore();
	json_player["ran"] = player.GetRank();
	json_player["pph"] = player.GetPPH();
	json_player["kills"] = player.GetKills();
	json_player["deaths"] = player.GetDeaths();
	json_player["suicides"] = player.GetSuicides();
	json_player["time"] = player.GetTime();
	json_player["vehicles"] = player.GetVehiclesDestroyed();
	json_player["lavd"] = player.GetLAVsDestroyed();
	json_player["mavd"] = player.GetMAVsDestroyed();
	json_player["havd"] = player.GetHAVsDestroyed();
	json_player["hed"] = player.GetHelicoptersDestroyed();
	json_player["pld"] = player.GetPlanesDestroyed();
	json_player["bod"] = player.GetBoatsDestroyed();
	json_player["k1"] = player.GetKillsAssualtKit();
	json_player["s1"] = player.GetSpawnsAssualtKit();
	json_player["k2"] = player.GetKillsSniperKit();
	json_player["s2"] = player.GetSpawnsSniperKit();
	json_player["k3"] = player.GetKillsSpecialOpKit();
	json_player["s3"] = player.GetSpawnsSpecialOpKit();
	json_player["k4"] = player.GetKillsCombatEngineerKit();
	json_player["s4"] = player.GetSpawnsCombatEngineerKit();
	json_player["k5"] = player.GetKillsSupportKit();
	json_player["s5"] = player.GetSpawnsSupportKit();
	json_player["tk"] = player.GetTeamKills();
	json_player["medals"] = player.GetMedals();
	json_player["ttb"] = player.GetTotalTopPlayer();
	json_player["mv"] = player.GetTotalVictories();
	json_player["ngp"] = player.GetTotalGameSessions();
	
	Json::Value json_clan;
	
	json_clan["clanid"]   = clan.GetClanId();
	json_clan["name"]     = clan.GetName();
	json_clan["tag"]      = clan.GetTag();
	
	json_player["clan"] = json_clan;
	
	this->Send(json_player);

	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestAPIClan(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	Json::Value json_clan;
	Battlefield::Clan clan;
	
	// Get player profileid
	auto it = url_variables.find("clanid");
	if (it != url_variables.end())
	{
		clan.SetClanId(it->second);
		
		// Get clan information from database
		g_database->queryClanByClanId(clan);
	}
	
	// Get clan ranks information from database
	g_database->queryClanRanksByClanId(clan);
	
	// Clan information
	json_clan["clanid"]     = clan.GetClanId();
	json_clan["name"]       = clan.GetName();
	json_clan["tag"]        = clan.GetTag();
	json_clan["homepage"]   = clan.GetHomepage();
	json_clan["info"]       = clan.GetInfo();
	json_clan["region"]     = static_cast<uint8_t>(clan.GetRegion());
	json_clan["created_at"] = clan.GetCreatedAt();
	
	// Secret
	//json_clan["rating"] = clan.GetRating();
	//json_clan["wins"]   = clan.GetWins();
	//json_clan["losses"] = clan.GetLosses();
	//json_clan["draws"]  = clan.GetDraws();
	
	// Clan ranks information
	Json::Value json_members(Json::arrayValue);
	for (const auto& pair : clan.GetRanks())
	{
		Json::Value json_member;
		
		json_member["profileid"] = pair.first;
		json_member["rank"] = static_cast<uint8_t>(pair.second);
		
		json_members.append(json_member);
	}
	json_clan["members"] = json_members;
	
	this->Send(json_clan);

	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestAPILeaderboard(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	Json::Value json_leaderboard(Json::arrayValue);
	std::string sort = "";
	uint32_t limit = 10;
	uint32_t offset = 0;
	
	auto it = url_variables.find("limit");
	if (it != url_variables.end())
	{
		if(it->second == "25")       limit = 25;
		else if(it->second == "50")  limit = 50;
		else if(it->second == "75")  limit = 75;
		else if(it->second == "100") limit = 100;
	}
	
	it = url_variables.find("offset");
	if (it != url_variables.end())
	{
		try
		{
			offset = std::stoul(it->second);
		}
		catch(...) {}
	}
	
	// Get leaderboard from Database 
	Battlefield::RankPlayers rank_players;
	it = url_variables.find("sort");
	if (it != url_variables.end())
	{
		sort = it->second;
		
		if(sort == "rank")          g_database->queryLeaderboardRank(rank_players, limit, offset);
		else if(sort == "score")    g_database->queryLeaderboardType(rank_players, "score", limit, offset);
		else if(sort == "pph")      g_database->queryLeaderboardType(rank_players, "pph", limit, offset);
		else if(sort == "k1")       g_database->queryLeaderboardType(rank_players, "k1", limit, offset);
		else if(sort == "k2")       g_database->queryLeaderboardType(rank_players, "k2", limit, offset);
		else if(sort == "k3")       g_database->queryLeaderboardType(rank_players, "k3", limit, offset);
		else if(sort == "k4")       g_database->queryLeaderboardType(rank_players, "k4", limit, offset);
		else if(sort == "k5")       g_database->queryLeaderboardType(rank_players, "k5", limit, offset);
		else if(sort == "kills")    g_database->queryLeaderboardType(rank_players, "kills", limit, offset);
		else if(sort == "lavd")     g_database->queryLeaderboardType(rank_players, "lavd", limit, offset);
		else if(sort == "mavd")     g_database->queryLeaderboardType(rank_players, "mavd", limit, offset);
		else if(sort == "havd")     g_database->queryLeaderboardType(rank_players, "havd", limit, offset);
		else if(sort == "hed")      g_database->queryLeaderboardType(rank_players, "hed", limit, offset);
		else if(sort == "bod")      g_database->queryLeaderboardType(rank_players, "bod", limit, offset);
		else if(sort == "vehicles") g_database->queryLeaderboardType(rank_players, "vehicles", limit, offset);
		else if(sort == "ratio_k1") g_database->queryLeaderboardRatio(rank_players, "k1", "s1", limit, offset);
		else if(sort == "ratio_k2") g_database->queryLeaderboardRatio(rank_players, "k2", "s2", limit, offset);
		else if(sort == "ratio_k3") g_database->queryLeaderboardRatio(rank_players, "k3", "s3", limit, offset);
		else if(sort == "ratio_k4") g_database->queryLeaderboardRatio(rank_players, "k4", "s4", limit, offset);
		else if(sort == "ratio_k5") g_database->queryLeaderboardRatio(rank_players, "k5", "s5", limit, offset);
		else if(sort == "ratio_kd") g_database->queryLeaderboardRatio(rank_players, "kills", "deaths", limit, offset);
	}
	
	// Write leaderboard
	for (const auto& pair : rank_players)
	{
		Json::Value json_player;
		
		json_player["position"]   = pair.first;
		json_player["profileid"] = pair.second.GetProfileId();
		json_player["uniquenick"] = pair.second.GetUniquenick();
		
		if(sort == "rank")
		{
			json_player["rank"] = pair.second.GetRank();
		}
		else
		{
			// to-do: optimization to get full list in one query
			Battlefield::Player player;
			
			player.SetProfileId(pair.second.GetProfileId());
			
			g_database->queryPlayerByProfileid(player);
			
			json_player["rank"] = player.GetRank();
		}
		
		if(sort == "pph" || sort == "rank")   json_player["pph"]      = pair.second.GetPPH();
		if(sort == "score" || sort == "rank") json_player["score"]    = pair.second.GetScore();
		
		if(sort == "k1")                      json_player["k1"]       = pair.second.GetKillsAssualtKit();
		else if(sort == "k2")                 json_player["k2"]       = pair.second.GetKillsSniperKit();
		else if(sort == "k3")                 json_player["k3"]       = pair.second.GetKillsSpecialOpKit();
		else if(sort == "k4")                 json_player["k4"]       = pair.second.GetKillsCombatEngineerKit();
		else if(sort == "k5")                 json_player["k5"]       = pair.second.GetKillsSupportKit();
		else if(sort == "kills")              json_player["kills"]    = pair.second.GetKills();
		else if(sort == "lavd")               json_player["lavd"]     = pair.second.GetLAVsDestroyed();
		else if(sort == "mavd")               json_player["mavd"]     = pair.second.GetMAVsDestroyed();
		else if(sort == "havd")               json_player["havd"]     = pair.second.GetHAVsDestroyed();
		else if(sort == "hed")                json_player["hed"]      = pair.second.GetHelicoptersDestroyed();
		else if(sort == "bod")                json_player["bod"]      = pair.second.GetBoatsDestroyed();
		else if(sort == "vehicles")           json_player["vehicles"] = pair.second.GetVehiclesDestroyed();
		else if(sort == "ratio_k1")           json_player["ratio"]    = pair.second.GetRatioAssualtKit();
		else if(sort == "ratio_k2")           json_player["ratio"]    = pair.second.GetRatioSniperKit();
		else if(sort == "ratio_k3")           json_player["ratio"]    = pair.second.GetRatioSpecialOpKit();
		else if(sort == "ratio_k4")           json_player["ratio"]    = pair.second.GetRatioCombatEngineerKit();
		else if(sort == "ratio_k5")           json_player["ratio"]    = pair.second.GetRatioSupportKit();
		else if(sort == "ratio_kd")           json_player["ratio"]    = pair.second.GetRatio();
		
		json_leaderboard.append(json_player);
	}
	
	this->Send(json_leaderboard);

	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

#include <random>
#include <chrono>

#include <battlefield/gamestat.h>

#define SIMULATION_TOTAL_CLANS 2
#define SIMULATION_TOTAL_FIGHTS 1

void Webserver::Client::requestAPIClanSimulation(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	Battlefield::Clans clans;
	Json::Value json_results;
	
	// Random generator
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

	// Create random integer functions with ranges
    std::uniform_int_distribution<int> gen_random_clan_id(1, SIMULATION_TOTAL_CLANS);
	std::uniform_int_distribution<int> gen_random_team_win(0, 1);
	std::uniform_int_distribution<int> gen_random_victory(1, 3);

	// Generate random clans
	for(int i = 1; i <= SIMULATION_TOTAL_CLANS; i++)
	{
		Battlefield::Clan clan;

		clan.SetClanId(i);
		
		std::string clan_name(1, static_cast<char>(0x40 + i));
		clan_name = "Clan " + clan_name;
		clan.SetName(clan_name);

		clan.SetScore(0);

		clans.push_back(clan);
	}
	
	clans[0].SetScore(1000);

	// Battle random clans
	Json::Value json_fights(Json::arrayValue);
	Battlefield::GameStats game_stats;

	for(int i = 0; i < SIMULATION_TOTAL_FIGHTS; i++)
	{
		Json::Value json_fight;

		// Generate random game_stat
		Battlefield::GameStat game_stat;

		// Pick first random clan
		int clanid_t0 = gen_random_clan_id(generator);
		
		// Pick second random clan 
		int clanid_t1;
		do
		{
			clanid_t1 = gen_random_clan_id(generator);		
		}
		while(clanid_t1 == clanid_t0); // Clan cant fight themselfs

		// set clan id's
		game_stat.SetTeam1ClanId(clanid_t0);
		game_stat.SetTeam2ClanId(clanid_t1);

		int team_win = gen_random_team_win(generator);
		int victory = gen_random_victory(generator);
		
		if(team_win == 0)
		{
			game_stat.SetTeam1Victory(victory);

			if(victory == 3) // If its draw
				game_stat.SetTeam2Victory(3);
		}
		else
		{
			game_stat.SetTeam2Victory(victory);

			if(victory == 3) // If its draw
				game_stat.SetTeam1Victory(3);
		}
		game_stats.push_back(game_stat);

		// Create json game stat 
		json_fight["clanid_t0"] = game_stat.GetTeam1ClanId();
		json_fight["clanid_t1"] = game_stat.GetTeam2ClanId();
		json_fight["victory_t0"] = game_stat.GetTeam1Victory();
		json_fight["victory_t1"] = game_stat.GetTeam2Victory();

		json_fights.append(json_fight);
	}
	json_results["fights"] = json_fights;

	// After all clan fight have been fought we can update the clan scores
	for(const Battlefield::GameStat& game_stat : game_stats)
	{
		Battlefield::Clan* clan1 = &(clans[game_stat.GetTeam1ClanId() - 1]);
		Battlefield::Clan* clan2 = &(clans[game_stat.GetTeam2ClanId() - 1]);
		
		double K = Battlefield::Clan::ELO_WEIGHT;
		double R1 = 1.0;
		double R2 = 1.0;
		
		if(game_stat.GetTeam1Victory() == static_cast<u_int8_t>(Battlefield::GameStat::VictoryState::Minor) || 
			game_stat.GetTeam2Victory() == static_cast<u_int8_t>(Battlefield::GameStat::VictoryState::Minor))
		{
			K *= 2;
		}

		if(game_stat.GetTeam1Victory() == static_cast<u_int8_t>(Battlefield::GameStat::VictoryState::Major) || 
			game_stat.GetTeam2Victory() == static_cast<u_int8_t>(Battlefield::GameStat::VictoryState::Major))
		{
			K *= 4;
		}

		if(game_stat.GetTeam1Victory() == static_cast<u_int8_t>(Battlefield::GameStat::VictoryState::Lost))
		{
			R1 = 0.0;
		}

		if(game_stat.GetTeam2Victory() == static_cast<u_int8_t>(Battlefield::GameStat::VictoryState::Lost))
		{
			R2 = 0.0;
		}

		// Calculate the difference between two clan score
		double diff1 = static_cast<int64_t>(clan2->GetScore()) - static_cast<int64_t>(clan1->GetScore());
		double diff2 = static_cast<int64_t>(clan1->GetScore()) - static_cast<int64_t>(clan2->GetScore());

		// Calculate the procentage between difference of clan score.
		double P1 = (1.0 / (1.0 + std::pow(10, diff1 / 400)));
		double P2 = (1.0 / (1.0 + std::pow(10, diff2 / 400)));

		// Calculate how much score will be added and substracted
		double score1 = K * (R1 - P1);
		double score2 = K * (R2 - P2);

		// Debug
		//Logger::debug("clan1 = " + std::to_string(clan1->GetScore()));
		//Logger::debug("clan2 = " + std::to_string(clan2->GetScore()));
		//Logger::debug("R1 = " + std::to_string(R1));
		//Logger::debug("R2 = " + std::to_string(R2));
		//Logger::debug("P1 = " + std::to_string(P1));
		//Logger::debug("P2 = " + std::to_string(P2));
		//Logger::debug("score1 = " + std::to_string(score1));
		//Logger::debug("score2 = " + std::to_string(score2));

		clan1->SetScore(std::max(0, static_cast<int>(std::floor(score1) + clan1->GetScore())));
		clan2->SetScore(std::max(0, static_cast<int>(std::floor(score2) + clan2->GetScore())));
	}

	// Show clans results
	Json::Value json_clans(Json::arrayValue);
	for(const Battlefield::Clan& clan : clans)
	{
		Json::Value json_clan;

		json_clan["id"] = clan.GetClanId();
		json_clan["name"] = clan.GetName();
		json_clan["score"] = clan.GetScore();

		json_clans.append(json_clan);
	}

	json_results["clans"] = json_clans;

	this->Send(json_results);

	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}