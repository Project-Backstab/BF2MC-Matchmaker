#include <unistd.h>
#include <iostream>
#include <thread>
#include <iomanip>
#include <map>
#include <fstream>

#include <server.h>
#include <globals.h>
#include <util.h>
#include <urlrequest.h>
#include <battlefield.h>
#include <battlefield/playerstats.h>

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
	{ "/BFMC/Stats/getplayerinfo.aspx",                       &Webserver::Client::requestGetPlayerInfo },
	{ "/BFMC/Stats/stats.aspx",                               &Webserver::Client::requestStats },
	
	// Clan
	{ "/BFMC/Clans/claninfo.aspx",                            &Webserver::Client::requestClanInfo },
	{ "/BFMC/Clans/clanmembers.aspx",                         &Webserver::Client::requestClanMembers },
	{ "/BFMC/Clans/leaderboard.aspx",                         &Webserver::Client::requestLeaderboard },
	{ "/BFMC/Clans/createclan.aspx",                          &Webserver::Client::requestEmpty },
	{ "/BFMC/Clans/updateclan.aspx",                          &Webserver::Client::requestEmpty },
	{ "/BFMC/Clans/disband.aspx",                             &Webserver::Client::requestEmpty },
	{ "/BFMC/Clans/changerank.aspx",                          &Webserver::Client::requestEmpty },
	{ "/BFMC/Clans/addmember.aspx",                           &Webserver::Client::requestEmpty },
	{ "/BFMC/Clans/deletemember.aspx",                        &Webserver::Client::requestEmpty },
	{ "/BFMC/Clans/clanmessage.aspx",                         &Webserver::Client::requestEmpty },
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
		std::string request;
		std::vector<char> buffer(4096, 0);
		
		int v = read(this->_socket, &(buffer[0]), 4096);
		
		// If error or no data is recieved we end the connection
		if(v <= 0)
		{
			break;
		}
		
		// Resize buffer
		buffer.resize(v);
		
		HTTPMessageParser http_parser;
		HTTPMessage http_request;

		http_parser.Parse(&http_request, &(buffer[0]));

		//std::cout << http_request.ToString() << std::endl;
		
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
			
			this->Disconnect();
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
	stats.useExample();
	
	atomizes::HTTPMessage http_response = this->_defaultRequestHeader();
	http_response.SetStatusCode(200);
	http_response.SetMessageBody(stats.ToString());
	
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

void Webserver::Client::requestClanInfo(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	this->_SendFile("data/clan/claninfo/clanid=543151.txt");
}

void Webserver::Client::requestClanMembers(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{	
	this->_SendFile("data/clan/clanmembers/clanid=561717.txt");
}

void Webserver::Client::requestLeaderboard(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	this->_SendFile("data/leaderboard/startrank=1&endrank=7.txt");
}

void Webserver::Client::requestCreateClan(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	
}

void Webserver::Client::requestUpdateClan(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	
}

void Webserver::Client::requestDisband(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	
}

void Webserver::Client::requestEmpty(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables)
{
	HTTPMessage http_response;
	
	http_response.SetStatusCode(200);
	http_response.SetHeader("Accept-Ranges", "bytes");
	http_response.SetHeader("Content-Type", "text/plain");
	http_response.SetHeader("Host", "BF2-MC");
	http_response.SetMessageBody("\r\n");
	
	this->Send(http_response.ToString());
	
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

atomizes::HTTPMessage Webserver::Client::_defaultRequestHeader() const
{
	HTTPMessage http_response;
	
	http_response.SetHeader("Accept-Ranges", "bytes");
	http_response.SetHeader("Content-Type", "text/plain");
	http_response.SetHeader("Host", "BF2-MC");
	
	return http_response;
}

std::string Webserver::Client::_readFile(const std::string &file_name) const
{
	std::ifstream input("../" + file_name, std::ifstream::in);
	
	std::string data((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	
	input.close();
	
	return data;
}

void Webserver::Client::_SendFile(const std::string &file_name) const
{
	atomizes::HTTPMessage http_response = this->_defaultRequestHeader();
	
	std::string data = this->_readFile(file_name);
	
	http_response.SetStatusCode(200);
	http_response.SetMessageBody(data);
	
	this->Send(http_response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

