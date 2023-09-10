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
		// Get game server players from database
		g_database->queryGameServerPlayers(game_server);
		
		Json::Value value;
		
		// Secret
		//value["ip"] = game_server.GetIp();
		//value["port"] = game_server.GetPort();
		//value["localip0"] = game_server.GetLocalIp();
		//value["localport"] = game_server.GetLocalPort();
		//value["hostport"] = game_server.GetHostPort();
		
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
