#ifndef QR_CLIENT_H
#define QR_CLIENT_H

#include <gamespy.h>
#include <net/socket.h>

namespace QR
{
	/**
	 * @brief Represents a client for QR protocol.
	 */
	class Client : public Net::Socket
	{
		public:
			/**
			 * @brief Constructor for Client.
			 * @param socket The socket of the client.
			 * @param address The address information of the client.
			 */
			Client(int socket, struct sockaddr_in address);

			/**
			 * @brief Destructor for Client.
			 */
			~Client();
			
			/*
				Events
			*/
			/**
			 * @brief Event handler for incoming requests.
			 * @param request The incoming request data.
			 */
			void onRequest(const std::vector<unsigned char>& request);
			
			/*
				Requests
			*/
			/**
			 * @brief Sends a request for challenge to the client.
			 * @param request The challenge request data.
			 */
			void requestChallenge(const std::vector<unsigned char>& request) const;
			
			/**
			 * @brief Sends a request for available servers to the client.
			 * @param request The available servers request data.
			 */
			void requestAvailable(const std::vector<unsigned char>& request) const;
			
			/**
			 * @brief Sends a heartbeat request to the client.
			 * @param request The heartbeat request data.
			 */
			void requestHeartbeat(const std::vector<unsigned char>& request) const;
			
			/**
			 * @brief Sends a keep-alive request to the client.
			 * @param request The keep-alive request data.
			 */
			void requestKeepAlive(const std::vector<unsigned char>& request) const;
			
		private:
			/**
			 * @brief Logs a transaction with direction and response.
			 * @param direction The direction of the transaction ("<--" or "-->").
			 * @param response The response message to log.
			 */
			void _LogTransaction(const std::string& direction, const std::string& response) const;
	};
}

#endif // QR_CLIENT_H
