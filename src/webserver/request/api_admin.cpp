#include <atomizes.hpp>
#include <json/json.h>
#include <random>
#include <chrono>

#include <logger.h>
#include <globals.h>
#include <settings.h>
#include <database.h>
#include <gpcm/client.h>
#include <battlefield/gamestat.h>

#include <webserver/client.h>

void Webserver::Client::requestAPIAdminClients(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	// Check password
	auto it = url_variables.find("password");
	if (it == url_variables.end() || it->second != g_settings["webserver"]["password"].asString())
	{
		return;
	}

	Json::Value json_results;

	// Browsing
	Json::Value json_browsing(Json::arrayValue);
	for(std::shared_ptr<Net::Socket> client : g_browsing_server->GetClients())
	{
		Json::Value json_client;

		json_client["ip"] = client.get()->GetIP();
		json_client["port"] = client.get()->GetPort();

		time_t last_recieved_time = std::chrono::system_clock::to_time_t(client.get()->GetLastRecievedTime());
		json_client["last_recieved_time"] = std::string(std::ctime(&last_recieved_time));

		// Debug
		//json_client["ref_count"] = client.use_count();

		json_browsing.append(json_client);
	}
	json_results["browsing"] = json_browsing;

	// Gamestats
	Json::Value json_gamestats(Json::arrayValue);
	for(std::shared_ptr<Net::Socket> client : g_gamestats_server->GetClients())
	{
		Json::Value json_client;
		
		json_client["ip"] = client.get()->GetIP();
		json_client["port"] = client.get()->GetPort();

		time_t last_recieved_time = std::chrono::system_clock::to_time_t(client.get()->GetLastRecievedTime());
		json_client["last_recieved_time"] = std::string(std::ctime(&last_recieved_time));

		// Debug
		//json_client["ref_count"] = client.use_count();

		json_gamestats.append(json_client);
	}
	json_results["gamestats"] = json_gamestats;

	// GPCM
	Json::Value json_gpcm(Json::arrayValue);
	for(std::shared_ptr<Net::Socket> client : g_gpcm_server->GetClients())
	{
		Json::Value json_client;
		Json::Value json_session;
		
		std::shared_ptr<GPCM::Client> gpcm_client = std::dynamic_pointer_cast<GPCM::Client>(client);
		
		// Find session
		GPCM::Session session = gpcm_client->GetSession();

		Battlefield::Player player;
		player.SetProfileId(session.profileid);
		g_database->queryPlayerByProfileId(player);

		json_client["ip"] = client.get()->GetIP();
		json_client["port"] = client.get()->GetPort();

		time_t last_recieved_time = std::chrono::system_clock::to_time_t(client.get()->GetLastRecievedTime());
		json_client["last_recieved_time"] = std::string(std::ctime(&last_recieved_time));

		json_session["authtoken"] = session.authtoken;
		json_session["challenge"] = session.challenge;
		json_session["profileid"] = std::to_string(session.profileid) + " (" + player.GetUniquenick() + ")";
		json_session["status"] = session.status;

		json_client["session"] = json_session;
		
		// Debug
		//json_client["ref_count"] = client.use_count();

		json_gpcm.append(json_client);
	}
	json_results["gpcm"] = json_gpcm;

	// GPSP
	Json::Value json_gpsp(Json::arrayValue);
	for(std::shared_ptr<Net::Socket> client : g_gpsp_server->GetClients())
	{
		Json::Value json_client;

		json_client["ip"] = client.get()->GetIP();
		json_client["port"] = client.get()->GetPort();

		time_t last_recieved_time = std::chrono::system_clock::to_time_t(client.get()->GetLastRecievedTime());
		json_client["last_recieved_time"] = std::string(std::ctime(&last_recieved_time));
		
		// Debug
		//json_client["ref_count"] = client.use_count();

		json_gpsp.append(json_client);
	}
	json_results["gpsp"] = json_gpsp;

	// Webserver
	Json::Value json_webserver(Json::arrayValue);
	for(std::shared_ptr<Net::Socket> client : g_webserver_server->GetClients())
	{
		Json::Value json_client;

		json_client["ip"] = client.get()->GetIP();
		json_client["port"] = client.get()->GetPort();

		time_t last_recieved_time = std::chrono::system_clock::to_time_t(client.get()->GetLastRecievedTime());
		json_client["last_recieved_time"] = std::string(std::ctime(&last_recieved_time));

		// Debug
		//json_client["ref_count"] = client.use_count();

		json_webserver.append(json_client);
	}
	json_results["webserver"] = json_webserver;

	this->Send(json_results);

	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestAPIAdminKick(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	Json::Value json_results;
	
	json_results["result"] = "FAIL";

	// Check password
	auto it = url_variables.find("password");
	if (it == url_variables.end() || it->second != g_settings["webserver"]["password"].asString())
	{
		return;
	}

	Battlefield::Player player;

	// Get profile id
	it = url_variables.find("profileid");
	if (it == url_variables.end() || !player.SetProfileId(it->second))
	{
		return;
	}

	for(std::shared_ptr<Net::Socket> client : g_gpcm_server->GetClients())
	{
		std::shared_ptr<GPCM::Client> gpcm_client = std::dynamic_pointer_cast<GPCM::Client>(client);
		
		// Find session
		GPCM::Session session = gpcm_client->GetSession();

		if(session.profileid == player.GetProfileId())
		{
			gpcm_client.get()->Disconnect();
			json_results["result"] = "OK";
		}
	}

	this->Send(json_results);

	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestAPIAdminMessage(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	// Check password
	auto it = url_variables.find("password");
	if (it == url_variables.end() || it->second != g_settings["webserver"]["password"].asString())
	{
		return;
	}

	// Get message
	it = url_variables.find("message");
	if (it == url_variables.end() || !Util::UTF8::isValid(it->second))
	{
		return;
	}
	std::string message = it->second;

	// Get profile id
	Battlefield::Player player;
	it = url_variables.find("profileid");
	if (it == url_variables.end())
	{
		player.SetProfileId(it->second);
	}
	
	Json::Value json_results;

	for(std::shared_ptr<Net::Socket> client : g_gpcm_server->GetClients())
	{
		std::shared_ptr<GPCM::Client> gpcm_client = std::dynamic_pointer_cast<GPCM::Client>(client);
		
		// Find session
		GPCM::Session session = gpcm_client->GetSession();

		if(session.profileid == player.GetProfileId() || player.GetProfileId() == -1)
		{
			GPCM::Client::SendBuddyMessage(
				1,
				session.profileid,
				"1",
				message
			);

			json_results["result"] = "OK";
		}
	}

	this->Send(json_results);

	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestAPIAdminPlayerStatsRecalc(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	// Check password
	auto it = url_variables.find("password");
	if (it == url_variables.end() || it->second != g_settings["webserver"]["password"].asString())
	{
		return;
	}

	Battlefield::Player player;
	Battlefield::GameStatPlayers gsplayers;

	// Get profile id
	it = url_variables.find("profileid");
	if (it == url_variables.end() || !player.SetProfileId(it->second))
	{
		return;	
	}

	// Get all games results that the player has played
	g_database->queryGameStatPlayersByProfileId(player, gsplayers);

	Json::Value json_results;
	Json::Value json_gsplayers(Json::arrayValue);

	for(const Battlefield::GameStatPlayer& gsplayer : gsplayers)
	{
		if(gsplayer.IsDisabled())
			continue;
		
		// Calculate new pph
		player.calcNewPPH(gsplayer.GetTime(), gsplayer.GetScore());

		player.SetBoatsDestroyed(          player.GetBoatsDestroyed()          + gsplayer.GetBoatsDestroyed()          ); // bod
		player.SetHAVsDestroyed(           player.GetHAVsDestroyed()           + gsplayer.GetHAVsDestroyed()           ); // havd
		player.SetHelicoptersDestroyed(    player.GetHelicoptersDestroyed()    + gsplayer.GetHelicoptersDestroyed()    ); // hed
		player.SetKillsAssualtKit(         player.GetKillsAssualtKit()         + gsplayer.GetKillsAssualtKit()         ); // k1
		player.SetKillsSniperKit(          player.GetKillsSniperKit()          + gsplayer.GetKillsSniperKit()          ); // k2
		player.SetKillsSpecialOpKit(       player.GetKillsSpecialOpKit()       + gsplayer.GetKillsSpecialOpKit()       ); // k3
		player.SetKillsCombatEngineerKit(  player.GetKillsCombatEngineerKit()  + gsplayer.GetKillsCombatEngineerKit()  ); // k4
		player.SetKillsSupportKit(         player.GetKillsSupportKit()         + gsplayer.GetKillsSupportKit()         ); // k5
		player.SetKills(                   player.GetKills()                   + gsplayer.GetKills()                   ); // kills
		player.SetDeaths(                  player.GetDeaths()                  + gsplayer.GetDeaths()                  ); // deaths
		player.SetLAVsDestroyed(           player.GetLAVsDestroyed()           + gsplayer.GetLAVsDestroyed()           ); // lavd
		player.SetMAVsDestroyed(           player.GetMAVsDestroyed()           + gsplayer.GetMAVsDestroyed()           ); // mavd
		player.SetTotalVictories(          player.GetTotalVictories()          + gsplayer.GetTotalVictories()          ); // mv
		player.SetTotalGameSessions(       player.GetTotalGameSessions()       + gsplayer.GetTotalGameSessions()       ); // ngp
		player.SetDeathsAssualtKit(        player.GetSpawnsAssualtKit()        + gsplayer.GetSpawnsAssualtKit()        ); // s1
		player.SetDeathsSniperKit(         player.GetSpawnsSniperKit()         + gsplayer.GetSpawnsSniperKit()         ); // s2
		player.SetDeathsSpecialOpKit(      player.GetSpawnsSpecialOpKit()      + gsplayer.GetSpawnsSpecialOpKit()      ); // s3
		player.SetDeathsCombatEngineerKit( player.GetSpawnsCombatEngineerKit() + gsplayer.GetSpawnsCombatEngineerKit() ); // s4
		player.SetDeathsSupportKit(        player.GetSpawnsSupportKit()        + gsplayer.GetSpawnsSupportKit()        ); // s5
		player.SetScore(                   player.GetScore()                   + gsplayer.GetScore()                   ); // score
		player.SetSuicides(                player.GetSuicides()                + gsplayer.GetSuicides()                ); // suicides
		player.SetTime(                    player.GetTime()                    + gsplayer.GetTime()                    ); // time
		player.SetTotalTopPlayer(          player.GetTotalTopPlayer()          + gsplayer.GetTotalTopPlayer()          ); // ttb

		player.SetVehiclesDestroyed(
			player.GetLAVsDestroyed() +
			player.GetMAVsDestroyed() +
			player.GetHAVsDestroyed() +
			player.GetHelicoptersDestroyed() +
			player.GetBoatsDestroyed()
		);
		
		player.SetMedals(gsplayer.GetMedals()); // medals
		
		// Give json feedback
		Json::Value json_gsplayer;

		json_gsplayer["id"] = gsplayer.GetId();
		json_gsplayer["medals"] = player.GetMedals();
		json_gsplayer["score"] = player.GetScore();
		json_gsplayer["pph"] = player.GetPPH();
		json_gsplayer["rank"] = player.GetRank();

		json_gsplayers.append(json_gsplayer);
	}
	json_results["gsplayers"] = json_gsplayers;

	// Update player stats on database
	g_database->updatePlayerStats(player);

	this->Send(json_results);

	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

