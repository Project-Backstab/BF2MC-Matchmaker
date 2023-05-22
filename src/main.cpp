#include <iostream>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <thread>
#include <mysql/mysql.h>

#include <server.h>
#include <database.h>
#include <globals.h>

// Globals
Database*  g_database;
Server*    g_gpsp_server;
Server*    g_gpcm_server;
Server*    g_browsing_server;
Server*    g_easports_server;

std::mutex g_mutex_io;

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

void start_browsing_server()
{
	g_browsing_server = new Server(Server::Type::Browsing, 28910);	
	g_browsing_server->Listen();
}

void start_easports_server()
{
	g_easports_server = new Server(Server::Type::EASports, 80);	
	g_easports_server->Listen();
}

/*
#include <mysql/mysql.h>

void example_mysql()
{
	// Initialize the MySQL library
    mysql_library_init(0, nullptr, nullptr);
	
	MYSQL* connection = mysql_init(nullptr);
    
	if (!mysql_real_connect(connection, "localhost", "root", "", "BF2MC", 0, nullptr, 0))
	{
        std::cerr << "Failed to connect to the database: " << mysql_error(connection) << std::endl;
    
		return;
    }
	
	// Execute a query
    if (mysql_query(connection, "SELECT * FROM Users"))
	{
        std::cerr << "Failed to execute the query: " << mysql_error(connection) << std::endl;
        
		return;
    }
	
    // Fetch and print the results
    MYSQL_RES* result = mysql_use_result(connection);
    if (result)
	{
        MYSQL_ROW row;
		std::string line;
		
		while ((row = mysql_fetch_row(result)) != nullptr) {
            for (unsigned int i = 0; i < mysql_num_fields(result); ++i) {
				
				if (row[i])
				{
					line += row[i];
					line += " ";
				}
            }
			
			std::cout << line << std::endl;
        }
		
        mysql_free_result(result);
    }
	
    mysql_close(connection);
    mysql_library_end();
}
*/

void signal_callback(int signum)
{
	std::unique_lock<std::mutex> guard(g_mutex_io);
	
	std::cout << "Caught signal " << signum << std::endl;
	
	guard.unlock();
	
	// Close services
	g_gpsp_server->Close();
	g_easports_server->Close();
	
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
	
	// Start servers
	std::thread t_db(&start_db);
	std::thread t_gpsp(&start_gpsp_server);
	std::thread t_gpcm(&start_gpcm_server);
	std::thread t_browsing(&start_browsing_server);
	std::thread t_easports(&start_easports_server);
	
	t_db.detach();
	t_gpsp.detach();
	t_gpcm.detach();
	t_browsing.detach();
	t_easports.detach();
	
	// Sleep ZZZZZZzzzzzZZZZZ
	while(true)
	{
		sleep(1);
	}
	
	return EXIT_SUCCESS;
}

