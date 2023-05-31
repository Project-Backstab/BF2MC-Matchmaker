#include <iostream>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <thread>
#include <fstream>
#include <mysql/mysql.h>
#include <json/json.h>

#include <server.h>
#include <database.h>
#include <globals.h>

// Globals
Json::Value       g_settings;
Database*         g_database;
Server*           g_gpsp_server;
Server*           g_gpcm_server;
Server*           g_webserver_server;
Server*           g_browsing_server;

std::mutex        g_mutex_io;
std::shared_mutex g_mutex_settings;

void load_settings()
{
	//std::shared_lock<std::shared_mutex> guard(g_mutex_settings); // Read only
	std::unique_lock<std::shared_mutex> guard(g_mutex_settings); // write
	
	Json::CharReaderBuilder builder;
	
	std::ifstream ifs;
	ifs.open("../data/settings.json");
	
	if(ifs.is_open())
	{
		if(!parseFromStream(builder, ifs, &g_settings, nullptr))
		{
			perror("[Error] Cant parse settings.json.");
			perror("[Error] load_settings() at parseFromStream");
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
	g_gpsp_server = new Server(Server::Type::GPSP, 29901);	
	g_gpsp_server->Listen();
}

void start_gpcm_server()
{
	g_gpcm_server = new Server(Server::Type::GPCM, 29900);	
	g_gpcm_server->Listen();
}

void start_webserver_server()
{
	g_webserver_server = new Server(Server::Type::Webserver, 80);	
	g_webserver_server->Listen();
}

void start_browsing_server()
{
	g_browsing_server = new Server(Server::Type::Browsing, 28910);	
	g_browsing_server->Listen();
}

void signal_callback(int signum)
{
	std::unique_lock<std::mutex> guard(g_mutex_io); // io lock (read/write)
	
	std::cout << "Caught signal " << signum << std::endl;
	
	guard.unlock();
	
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
	// Register signal callbacks
	signal(SIGINT, signal_callback);
	signal(SIGINT, signal_callback);
	signal(SIGTERM, signal_callback);
	signal(SIGQUIT, signal_callback);
	signal(SIGTSTP, signal_callback);
	
	std::thread t_settings(&load_settings);
	t_settings.detach();
	
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

