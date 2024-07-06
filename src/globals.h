#ifndef GLOBALS_H
#define GLOBALS_H

// Forward declare
class Server;
class Database;

namespace Service
{
	class File_System;
}

/**
 * @brief Pointer to the global Database instance.
 */
extern Database*                    g_database;

/**
 * @brief Pointer to the global QR Server instance.
 */
extern Server*                      g_qr_server;

/**
 * @brief Pointer to the global GPSP Server instance.
 */
extern Server*                      g_gpsp_server;

/**
 * @brief Pointer to the global GPCM Server instance.
 */
extern Server*                      g_gpcm_server;

/**
 * @brief Pointer to the global Webserver Server instance.
 */
extern Server*                      g_webserver_server;

/**
 * @brief Pointer to the global Browsing Server instance.
 */
extern Server*                      g_browsing_server;

/**
 * @brief Pointer to the global Gamestats Server instance.
 */
extern Server*                      g_gamestats_server;

/**
 * @brief Pointer to the global Websocket Server instance.
 */
extern Server*                      g_websocket_server;

/**
 * @brief Pointer to the global DNS Server instance.
 */
extern Server*                      g_dns_server;

/**
 * @brief Pointer to the global File_System instance.
 */
extern class Service::File_System*  g_file_system;

#endif // GLOBALS_H
