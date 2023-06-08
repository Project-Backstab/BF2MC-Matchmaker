#include <iostream>
#include <signal.h>
#include <thread>
#include <fstream>
#include <mysql/mysql.h>

#include <version.h>
#include <logger.h>
#include <settings.h>
#include <globals.h>
#include <server.h>
#include <database.h>

// Globals
Database*         g_database;
Server*           g_gpsp_server;
Server*           g_gpcm_server;
Server*           g_webserver_server;
Server*           g_browsing_server;

Json::Value       g_settings;
std::shared_mutex g_mutex_settings;

void load_settings()
{
	Json::CharReaderBuilder builder;
	
	std::ifstream ifs;
	ifs.open("../data/settings.json");
	
	if(ifs.is_open())
	{
		if(!parseFromStream(builder, ifs, &g_settings, nullptr))
		{
			Logger::error("Cant parse settings.json.");
			Logger::error("load_settings() at parseFromStream");
			exit(EXIT_FAILURE);
		}
		
		ifs.close();
	}
}

void start_db()
{	
	// Initialize the MySQL library
    mysql_library_init(0, nullptr, nullptr);
	
	g_database = new Database();
}

void start_gpsp_server()
{
	g_gpsp_server = new Server(Server::Type::GPSP);	
	g_gpsp_server->Listen();
}

void start_gpcm_server()
{
	g_gpcm_server = new Server(Server::Type::GPCM);	
	g_gpcm_server->Listen();
}

void start_webserver_server()
{
	g_webserver_server = new Server(Server::Type::Webserver);	
	g_webserver_server->Listen();
}

void start_browsing_server()
{
	//Browsing::Client::Test();
	//Browsing::Client::Crack();
	
	g_browsing_server = new Server(Server::Type::Browsing);	
	g_browsing_server->Listen();
}

void signal_callback(int signum)
{
	Logger::info("Caught signal " + std::to_string(signum));
	
	//Disconnect all clients
	g_gpsp_server->DisconnectAllClients();
	g_gpcm_server->DisconnectAllClients();
	g_webserver_server->DisconnectAllClients();
	g_browsing_server->DisconnectAllClients();
	
	// Close services
	g_gpsp_server->Close();
	g_gpcm_server->Close();
	g_webserver_server->Close();
	g_browsing_server->Close();
	
	// Exit application
	exit(signum);
}

int main(int argc, char const* argv[])
{
	Logger::Initialize();
	load_settings();
	
	Logger::info("--- PROJECT INFO ---");
	Logger::info("Project name     = " + std::string(PROJECT_GIT_NAME));
	Logger::info("Project toplevel = " + std::string(PROJECT_GIT_TOPLEVEL));
	Logger::info("Branch name      = " + std::string(PROJECT_GIT_BRANCH_NAME));
	Logger::info("Branch hash      = " + std::string(PROJECT_GIT_BRANCH_HASH));
	Logger::info("Version          = " + std::string(PROJECT_VERSION_STRING));
	
	// Register signal callbacks
	signal(SIGINT, signal_callback);
	signal(SIGINT, signal_callback);
	signal(SIGTERM, signal_callback);
	signal(SIGQUIT, signal_callback);
	signal(SIGTSTP, signal_callback);
	
	// Start servers
	std::thread t_db(&start_db);
	std::thread t_gpsp(&start_gpsp_server);
	std::thread t_gpcm(&start_gpcm_server);
	std::thread t_webserver(&start_webserver_server);
	std::thread t_browsing(&start_browsing_server);
	
	t_db.detach();
	t_gpsp.detach();
	t_gpcm.detach();
	t_webserver.detach();
	t_browsing.detach();
	
	// Sleep ZZZZZZzzzzzZZZZZ
	while(true)
	{
		sleep(1);
	}
	
	return EXIT_SUCCESS;
}

