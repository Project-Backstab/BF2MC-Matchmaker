#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <regex>

#include <settings.h>
#include <logger.h>
#include <server.h>
#include <gpcm/client.h>
#include <globals.h>
#include <database.h>
#include <util.h>
#include <atomizes.hpp>

#include <webserver/client.h>

using namespace atomizes;

typedef void (Webserver::Client::*RequestActionFunc)(const atomizes::HTTPMessage&, const std::string&, const Util::Url::Variables&);

static std::map<std::string, RequestActionFunc> mRequestActions = 
{
	// www.easports.com
	// News
	{ "/gamescripts/bfmc/ps2/en/PS2news_de_DE.txt",           &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/en/PS2news_en_GB.txt",           &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/en/PS2news_en_US.txt",           &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/en/PS2news_es_ES.txt",           &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/en/PS2news_fr_FR.txt",           &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/en/PS2news_it_IT.txt",           &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/en/PS2news_jp.txt",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/en/PS2news_nl.txt",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/en/PS2news_sv.txt",              &Webserver::Client::requestFile               },
	
	// GameSpy Licenses
	{ "/gamescripts/bfmc/ps2/GameSpy/gs.de",                  &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/GameSpy/gs.en",                  &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/GameSpy/gs.es",                  &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/GameSpy/gs.fr",                  &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/GameSpy/gs.gb",                  &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/GameSpy/gs.it",                  &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/GameSpy/gs.jp",                  &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/GameSpy/gs.nl",                  &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/GameSpy/gs.roa",                 &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/GameSpy/gs.sv",                  &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/GameSpy/gs.uk",                  &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/GameSpy/gs.us",                  &Webserver::Client::requestFile               },
	
	// EA Licenses
	{ "/gamescripts/bfmc/ps2/Ea/Ea.de",                       &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ea/Ea.en",                       &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ea/Ea.es",                       &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ea/Ea.fr",                       &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ea/Ea.gb",                       &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ea/Ea.it",                       &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ea/Ea.jp",                       &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ea/Ea.nl",                       &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ea/Ea.roa",                      &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ea/Ea.sv",                       &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ea/Ea.uk",                       &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ea/Ea.us",                       &Webserver::Client::requestFile               },
	
	// PunkBuster Licenses
	{ "/gamescripts/bfmc/ps2/PunkBuster/pb.de",               &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/PunkBuster/pb.en",               &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/PunkBuster/pb.es",               &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/PunkBuster/pb.fr",               &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/PunkBuster/pb.gb",               &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/PunkBuster/pb.it",               &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/PunkBuster/pb.jp",               &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/PunkBuster/pb.nl",               &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/PunkBuster/pb.roa",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/PunkBuster/pb.sv",               &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/PunkBuster/pb.uk",               &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/PunkBuster/pb.us",               &Webserver::Client::requestFile               },
	
	// Settings
	{ "/gamescripts/bfmc/ps2/Settings/GameModes.txt",         &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Settings/QuickMatchOptions.txt", &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Settings/PlayLists.txt",         &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Settings/ClanMatchOptions.txt",  &Webserver::Client::requestFile               },
	
	// Locale
	{ "/gamescripts/bfmc/ps2/Locale/language.hdt",            &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Locale/brittish.pus",            &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Locale/dutch.pus",               &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Locale/english.pus",             &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Locale/french.pus",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Locale/german.pus",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Locale/italian.pus",             &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Locale/japanese.pus",            &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Locale/spanish.pus",             &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Locale/swedish.pus",             &Webserver::Client::requestFile               },
	
	// Advertisements
	{ "/gamescripts/bfmc/ps2/Ads/advert.de.sux",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ads/advert.de.txt",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ads/advert.en.sux",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ads/advert.en.txt",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ads/advert.es.sux",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ads/advert.es.txt",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ads/advert.fr.sux",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ads/advert.fr.txt",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ads/advert.gb.sux",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ads/advert.it.txt",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ads/advert.jp.sux",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ads/advert.jp.txt",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ads/advert.nl.sux",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ads/advert.nl.txt",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ads/advert.roa.sux",             &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ads/advert.roa.txt",             &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ads/advert.sv.sux",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ads/advert.sv.txt",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ads/advert.uk.sux",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ads/advert.uk.txt",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ads/advert.us.sux",              &Webserver::Client::requestFile               },
	{ "/gamescripts/bfmc/ps2/Ads/advert.us.txt",              &Webserver::Client::requestFile               },
	
	// bfmc.gamespy.com
	// Stats
	{ "/BFMC/Stats/getplayerinfo.aspx",                       &Webserver::Client::requestGetPlayerInfo      }, // Done
	{ "/BFMC/Stats/stats.aspx",                               &Webserver::Client::requestStats              }, // Done
	
	// Clan
	{ "/BFMC/Clans/claninfo.aspx",                            &Webserver::Client::requestClanInfo           }, // Done
	{ "/BFMC/Clans/clanmembers.aspx",                         &Webserver::Client::requestClanMembers        }, // Done
	{ "/BFMC/Clans/leaderboard.aspx",                         &Webserver::Client::requestLeaderboard        }, // 
	{ "/BFMC/Clans/createclan.aspx",                          &Webserver::Client::requestCreateClan         }, // Done
	{ "/BFMC/Clans/updateclan.aspx",                          &Webserver::Client::requestUpdateClan         }, // Done
	{ "/BFMC/Clans/disband.aspx",                             &Webserver::Client::requestDisband            }, // Done
	{ "/BFMC/Clans/changerank.aspx",                          &Webserver::Client::requestChangeRank         }, // Done
	{ "/BFMC/Clans/addmember.aspx",                           &Webserver::Client::requestAddMember          }, // Done
	{ "/BFMC/Clans/deletemember.aspx",                        &Webserver::Client::requestDeleteMember       }, // Done
	{ "/BFMC/Clans/clanmessage.aspx",                         &Webserver::Client::requestClanMessage        }, // Done
	
	// I like memes :D
	{ "/",                                                    &Webserver::Client::requestMeme               },
	{ "/favicon.ico",                                         &Webserver::Client::requestEmpty              },
	
	// API
	{ "/API/servers/live",                                    &Webserver::Client::requestAPIServersLive     },
	{ "/API/player",                                          &Webserver::Client::requestAPIPlayer          },
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
		Util::Url::Variables url_variables;
		
		this->_LogTransaction("-->", http_request.GetPath());
		
		// Split url into url base and variables
		Util::Url::GetElements(http_request.GetPath(), url_base, url_variables);
		
		auto it = mRequestActions.find(url_base);
		if (it != mRequestActions.end())
		{
			// Get Function address
			RequestActionFunc func = it->second;
		
			// Execute action function with class object.
			(this->*(func))(http_request, url_base, url_variables);
		}
		else
		{		
			Logger::warning("action \"" + url_base + "\" not implemented!", Server::Type::Webserver);
		}
	}
}

// www.easports.com
void Webserver::Client::requestFile(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	this->_SendFile("data" + url_base);
}

// bfmc.gamespy.com
// Stats
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
void Webserver::Client::requestGetPlayerInfo(const atomizes::HTTPMessage& http_request,  const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	atomizes::HTTPMessage http_response = this->_defaultResponseHeader();
	Battlefield::Player player;
	std::string response;
	
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
	
	response += std::to_string(player.GetScore()) + ",";
	response += std::to_string(player.GetRank()) + ",";
	response += std::to_string(player.GetPPH()) + ",";
	response += std::to_string(player.GetKills()) + ",";
	response += std::to_string(player.GetSuicides()) + ",";
	response += std::to_string(player.GetTime()) + ",";
	response += std::to_string(player.GetLAVsDestroyed()) + ",";
	response += std::to_string(player.GetMAVsDestroyed()) + ",";
	response += std::to_string(player.GetHAVsDestroyed()) + ",";
	response += std::to_string(player.GetHelicoptersDestroyed()) + ",";
	response += std::to_string(player.GetPlanesDestroyed()) + ",";
	response += std::to_string(player.GetBoatsDestroyed()) + ",";
	response += std::to_string(player.GetKillsAssualtKit()) + ",";
	response += std::to_string(player.GetDeathsAssualtKit()) + ",";
	response += std::to_string(player.GetKillsSniperKit()) + ",";
	response += std::to_string(player.GetDeathsSniperKit()) + ",";
	response += std::to_string(player.GetKillsSpecialOpKit()) + ",";
	response += std::to_string(player.GetDeathsSpecialOpKit()) + ",";
	response += std::to_string(player.GetKillsCombatEngineerKit()) + ",";
	response += std::to_string(player.GetDeathsCombatEngineerKit()) + ",";
	response += std::to_string(player.GetKillsSupportKit()) + ",";
	response += std::to_string(player.GetDeathsSupportKit()) + ",";
	response += std::to_string(player.GetTeamKills()) + ",";
	response += std::to_string(player.GetMedals()) + ",";
	response += std::to_string(player.GetTotalTopPlayer()) + ",";
	response += std::to_string(player.GetTotalVictories()) + ",";
	response += std::to_string(player.GetTotalGameSessions());
	
	http_response.SetStatusCode(200);
	http_response.SetMessageBody(response);
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestStats(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
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
		
		this->_LogTransaction("<--", "HTTP/1.1 200 OK");
	}
}

// Clan
void Webserver::Client::requestClanInfo(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	Battlefield::Clan clan;
	Battlefield::Player player;
	HTTPMessage http_response = this->_defaultResponseHeader();
	std::string response = "\r\n";
	
	// Get player
	auto it = url_variables.find("profileid");
	if (it != url_variables.end())
	{
		player.SetProfileId(it->second);
		
		// Get Clan id
		g_database->queryClanByPlayer(clan, player);
	}
	
	it = url_variables.find("name");
	if(it != url_variables.end())
	{
		clan.SetName(it->second);
		clan.SetTag(it->second);
		
		g_database->queryClanByNameOrTag(clan);
	}
	
	it = url_variables.find("clanid");
	if(it != url_variables.end())
	{
		clan.SetClanId(it->second);
		
		g_database->queryClanByClanId(clan);
	}
	
	
	// Get clan information
	g_database->queryClanByClanId(clan);
	
	if(clan.GetClanId() != -1)
	{
		response = "OK\r\n";
		response += "clanID," + std::to_string(clan.GetClanId()) + "\r\n";
		response += "name," + clan.GetName() + "\r\n";
		response += "tag," + clan.GetTag() + "\r\n";
		response += "homepage," + clan.GetHomepage() + "\r\n";
		response += "info," + clan.GetInfo() + "\r\n";
		response += "region," + std::to_string(clan.GetRegion()) + "\r\n";
		response += "lastreportdate," + clan.GetDate() + "\r\n";
		
		response += "rating," + std::to_string(clan.GetRating()) + "\r\n";
		response += "wins," + std::to_string(clan.GetWins()) + "\r\n";
		response += "losses," + std::to_string(clan.GetLosses()) + "\r\n";
		response += "draws," + std::to_string(clan.GetDraws()) + "\r\n";
		
		response += "membercount," + std::to_string(clan.GetRanks().size()) + "\r\n";
	}
	
	http_response.SetStatusCode(200);
	http_response.SetMessageBody(response);
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestClanMembers(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
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
	
	// Create response
	std::string response = "\r\n";
	if(clan.GetClanId() != -1)
	{
		response = "OK\r\n";
		
		for (const auto& pair : clan.GetRanks())
		{
			response += "\r\n" + std::to_string(pair.first) + "," + std::to_string(pair.second);
		}
	}
	
	// Create http response
	HTTPMessage http_response = this->_defaultResponseHeader();
	http_response.SetStatusCode(200);
	http_response.SetMessageBody(response);
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestLeaderboard(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	this->_SendFile("data/examples/leaderboard/startrank=1&endrank=7.txt");
}

void Webserver::Client::requestCreateClan(const atomizes::HTTPMessage& http_request, const std::string& url_base, 
		const Util::Url::Variables& url_variables)
{
	HTTPMessage http_response = this->_defaultResponseHeader();
	Battlefield::Clan clan;
	Battlefield::Player player;
	
	// Get clan and player information based of the authtoken
	this->_GetSessionPlayerAndClan(url_variables, clan, player);
	
	// If player can be found and player is not in clan
	if(player.GetProfileId() != -1 && clan.GetClanId() == -1)
	{
		auto it = url_variables.find("tag");
		
		// clan tag is 3 characters
		if(it->second.size() == 3)
		{
			Battlefield::Clan new_clan;
			
			// Copy url variables into clan
			this->_updateClanInformation(new_clan, url_variables);
			
			// get clan by name or tag
			g_database->queryClanByNameOrTag(new_clan);
			
			// Check Clan name is not in use
			if(new_clan.GetClanId() == -1)
			{
				// Insert clan in database
				g_database->insertClan(new_clan);
				
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
			http_response.SetMessageBody("ERROR");
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

void Webserver::Client::requestUpdateClan(const atomizes::HTTPMessage &http_request, const std::string& url_base,
		const Util::Url::Variables &url_variables)
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
			this->_updateClanInformation(clan, url_variables, true);
			
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

void Webserver::Client::requestDisband(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
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

void Webserver::Client::requestChangeRank(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
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

void Webserver::Client::requestAddMember(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
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

void Webserver::Client::requestDeleteMember(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
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

void Webserver::Client::requestClanMessage(const atomizes::HTTPMessage& http_request, const std::string& url_base, 
		const Util::Url::Variables& url_variables)
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

void Webserver::Client::requestEmpty(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	HTTPMessage http_response = this->_defaultResponseHeader();
	
	http_response.SetStatusCode(200);
	http_response.SetMessageBody("\r\n");
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestMeme(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
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
	std::shared_lock<std::shared_mutex> guard(g_mutex_settings); // settings lock (read)
	
	bool show_console = (g_settings["webserver"]["show_requests"].asBool() && direction == "-->") ||
						(g_settings["webserver"]["show_responses"].asBool() && direction == "<--");
	
	Logger::info(this->GetAddress() + " " + direction + " " + response,
			Server::Type::Webserver, show_console);
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
	
	if(data.size() != 0)
	{
		http_response.SetMessageBody(data);
	}
	else
	{ // fix: Prevent to hang the http connection
		http_response.SetMessageBody("\r\n");
	}
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::_GetSessionPlayerAndClan(const Util::Url::Variables& url_variables, Battlefield::Clan& clan, Battlefield::Player& player) const
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

void Webserver::Client::_updateClanInformation(Battlefield::Clan& clan,
		const Util::Url::Variables &url_variables, bool is_update)
{
	for(const auto &url_variable : url_variables)
	{
		if(url_variable.first == "name" && !is_update)
		{
			clan.SetName(url_variable.second);
		}
		else if(url_variable.first == "tag")
		{
			clan.SetTag(url_variable.second);
		}
		else if(url_variable.first == "homepage")
		{
			clan.SetHomepage(url_variable.second);
		}
		else if(url_variable.first == "info")
		{
			clan.SetInfo(url_variable.second);
		}
		else if(url_variable.first == "region")
		{
			clan.SetRegion(url_variable.second);
		}
	}
}

