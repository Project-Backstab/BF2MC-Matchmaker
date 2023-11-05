#ifndef GLOBALS_H
#define GLOBALS_H

// Forward declare
class Server;
class Database;

namespace Service
{
	class File_System;
}

extern Database*                g_database;
extern Server*                  g_qr_server;
extern Server*                  g_gpsp_server;
extern Server*                  g_gpcm_server;
extern Server*                  g_webserver_server;
extern Server*                  g_browsing_server;
extern Server*                  g_gamestats_server;
extern Server*                  g_websocket_server;

extern Service::File_System*    g_file_system;

#endif // GLOBALS_H
