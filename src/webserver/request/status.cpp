#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <regex>
#include <thread>

#include <settings.h>
#include <logger.h>
#include <server.h>
#include <gpcm/client.h>
#include <globals.h>
#include <database.h>
#include <util.h>
#include <atomizes.hpp>
#include <service/file_system.h>

#include <webserver/client.h>

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
	g_database->queryPlayerStatsByProfileId(player);
	
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
	response += std::to_string(player.GetSpawnsAssualtKit()) + ",";
	response += std::to_string(player.GetKillsSniperKit()) + ",";
	response += std::to_string(player.GetSpawnsSniperKit()) + ",";
	response += std::to_string(player.GetKillsSpecialOpKit()) + ",";
	response += std::to_string(player.GetSpawnsSpecialOpKit()) + ",";
	response += std::to_string(player.GetKillsCombatEngineerKit()) + ",";
	response += std::to_string(player.GetSpawnsCombatEngineerKit()) + ",";
	response += std::to_string(player.GetKillsSupportKit()) + ",";
	response += std::to_string(player.GetSpawnsSupportKit()) + ",";
	response += std::to_string(player.GetTeamKills()) + ",";
	response += std::to_string(player.GetMedals()) + ",";
	response += std::to_string(player.GetTotalTopPlayer()) + ",";
	response += std::to_string(player.GetTotalVictories()) + ",";
	response += std::to_string(player.GetTotalGameSessions());
	
	http_response.SetStatusCode(200);
	http_response.SetMessageBody(response);
	
	this->Send(http_response);
	
	//this->_LogTransaction("<--", "HTTP/1.1 200 OK");
	this->_LogTransaction("<--", "HTTP/1.1 200 OK -> " + response);
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
				g_database->queryLeaderboardRank(rank_players);
			}
			else if(profileids.size() == 1)
			{
				g_database->queryLeaderboardRankByProfileId(rank_players, self_profileid);
			}
			else
			{
				g_database->queryLeaderboardRankByFriends(rank_players, profileids);
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
				g_database->queryLeaderboardType(rank_players, "score");
			}
			else if(profileids.size() == 1)
			{
				g_database->queryLeaderboardTypeByProfileid(rank_players, "score", self_profileid);
			}
			else
			{
				g_database->queryLeaderboardTypeByFriends(rank_players, "score", profileids);
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
				g_database->queryLeaderboardType(rank_players, "pph");
			}
			else if(profileids.size() == 1)
			{
				g_database->queryLeaderboardTypeByProfileid(rank_players, "pph", self_profileid);
			}
			else
			{
				g_database->queryLeaderboardTypeByFriends(rank_players, "pph", profileids);
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
						g_database->queryLeaderboardType(rank_players, "k1");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryLeaderboardTypeByProfileid(rank_players, "k1", self_profileid);
					}
					else
					{
						g_database->queryLeaderboardTypeByFriends(rank_players, "k1", profileids);
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
						g_database->queryLeaderboardType(rank_players, "k2");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryLeaderboardTypeByProfileid(rank_players, "k2", self_profileid);
					}
					else
					{
						g_database->queryLeaderboardTypeByFriends(rank_players, "k2", profileids);
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
						g_database->queryLeaderboardType(rank_players, "k3");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryLeaderboardTypeByProfileid(rank_players, "k3", self_profileid);
					}
					else
					{
						g_database->queryLeaderboardTypeByFriends(rank_players, "k3", profileids);
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
						g_database->queryLeaderboardType(rank_players, "k4");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryLeaderboardTypeByProfileid(rank_players, "k4", self_profileid);
					}
					else
					{
						g_database->queryLeaderboardTypeByFriends(rank_players, "k4", profileids);
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
						g_database->queryLeaderboardType(rank_players, "k5");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryLeaderboardTypeByProfileid(rank_players, "k5", self_profileid);
					}
					else
					{
						g_database->queryLeaderboardTypeByFriends(rank_players, "k5", profileids);
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
					g_database->queryLeaderboardType(rank_players, "kills");
				}
				else if(profileids.size() == 1)
				{
					g_database->queryLeaderboardTypeByProfileid(rank_players, "kills", self_profileid);
				}
				else
				{
					g_database->queryLeaderboardTypeByFriends(rank_players, "kills", profileids);
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
						g_database->queryLeaderboardRatio(rank_players, "k1", "s1");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryLeaderboardRatioByProfileid(rank_players, self_profileid, "k1", "s1");
					}
					else
					{
						g_database->queryLeaderboardRatioByFriends(rank_players, profileids, "k1", "s1");
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
						g_database->queryLeaderboardRatio(rank_players, "k2", "s2");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryLeaderboardRatioByProfileid(rank_players, self_profileid, "k2", "s2");
					}
					else
					{
						g_database->queryLeaderboardRatioByFriends(rank_players, profileids, "k2", "s2");
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
						g_database->queryLeaderboardRatio(rank_players, "k3", "s3");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryLeaderboardRatioByProfileid(rank_players, self_profileid, "k3", "s3");
					}
					else
					{
						g_database->queryLeaderboardRatioByFriends(rank_players, profileids, "k3", "s3");
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
						g_database->queryLeaderboardRatio(rank_players, "k4", "s4");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryLeaderboardRatioByProfileid(rank_players, self_profileid, "k4", "s4");
					}
					else
					{
						g_database->queryLeaderboardRatioByFriends(rank_players, profileids, "k4", "s4");
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
						g_database->queryLeaderboardRatio(rank_players, "k5", "s5");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryLeaderboardRatioByProfileid(rank_players, self_profileid, "k5", "s5");
					}
					else
					{
						g_database->queryLeaderboardRatioByFriends(rank_players, profileids, "k5", "s5");
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
					g_database->queryLeaderboardRatio(rank_players, "kills", "deaths");
				}
				else if(profileids.size() == 1)
				{
					g_database->queryLeaderboardRatioByProfileid(rank_players, self_profileid, "kills", "deaths");
				}
				else
				{
					g_database->queryLeaderboardRatioByFriends(rank_players, profileids, "kills", "deaths");
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
						g_database->queryLeaderboardType(rank_players, "lavd");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryLeaderboardTypeByProfileid(rank_players, "lavd", self_profileid);
					}
					else
					{
						g_database->queryLeaderboardTypeByFriends(rank_players, "lavd", profileids);
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
						g_database->queryLeaderboardType(rank_players, "mavd");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryLeaderboardTypeByProfileid(rank_players, "mavd", self_profileid);
					}
					else
					{
						g_database->queryLeaderboardTypeByFriends(rank_players, "mavd", profileids);
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
						g_database->queryLeaderboardType(rank_players, "havd");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryLeaderboardTypeByProfileid(rank_players, "havd", self_profileid);
					}
					else
					{
						g_database->queryLeaderboardTypeByFriends(rank_players, "havd", profileids);
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
						g_database->queryLeaderboardType(rank_players, "hed");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryLeaderboardTypeByProfileid(rank_players, "hed", self_profileid);
					}
					else
					{
						g_database->queryLeaderboardTypeByFriends(rank_players, "hed", profileids);
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
						g_database->queryLeaderboardType(rank_players, "bod");
					}
					else if(profileids.size() == 1)
					{
						g_database->queryLeaderboardTypeByProfileid(rank_players, "bod", self_profileid);
					}
					else
					{
						g_database->queryLeaderboardTypeByFriends(rank_players, "bod", profileids);
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
					g_database->queryLeaderboardType(rank_players, "vehicles");
				}
				else if(profileids.size() == 1)
				{
					g_database->queryLeaderboardTypeByProfileid(rank_players, "vehicles", self_profileid);
				}				
				else
				{
					g_database->queryLeaderboardTypeByFriends(rank_players, "vehicles", profileids);
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

