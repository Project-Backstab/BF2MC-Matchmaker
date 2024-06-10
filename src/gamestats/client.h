#ifndef GAMESTATS_CLIENT_H
#define GAMESTATS_CLIENT_H

#include <net/socket.h>
#include <gamespy.h>

namespace GameStats
{
	/**
	 * @brief Represents a client for game statistics.
	 * 
	 * This class extends the Net::Socket class to handle client
	 * connections and requests for game statistics.
	 */
	class Client : public Net::Socket
	{	
		public:
			/**
			 * @brief Constructor for Game Statistics Client.
			 *
			 * @param socket The socket ID.
			 * @param address The client's address.
			 */
			Client(int socket, struct sockaddr_in address);

			/**
			 * @brief Destructor for Game Statistics Client.
			 */
			~Client();
			
			/**
			 * @brief Start listening for client requests.
			 * 
			 * This function starts listening for incoming requests from the client.
			 * It is typically called after the client has been initialized.
			 */
			void Listen();

			/**
			 * @brief Disconnect the client.
			 * 
			 * This function disconnects the client from the server.
			 * It closes the socket connection and cleans up resources.
			 */
			void Disconnect();
			
			/*
				Events
			*/
			/**
			 * @brief Handle incoming requests from the client.
			 * 
			 * This function is called to process incoming requests from the client.
			 *
			 * @param request The incoming request data.
			 */
			void onRequest(const std::string& request);
			
			/*
				Requests
			*/
			/**
			 * @brief Request a challenge from the client.
			 * 
			 * This function sends a response to the client to provide a challenge
			 * for authentication purposes.
			 */
			void requestChallenge();

			/**
			 * @brief Request authentication with provided parameters.
			 * 
			 * This function sends a response to the client for authentication
			 * with the provided parameters.
			 *
			 * @param parameter The authentication parameters.
			 */
			void requestAuth(const GameSpy::Parameter& parameter);

			/**
			 * @brief Request to create a new game with provided parameters.
			 * 
			 * This function sends a request to the client to create a new game
			 * with the provided parameters.
			 *
			 * @param parameter The parameters for creating a new game.
			 */
			void requestNewGame(const GameSpy::Parameter& parameter);

			/**
			 * @brief Request to update an existing game with provided parameters.
			 * 
			 * This function sends a response to the client to update an existing game
			 * with the provided parameters.
			 *
			 * @param parameter The parameters for updating an existing game.
			 */
			void requestUpdateGame(const GameSpy::Parameter& parameter);
		
		private:
			/**
			 * @brief Log a transaction with direction and response.
			 * 
			 * This function logs a transaction with the specified direction (e.g., "<--" or "-->")
			 * and the response received.
			 *
			 * @param direction The direction of the transaction ("<--" or "-->").
			 * @param response The response data to be logged.
			 */
			void _LogTransaction(const std::string& direction, const std::string& response) const;

			/**
			 * @brief Get the player index based on a key.
			 * 
			 * This function retrieves the player index based on the provided key.
			 * The key could be a player ID, name, or other unique identifier.
			 *
			 * @param key The key used to identify the player.
			 * @return The index of the player.
			 */
			uint8_t _GetPlayerIndex(std::string key);
			
		public:
			/**
			 * @brief Heartbeat function to manage client connections.
			 *
			 * This function periodically checks for client connections and closes
			 * connections that have not sent any data in the last minute.
			 */
			static void Heartbeat();
			
			/**
			 * @brief Encrypts a response message.
			 * 
			 * This static function encrypts a response message before sending it over the network.
			 * It takes a string message and returns the encrypted message as a vector of unsigned chars.
			 *
			 * @param response The response message to encrypt.
			 * @return The encrypted message as a vector of unsigned chars.
			 */
			static std::vector<unsigned char> Encrypt(const std::string& response);
			
			/**
			 * @brief Decrypts a request message.
			 * 
			 * This static function decrypts a received request message.
			 * It takes a vector of unsigned chars (encrypted message) and returns the decrypted message as a string.
			 *
			 * @param request The encrypted request message to decrypt.
			 * @return The decrypted message as a string.
			 */
			static std::string Decrypt(const std::vector<unsigned char>& request);

			static void Test();
	};
}

#endif // GAMESTATS_CLIENT_H
