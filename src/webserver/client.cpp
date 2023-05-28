#include <unistd.h>
#include <iostream>
#include <thread>
#include <iomanip>
#include <map>
#include <fstream>

#include <server.h>
#include <gpcm/client.h>
#include <globals.h>
#include <database.h>
#include <util.h>
#include <urlrequest.h>
#include <battlefield/playerstats.h>
#include <battlefield/clan.h>

#include <webserver/client.h>

using namespace atomizes;

typedef void (Webserver::Client::*RequestActionFunc)(const atomizes::HTTPMessage&, const UrlRequest::UrlVariables&);

static std::map<std::string, RequestActionFunc> mRequestActions = 
{
	// www.easports.com
	// News
	{ "/gamescripts/bfmc/ps2/en/PS2news_en_US.txt",           &Webserver::Client::requestNews  },
	
	// Licenses
	{ "/gamescripts/bfmc/ps2/GameSpy/gs.us",                  &Webserver::Client::requestEmpty },
	{ "/gamescripts/bfmc/ps2/Ea/Ea.us",                       &Webserver::Client::requestEmpty },
	{ "/gamescripts/bfmc/ps2/PunkBuster/pb.us",               &Webserver::Client::requestEmpty },
	
	// Configs
	{ "/gamescripts/bfmc/ps2/Settings/GameModes.txt",         &Webserver::Client::requestEmpty },
	{ "/gamescripts/bfmc/ps2/Settings/QuickMatchOptions.txt", &Webserver::Client::requestEmpty },
	{ "/gamescripts/bfmc/ps2/Settings/PlayLists.txt",         &Webserver::Client::requestEmpty },
	{ "/gamescripts/bfmc/ps2/Settings/ClanMatchOptions.txt",  &Webserver::Client::requestEmpty },
	
	// Local
	{ "/gamescripts/bfmc/ps2/Locale/english.pus",             &Webserver::Client::requestEmpty },
	{ "/gamescripts/bfmc/ps2/Locale/language.hdt",            &Webserver::Client::requestEmpty },
	
	// Advertisements
	{ "/gamescripts/bfmc/ps2/Ads/advert.us.sux",              &Webserver::Client::requestEmpty },
	{ "/gamescripts/bfmc/ps2/Ads/advert.us.txt",              &Webserver::Client::requestEmpty },
	
	// bfmc.gamespy.com
	// Stats
	{ "/BFMC/Stats/getplayerinfo.aspx",                       &Webserver::Client::requestGetPlayerInfo },  // Need to intergrate with database
	{ "/BFMC/Stats/stats.aspx",                               &Webserver::Client::requestStats },          // Once players has stats in database this will be completed
	
	// Clan
	{ "/BFMC/Clans/claninfo.aspx",                            &Webserver::Client::requestClanInfo },       // Done
	{ "/BFMC/Clans/clanmembers.aspx",                         &Webserver::Client::requestClanMembers },    // Done
	{ "/BFMC/Clans/leaderboard.aspx",                         &Webserver::Client::requestLeaderboard },    // 
	{ "/BFMC/Clans/createclan.aspx",                          &Webserver::Client::requestCreateClan },     // Done
	{ "/BFMC/Clans/updateclan.aspx",                          &Webserver::Client::requestUpdateClan },     // Done
	{ "/BFMC/Clans/disband.aspx",                             &Webserver::Client::requestDisband },        // Done
	{ "/BFMC/Clans/changerank.aspx",                          &Webserver::Client::requestChangeRank },     // Done
	{ "/BFMC/Clans/addmember.aspx",                           &Webserver::Client::requestAddMember },      // Done
	{ "/BFMC/Clans/deletemember.aspx",                        &Webserver::Client::requestDeleteMember },   // Done
	{ "/BFMC/Clans/clanmessage.aspx",                         &Webserver::Client::requestClanMessage },    // Done
};

Webserver::Client::Client(int socket, struct sockaddr_in address)
{
	this->_socket = socket;
	this->_address = address;
}

