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
	{ "/BFMC/Clans/leaderboard.aspx",                         &Webserver::Client::requestLeaderboard        }, // Done
	{ "/BFMC/Clans/createclan.aspx",                          &Webserver::Client::requestCreateClan         }, // Done
	{ "/BFMC/Clans/updateclan.aspx",                          &Webserver::Client::requestUpdateClan         }, // Done
	{ "/BFMC/Clans/disband.aspx",                             &Webserver::Client::requestDisband            }, // Done
	{ "/BFMC/Clans/changerank.aspx",                          &Webserver::Client::requestChangeRank         }, // Done
	{ "/BFMC/Clans/addmember.aspx",                           &Webserver::Client::requestAddMember          }, // Done
	{ "/BFMC/Clans/deletemember.aspx",                        &Webserver::Client::requestDeleteMember       }, // Done
	{ "/BFMC/Clans/clanmessage.aspx",                         &Webserver::Client::requestClanMessage        }, // Done
	
	// I like memes :D
	{ "/",                                                    &Webserver::Client::requestMeme               },
	{ "/index.html",                                          &Webserver::Client::requestMeme               },
	{ "/favicon.ico",                                         &Webserver::Client::requestEmpty              },
	
	// API
	{ "/API/servers/live",                                    &Webserver::Client::requestAPIServersLive            },
	{ "/API/game",                                            &Webserver::Client::requestAPIGame                   },
	{ "/API/games",                                           &Webserver::Client::requestAPIGames                  },
	{ "/API/player",                                          &Webserver::Client::requestAPIPlayer                 },
	{ "/API/clan",                                            &Webserver::Client::requestAPIClan                   },
	{ "/API/leaderboard",                                     &Webserver::Client::requestAPILeaderboard            },
	{ "/API/leaderboard/clan",                                &Webserver::Client::requestAPILeaderboardClan        },
	{ "/API/clan/simulation",                                 &Webserver::Client::requestAPIClanSimulation         },
	{ "/API/admin/clients",                                   &Webserver::Client::requestAPIAdminClients           },
	{ "/API/admin/kick",                                      &Webserver::Client::requestAPIAdminKick              },
	{ "/API/admin/message",                                   &Webserver::Client::requestAPIAdminMessage           },
	{ "/API/admin/playerstats/recalc",                        &Webserver::Client::requestAPIAdminPlayerStatsRecalc },
};

Webserver::Client::Client(int socket, struct sockaddr_in address)
{
	this->_socket = socket;
	this->_address = address;
	this->UpdateLastRecievedTime();
}

Webserver::Client::~Client()
{
	this->Disconnect();
}

void Webserver::Client::Listen()
{
	std::vector<char> buffer(4096, 0);
	atomizes::HTTPMessageParser http_parser;
	atomizes::HTTPMessage http_request;
	
	// Read socket
	int recv_size = read(this->_socket, &(buffer[0]), 4096);
	
	// If error or no data is recieved we end the connection
	if(recv_size <= 0)
	{
		this->Disconnect();

		return;
	}
	
	// Resize buffer
	buffer.resize(recv_size);
	
	this->UpdateLastRecievedTime();

	// Parse buffer to http header
	http_parser.Parse(&http_request, &(buffer[0]));
	
	// Trigger onRequest event
	this->onRequest(http_request);
	
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

// Events

void Webserver::Client::onRequest(const atomizes::HTTPMessage &http_request)
{
	if(http_request.GetMethod() == atomizes::MessageMethod::GET)
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

void Webserver::Client::requestFile(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	this->_SendFile("../data" + url_base);
}

void Webserver::Client::requestEmpty(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	atomizes::HTTPMessage http_response = this->_defaultResponseHeader();
	
	http_response.SetStatusCode(200);
	http_response.SetMessageBody("\r\n");
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestMeme(const atomizes::HTTPMessage& http_request, const std::string& url_base,
		const Util::Url::Variables& url_variables)
{
	this->_SendFile("../data/meme/index.html");
}

// Private functions

void Webserver::Client::_LogTransaction(const std::string& direction, const std::string& response) const
{
	std::shared_lock<std::shared_mutex> guard(g_settings_mutex); // settings lock (read)
	
	//if ((g_logger_mode & Logger::Mode::Development) == 0)
	//{
	//	return;
	//}
	
	bool show_console = (g_settings["webserver"]["show_requests"].asBool() && direction == "-->") ||
						(g_settings["webserver"]["show_responses"].asBool() && direction == "<--");
	
	Logger::info(this->GetAddress() + " " + direction + " " + response,
			Server::Type::Webserver, show_console);
}

atomizes::HTTPMessage Webserver::Client::_defaultResponseHeader(bool isPlainText) const
{
	atomizes::HTTPMessage http_response;
	
	http_response.SetHeader("Server", "BF2MC-Matchmaker");
	http_response.SetHeader("Accept-Ranges", "bytes");

	if(isPlainText)
		http_response.SetHeader("Content-Type", "text/plain");
	
	return http_response;
}

bool Webserver::Client::_readFile(const std::string& file_name, std::string& data) const
{
	bool finished = false;

	// Load file from memory
	if(g_file_system->GetFile(file_name, data))
	{
		// Debug
		//Logger::debug("file_name = " + file_name);
		//Logger::debug("file size = " + std::to_string(data.size()));
		
		return true;
	}
	
	std::ifstream input;

	try
	{
		input.open(file_name, std::ifstream::in | std::ifstream::binary);

		if(input.is_open())
		{
			data.append((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
			
			input.close();
			
			return true;
		}
	}
	catch(...)
	{
		Logger::error(file_name);
	}
	
	//Logger::debug(std::to_string(data.size()));
	
	return false;
}

void Webserver::Client::_SendFile(const std::string& file_name) const
{
	std::string data;
	atomizes::HTTPMessage http_response = this->_defaultResponseHeader(false);
	
	if(this->_readFile(file_name, data) && data.size() != 0)
	{
		http_response.SetStatusCode(200);
		http_response.SetMessageBody(data);
	}
	else
	{ // fix: Prevent to hang the http connection
		http_response.SetStatusCode(404);
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
		g_database->queryClanByProfileId(clan, player);
	}
}

bool Webserver::Client::_updateClanInformation(Battlefield::Clan& clan,
		const Util::Url::Variables &url_variables, bool is_update)
{
	for(const auto &url_variable : url_variables)
	{
		std::string key = url_variable.first;
		std::string value = Util::Url::Decode(url_variable.second);
		
		if(key == "authtoken")
			continue;

		if(!Util::isAscii(value) || (key == "name" && is_update))
		{
			return false;
		}

		auto it = Battlefield::Clan::SetterMap.find(key);
		if(it == Battlefield::Clan::SetterMap.end() || (clan.*(it->second))(value))
		{
			return false;
		}
	}

	return true;
}

// Static functions

void Webserver::Client::Heartbeat()
{
	Logger::info("Heartbeat started", Server::Webserver);

	while(true)
	{
		std::this_thread::sleep_for (std::chrono::seconds(60));

		auto target_time = std::chrono::system_clock::now() - std::chrono::minutes(1);

		for(std::shared_ptr<Net::Socket> client : g_webserver_server->GetClients())
		{
			auto last_recieved = client.get()->GetLastRecievedTime();

			if (last_recieved <= target_time)
			{
				client.get()->Close();
			}
		}
	}
}

