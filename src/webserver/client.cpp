#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <regex>

#include <settings.h>
#include <server.h>
#include <gpcm/client.h>
#include <globals.h>
#include <database.h>
#include <util.h>
#include <atomizes.hpp>

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
	{ "/gamescripts/bfmc/ps2/Ads/advert.us.sux",              &Webserver::Client::requestAdvertSux },
	{ "/gamescripts/bfmc/ps2/Ads/advert.us.txt",              &Webserver::Client::requestAdvertTxt },
	
	// bfmc.gamespy.com
	// Stats
	{ "/BFMC/Stats/getplayerinfo.aspx",                       &Webserver::Client::requestGetPlayerInfo },  // Done
	{ "/BFMC/Stats/stats.aspx",                               &Webserver::Client::requestStats },          // Done
	
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
	
	// I like memes :D
	{ "/",                                                    &Webserver::Client::requestMeme  },
	{ "/favicon.ico",                                         &Webserver::Client::requestEmpty  },
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
	this->Close();
	g_webserver_server->onClientDisconnect(*this);
}

void Webserver::Client::Send(const atomizes::HTTPMessage &http_response) const
{
	this->Net::Socket::Send(http_response.ToString());
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
		
		auto it = mRequestActions.find(url_base);
		if (it != mRequestActions.end())
		{
			// Get Function address
			RequestActionFunc func = it->second;
		
			// Execute action function with class object.
			(this->*(func))(http_request, url_variables);
		}
		else
		{		
			this->onUnimplementedAction(url_base);
		}
	}
}

void Webserver::Client::onUnimplementedAction(const std::string &action)
{
	std::shared_lock<std::shared_mutex> guard2(g_mutex_settings); // settings lock (read)
	
	std::cout << "action \"" << action << "\" not implemented!" << std::endl;
	
	Json::Value banned_requests = g_settings["webserver"]["banned_requests"];
	
	if(!banned_requests.isArray())
	{
		return;
	}
	
	std::string client_ip = this->GetIP();
	std::regex ip_pattern("^10.10.10.");
	
	for(Json::Value banned_request : banned_requests)
	{
		if(banned_request.isString())
		{
			std::regex pattern(banned_request.asString());
			
			if(std::regex_search(action, pattern) && !std::regex_search(client_ip, ip_pattern))
			{
				std::cout << "Bannable action found \"" << action << "\" by " << client_ip << std::endl;
				
				std::string query = "sudo iptables -I INPUT -s " + client_ip + " -j DROP";
				int result = system(query.c_str());
			}
		}
	}
}

/*
	
*/
void Webserver::Client::requestNews(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables)
{
	this->_SendFile("data/news.txt");
}

void Webserver::Client::requestAdvertSux(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables)
{
	this->_SendFile("data/advert/advert.us.sux");
}

void Webserver::Client::requestAdvertTxt(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables)
{
	this->_SendFile("data/advert/advert.us.txt");
}

