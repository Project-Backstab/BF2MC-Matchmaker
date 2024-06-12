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

void Webserver::Client::requestClanInfo(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	Battlefield::Clan clan;
	Battlefield::Player player;
	atomizes::HTTPMessage http_response = this->_defaultResponseHeader();
	std::string response = "\r\n";
	
	// Get player
	auto it = url_variables.find("profileid");
	if (it != url_variables.end())
	{
		player.SetProfileId(it->second);
		
		// Get Clan id
		g_database->queryClanByProfileId(clan, player);
	}
	
	it = url_variables.find("name");
	if(it != url_variables.end() && Util::isAscii(it->second))
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
	
	// Get clan ranks information from database
	g_database->queryClanRanksByClanId(clan);
	
	if(clan.GetClanId() != -1)
	{
		response = "OK\r\n";
		response += "clanID," + std::to_string(clan.GetClanId()) + "\r\n";
		response += "name," + clan.GetName() + "\r\n";
		response += "tag," + clan.GetTag() + "\r\n";
		response += "homepage," + clan.GetHomepage() + "\r\n";
		response += "info," + clan.GetInfo() + "\r\n";
		response += "region," + std::to_string(static_cast<uint8_t>(clan.GetRegion())) + "\r\n";
		response += "lastreportdate,69_1337_69\r\n";
		
		response += "rating," + std::to_string(clan.GetScore()) + "\r\n";
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
			response += "\r\n" + std::to_string(pair.first) + "," + std::to_string(static_cast<uint8_t>(pair.second));
		}
	}
	
	// Create http response
	atomizes::HTTPMessage http_response = this->_defaultResponseHeader();
	http_response.SetStatusCode(200);
	http_response.SetMessageBody(response);
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestLeaderboard(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	atomizes::HTTPMessage http_response = this->_defaultResponseHeader();
	std::string response = "\r\n";

	Battlefield::RankClans rank_clans;

	auto it = url_variables.find("clanid");
	if (it != url_variables.end())
	{
		Battlefield::Clan clan;

		clan.SetClanId(it->second);

		g_database->queryLeaderboardClanByClanId(rank_clans, clan);
	}
	else
	{
		uint32_t offset = 0;
		uint32_t limit = 7;

		it = url_variables.find("startrank");
		auto it2 = url_variables.find("endrank");

		if (it != url_variables.end() && it2 != url_variables.end())
		{
			try
			{
				offset = std::stoul(it->second) - 1;				
			}
			catch(...) {};
		}
		
		g_database->queryLeaderboardClan(rank_clans, limit, offset);
	}

	if(rank_clans.size() > 0)
	{
		response = "OK\r\n";
		for (const auto& pair : rank_clans)
		{
			response += std::to_string(pair.second.GetClanId()) + ",";
			response += std::to_string(pair.first) + ",";
			response += pair.second.GetTag() + ",";
			response += pair.second.GetName() + ",";
			response += std::to_string(pair.second.GetScore()) + ",";
			response += std::to_string(pair.second.GetWins()) + ",";
			response += std::to_string(pair.second.GetLosses()) + ",";
			response += std::to_string(pair.second.GetDraws()) + "\n";
		}
	}

	http_response.SetStatusCode(200);
	http_response.SetMessageBody(response);
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestCreateClan(const atomizes::HTTPMessage& http_request, const std::string& url_base, 
		const Util::Url::Variables& url_variables)
{
	atomizes::HTTPMessage http_response = this->_defaultResponseHeader();
	Battlefield::Clan clan;
	Battlefield::Player player;
	
	// Get clan and player information based of the authtoken
	this->_GetSessionPlayerAndClan(url_variables, clan, player);
	
	// If player can be found and player is not in clan
	if(player.GetProfileId() != -1 && clan.GetClanId() == -1)
	{
		Battlefield::Clan new_clan;

		// Copy url variables into clan
		if(this->_updateClanInformation(new_clan, url_variables))
		{
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
	atomizes::HTTPMessage http_response = this->_defaultResponseHeader();
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
			if(this->_updateClanInformation(clan, url_variables, true))
			{
				Battlefield::Clan check_clan;

				check_clan.SetTag(clan.GetTag());

				g_database->queryClanByNameOrTag(check_clan);

				if(check_clan.GetClanId() == -1 || // New clan tag is not been used yet
					check_clan.GetClanId() == clan.GetClanId() // Or clan tag is owned by its own
				)
				{
					// Insert clan in database
					g_database->updateClan(clan);
			
					http_response.SetMessageBody("OK");
				}
				else
				{
					http_response.SetMessageBody("ERROR");
				}
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

void Webserver::Client::requestDisband(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	atomizes::HTTPMessage http_response = this->_defaultResponseHeader();
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
			g_database->disableClan(clan);
			
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
	atomizes::HTTPMessage http_response = this->_defaultResponseHeader();
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
		g_database->queryClanByProfileId(target_clan, target_player);
		
		// Get new rank
		Battlefield::Clan::Ranks new_rank = Battlefield::Clan::Ranks::Unknown;
		
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
			new_rank != Battlefield::Clan::Ranks::Unknown
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
				
				// Send to the invited player a Clan update call
				GPCM::Client::SendBuddyMessage(
					player.GetProfileId(),
					target_player.GetProfileId(),
					"1",
					"BFMCC-UPDATE	"
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
	atomizes::HTTPMessage http_response = this->_defaultResponseHeader();
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
			Battlefield::Clan target_clan;
			
			auto it = url_variables.find("profileid");
			if (it != url_variables.end())
			{
				target_player.SetProfileId(it->second);
			}
			
			if(target_player.GetProfileId() != -1)
			{
				// Get target clan
				g_database->queryClanByProfileId(target_clan, target_player);
				
				// Check if target player is already in a clan
				if(target_clan.GetClanId() != -1)
				{
					// Remove target player from old clan
					g_database->removeClanRank(target_clan, target_player);
				}
				
				// Add target player to clan
				g_database->insertClanRank(clan, target_player, Battlefield::Clan::Ranks::Member);
				
				// Send to the accepted clan member a clan update call
				GPCM::Client::SendBuddyMessage(
					player.GetProfileId(),
					target_player.GetProfileId(),
					"1",
					"BFMCC-UPDATE	"
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
	atomizes::HTTPMessage http_response = this->_defaultResponseHeader();
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
		g_database->queryClanByProfileId(target_clan, target_player);
		
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
					// Send to the invited player a Clan update call
					GPCM::Client::SendBuddyMessage(
						player.GetProfileId(),
						target_player.GetProfileId(),
						"1",
						"BFMCC-UPDATE	"
					);
					
					// Send resignation letter
					GPCM::Client::SendBuddyMessage(
						player.GetProfileId(),
						target_player.GetProfileId(),
						"1",
						"You have been removed from the clan."
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
	atomizes::HTTPMessage http_response = this->_defaultResponseHeader();
	Battlefield::Clan clan;
	Battlefield::Player player;
	
	// Get clan and player information based of the authtoken
	this->_GetSessionPlayerAndClan(url_variables, clan, player);
	
	// If player can be found and player is in a clan
	if(player.GetProfileId() != -1 || clan.GetClanId() != -1)
	{
		auto it = url_variables.find("message");
		if (it != url_variables.end() && Util::isAscii(it->second))
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
					GPCM::Client::SendBuddyMessage(player_profileid, rank.first, "1", "Clan message: " + message);
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