Webserver::Client::~Client()
{
	this->Disconnect();
}

void Webserver::Client::Listen()
{
	while(true)
	{
		std::vector<char> buffer(4096, 0);
		HTTPMessageParser http_parser;
		HTTPMessage http_request;
		
		// Read socket
		int v = read(this->_socket, &(buffer[0]), 4096);
		
		// If error or no data is recieved we end the connection
		if(v <= 0)
		{
			break;
		}
		
		// Resize buffer
		buffer.resize(v);
		
		// Parse buffer to http header
		http_parser.Parse(&http_request, &(buffer[0]));
		
		// Trigger onRequest event
		this->onRequest(http_request);
	}
	
	this->Disconnect();
}

void Webserver::Client::Disconnect()
{
	close(this->_socket);
	g_webserver_server->onClientDisconnect(*this);
}

void Webserver::Client::Send(const std::string &msg) const
{
	send(this->_socket, msg.c_str(), msg.size(), 0);
}

void Webserver::Client::Send(const atomizes::HTTPMessage &http_response) const
{
	this->Send(http_response.ToString());
}

/*
	Events
*/
void Webserver::Client::onRequest(const atomizes::HTTPMessage &http_request)
{
	if(http_request.GetMethod() == MessageMethod::GET)
	{
		std::string url_base;
		UrlRequest::UrlVariables url_variables;
		
		this->_LogTransaction("-->", http_request.GetPath());
		
		// Split url into url base and variables
		UrlRequest::GetUrlElements(http_request.GetPath(), url_base, url_variables);
		
		if (mRequestActions.find(url_base) != mRequestActions.end())
		{
			// Get Function address
			RequestActionFunc func = mRequestActions[url_base];
		
			// Execute action function with class object.
			(this->*(func))(http_request, url_variables);
		}
		else
		{
			std::unique_lock<std::mutex> guard(g_mutex_io);
			
			std::cout << "action \"" << url_base << "\"not implemented!" << std::endl;
			
			guard.unlock();
			
			//this->Disconnect();
		}
	}
}

/*
	
*/
void Webserver::Client::requestNews(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	this->_SendFile("data/news.txt");
}