/*
	pid:		Player profileid
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
void Webserver::Client::requestGetPlayerInfo(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables)
{
	atomizes::HTTPMessage http_response = this->_defaultResponseHeader();
	Battlefield::Player player;
	
	// Get player profileid
	auto it = url_variables.find("pid");
	if (it != url_variables.end())
	{
		player.SetProfileId(it->second);
	}
	
	// Patch player stats
	//player.PlayerStats::useExample();
	//g_database->updatePlayerStats(player);
	
	// Get player stats
	g_database->queryPlayerStats(player);
	
	std::vector<int> v_stats = player.GetStatsVector();
	
	http_response.SetStatusCode(200);
	http_response.SetMessageBody(Util::ToString(v_stats));
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestStats(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables)
{
	atomizes::HTTPMessage http_response = this->_defaultResponseHeader();
	Battlefield::RankPlayers rank_players;
	std::string data = "";
	std::vector<int> profileids;
	int self_profileid = -1;
	
	auto it = url_variables.find("pid");
	if (it != url_variables.end())
	{
		profileids = Util::convertProfileIdToVector(it->second);
		
		if(profileids.size() >= 1)
		{
			self_profileid = profileids[0];
		}
	}
	
	it = url_variables.find("sort");
	if (it != url_variables.end())
	{
		std::string sort = it->second;
		
		if(sort == "rank")
		{
			if(profileids.size() == 0)
			{
				g_database->queryRankPlayersTopByRank(rank_players);
			}
			else if(profileids.size() == 1)
			{
				g_database->queryRankPlayersSelfByRank(rank_players, self_profileid);
			}
			else
			{
				g_database->queryRankPlayersTopFriendsByRank(rank_players, profileids);
			}
			
			for (const auto& pair : rank_players)
			{
				if(self_profileid == pair.second.GetProfileId())
				{
					data += "-";
				}
				
				data += std::to_string(pair.first) + ",";
				data += pair.second.GetUniquenick() + ",";
				data += std::to_string(pair.second.GetRank()) + ",";
				data += std::to_string(pair.second.GetPPH()) + ",";
				data += std::to_string(pair.second.GetScore()) + "\n";
			}
		}
		else if(sort == "score")
		{	
			if(profileids.size() == 0)
			{
				g_database->queryRankPlayersTopByType(rank_players, "score");
			}
			else if(profileids.size() == 1)
			{
				g_database->queryRankPlayersSelfByType(rank_players, "score", self_profileid);
			}
			else
			{
				g_database->queryRankPlayersTopFriendsByType(rank_players, "score", profileids);
			}
			
			for (const auto& pair : rank_players)
			{
				if(self_profileid == pair.second.GetProfileId())
				{
					data += "-";
				}
				
				data += std::to_string(pair.first) + ",";
				data += pair.second.GetUniquenick() + ",";
				data += std::to_string(pair.second.GetScore()) + "\n";
			}
		}
		else if(sort == "pph")
		{
			if(profileids.size() == 0)
			{
				g_database->queryRankPlayersTopByType(rank_players, "pph");
			}
			else if(profileids.size() == 1)
			{
				g_database->queryRankPlayersSelfByType(rank_players, "pph", self_profileid);
			}
			else
			{
				g_database->queryRankPlayersTopFriendsByType(rank_players, "pph", profileids);
			}
			
			for (const auto& pair : rank_players)
			{
				if(self_profileid == pair.second.GetProfileId())
				{
					data += "-";
				}
				
				data += std::to_string(pair.first) + ",";
				data += pair.second.GetUniquenick() + ",";
				data += std::to_string(pair.second.GetPPH()) + "\n";
			}
		}
		else if(sort == "kills")
		{
			auto it2 = url_variables.find("k_filter");
			if (it2 != url_variables.end())
			{
				std::string k_filter = it2->second;
				
				if(k_filter == "assault")
				{
					if(profileids.size() == 0)
					{
						g_database->queryRankPlayersTopByType(rank_players, "k1");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryRankPlayersSelfByType(rank_players, "k1", self_profileid);
					}
					else
					{
						g_database->queryRankPlayersTopFriendsByType(rank_players, "k1", profileids);
					}
					
					for (const auto& pair : rank_players)
					{
						if(self_profileid == pair.second.GetProfileId())
						{
							data += "-";
						}
						
						data += std::to_string(pair.first) + ",";
						data += pair.second.GetUniquenick() + ",";
						data += std::to_string(pair.second.GetKillsAssualtKit()) + "\n";
					}
				}
				else if(k_filter == "sniper")
				{
					if(profileids.size() == 0)
					{
						g_database->queryRankPlayersTopByType(rank_players, "k2");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryRankPlayersSelfByType(rank_players, "k2", self_profileid);
					}
					else
					{
						g_database->queryRankPlayersTopFriendsByType(rank_players, "k2", profileids);
					}
					
					for (const auto& pair : rank_players)
					{
						if(self_profileid == pair.second.GetProfileId())
						{
							data += "-";
						}
						
						data += std::to_string(pair.first) + ",";
						data += pair.second.GetUniquenick() + ",";
						data += std::to_string(pair.second.GetKillsSniperKit()) + "\n";
					}
				}
				else if(k_filter == "specialops")
				{
					if(profileids.size() == 0)
					{
						g_database->queryRankPlayersTopByType(rank_players, "k3");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryRankPlayersSelfByType(rank_players, "k3", self_profileid);
					}
					else
					{
						g_database->queryRankPlayersTopFriendsByType(rank_players, "k3", profileids);
					}
					
					for (const auto& pair : rank_players)
					{
						if(self_profileid == pair.second.GetProfileId())
						{
							data += "-";
						}
				
						data += std::to_string(pair.first) + ",";
						data += pair.second.GetUniquenick() + ",";
						data += std::to_string(pair.second.GetKillsSpecialOpKit()) + "\n";
					}
				}
				else if(k_filter == "engineer")
				{
					if(profileids.size() == 0)
					{
						g_database->queryRankPlayersTopByType(rank_players, "k4");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryRankPlayersSelfByType(rank_players, "k4", self_profileid);
					}
					else
					{
						g_database->queryRankPlayersTopFriendsByType(rank_players, "k4", profileids);
					}
					
					for (const auto& pair : rank_players)
					{
						if(self_profileid == pair.second.GetProfileId())
						{
							data += "-";
						}
						
						data += std::to_string(pair.first) + ",";
						data += pair.second.GetUniquenick() + ",";
						data += std::to_string(pair.second.GetKillsCombatEngineerKit()) + "\n";
					}
				}
				else if(k_filter == "support")
				{
					if(profileids.size() == 0)
					{
						g_database->queryRankPlayersTopByType(rank_players, "k5");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryRankPlayersSelfByType(rank_players, "k5", self_profileid);
					}
					else
					{
						g_database->queryRankPlayersTopFriendsByType(rank_players, "k5", profileids);
					}
					
					for (const auto& pair : rank_players)
					{
						if(self_profileid == pair.second.GetProfileId())
						{
							data += "-";
						}
						
						data += std::to_string(pair.first) + ",";
						data += pair.second.GetUniquenick() + ",";
						data += std::to_string(pair.second.GetKillsSupportKit()) + "\n";
					}
				}
			}
			else
			{
				if(profileids.size() == 0)
				{
					g_database->queryRankPlayersTopByType(rank_players, "kills");
				}
				else if(profileids.size() == 1)
				{
					g_database->queryRankPlayersSelfByType(rank_players, "kills", self_profileid);
				}
				else
				{
					g_database->queryRankPlayersTopFriendsByType(rank_players, "kills", profileids);
				}
				
				for (const auto& pair : rank_players)
				{
					if(self_profileid == pair.second.GetProfileId())
					{
						data += "-";
					}
					
					data += std::to_string(pair.first) + ",";
					data += pair.second.GetUniquenick() + ",";
					data += std::to_string(pair.second.GetKills()) + "\n";
				}
			}
		}
		else if(sort == "ratio")
		{
			auto it2 = url_variables.find("k_filter");
			if (it2 != url_variables.end())
			{
				std::string k_filter = it2->second;
			
				if(k_filter == "assault")
				{
					if(profileids.size() == 0)
					{
						g_database->queryRankPlayersTopByRatio(rank_players, "k1", "s1");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryRankPlayersSelfByRatio(rank_players, self_profileid, "k1", "s1");
					}
					else
					{
						g_database->queryRankPlayersTopFriendsByRatio(rank_players, profileids, "k1", "s1");
					}
					
					for (const auto& pair : rank_players)
					{
						if(self_profileid == pair.second.GetProfileId())
						{
							data += "-";
						}
						
						data += std::to_string(pair.first) + ",";
						data += pair.second.GetUniquenick() + ",";
						data += std::to_string(pair.second.GetRatioAssualtKit()) + "\n";
					}
				}
				else if(k_filter == "sniper")
				{
					if(profileids.size() == 0)
					{
						g_database->queryRankPlayersTopByRatio(rank_players, "k2", "s2");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryRankPlayersSelfByRatio(rank_players, self_profileid, "k2", "s2");
					}
					else
					{
						g_database->queryRankPlayersTopFriendsByRatio(rank_players, profileids, "k2", "s2");
					}
					
					for (const auto& pair : rank_players)
					{
						if(self_profileid == pair.second.GetProfileId())
						{
							data += "-";
						}
						
						data += std::to_string(pair.first) + ",";
						data += pair.second.GetUniquenick() + ",";
						data += std::to_string(pair.second.GetRatioSniperKit()) + "\n";
					}
				}
				else if(k_filter == "specialops")
				{
					if(profileids.size() == 0)
					{
						g_database->queryRankPlayersTopByRatio(rank_players, "k3", "s3");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryRankPlayersSelfByRatio(rank_players, self_profileid, "k3", "s3");
					}
					else
					{
						g_database->queryRankPlayersTopFriendsByRatio(rank_players, profileids, "k3", "s3");
					}
					
					for (const auto& pair : rank_players)
					{
						if(self_profileid == pair.second.GetProfileId())
						{
							data += "-";
						}
						
						data += std::to_string(pair.first) + ",";
						data += pair.second.GetUniquenick() + ",";
						data += std::to_string(pair.second.GetRatioSpecialOpKit()) + "\n";
					}
				}
				else if(k_filter == "engineer")
				{
					if(profileids.size() == 0)
					{
						g_database->queryRankPlayersTopByRatio(rank_players, "k4", "s4");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryRankPlayersSelfByRatio(rank_players, self_profileid, "k4", "s4");
					}
					else
					{
						g_database->queryRankPlayersTopFriendsByRatio(rank_players, profileids, "k4", "s4");
					}
					
					for (const auto& pair : rank_players)
					{
						if(self_profileid == pair.second.GetProfileId())
						{
							data += "-";
						}
						
						data += std::to_string(pair.first) + ",";
						data += pair.second.GetUniquenick() + ",";
						data += std::to_string(pair.second.GetRatioCombatEngineerKit()) + "\n";
					}
				}
				else if(k_filter == "support")
				{
					if(profileids.size() == 0)
					{
						g_database->queryRankPlayersTopByRatio(rank_players, "k5", "s5");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryRankPlayersSelfByRatio(rank_players, self_profileid, "k5", "s5");
					}
					else
					{
						g_database->queryRankPlayersTopFriendsByRatio(rank_players, profileids, "k5", "s5");
					}
					
					for (const auto& pair : rank_players)
					{
						if(self_profileid == pair.second.GetProfileId())
						{
							data += "-";
						}
						
						data += std::to_string(pair.first) + ",";
						data += pair.second.GetUniquenick() + ",";
						data += std::to_string(pair.second.GetRatioSupportKit()) + "\n";
					}
				}
			}
			else
			{
				if(profileids.size() == 0)
				{
					g_database->queryRankPlayersTopByRatio(rank_players, "kills", "deaths");
				}
				else if(profileids.size() == 1)
				{
					g_database->queryRankPlayersSelfByRatio(rank_players, self_profileid, "kills", "deaths");
				}
				else
				{
					g_database->queryRankPlayersTopFriendsByRatio(rank_players, profileids, "kills", "deaths");
				}
				
				for (const auto& pair : rank_players)
				{
					if(self_profileid == pair.second.GetProfileId())
					{
						data += "-";
					}
					
					data += std::to_string(pair.first) + ",";
					data += pair.second.GetUniquenick() + ",";
					data += std::to_string(pair.second.GetRatio()) + "\n";
				}
			}
		}
		else if(sort == "vehicles")
		{
			auto it2 = url_variables.find("v_filter");
			if (it2 != url_variables.end())
			{
				std::string v_filter = it2->second;
			
				if(v_filter == "light")
				{
					if(profileids.size() == 0)
					{
						g_database->queryRankPlayersTopByType(rank_players, "lavd");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryRankPlayersSelfByType(rank_players, "lavd", self_profileid);
					}
					else
					{
						g_database->queryRankPlayersTopFriendsByType(rank_players, "lavd", profileids);
					}
					
					for (const auto& pair : rank_players)
					{
						if(self_profileid == pair.second.GetProfileId())
						{
							data += "-";
						}
						
						data += std::to_string(pair.first) + ",";
						data += pair.second.GetUniquenick() + ",";
						data += std::to_string(pair.second.GetLAVsDestroyed()) + "\n";
					}
				}
				else if(v_filter == "medium")
				{
					if(profileids.size() == 0)
					{
						g_database->queryRankPlayersTopByType(rank_players, "mavd");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryRankPlayersSelfByType(rank_players, "mavd", self_profileid);
					}
					else
					{
						g_database->queryRankPlayersTopFriendsByType(rank_players, "mavd", profileids);
					}
					
					for (const auto& pair : rank_players)
					{
						if(self_profileid == pair.second.GetProfileId())
						{
							data += "-";
						}
						
						data += std::to_string(pair.first) + ",";
						data += pair.second.GetUniquenick() + ",";
						data += std::to_string(pair.second.GetMAVsDestroyed()) + "\n";
					}
				}
				else if(v_filter == "heavy")
				{
					if(profileids.size() == 0)
					{
						g_database->queryRankPlayersTopByType(rank_players, "havd");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryRankPlayersSelfByType(rank_players, "havd", self_profileid);
					}
					else
					{
						g_database->queryRankPlayersTopFriendsByType(rank_players, "havd", profileids);
					}
					
					for (const auto& pair : rank_players)
					{
						if(self_profileid == pair.second.GetProfileId())
						{
							data += "-";
						}
						
						data += std::to_string(pair.first) + ",";
						data += pair.second.GetUniquenick() + ",";
						data += std::to_string(pair.second.GetHAVsDestroyed()) + "\n";
					}
				}
				else if(v_filter == "helis")
				{
					if(profileids.size() == 0)
					{
						g_database->queryRankPlayersTopByType(rank_players, "hed");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryRankPlayersSelfByType(rank_players, "hed", self_profileid);
					}
					else
					{
						g_database->queryRankPlayersTopFriendsByType(rank_players, "hed", profileids);
					}
					
					for (const auto& pair : rank_players)
					{
						if(self_profileid == pair.second.GetProfileId())
						{
							data += "-";
						}
						
						data += std::to_string(pair.first) + ",";
						data += pair.second.GetUniquenick() + ",";
						data += std::to_string(pair.second.GetHelicoptersDestroyed()) + "\n";
					}
				}
				else if(v_filter == "boats")
				{
					if(profileids.size() == 0)
					{
						g_database->queryRankPlayersTopByType(rank_players, "bod");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryRankPlayersSelfByType(rank_players, "bod", self_profileid);
					}
					else
					{
						g_database->queryRankPlayersTopFriendsByType(rank_players, "bod", profileids);
					}
					
					for (const auto& pair : rank_players)
					{
						if(self_profileid == pair.second.GetProfileId())
						{
							data += "-";
						}
						
						data += std::to_string(pair.first) + ",";
						data += pair.second.GetUniquenick() + ",";
						data += std::to_string(pair.second.GetBoatsDestroyed()) + "\n";
					}
				}
			}
			else
			{
				if(profileids.size() == 0)
				{
					g_database->queryRankPlayersTopByType(rank_players, "vehicles");
				}
				else if(profileids.size() == 1)
				{
					g_database->queryRankPlayersSelfByType(rank_players, "vehicles", self_profileid);
				}				
				else
				{
					g_database->queryRankPlayersTopFriendsByType(rank_players, "vehicles", profileids);
				}
				
				for (const auto& pair : rank_players)
				{
					if(self_profileid == pair.second.GetProfileId())
					{
						data += "-";
					}
					
					data += std::to_string(pair.first) + ",";
					data += pair.second.GetUniquenick() + ",";
					data += std::to_string(pair.second.GetVehiclesDestroyed()) + "\n";
				}
			}
		}
		
		http_response.SetStatusCode(200);
		http_response.SetMessageBody(data);
		
		this->Send(http_response);
		
		this->_LogTransaction("<--", http_response.ToString());
	}
}

/*
	
*/
void Webserver::Client::requestClanInfo(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables)
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

