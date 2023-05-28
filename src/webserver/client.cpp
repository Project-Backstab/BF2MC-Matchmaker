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
	{ "/BFMC/Clans/createclan.aspx",                          &Webserver::Client::requestCreateClan },     // Working, needs to add some extra checks
	{ "/BFMC/Clans/updateclan.aspx",                          &Webserver::Client::requestEmpty },          // 
	{ "/BFMC/Clans/disband.aspx",                             &Webserver::Client::requestEmpty },          // 
	{ "/BFMC/Clans/changerank.aspx",                          &Webserver::Client::requestEmpty },          // 
	{ "/BFMC/Clans/addmember.aspx",                           &Webserver::Client::requestEmpty },          // 
	{ "/BFMC/Clans/deletemember.aspx",                        &Webserver::Client::requestEmpty },          // 
	{ "/BFMC/Clans/clanmessage.aspx",                         &Webserver::Client::requestEmpty },          // 
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
	
	try
	{
		std::string profileid = url_variables.at("profileid");
		
		if(!g_database->queryClanByProfileId(clan, profileid))
		{
			return;
		}
	}
	catch(...) {};
	
	if(clan.GetClanId() != -1)
	{
		if(!g_database->queryClanByClanId(clan))
		{
			return;
		}
	}
	
	HTTPMessage http_response = this->_defaultResponseHeader();
	
	http_response.SetStatusCode(200);
	http_response.SetMessageBody(clan.responseGetClanInfo());
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestClanMembers(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	Battlefield::Clan clan;
	
	try
	{
		std::string clanid = url_variables.at("clanid");
		
		clan.SetClanId(clanid);
		
		if(!g_database->queryClanRolesByClanId(clan))
		{
			return;
		}
	}
	catch(...) {};
	
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
	
	int session_profileid = -1;
	std::string authtoken;
	std::string name;
	std::string tag;
	std::string homepage;
	std::string info;
	std::string region;
	
	try
	{
		authtoken = url_variables.at("authtoken");
		name = url_variables.at("name");
		tag = url_variables.at("tag");
		homepage = url_variables.at("homepage");
		info = url_variables.at("info");
		region = url_variables.at("region");
		
		// Find player profileid with current gpcm session
		for(Net::Socket* client : g_gpcm_server->_clients)
		{
			GPCM::Client* gpcm_client = reinterpret_cast<GPCM::Client*>(client);
			
			if(gpcm_client->_session_authtoken == authtoken)
			{
				session_profileid = gpcm_client->_session_profileid;
			}	
		}
	}
	catch(...) {};
	
	// If url variables are not correct or session profileid cant be find
	if(session_profileid == -1)
	{
		http_response.SetMessageBody("CANTJOIN");
	}
	else
	{
		Battlefield::Clan clan;
		
		if(!g_database->queryClanByNameOrTag(clan, name, tag))
		{
			return;
		}
		
		// Clan name already in use
		if(clan.GetClanId() != -1)
		{
			http_response.SetMessageBody("NAMEINUSE"); // Clan name already used
		}
		else
		{
			Battlefield::Clan new_clan;
			new_clan.SetName(name);
			new_clan.SetTag(tag);
			new_clan.SetHomepage(homepage);
			new_clan.SetInfo(info);
			new_clan.SetRegion(region);
			new_clan.SetName(name);
			
			// Insert clan in database
			if(!g_database->insertClan(new_clan))
			{
				return;
			}
			
			// Get Clan id
			if(!g_database->queryClanByNameOrTag(new_clan, name, tag))
			{
				return;
			}
			
			Battlefield::Player player;
			
			player.SetProfileId(session_profileid);
			
			if(!g_database->insertClanRole(new_clan, player, 0))
			{
				return;
			}
			
			http_response.SetMessageBody("OK");        // Clan succesfull created!
		}
		
		//http_response.SetMessageBody("CANTJOIN");  // When you are already in clan
		//http_response.SetMessageBody("BANNEDWORD");  // Clan name has bad word
		
	}
	
	http_response.SetStatusCode(200);
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

void Webserver::Client::requestUpdateClan(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	
}

void Webserver::Client::requestDisband(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	
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

