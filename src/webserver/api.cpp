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
	Json::Value response(Json::arrayValue);
	Battlefield::GameServers game_servers;

	// Get game servers from database
	g_database->queryGameServers(game_servers);

	for(Battlefield::GameServer game_server : game_servers)
	{
		Json::Value value;
		
		// Get game server players from database
		g_database->queryGameServerPlayers(game_server);
		
		// Game Server information
		value["id"] = game_server.GetId();
		value["flag"] = game_server.GetFlag();
		value["natneg"] = game_server.GetNatNeg();
		value["gamename"] = game_server.GetGameName();
		value["hostname"] = game_server.GetHostName();
		value["gamever"] = game_server.GetGameVersion();
		value["cl"] = game_server.GetClientVersion();
		value["rv"] = game_server.GetRV();
		value["map"] = game_server.GetMap();
		value["mc"] = game_server.GetMapCycling();
		value["mapname"] = game_server.GetMapName();
		value["gc"] = game_server.GetGC();
		value["gametype"] = game_server.GetGameType();
		value["numplayers"] = game_server.GetNumPlayers();
		value["maxplayers"] = game_server.GetMaxPlayers();
		value["numteams"] = game_server.GetNumTeams();
		value["gamemode"] = game_server.GetGameMode();
		value["teamplay"] = game_server.GetTeamplay();
		value["fraglimit"] = game_server.GetFlagLimit();
		value["teamfraglimit"] = game_server.GetTeamFragLimit();
		value["timelimit"] = game_server.GetTimeLimit();
		value["timeelapsed"] = game_server.GetTimeElapsed();
		value["password"] = game_server.GetPassword();
		value["nr"] = game_server.GetMinRank();
		value["xr"] = game_server.GetMaxRank();
		value["ff"] = game_server.GetFriendlyFire();
		value["sr"] = game_server.GetStatsTracking();
		value["rc"] = game_server.GetReconfigurable();
		value["ni"] = game_server.GetMinIpRange();
		value["xi"] = game_server.GetMaxIpRange();
		value["qm"] = game_server.GetQM();
		value["region"] = game_server.GetRegion();
		value["c0"] = game_server.GetClan1Id();
		value["c1"] = game_server.GetClan2Id();
		value["n0"] = game_server.GetClan1Name();
		value["n1"] = game_server.GetClan2Name();
		value["c0c"] = game_server.GetClan1Claimed();
		value["c1c"] = game_server.GetClan2Claimed();
		value["team0"] = game_server.GetTeam1Name();
		value["team1"] = game_server.GetTeam2Name();
		value["score0"] = game_server.GetTeam1Score();
		value["score1"] = game_server.GetTeam2Score();
		value["updated_at"] = game_server.GetUpdatedAt();
		value["verified"] = game_server.isVerified();
		
		// Secret
		//value["ip"] = game_server.GetIp();
		//value["port"] = game_server.GetPort();
		//value["localip0"] = game_server.GetLocalIp();
		//value["localport"] = game_server.GetLocalPort();
		//value["hostport"] = game_server.GetHostPort();
		
		// Game server player information
		Json::Value players(Json::arrayValue);
		for(const Battlefield::GameServerPlayer gsplayer : game_server.GetPlayers())
		{
			Json::Value player;
			
			player["name"] = gsplayer.GetName();
			player["score"] = gsplayer.GetScore();
			player["skill"] = gsplayer.GetSkill();
			player["ping"] = gsplayer.GetPing();
			player["team"] = gsplayer.GetTeam();
			player["deaths"] = gsplayer.GetDeaths();
			player["profileid"] = gsplayer.GetProfileId();
			
			players.append(player);
		}
		value["players"] = players;
		
		response.append(value);
	}

	this->Send(response);

	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestAPIGame(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	Json::Value response;
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
	response["id"]         = game_stat.GetId();
	response["gametype"]   = game_stat.GetGameType();
	response["gamver"]     = game_stat.GetGameVersion();
	response["hostname"]   = game_stat.GetHostName();
	response["mapid"]      = game_stat.GetMapId();
	response["numplayers"] = game_stat.GetNumPlayers();
	response["pplayers"]   = game_stat.GetPPlayers();
	response["tplayed"]    = game_stat.GetTimePlayed();
	response["clanid_t0"]  = game_stat.GetTeam1ClanId();
	response["clanid_t1"]  = game_stat.GetTeam2ClanId();
	response["country_t0"] = game_stat.GetTeam1Country();
	response["country_t1"] = game_stat.GetTeam2Country();
	response["victory_t0"] = game_stat.GetTeam1Victory();
	response["victory_t1"] = game_stat.GetTeam2Victory();
	response["created_at"] = game_stat.GetCreatedAt();
	
	// Game server player information
	Json::Value players(Json::arrayValue);
	for(const Battlefield::GameStatPlayer gsplayer : game_stat.GetPlayers())
	{
		Json::Value player;
		
		player["id"]       = gsplayer.GetId();
		player["auth"]     = gsplayer.GetAuth();
		player["pid"]      = gsplayer.GetProfileId();
		player["score"]    = gsplayer.GetScore();
		player["rank"]     = gsplayer.GetRank();
		player["pph"]      = gsplayer.GetPPH();
		player["kills"]    = gsplayer.GetKills();
		player["suicides"] = gsplayer.GetSuicides();
		player["time"]     = gsplayer.GetTime();
		player["lavd"]     = gsplayer.GetLAVsDestroyed();
		player["mavd"]     = gsplayer.GetMAVsDestroyed();
		player["havd"]     = gsplayer.GetHAVsDestroyed();
		player["hed"]      = gsplayer.GetHelicoptersDestroyed();
		player["pld"]      = gsplayer.GetPlanesDestroyed();
		player["bod"]      = gsplayer.GetBoatsDestroyed();
		player["k1"]       = gsplayer.GetKillsAssualtKit();
		player["s1"]       = gsplayer.GetDeathsAssualtKit();
		player["k2"]       = gsplayer.GetKillsSniperKit();
		player["s2"]       = gsplayer.GetDeathsSniperKit();
		player["k3"]       = gsplayer.GetKillsSpecialOpKit();
		player["s3"]       = gsplayer.GetDeathsSpecialOpKit();
		player["k4"]       = gsplayer.GetKillsCombatEngineerKit();
		player["s4"]       = gsplayer.GetDeathsCombatEngineerKit();
		player["k5"]       = gsplayer.GetKillsSupportKit();
		player["s5"]       = gsplayer.GetDeathsSupportKit();
		player["tk"]       = gsplayer.GetTeamKills();
		player["medals"]   = gsplayer.GetMedals();
		player["ttb"]      = gsplayer.GetTotalTopPlayer();
		player["mv"]       = gsplayer.GetTotalVictories();
		player["ngp"]      = gsplayer.GetTotalGameSessions();
		
		players.append(player);
	}
	response["players"] = players;
	
	this->Send(response);

	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestAPIGames(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	Json::Value response(Json::arrayValue);
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
		response.append(game_stat.GetId());
	}
	
	
	this->Send(response);

	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestAPIPlayer(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	Json::Value response;
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
	response["profileid"]  = player.GetProfileId();
	response["userid"]     = player.GetUserId();
	response["nick"]       = player.GetNick();
	response["uniquenick"] = player.GetUniquenick();
	response["last_login"] = player.GetLastLogin();
	response["created_at"] = player.GetCreatedAt();
	
	// Secret
	//response["email"]         = player.GetEmail();
	//response["password"]      = player.GetPassword();
	//response["last_login_ip"] = player.GetLastLoginIp();
	
	// Player stats informaton
	response["score"] = player.GetScore();
	response["ran"] = player.GetRank();
	response["pph"] = player.GetPPH();
	response["kills"] = player.GetKills();
	response["deaths"] = player.GetDeaths();
	response["suicides"] = player.GetSuicides();
	response["time"] = player.GetTime();
	response["vehicles"] = player.GetVehiclesDestroyed();
	response["lavd"] = player.GetLAVsDestroyed();
	response["mavd"] = player.GetMAVsDestroyed();
	response["havd"] = player.GetHAVsDestroyed();
	response["hed"] = player.GetHelicoptersDestroyed();
	response["pld"] = player.GetPlanesDestroyed();
	response["bod"] = player.GetBoatsDestroyed();
	response["k1"] = player.GetKillsAssualtKit();
	response["s1"] = player.GetDeathsAssualtKit();
	response["k2"] = player.GetKillsSniperKit();
	response["s2"] = player.GetDeathsSniperKit();
	response["k3"] = player.GetKillsSpecialOpKit();
	response["s3"] = player.GetDeathsSpecialOpKit();
	response["k4"] = player.GetKillsCombatEngineerKit();
	response["s4"] = player.GetDeathsCombatEngineerKit();
	response["k5"] = player.GetKillsSupportKit();
	response["s5"] = player.GetDeathsSupportKit();
	response["tk"] = player.GetTeamKills();
	response["medals"] = player.GetMedals();
	response["ttb"] = player.GetTotalTopPlayer();
	response["mv"] = player.GetTotalVictories();
	response["ngp"] = player.GetTotalGameSessions();
	
	Json::Value json_clan;
	
	json_clan["clanid"]   = clan.GetClanId();
	json_clan["name"]     = clan.GetName();
	json_clan["tag"]      = clan.GetTag();
	
	response["clan"] = json_clan;
	
	this->Send(response);

	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestAPIClan(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	Json::Value response;
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
	response["clanid"]     = clan.GetClanId();
	response["name"]       = clan.GetName();
	response["tag"]        = clan.GetTag();
	response["homepage"]   = clan.GetHomepage();
	response["info"]       = clan.GetInfo();
	response["region"]     = clan.GetRegion();
	response["created_at"] = clan.GetCreatedAt();
	
	// Secret
	//response["rating"] = clan.GetRating();
	//response["wins"]   = clan.GetWins();
	//response["losses"] = clan.GetLosses();
	//response["draws"]  = clan.GetDraws();
	
	// Clan ranks information
	Json::Value members(Json::arrayValue);
	for (const auto& pair : clan.GetRanks())
	{
		Json::Value rank;
		
		rank["profileid"] = pair.first;
		rank["rank"] = pair.second;
		
		members.append(rank);
	}
	response["members"] = members;
	
	this->Send(response);

	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestAPILeaderboard(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	Json::Value response(Json::arrayValue);
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
		Json::Value item;
		
		item["position"]   = pair.first;
		item["uniquenick"] = pair.second.GetUniquenick();
		
		if(sort == "rank")                         item["rank"]     = pair.second.GetRank();
		if(sort == "pph" || sort == "rank")        item["pph"]      = pair.second.GetPPH();
		if(sort == "score" || sort == "rank")      item["score"]    = pair.second.GetScore();
		else if(sort == "k1")                      item["k1"]       = pair.second.GetKillsAssualtKit();
		else if(sort == "k2")                      item["k2"]       = pair.second.GetKillsSniperKit();
		else if(sort == "k3")                      item["k3"]       = pair.second.GetKillsSpecialOpKit();
		else if(sort == "k4")                      item["k4"]       = pair.second.GetKillsCombatEngineerKit();
		else if(sort == "k5")                      item["k5"]       = pair.second.GetKillsSupportKit();
		else if(sort == "kills")                   item["kills"]    = pair.second.GetKills();
		else if(sort == "lavd")                    item["lavd"]     = pair.second.GetLAVsDestroyed();
		else if(sort == "mavd")                    item["mavd"]     = pair.second.GetMAVsDestroyed();
		else if(sort == "havd")                    item["havd"]     = pair.second.GetHAVsDestroyed();
		else if(sort == "hed")                     item["hed"]      = pair.second.GetHelicoptersDestroyed();
		else if(sort == "bod")                     item["bod"]      = pair.second.GetBoatsDestroyed();
		else if(sort == "vehicles")                item["vehicles"] = pair.second.GetVehiclesDestroyed();
		else if(sort == "ratio_k1")                item["ratio"]    = pair.second.GetRatioAssualtKit();
		else if(sort == "ratio_k2")                item["ratio"]    = pair.second.GetRatioSniperKit();
		else if(sort == "ratio_k3")                item["ratio"]    = pair.second.GetRatioSpecialOpKit();
		else if(sort == "ratio_k4")                item["ratio"]    = pair.second.GetRatioCombatEngineerKit();
		else if(sort == "ratio_k5")                item["ratio"]    = pair.second.GetRatioSupportKit();
		else if(sort == "ratio_kd")                item["ratio"]    = pair.second.GetRatio();
		
		response.append(item);
	}
	
	this->Send(response);

	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}