void Webserver::Client::requestClanMembers(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables)
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

void Webserver::Client::requestLeaderboard(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables)
{
	this->_SendFile("data/leaderboard/startrank=1&endrank=7.txt");
}

void Webserver::Client::requestCreateClan(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables)
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

void Webserver::Client::requestDisband(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables)
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

void Webserver::Client::requestChangeRank(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables)
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
				
				GPCM::Client::SendBuddyMessage(
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

void Webserver::Client::requestAddMember(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables)
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
				
				GPCM::Client::SendBuddyMessage(
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

void Webserver::Client::requestDeleteMember(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables)
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
					GPCM::Client::SendBuddyMessage(
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

void Webserver::Client::requestClanMessage(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables)
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
					GPCM::Client::SendBuddyMessage(player_profileid, rank.first, "Clan message: " + message);
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

void Webserver::Client::requestEmpty(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables)
{
	HTTPMessage http_response = this->_defaultResponseHeader();
	
	http_response.SetStatusCode(200);
	http_response.SetMessageBody("\r\n");
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestMeme(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables)
{
	HTTPMessage http_response;
	
	http_response.SetHeader("Accept-Ranges", "bytes");
	http_response.SetHeader("Server", "BF2-MC");
	
	std::string data = this->_readFile("data/meme/index.html");
	
	http_response.SetStatusCode(200);
	http_response.SetMessageBody(data);
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

/*
	Private functions
*/
void Webserver::Client::_LogTransaction(const std::string& direction, const std::string& response) const
{
	std::lock_guard<std::mutex>         guard(g_mutex_io);        // io lock       (read/write)
	std::shared_lock<std::shared_mutex> guard2(g_mutex_settings); // settings lock (read)
	
	if(
		(g_settings["webserver"]["show_requests"].asBool() && direction == "-->") ||
		(g_settings["webserver"]["show_responses"].asBool() && direction == "<--")
	)
	{
		std::cout << std::setfill(' ') << std::setw(21) << this->GetAddress() << " " << direction << " " << response << std::endl;
	}
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

void Webserver::Client::_SendFile(const std::string& file_name) const
{
	atomizes::HTTPMessage http_response = this->_defaultResponseHeader();
	
	std::string data = this->_readFile(file_name);
	
	http_response.SetStatusCode(200);
	http_response.SetMessageBody(data);
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::_GetSessionPlayerAndClan(const UrlRequest::UrlVariables& url_variables, Battlefield::Clan& clan, Battlefield::Player& player) const
{
	GPCM::Session session;
	
	auto it = url_variables.find("authtoken");
	if (it != url_variables.end())
	{
		std::string authtoken = it->second;
		
		session = GPCM::Client::findSessionByAuthtoken(authtoken);
	}
	
	if(session.profileid != -1)
	{
		// Set player profileid
		player.SetProfileId(session.profileid);
		
		// Get clan information based of player profileid
		g_database->queryClanByPlayer(clan, player);
	}
}



