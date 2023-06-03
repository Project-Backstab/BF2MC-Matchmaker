#ifndef GLOBALS_H
#define GLOBALS_H

#include <mutex>

// Forward declare
class Server;
class Database;

extern Database*         g_database;
extern Server*           g_gpsp_server;
extern Server*           g_gpcm_server;
extern Server*           g_webserver_server;
extern Server*           g_browsing_server;

extern std::mutex        g_mutex_io;

#endif // GLOBALS_H
