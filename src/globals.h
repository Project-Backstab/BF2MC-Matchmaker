#ifndef GLOBALS_H
#define GLOBALS_H

#include <mutex>
#include <shared_mutex>

#include <json/json.h>

// Forward declare
class Server;
class Database;

extern Json::Value       g_settings;
extern Database*         g_database;
extern Server*           g_gpsp_server;
extern Server*           g_gpcm_server;
extern Server*           g_webserver_server;
extern Server*           g_browsing_server;

extern std::mutex        g_mutex_io;
extern std::shared_mutex g_mutex_settings;;

#endif // GLOBALS_H
