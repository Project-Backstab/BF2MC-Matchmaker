#include <iostream>
#include <signal.h>
#include <thread>
#include <fstream>
#include <mysql/mysql.h>
#include <filesystem>

#include <version.h>
#include <logger.h>
#include <settings.h>
#include <globals.h>
#include <server.h>
#include <database.h>
#include <gpcm/client.h>
#include <gamestats/client.h>
#include <browsing/client.h>
#include <websocket/client.h>
#include <webserver/client.h>
#include <service/file_system.h>

// Globals

Database*               g_database;

Server*                 g_qr_server;
Server*                 g_gpsp_server;
Server*                 g_gpcm_server;
Server*                 g_webserver_server;
Server*                 g_browsing_server;
Server*                 g_gamestats_server;
Server*                 g_websocket_server;
Server*                 g_dns_server;

class Service::File_System*   g_file_system;

// Settings

Json::Value             g_settings;
std::shared_mutex       g_settings_mutex;

void load_settings()
{
	std::unique_lock<std::shared_mutex> guard(g_settings_mutex); // settings lock (read/write)
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

void start_qr_server()
{
	g_qr_server = new Server(Server::Type::QR);	
	g_qr_server->UDPListen();
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

void start_gamestats_server()
{	
	//GameStats::Client::Test();
	
	g_gamestats_server = new Server(Server::Type::GameStats);	
	g_gamestats_server->Listen();
}

void start_websocket_server()
{
	g_websocket_server = new Server(Server::Type::Websocket);	
	g_websocket_server->Listen();
}

void start_dns_server()
{
	g_dns_server = new Server(Server::Type::DNS);
	g_dns_server->UDPListen();
}

void start_file_system()
{
	g_file_system = new class Service::File_System();

	// Load "data/gamescripts" files in memory
	for (const auto& entry : std::filesystem::recursive_directory_iterator("../data/gamescripts"))
	{
		if (entry.is_regular_file())
		{
			// load file in memory
			g_file_system->Load(entry.path());
		}
	}

	Logger::info("Load all files in memory.", Service::File_System);

	// Example code how to use
	/*
	std::string data;

	if(!g_file_system->GetFile("../data/gamescripts/bfmc/ps2/en/PS2news_en_US.txt", data))
	{
		Logger::error("Oepsie doepsie");
	}

	Logger::debug("data = " + data);

	g_file_system->UnLoadAll();
	*/
}

void signal_callback(int signum)
{
	Logger::info("Caught signal " + std::to_string(signum));
	
	//Disconnect all clients
	g_gpsp_server->DisconnectAllClients();
	g_gpcm_server->DisconnectAllClients();
	g_webserver_server->DisconnectAllClients();
	g_browsing_server->DisconnectAllClients();
	g_gamestats_server->DisconnectAllClients();
	g_websocket_server->DisconnectAllClients();
	
	// Close services
	g_qr_server->Close();
	g_gpsp_server->Close();
	g_gpcm_server->Close();
	g_webserver_server->Close();
	g_browsing_server->Close();
	g_gamestats_server->Close();
	g_websocket_server->Close();
	g_dns_server->Close();
	
	g_file_system->UnLoadAll();
	
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
	signal(SIGTERM, signal_callback);
	signal(SIGQUIT, signal_callback);
	signal(SIGTSTP, signal_callback);
	signal(SIGKILL, signal_callback);
	
	// Start servers
	std::thread t_db(&start_db);

	std::thread t_qr(&start_qr_server);
	std::thread t_gpsp(&start_gpsp_server);
	std::thread t_gpcm(&start_gpcm_server);
	std::thread t_gpcm_heartbeat(&GPCM::Client::Heartbeat);
	std::thread t_webserver(&start_webserver_server);
	std::thread t_webserver_heartbeat(&Webserver::Client::Heartbeat);
	std::thread t_browsing(&start_browsing_server);
	std::thread t_browsing_heartbeat(&Browsing::Client::Heartbeat);
	std::thread t_gamestats(&start_gamestats_server);
	std::thread t_gamestats_heartbeat(&GameStats::Client::Heartbeat);
	//std::thread t_websocket(&start_websocket_server);
	//std::thread t_dns(&start_dns_server);

	std::thread t_file_system(&start_file_system);

	t_db.detach();

	t_qr.detach();
	t_gpsp.detach();
	t_gpcm.detach();
	t_gpcm_heartbeat.detach();
	t_browsing.detach();
	t_browsing_heartbeat.detach();
	t_gamestats.detach();
	t_gamestats_heartbeat.detach();
	t_webserver.detach();
	t_webserver_heartbeat.detach();
	//t_websocket.detach();
	//t_dns.detach();

	t_file_system.detach();

	// Sleep ZZZZZZzzzzzZZZZZ
	while(true)
	{
		sleep(1);
	}
	
	return EXIT_SUCCESS;
}

