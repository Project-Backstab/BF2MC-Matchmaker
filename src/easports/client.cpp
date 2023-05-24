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

#include <easports/client.h>

typedef void (EASports::Client::*RequestActionFunc)(const UrlRequest::Parameter&, const UrlRequest::UrlVariables &);

static std::map<std::string, RequestActionFunc> mRequestActions = 
{
	{ "/gamescripts/bfmc/ps2/en/PS2news_en_US.txt",           &EASports::Client::requestNews  },
	
	// Licenses
	{ "/gamescripts/bfmc/ps2/GameSpy/gs.us",                  &EASports::Client::requestEmpty },
	{ "/gamescripts/bfmc/ps2/Ea/Ea.us",                       &EASports::Client::requestEmpty },
	{ "/gamescripts/bfmc/ps2/PunkBuster/pb.us",               &EASports::Client::requestEmpty },
	
	// Configs
	{ "/gamescripts/bfmc/ps2/Settings/GameModes.txt",         &EASports::Client::requestEmpty },
	{ "/gamescripts/bfmc/ps2/Settings/QuickMatchOptions.txt", &EASports::Client::requestEmpty },
	{ "/gamescripts/bfmc/ps2/Settings/PlayLists.txt",         &EASports::Client::requestEmpty },
	{ "/gamescripts/bfmc/ps2/Settings/ClanMatchOptions.txt",  &EASports::Client::requestEmpty },
	
	// Local
	{ "/gamescripts/bfmc/ps2/Locale/english.pus",             &EASports::Client::requestEmpty },
	{ "/gamescripts/bfmc/ps2/Locale/language.hdt",            &EASports::Client::requestEmpty },
	
	// Advertisements
	{ "/gamescripts/bfmc/ps2/Ads/advert.us.sux",              &EASports::Client::requestEmpty },
	{ "/gamescripts/bfmc/ps2/Ads/advert.us.txt",              &EASports::Client::requestEmpty },
	
	// Stats
	{ "/BFMC/Stats/getplayerinfo.aspx",                       &EASports::Client::requestGetPlayerInfo },
	{ "/BFMC/Stats/stats.aspx",                               &EASports::Client::requestStats },
	
	// Clan
	{ "/BFMC/Clans/claninfo.aspx",                            &EASports::Client::requestClanInfo },
	{ "/BFMC/Clans/clanmembers.aspx",                         &EASports::Client::requestClanMembers },
	{ "/BFMC/Clans/leaderboard.aspx",                         &EASports::Client::requestLeaderboard },
	{ "/BFMC/Clans/createclan.aspx",                          &EASports::Client::requestEmpty },
	{ "/BFMC/Clans/updateclan.aspx",                          &EASports::Client::requestEmpty },
	{ "/BFMC/Clans/disband.aspx",                             &EASports::Client::requestEmpty },
	{ "/BFMC/Clans/changerank.aspx",                          &EASports::Client::requestEmpty },
	{ "/BFMC/Clans/addmember.aspx",                           &EASports::Client::requestEmpty },
	{ "/BFMC/Clans/deletemember.aspx",                        &EASports::Client::requestEmpty },
	{ "/BFMC/Clans/clanmessage.aspx",                         &EASports::Client::requestEmpty },
};

EASports::Client::Client(int socket, struct sockaddr_in address)
{
	this->_socket = socket;
	this->_address = address;
}

EASports::Client::~Client()
{
	this->Disconnect();
}

void EASports::Client::Listen()
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
		
		request = Util::Buffer2String(buffer);
		
		this->onRequest(request);
	}
	
	this->Disconnect();
}

void EASports::Client::Disconnect()
{
	close(this->_socket);
	g_easports_server->onClientDisconnect(*this);
}

void EASports::Client::Send(const std::string &msg) const
{
	send(this->_socket, msg.c_str(), msg.size(), 0);
}

