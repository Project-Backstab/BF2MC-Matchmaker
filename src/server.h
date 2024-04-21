#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <memory>

#include <net/socket.h>

class Server : public Net::Socket
{
	public:
		/**
		 * @brief Enum defining the types of servers.
		 */
		enum Type
		{
			QR,        /**< Query and Report (QR) server. UDP:27900, Domain: bfield1942ps2.master.bfmcspy.net */
			GPSP,      /**< GameSpy Protocol (GPSP) server. TCP:29901, Domain: gpsp.gamespy.com */
			GPCM,      /**< GameSpy Connection Manager (GPCM) server. TCP:29900, Domain: gpcm.gamespy.com */
			Webserver, /**< Web server. TCP:80, Domain: www.easports.com and bfmc.gamespy.com */
			Browsing,  /**< Browsing server. TCP:28910, Domain: bfield1942ps2.available.gamespy.com and bfield1942ps2.ms7.gamespy.com */
			GameStats, /**< Game statistics server. TCP:29920, Domain: bfield1942ps2.gamestats.bfmcspy.net */
			Websocket, /**< Websocket server. TCP:55001 */
			None,      /**< No specific server type. */
		};
	
	private:
		std::vector<std::shared_ptr<Net::Socket>> _clients; /**< Vector of client sockets connected to this server. */
		Server::Type                              _type;    /**< Type of the server. */
		mutable std::mutex                        _mutex;   /**< Mutex for thread-safe operations on the server. */
	
	public:
		/**
		 * @brief Constructor for the Server class.
		 * 
		 * @param type The type of the server.
		 */
		Server(Server::Type type);
		
		/**
		 * @brief Get the vector of client sockets connected to this server.
		 * 
		 * @return Vector of client sockets.
		 */
		std::vector<std::shared_ptr<Net::Socket>> GetClients();
		
		/**
		 * @brief Start listening for incoming connections on the server.
		 */
		void Listen();

		/**
		 * @brief Start listening for incoming UDP packets on the server.
		 */
		void UDPListen();

		/**
		 * @brief Disconnect all connected clients from the server.
		 */
		void DisconnectAllClients();

		/**
		 * @brief Close the server and stop listening for incoming connections.
		 */
		void Close();
		
		// Events
		/**
		 * @brief Called when the server starts listening for incoming connections.
		 * 
		 * This function is called when the server starts listening for incoming connections.
		 */
		void onServerListen() const;

		/**
		 * @brief Called when the server is shutting down.
		 * 
		 * This function is called when the server is shutting down.
		 */
		void onServerShutdown() const;
		
		/**
		 * @brief Called when a client connects to the server.
		 * 
		 * @param client The client socket that connected.
		 */
		void onClientConnect(const Net::Socket& client) const;

		/**
		 * @brief Called when a client connects to the server.
		 * 
		 * @param client The shared pointer to the client socket that connected.
		 */
		void onClientConnect(const std::shared_ptr<Net::Socket>& client) const;

		/**
		 * @brief Called when a client disconnects from the server.
		 * 
		 * @param client The client socket that disconnected.
		 */
		void onClientDisconnect(const Net::Socket& client);
};

#endif // SERVER_H
