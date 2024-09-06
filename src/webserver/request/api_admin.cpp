#include <atomizes.hpp>
#include <json/json.h>
#include <random>
#include <thread>
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
	Json::Value json_results;

	// Check password
	auto it = url_variables.find("password");
	if (it == url_variables.end() || it->second != g_settings["webserver"]["password"].asString())
	{
		json_results["result"] = "FAIL";
		json_results["message"] = "Bad password";
		this->Send(json_results, 403);
		this->_LogTransaction("<--", "HTTP/1.1 403 Bad password");

		return;
	}

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
		json_session["profileid"] = session.profileid;
		json_session["uniquenick"] = player.GetUniquenick();
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

	// Check password
	auto it = url_variables.find("password");
	if (it == url_variables.end() || it->second != g_settings["webserver"]["password"].asString())
	{
		json_results["result"] = "FAIL";
		json_results["message"] = "Bad password";
		this->Send(json_results, 403);
		this->_LogTransaction("<--", "HTTP/1.1 403 Bad password");

		return;
	}

	Battlefield::Player player;

	// Get profile id
	it = url_variables.find("profileid");
	if (it == url_variables.end() || !player.SetProfileId(it->second))
	{
		json_results["result"] = "FAIL";
		json_results["message"] = "Bad profileid";
		this->Send(json_results, 403);
		this->_LogTransaction("<--", "HTTP/1.1 403 Bad profileid");

		return;
	}

	// By default send FAIL.
	json_results["result"] = "FAIL";

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
	Json::Value json_results;

	// Check password
	auto it = url_variables.find("password");
	if (it == url_variables.end() || it->second != g_settings["webserver"]["password"].asString())
	{
		json_results["result"] = "FAIL";
		json_results["message"] = "Bad password";
		this->Send(json_results, 403);
		this->_LogTransaction("<--", "HTTP/1.1 403 Bad password");

		return;
	}

	// Get message
	it = url_variables.find("message");
	if (it == url_variables.end() || !Util::UTF8::isValid(it->second))
	{
		json_results["result"] = "FAIL";
		json_results["message"] = "Bad message";
		this->Send(json_results, 403);
		this->_LogTransaction("<--", "HTTP/1.1 403 Bad message");

		return;
	}
	std::string message = it->second;

	// Get to profileid
	Battlefield::Player to_player;
	it = url_variables.find("to");
	if (
		it != url_variables.end() &&                       // No "to" url parameter is provided.
		(
			!to_player.SetProfileId(it->second) ||             // Provided "to" parameter is wrong.
			!g_database->queryPlayerByProfileId(to_player) ||  // Query player information went wrong
			to_player.GetUserId() == -1                        // No to_player is found in database
		)
	)
	{
		json_results["result"] = "FAIL";
		json_results["message"] = "Bad to profileid";
		this->Send(json_results, 403);
		this->_LogTransaction("<--", "HTTP/1.1 403 Bad to profileid");

		return;
	}

	// Get from profileid
	Battlefield::Player from_player;
	from_player.SetProfileId(1);

	it = url_variables.find("from");
	if (
		it != url_variables.end() &&                       // No "from" url parameter is provided.
		(
			!from_player.SetProfileId(it->second) ||             // Provided "from" parameter is wrong.
			!g_database->queryPlayerByProfileId(from_player) ||  // Query player information went wrong
			from_player.GetUserId() == -1                        // No from_player is found in database
		)
	)
	{
		json_results["result"] = "FAIL";
		json_results["message"] = "Bad from profileid";
		this->Send(json_results, 403);
		this->_LogTransaction("<--", "HTTP/1.1 403 Bad from profileid");

		return;
	}

	// Check "from" are friends with "to".
	// Exception: BF2MC-Admin(profileid = 1) dont need to be checked.
	if(it != url_variables.end() && from_player.GetProfileId() != 1)
	{
		if(
			!g_database->queryPlayerFriendsByProfileId(from_player)	||
			!from_player.isFriend(to_player.GetProfileId())
		)
		{
			json_results["result"] = "FAIL";
			json_results["message"] = "Bad friendship";
			this->Send(json_results, 403);
			this->_LogTransaction("<--", "HTTP/1.1 403 Bad friendship");

			return;
		}
	}
	
	Json::Value json_send(Json::arrayValue);
	for(std::shared_ptr<Net::Socket> client : g_gpcm_server->GetClients())
	{
		std::shared_ptr<GPCM::Client> gpcm_client = std::dynamic_pointer_cast<GPCM::Client>(client);
		
		// Find session
		GPCM::Session session = gpcm_client->GetSession();

		if(session.profileid == to_player.GetProfileId() || to_player.GetProfileId() == -1)
		{
			GPCM::Client::SendBuddyMessage(
				from_player.GetProfileId(),
				session.profileid,
				"1",
				message
			);

			Json::Value json_msg;

			json_msg["from"] = from_player.GetProfileId();
			json_msg["to"] = session.profileid;
			json_send.append(json_msg);
		}
	}

	json_results["result"] = "OK";
	json_results["send"] = json_send;

	this->Send(json_results);

	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestAPIAdminPlayerStatsRecalc(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	Json::Value json_results;

	// Check password
	auto it = url_variables.find("password");
	if (it == url_variables.end() || it->second != g_settings["webserver"]["password"].asString())
	{
		json_results["result"] = "FAIL";
		json_results["message"] = "Bad password";
		this->Send(json_results, 403);
		this->_LogTransaction("<--", "HTTP/1.1 403 Bad password");

		return;
	}

	Battlefield::Player player;

	// Get profile id
	it = url_variables.find("profileid");
	if (it == url_variables.end() || !player.SetProfileId(it->second) && player.GetProfileId() > 0)
	{
		json_results["result"] = "FAIL";
		json_results["message"] = "Bad profileid";
		this->Send(json_results, 403);
		this->_LogTransaction("<--", "HTTP/1.1 403 Bad profileid");

		return;
	}

	Json::Value json_gsplayers(Json::arrayValue);

	// Get total games needs to be process
	uint32_t total;
	g_database->countGameStatPlayersByProfileId(player, total);
	json_results["total"] = total;

	for(uint32_t offset = 0; offset < total; offset += 250)
	{
		Battlefield::GameStatPlayers gsplayers;
		
		// Sleep for 50 milliseconds
    	std::this_thread::sleep_for(std::chrono::milliseconds(50));

		// Get games that the player has played in the limit and offset range
		g_database->queryGameStatPlayersByProfileId(player, gsplayers, 250, offset);

		for(const Battlefield::GameStatPlayer& gsplayer : gsplayers)
		{
			if(gsplayer.IsDisabled())
				continue;
			
			// Update player stats with gsplayer
			player.Update(gsplayer);
			
			// Give json feedback
			Json::Value json_gsplayer;

			json_gsplayer["id"] = gsplayer.GetId();
			json_gsplayer["medals"] = player.GetMedals();
			json_gsplayer["score"] = player.GetScore();
			json_gsplayer["pph"] = player.GetPPH();
			json_gsplayer["rank"] = player.GetRank();

			json_gsplayers.append(json_gsplayer);
		}
	}

	json_results["gsplayers"] = json_gsplayers;
	
	// Update player stats on database
	g_database->updatePlayerStats(player);

	this->Send(json_results);

	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