/*
	Events
*/
void EASports::Client::onRequest(const std::string &request)
{
	UrlRequest::Parameter parameter = UrlRequest::Request2Parameter(request);
	
	if(parameter.size() >= 2 && parameter[0] == "GET")
	{
		std::string url_base;
		UrlRequest::UrlVariables url_variables;
		
		this->_LogTransaction("-->", parameter[1]);
		
		// Split url into url base and variables
		UrlRequest::GetUrlElements(parameter[1], url_base, url_variables);
		
		if (mRequestActions.find(url_base) != mRequestActions.end())
		{
			// Get Function address
			RequestActionFunc func = mRequestActions[url_base];
		
			// Execute action function with class object.
			(this->*(func))(parameter, url_variables);
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
void EASports::Client::requestNews(const UrlRequest::Parameter& parameter, const UrlRequest::UrlVariables &url_variables)
{
	this->_SendFile("data/news.txt");
}

/*
	pid:		Player id
	getarg:		this argument defines the parameters it needs to return
		score:		Total score
		ran:		Rank
			This value can be ranged between 1 and 20
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
			This integer value can be read as binairy and can be explained like this:
			??????????????x		Service Cross
			?????????????x?		The Bronse Star
			????????????x??		Air Forse Cross
			???????????x???		The Silver star
			??????????x????		The Service Cross, 1st Class
			?????????x?????		The Bronse Star, 1st Class
			????????x??????		Air Force Cross, 1st Class
			???????x???????		Expert Killing
			??????x????????		Expert Shooting
			?????x?????????		Expert Demolition
			????x??????????		Expert Repair
			???x???????????		Expert Healer
			??x????????????		Navy Cross
			?x?????????????		Legion of Merit
			x??????????????		Legion of Merit, 1st Class
		ttb			Total times been the top player in the game
		mv			Total mayor victories
		ngp			Total Parcipated game sessions
*/
void EASports::Client::requestGetPlayerInfo(const UrlRequest::Parameter& parameter, const UrlRequest::UrlVariables &url_variables)
{
	this->_SendFile("data/getplayerinfo/pid=75559074&getarg=score,rank,pph,kills,suicides,time,lavd,mavd,havd,hed,pld,bod,k1,s1,k2,s2,k3,s3,k4,s4,k5,s5,tk,medals,ttb,mv,ngp.txt");
}

void EASports::Client::requestStats(const UrlRequest::Parameter& parameter, const UrlRequest::UrlVariables &url_variables)
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

void EASports::Client::requestClanInfo(const UrlRequest::Parameter& parameter, const UrlRequest::UrlVariables &url_variables)
{
	this->_SendFile("data/clan/claninfo/clanid=543151.txt");
}

void EASports::Client::requestClanMembers(const UrlRequest::Parameter& parameter, const UrlRequest::UrlVariables &url_variables)
{	
	this->_SendFile("data/clan/clanmembers/clanid=561717.txt");
}

void EASports::Client::requestLeaderboard(const UrlRequest::Parameter& parameter, const UrlRequest::UrlVariables &url_variables)
{
	this->_SendFile("data/leaderboard/startrank=1&endrank=7.txt");
}

void EASports::Client::requestCreateClan(const UrlRequest::Parameter& parameter, const UrlRequest::UrlVariables &url_variables)
{
	for(std::string v : parameter)
	{
		std::cout << v << std::endl;
	}
}

void EASports::Client::requestUpdateClan(const UrlRequest::Parameter& parameter, const UrlRequest::UrlVariables &url_variables)
{
	for(std::string v : parameter)
	{
		std::cout << v << std::endl;
	}
}

void EASports::Client::requestDisband(const UrlRequest::Parameter& parameter, const UrlRequest::UrlVariables &url_variables)
{
	for(std::string v : parameter)
	{
		std::cout << v << std::endl;
	}
}

void EASports::Client::requestEmpty(const UrlRequest::Parameter& parameter, const UrlRequest::UrlVariables &url_variables)
{	
	std::string response;
	
	response += "HTTP/1.1 200 OK\r\n";
	response += "Server: BF2-MC\r\n";
	response += "Accept-Ranges: bytes\r\n";
	response += "Content-Type: text/plain\r\n";
	response += "Content-Length: 0\r\n";
	response += "\r\n";
	
	this->Send(response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

/*
	Private functions
*/
void EASports::Client::_LogTransaction(const std::string &direction, const std::string &response) const
{
	std::lock_guard<std::mutex> guard(g_mutex_io);
	
	//std::cout << std::setfill(' ') << std::setw(21) << this->GetAddress() << " " << direction << " " << response << std::endl;
}

std::string EASports::Client::_readFile(const std::string &file_name) const
{
	std::ifstream input("../" + file_name, std::ifstream::in);
	
	std::string data((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	
	input.close();
	
	return data;
}

void EASports::Client::_SendFile(const std::string &file_name) const
{
	std::string response;
	
	response += "HTTP/1.1 200 OK\r\n";
	response += "Server: BF2-MC\r\n";
	response += "Accept-Ranges: bytes\r\n";
	response += "Content-Type: text/plain\r\n";
	
	std::string data = this->_readFile(file_name);
	
	response += "Content-Length: " + std::to_string(data.size()) + "\r\n";
	response += "\r\n";
	response += data;
	
	this->Send(response);
	
	this->_LogTransaction("<--", "HTTP/1.1 200 OK");
}

