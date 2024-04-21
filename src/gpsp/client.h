#ifndef GPSP_CLIENT_H
#define GPSP_CLIENT_H

#include <gamespy.h>
#include <net/socket.h>

namespace GPSP
{
	/**
     * @brief Client class for GPSP protocol.
     */
	class Client : public Net::Socket
	{
		public:
			/**
			 * @brief Construct a new Client object.
			 * 
			 * @param socket The socket descriptor.
			 * @param address The address information.
			 */
			Client(int socket, struct sockaddr_in address);

			/**
			 * @brief Destroy the Client object.
			 */
			~Client();
			
			/**
			 * @brief Listen for incoming messages.
			 */
			void Listen();

			/**
			 * @brief Disconnect the client.
			 */
			void Disconnect();
			
			/*
				Events
			*/
			/**
			 * @brief Handle incoming requests.
			 * 
			 * @param msg The incoming message.
			 */
			void onRequest(const std::string& msg);
			
			/*
				Requests
			*/
			/**
			 * @brief Request nicknames.
			 * 
			 * @param parameter The request parameters.
			 */
			void requestNicks(const GameSpy::Parameter& parameter) const;

			/**
			 * @brief Request validation.
			 * 
			 * @param parameter The request parameters.
			 */
			void requestValid(const GameSpy::Parameter& parameter) const;

			/**
			 * @brief Request a new user.
			 * 
			 * @param parameter The request parameters.
			 */
			void requestNewUser(const GameSpy::Parameter& parameter) const;

			/**
			 * @brief Search for users.
			 * 
			 * @param parameter The request parameters.
			 */
			void requestSearch(const GameSpy::Parameter& parameter) const;
			
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
	};
}

#endif // GPSP_CLIENT_H
