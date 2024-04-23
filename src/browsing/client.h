#ifndef BROWSING_CLIENT_H
#define BROWSING_CLIENT_H

#include <gamespy.h>
#include <net/socket.h>
#include <battlefield/gameserver.h>


namespace Browsing
{
	/**
     * @brief Client class for handling browsing requests.
     */
	class Client : public Net::Socket
	{
		private:
			/**
			 * @brief Challenge data sent to the client.
			 * 
			 * When a client connects to the browsing server, it receives
			 * a challenge to validate its connection.
			 */
			std::vector<unsigned char> _client_challenge;
		
		public:
			/**
			 * @brief Constructs a new Client object.
			 * 
			 * @param socket The socket associated with the client.
			 * @param address The address information of the client.
			 */
			Client(int socket, struct sockaddr_in address);

			/**
			 * @brief Destroys the Client object.
			 */
			~Client();
			
			/**
			 * @brief Listens for incoming requests from the client.
			 */
			void Listen();

			/**
			 * @brief Disconnects the client.
			 */
			void Disconnect();
			
			/*
				Events
			*/
			/**
			 * @brief Event handler for incoming requests.
			 * 
			 * @param request The incoming request.
			 */
			void onRequest(const std::vector<unsigned char>& request);
			
			/*
				Requests
			*/
			/**
			 * @brief Sends a request for the server list.
			 * 
			 * @param request The request data.
			 */
			void requestServerList(const std::vector<unsigned char>& request);

			/**
			 * @brief Sends a request for server information.
			 * 
			 * @param request The request data.
			 */
			void requestServerInfo(const std::vector<unsigned char>& request);
			
		private:
			/**
			 * @brief Converts a buffer to a list of requests.
			 *
			 * @param buffer The buffer containing the requests.
			 * @param[out] requests The vector to store the parsed requests.
			 */
			void _BufferToRequests(const std::vector<unsigned char>& buffer,
					std::vector<std::vector<unsigned char>>& requests);
			
			/**
			 * @brief Inserts client information into the response.
			 *
			 * @param response The response to modify.
			 */
			void _insertClientInfo(std::vector<unsigned char>& response);
			
			/**
			 * @brief Inserts game server flag, IP, and port into the response.
			 *
			 * @param response The response to modify.
			 * @param game_server The game server to get the information from.
			 */
			void _insertGameServerFlagIpPort(std::vector<unsigned char>& response,
					const Battlefield::GameServer& game_server);
			
			/**
			 * @brief Encrypts the response.
			 *
			 * @param response The response to encrypt.
			 */
			void _Encrypt(std::vector<unsigned char>& response);
			
			/**
			 * @brief Logs a transaction.
			 *
			 * @param direction The direction of the transaction ("<--" or "-->").
			 * @param response The response message to log.
			 */
			void _LogTransaction(const std::string& direction, const std::string& response) const;
			
			/**
			 * @brief Filters game servers based on the provided filter criteria.
			 *
			 * @param filter The filter string.
			 * @param game_servers The list of game servers to filter.
			 */
			void _FilterServers(const std::string& filter, Battlefield::GameServers& game_servers);
			
			/**
			 * @brief Checks if a game server matches the filter criteria based on game version.
			 *
			 * @param filter The filter string.
			 * @param game_server The game server to check.
			 * @return True if the game server should be filtered out, false otherwise.
			 */
			bool _FilterServerGameVersion(const std::string& filter, const Battlefield::GameServer& game_server);
			
			/**
			 * @brief Checks if a game server matches the filter criteria based on region.
			 *
			 * @param filter The filter string.
			 * @param game_server The game server to check.
			 * @return True if the game server should be filtered out, false otherwise.
			 */
			bool _FilterServerRegion(const std::string& filter, const Battlefield::GameServer& game_server);
			
			/**
			 * @brief Checks if a game server matches the filter criteria based on number of players.
			 *
			 * @param filter The filter string.
			 * @param game_server The game server to check.
			 * @return True if the game server should be filtered out, false otherwise.
			 */
			bool _FilterServerNumPlayers(const std::string& filter, const Battlefield::GameServer& game_server);
			
			/**
			 * @brief Checks if a game server matches the filter criteria based on game type.
			 *
			 * @param filter The filter string.
			 * @param game_server The game server to check.
			 * @return True if the game server should be filtered out, false otherwise.
			 */
			bool _FilterServerGameType(const std::string& filter, const Battlefield::GameServer& game_server);
			
			/**
			 * @brief Checks if a game server matches the filter criteria based on map name.
			 *
			 * @param filter The filter string.
			 * @param game_server The game server to check.
			 * @return True if the game server should be filtered out, false otherwise.
			 */
			bool _FilterServerMapName(const std::string& filter, const Battlefield::GameServer& game_server);
			
			/**
			 * @brief Checks if a game server matches the filter criteria based on stats tracking.
			 *
			 * @param filter The filter string.
			 * @param game_server The game server to check.
			 * @return True if the game server should be filtered out, false otherwise.
			 */
			bool _FilterServerStatsTracking(const std::string& filter, const Battlefield::GameServer& game_server);
			
			/**
			 * @brief Checks if a game server matches the filter criteria based on reconfigurability.
			 *
			 * @param filter The filter string.
			 * @param game_server The game server to check.
			 * @return True if the game server should be filtered out, false otherwise.
			 */
			bool _FilterServerReconfigurable(const std::string& filter, const Battlefield::GameServer& game_server);
			
			/**
			 * @brief Checks if a game server matches the filter criteria based on clan.
			 *
			 * @param filter The filter string.
			 * @param game_server The game server to check.
			 * @return True if the game server should be filtered out, false otherwise.
			 */
			bool _FilterServerClan(const std::string& filter, const Battlefield::GameServer& game_server);
			
			/**
			 * @brief Checks if a game server matches the filter criteria based on teamplay.
			 *
			 * @param filter The filter string.
			 * @param game_server The game server to check.
			 * @return True if the game server should be filtered out, false otherwise.
			 */
			bool _FilterServerTeamplay(const std::string& filter, const Battlefield::GameServer& game_server);
			
		public:
			/**
			 * @brief Testing function to encrypt/decrypt a package.
			 */
			static void Test();
			
			/**
			 * @brief Cracking function to find the secret key from a package.
			 */
			static void Crack();

			/**
			 * @brief Heartbeat function to manage client connections.
			 *
			 * This function periodically checks for client connections and closes
			 * connections that have not sent any data in the last minute.
			 */
			static void Heartbeat();
	};
}

#endif // BROWSING_CLIENT_H