/*
	pid:		Player id
	getarg:		this argument defines the parameters it needs to return
		score:		Total score
		ran:		Rank
		pph:		Point per Hour
		kills:		Total kills
		suicides:	Total suicides
		time:		Total time played in seconds
		lavd:		Total LAV's destroyed
		mavd:		Total MAV's destroyed
		havd:		Total HAV's destroyed
		hed:		Total Helicopters destroyed
		pld:		?? Some other vehicle ??
		bod:		Total Boats destoyed
		k1:			Total kills assualt kit
		s1:			?? 
		k2:			Total kills sniper kit 
		s2:			??
		k3:			Total kills special Op. kit
		s3:			??
		k4:			Total kills Combat engineer kit 
		s4:			??
		k5:			Total kills Support kit  
		s5:			??
		tk:			
		medals:		Earned medals.
		ttb			Total times been the top player in the game
		mv			Total mayor victories
		ngp			Total Parcipated game sessions
*/
void Webserver::Client::requestGetPlayerInfo(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	Battlefield::PlayerStats stats;
	
	try
	{
		std::string profileid = url_variables.at("pid");
		
		if(profileid == "10036819")
		{
			stats.useExample();
		}
	}
	catch(...) {}
	
	std::vector<int> v_stats = stats.GetStatsVector();
	
	atomizes::HTTPMessage http_response = this->_defaultResponseHeader();
	
	http_response.SetStatusCode(200);
	http_response.SetMessageBody(Util::ToString(v_stats));
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestStats(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	try
	{
		std::string sort = url_variables.at("sort");
		
		if(sort == "rank")
		{
			this->_SendFile("data/stats/sort=rank&pos=1&posafter=9.txt");
		}
		else if(sort == "score")
		{
			this->_SendFile("data/stats/sort=score&pos=1&posafter=9.txt");
		}
		else if(sort == "pph")
		{
			this->_SendFile("data/stats/sort=pph&pos=1&posafter=9.txt");
		}
		else if(sort == "kills")
		{
			try
			{
				std::string k_filter = url_variables.at("k_filter");
			
				if(k_filter == "assault")
				{
					this->_SendFile("data/stats/sort=kills&pos=1&posafter=9.txt");
				}
				else if(k_filter == "engineer")
				{
					this->_SendFile("data/stats/sort=kills&pos=1&posafter=9.txt");
				}
				else if(k_filter == "sniper")
				{
					this->_SendFile("data/stats/sort=kills&pos=1&posafter=9.txt");
				}
				else if(k_filter == "specialops")
				{
					this->_SendFile("data/stats/sort=kills&pos=1&posafter=9.txt");
				}
				else if(k_filter == "support")
				{
					this->_SendFile("data/stats/sort=kills&pos=1&posafter=9.txt");
				}
			}
			catch(const std::out_of_range &e)
			{
				this->_SendFile("data/stats/sort=kills&pos=1&posafter=9.txt");
			}
		}
		else if(sort == "ratio")
		{
			try
			{
				std::string k_filter = url_variables.at("k_filter");
			
				if(k_filter == "assault")
				{
					this->_SendFile("data/stats/sort=ratio&pos=1&posafter=9.txt");
				}
				else if(k_filter == "engineer")
				{
					this->_SendFile("data/stats/sort=ratio&pos=1&posafter=9.txt");
				}
				else if(k_filter == "sniper")
				{
					this->_SendFile("data/stats/sort=ratio&pos=1&posafter=9.txt");
				}
				else if(k_filter == "specialops")
				{
					this->_SendFile("data/stats/sort=ratio&pos=1&posafter=9.txt");
				}
				else if(k_filter == "support")
				{
					this->_SendFile("data/stats/sort=ratio&pos=1&posafter=9.txt");
				}
			}
			catch(const std::out_of_range &e)
			{
				this->_SendFile("data/stats/sort=ratio&pos=1&posafter=9.txt");
			}
		}
		else if(sort == "vehicles")
		{
			try
			{
				std::string v_filter = url_variables.at("v_filter");
			
				if(v_filter == "light")
				{
					this->_SendFile("data/stats/sort=vehicles&pos=1&posafter=9.txt");
				}
				else if(v_filter == "medium")
				{
					this->_SendFile("data/stats/sort=vehicles&pos=1&posafter=9.txt");
				}
				else if(v_filter == "heavy")
				{
					this->_SendFile("data/stats/sort=vehicles&pos=1&posafter=9.txt");
				}
				else if(v_filter == "boats")
				{
					this->_SendFile("data/stats/sort=vehicles&pos=1&posafter=9.txt");
				}
				else if(v_filter == "helis")
				{
					this->_SendFile("data/stats/sort=vehicles&pos=1&posafter=9.txt");
				}
			}
			catch(const std::out_of_range &e)
			{
				this->_SendFile("data/stats/sort=vehicles&pos=1&posafter=9.txt");
			}
		}
		else
		{
			this->_SendFile("data/stats/sort=score&pos=1&posafter=9.txt");
		}
	}
	catch(int e)
	{
		
	}
	
	this->Disconnect();
}

/*
	
*/
void Webserver::Client::requestClanInfo(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	Battlefield::Clan clan;
	Battlefield::Player player;
	
	// Get player
	auto it = url_variables.find("profileid");
	if (it != url_variables.end())
	{
		player.SetProfileId(it->second);
	}
	
	// Get Clan id
	g_database->queryClanByPlayer(clan, player);
	
	// Get clan information
	g_database->queryClanByClanId(clan);
	
	HTTPMessage http_response = this->_defaultResponseHeader();
	
	http_response.SetStatusCode(200);
	http_response.SetMessageBody(clan.responseGetClanInfo());
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestClanMembers(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	Battlefield::Clan clan;
	
	// Get player
	auto it = url_variables.find("clanid");
	if (it != url_variables.end())
	{
		clan.SetClanId(it->second);
	}

	// Get clan information
	g_database->queryClanRanksByClanId(clan);
	
	HTTPMessage http_response = this->_defaultResponseHeader();
	
	http_response.SetStatusCode(200);
	http_response.SetMessageBody(clan.responseGetClanMembers());
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestLeaderboard(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	this->_SendFile("data/leaderboard/startrank=1&endrank=7.txt");
}

void Webserver::Client::requestCreateClan(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	HTTPMessage http_response = this->_defaultResponseHeader();
	Battlefield::Clan clan;
	Battlefield::Player player;
	
	// Get clan and player information based of the authtoken
	this->_GetSessionPlayerAndClan(url_variables, clan, player);
	
	// If player can be found and player is not in clan
	if(player.GetProfileId() != -1 && clan.GetClanId() == -1)
	{
		Battlefield::Clan new_clan;
		
		// Copy url variables into clan
		new_clan.updateInformation(url_variables);
		
		// get clan by name or tag
		g_database->queryClanByNameOrTag(new_clan);
		
		// Check Clan name is not in use
		if(new_clan.GetClanId() == -1)
		{
			// Insert clan in database
			g_database->insertClan(new_clan);
			
			// Get Clan id
			g_database->queryClanByNameOrTag(new_clan);
			
			// Make player leader of clan
			g_database->insertClanRank(new_clan, player, Battlefield::Clan::Ranks::Leader);
			
			http_response.SetMessageBody("OK");        // Clan succesfull created!
		}
		else
		{
			http_response.SetMessageBody("NAMEINUSE"); // Clan name already used
		}
	}
	else
	{
		http_response.SetMessageBody("CANTJOIN");
	}
	
	http_response.SetStatusCode(200);
	//http_response.SetMessageBody("BANNEDWORD");  // Clan name has bad word
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestUpdateClan(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	HTTPMessage http_response = this->_defaultResponseHeader();
	Battlefield::Clan clan;
	Battlefield::Player player;
	
	// Get clan and player information based of the authtoken
	this->_GetSessionPlayerAndClan(url_variables, clan, player);
	
	// If player can be found and player is in clan
	if(player.GetProfileId() != -1 && clan.GetClanId() != -1)
	{
		// Get clan information
		g_database->queryClanByClanId(clan);
		
		// Get all ranks in clan
		g_database->queryClanRanksByClanId(clan);
		
		Battlefield::Clan::Ranks rank = clan.GetRank(player.GetProfileId());
		
		// If rank is clan leader
		if(rank == Battlefield::Clan::Ranks::Leader)
		{
			// Copy url variables into clan
			clan.updateInformation(url_variables, true);
			
			// Insert clan in database
			g_database->updateClan(clan);
			
			http_response.SetMessageBody("OK");
		}
		else
		{
			http_response.SetMessageBody("NOTLEADER");
		}
	}
	else
	{
		http_response.SetMessageBody("INVALIDCLAN");
	}
	
	http_response.SetStatusCode(200);
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestDisband(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	HTTPMessage http_response = this->_defaultResponseHeader();
	Battlefield::Clan clan;
	Battlefield::Player player;
	
	// Get clan and player information based of the authtoken
	this->_GetSessionPlayerAndClan(url_variables, clan, player);
	
	// If player can be found and player is in a clan
	if(player.GetProfileId() != -1 && clan.GetClanId() != -1)
	{
		// Get all ranks in clan
		g_database->queryClanRanksByClanId(clan);
		
		Battlefield::Clan::Ranks rank = clan.GetRank(player.GetProfileId());
		
		// If rank is clan leader
		if(rank == Battlefield::Clan::Ranks::Leader)
		{
			// Remove all clan ranks
			g_database->removeClanRanksByClanId(clan);
			
			// remove clan
			g_database->removeClan(clan);
			
			http_response.SetMessageBody("OK");
		}
		else
		{
			http_response.SetMessageBody("NOTLEADER");
		}
	}
	else
	{
		http_response.SetMessageBody("INVALIDCLAN");
	}
	
	http_response.SetStatusCode(200);
	
	//http_response.SetMessageBody("OK");
	//http_response.SetMessageBody("NOTLEADER");
	//http_response.SetMessageBody("INVALIDCLAN");
	//http_response.SetMessageBody("ERROR");
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestChangeRank(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	HTTPMessage http_response = this->_defaultResponseHeader();
	Battlefield::Clan clan;
	Battlefield::Player player;
	
	// Get clan and player information based of the authtoken
	this->_GetSessionPlayerAndClan(url_variables, clan, player);
	
	// If player can be found and player is in a clan
	if(player.GetProfileId() != -1 && clan.GetClanId() != -1)
	{
		Battlefield::Player target_player;
		Battlefield::Clan   target_clan;
		
		// Get target player
		auto it = url_variables.find("profileid");
		if (it != url_variables.end())
		{
			target_player.SetProfileId(it->second);
		}
		
		// Get target player information
		g_database->queryClanByPlayer(target_clan, target_player);
		
		// Get new rank
		Battlefield::Clan::Ranks new_rank = Battlefield::Clan::Ranks::Unknown_Rank;
		
		// Convert url variable rank
		auto it2 = url_variables.find("rank");
		if (it2 != url_variables.end())
		{
			new_rank = clan.convertRank(it2->second);
		}
		
		// Check target player
		if(
			target_player.GetProfileId() != -1 &&          // Valid target player profileid must be supplied
			target_clan.GetClanId() != -1 &&               // target player must be in a clan
			clan.GetClanId() == target_clan.GetClanId() && // player and target player must be in the same clan
			new_rank != Battlefield::Clan::Ranks::Unknown_Rank
		)
		{
			// Get all ranks in clan
			g_database->queryClanRanksByClanId(clan);
			
			// Get clan ranks
			Battlefield::Clan::Ranks rank = clan.GetRank(player.GetProfileId());
			Battlefield::Clan::Ranks target_rank = clan.GetRank(target_player.GetProfileId());
			
			// Check rank is higher
			if(rank == Battlefield::Clan::Ranks::Leader)
			{
				if(new_rank == Battlefield::Clan::Ranks::Leader)
				{
					// Demote rank
					g_database->updateClanRank(clan, player, Battlefield::Clan::Ranks::Co_Leader);
				}
				
				// give target player new rank
				g_database->updateClanRank(clan, target_player, new_rank);
				
				this->_SendBuddyMessage(
					player.GetProfileId(),
					target_player.GetProfileId(),
					"Your rank in the clan has been changed. Login again to get the update!"
				);
				
				http_response.SetMessageBody("OK");
			}
			else
			{
				http_response.SetMessageBody("NOTLEADER");
			}
		}
		else
		{
			http_response.SetMessageBody("ERROR");
		}
	}
	else
	{
		http_response.SetMessageBody("INVALIDCLAN");
	}
	
	http_response.SetStatusCode(200);
	
	//http_response.SetMessageBody("OK");
	//http_response.SetMessageBody("NOTMEMBER");
	//http_response.SetMessageBody("NOTLEADER");
	//http_response.SetMessageBody("INVALIDCLAN");
	//http_response.SetMessageBody("ERROR");
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestAddMember(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	HTTPMessage http_response = this->_defaultResponseHeader();
	Battlefield::Clan clan;
	Battlefield::Player player;
	
	// Get clan and player information based of the authtoken
	this->_GetSessionPlayerAndClan(url_variables, clan, player);
	
	// If player can be found and player is in a clan
	if(player.GetProfileId() != -1 || clan.GetClanId() != -1)
	{
		// Get all ranks in clan
		g_database->queryClanRanksByClanId(clan);
		
		Battlefield::Clan::Ranks rank = clan.GetRank(player.GetProfileId());
		
		// If the player rank is not Co-Leader or higher
		if(rank <= Battlefield::Clan::Ranks::Co_Leader)
		{
			Battlefield::Player target_player;
			
			auto it = url_variables.find("profileid");
			if (it != url_variables.end())
			{
				target_player.SetProfileId(it->second);
			}
			
			if(target_player.GetProfileId() != -1)
			{
				// Make player leader of clan
				g_database->insertClanRank(clan, target_player, Battlefield::Clan::Ranks::Member);
				
				this->_SendBuddyMessage(
					player.GetProfileId(),
					target_player.GetProfileId(),
					"You joined a new clan. Login again to get the update!"
				);
				
				http_response.SetMessageBody("OK");
			}
			else
			{
				http_response.SetMessageBody("ERROR");
			}
		}
		else
		{
			http_response.SetMessageBody("NOTLEADER");
		}
	}
	else
	{
		http_response.SetMessageBody("INVALIDCLAN");
	}
	
	http_response.SetStatusCode(200);
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestDeleteMember(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	HTTPMessage http_response = this->_defaultResponseHeader();
	Battlefield::Clan clan;
	Battlefield::Player player;
	
	// Get clan and player information based of the authtoken
	this->_GetSessionPlayerAndClan(url_variables, clan, player);
	
	// If player cant be found or player is not in clan
	if(player.GetProfileId() != -1 && clan.GetClanId() != -1)
	{
		Battlefield::Player target_player;
		Battlefield::Clan   target_clan;
		
		// Get target player
		auto it = url_variables.find("profileid");
		if (it != url_variables.end())
		{
			target_player.SetProfileId(it->second);
		}
		
		// Get target player information
		g_database->queryClanByPlayer(target_clan, target_player);
		
		// Check target player
		if(
			target_player.GetProfileId() != -1 &&       // Valid target player profileid must be supplied
			target_clan.GetClanId() != -1 &&            // target player must be in a clan
			clan.GetClanId() == target_clan.GetClanId() // player and target player must be in the same clan
		)
		{
			// Get all ranks in clan
			g_database->queryClanRanksByClanId(clan);
			
			// Get clan ranks
			Battlefield::Clan::Ranks rank = clan.GetRank(player.GetProfileId());
			Battlefield::Clan::Ranks target_rank = clan.GetRank(target_player.GetProfileId());
			
			// Check rank is higher or it tries to remove itself as a none leader. Leader can only disband clan.
			if(
				rank < target_rank ||
				(
					player.GetProfileId() == target_player.GetProfileId() &&
					rank != Battlefield::Clan::Ranks::Leader
				)
			)
			{
				// Remove player from clan
				g_database->removeClanRank(clan, target_player);
				
				if(player.GetProfileId() != target_player.GetProfileId())
				{
					this->_SendBuddyMessage(
						player.GetProfileId(),
						target_player.GetProfileId(),
						"You have been removed from the clan. Login again to get the update!"
					);
				}
				
				http_response.SetMessageBody("OK");
			}
			else
			{
				http_response.SetMessageBody("NOTLEADER");
			}
		}
		else
		{
			http_response.SetMessageBody("ERROR");
		}
	}
	else
	{
		http_response.SetMessageBody("INVALIDCLAN");
	}
	
	http_response.SetStatusCode(200);
	//http_response.SetMessageBody("NOTMEMBER");
	//http_response.SetMessageBody("NOTLEADER");
	//http_response.SetMessageBody("LEADER");
	//http_response.SetMessageBody("INVALIDCLAN");
	//http_response.SetMessageBody("ERROR");
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestClanMessage(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	HTTPMessage http_response = this->_defaultResponseHeader();
	Battlefield::Clan clan;
	Battlefield::Player player;
	
	// Get clan and player information based of the authtoken
	this->_GetSessionPlayerAndClan(url_variables, clan, player);
	
	// If player can be found and player is in a clan
	if(player.GetProfileId() != -1 || clan.GetClanId() != -1)
	{
		auto it = url_variables.find("message");
		if (it != url_variables.end())
		{
			std::string message = it->second;
			
			// Get all ranks in clan
			g_database->queryClanRanksByClanId(clan);
			
			std::map<int, Battlefield::Clan::Ranks> ranks = clan.GetRanks();
			int player_profileid = player.GetProfileId();
			
			for(const auto& rank : ranks)
			{
				if(player_profileid != rank.first)
				{
					this->_SendBuddyMessage(player_profileid, rank.first, "Clan message: " + message);
				}
			}
		
			http_response.SetMessageBody("OK");
		}
		else
		{
			http_response.SetMessageBody("ERROR");
		}
	}
	else
	{
		http_response.SetMessageBody("INVALIDCLAN");
	}
	
	http_response.SetStatusCode(200);
	//http_response.SetMessageBody("INVALIDCLAN");
	//http_response.SetMessageBody("ERROR");
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestEmpty(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	HTTPMessage http_response = this->_defaultResponseHeader();
	
	http_response.SetStatusCode(200);
	http_response.SetMessageBody("\r\n");
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

/*
	Private functions
*/
void Webserver::Client::_LogTransaction(const std::string &direction, const std::string &response) const
{
	std::lock_guard<std::mutex> guard(g_mutex_io);
	
	std::cout << std::setfill(' ') << std::setw(21) << this->GetAddress() << " " << direction << " " << response << std::endl;
}

atomizes::HTTPMessage Webserver::Client::_defaultResponseHeader() const
{
	HTTPMessage http_response;
	
	http_response.SetHeader("Accept-Ranges", "bytes");
	http_response.SetHeader("Content-Type", "text/plain");
	http_response.SetHeader("Server", "BF2-MC");
	
	return http_response;
}

std::string Webserver::Client::_readFile(const std::string &file_name) const
{
	std::ifstream input;
	std::string data;
	
	input.open("../" + file_name, std::ifstream::in);
	
	if(input.is_open())
	{
		data.append((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	
		input.close();
	}
	
	return data;
}

void Webserver::Client::_SendFile(const std::string &file_name) const
{
	atomizes::HTTPMessage http_response = this->_defaultResponseHeader();
	
	std::string data = this->_readFile(file_name);
	
	http_response.SetStatusCode(200);
	http_response.SetMessageBody(data);
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

int Webserver::Client::_GetSessionProfileId(const UrlRequest::UrlVariables &url_variables) const
{
	int profileid = -1;
	
	try
	{
		std::string authtoken = url_variables.at("authtoken");
		
		// Find player profileid with current gpcm session
		for(Net::Socket* client : g_gpcm_server->_clients)
		{
			GPCM::Client* gpcm_client = reinterpret_cast<GPCM::Client*>(client);
			
			if(gpcm_client->_session_authtoken == authtoken)
			{
				profileid = gpcm_client->_session_profileid;
			}	
		}
	}
	catch(...) {};
	
	return profileid;
}

void Webserver::Client::_GetSessionPlayerAndClan(const UrlRequest::UrlVariables &url_variables, Battlefield::Clan& clan, Battlefield::Player& player) const
{
	// Get session profileid base of the authtoken
	int profileid = this->_GetSessionProfileId(url_variables);
	
	// Set player profileid
	player.SetProfileId(profileid);
	
	// Get clan information based of player profileid
	g_database->queryClanByPlayer(clan, player);
}

void Webserver::Client::_SendBuddyMessage(int profileid, int target_profileid, const std::string& msg) const
{
	for(Net::Socket* client : g_gpcm_server->_clients)
	{
		GPCM::Client* gpcm_client = reinterpret_cast<GPCM::Client*>(client);
		
		if(gpcm_client->_session_profileid == target_profileid)
		{
			std::string response = GameSpy::Parameter2Response({
				"bm", "1",
				"f", std::to_string(profileid),
				"msg", msg,
				"final"
			});
			
			gpcm_client->Send(response);
			
			this->_LogTransaction("<--", response);
		}
	}
}

