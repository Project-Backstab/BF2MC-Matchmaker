#ifndef GPCM_CLIENT_H
#define GPCM_CLIENT_H

#include <gamespy.h>
#include <net/socket.h>


namespace GPCM
{
	class Client;
	
	/**
	 * @brief Represents a session with a GPCM client.
	 * 
	 * This struct holds information about a GPCM client's session, including challenge, profile ID,
	 * authentication token, associated client, and status.
	 */
	struct Session
	{
		std::string             challenge;
		int                     profileid = -1;
		std::string             authtoken;
		std::shared_ptr<Client> client;
		std::string             status;
	};

	/**
	 * @brief Represents a GPCM client.
	 * 
	 * This class extends the Net::Socket class to handle communication with GPCM clients.
	 * It holds information about the client's session.
	 */
	class Client : public Net::Socket
	{
		private:
			GPCM::Session       _session;        /**< The session information for the GPCM client. */
			std::vector<int>    _sync_friends;   /**< The list of synchronized friend IDs. */
		
		public:
			/**
			 * @brief Constructs a new GPCM client.
			 * 
			 * @param socket The socket descriptor.
			 * @param address The client's address.
			 */
			Client(int socket, struct sockaddr_in address);
			
			/**
			 * @brief Destroys the GPCM client.
			 * 
			 * The destructor closes the client's connection and cleans up resources.
			 */
			~Client();
			
			/**
			 * @brief Listens for incoming messages from the GPCM client.
			 * 
			 * This function continuously listens for incoming messages from the client
			 * and processes them accordingly.
			 */
			void Listen();

			/**
			 * @brief Disconnects the client.
			 * 
			 * Closes the connection with the client and cleans up resources associated
			 * with the client.
			 */
			void Disconnect();

			/**
			 * @brief Get the session information of the client.
			 * 
			 * @return GPCM::Session The session information.
			 */
			GPCM::Session GetSession() const;
			
			/*
				Events
			*/
			/**
			 * @brief Event handler for incoming messages from the client.
			 * 
			 * This function is called when the client sends a message to the server.
			 * 
			 * @param msg The incoming message.
			 */
			void onRequest(const std::string& msg);
			
			/*
				Requests
			*/
			/**
			 * @brief Sends a challenge to the client.
			 * 
			 * This function sends a challenge to the client.
			 */
			void requestChallenge();

			/**
			 * @brief Process a login request from the client.
			 * 
			 * This function process a login request from the client with the provided parameters.
			 * 
			 * @param parameter The login parameters.
			 */
			void requestLogin(const GameSpy::Parameter& parameter);

			/**
			 * @brief Process an invite request from the client.
			 * 
			 * This function process an invite request from the client with the provided parameters.
			 * 
			 * @param parameter The invite parameters.
			 */
			void requestInviteTo(const GameSpy::Parameter& parameter);

			/**
			 * @brief Process a get-profile request from the client.
			 * 
			 * This function process a get-profile request from the client with the provided parameters.
			 * 
			 * @param parameter The get-profile parameters.
			 */
			void requestGetProfile(const GameSpy::Parameter& parameter);

			/**
			 * @brief Process a status request from the client.
			 * 
			 * This function process a status request from the client with the provided parameters.
			 * 
			 * @param parameter The status parameters.
			 */
			void requestStatus(const GameSpy::Parameter& parameter);

			/**
			 * @brief Process a BM request from the client.
			 * 
			 * This function process a BM request from the client with the provided parameters.
			 * 
			 * @param parameter The BM parameters.
			 */
			void requestBm(const GameSpy::Parameter& parameter);

			/**
			 * @brief Process an add-buddy request from the client.
			 * 
			 * This function process an add-buddy request from the client with the provided parameters.
			 * 
			 * @param parameter The add-buddy parameters.
			 */
			void requestAddBuddy(const GameSpy::Parameter& parameter);

			/**
			 * @brief Process a revoke request from the client.
			 * 
			 * This function process a revoke request from the client with the provided parameters.
			 * 
			 * @param parameter The revoke parameters.
			 */
			void requestRevoke(const GameSpy::Parameter& parameter);

			/**
			 * @brief Process a delete-buddy request from the client.
			 * 
			 * This function process a delete-buddy request from the client with the provided parameters.
			 * 
			 * @param parameter The delete-buddy parameters.
			 */
			void requestDeleteBuddy(const GameSpy::Parameter& parameter);

			/**
			 * @brief Process an auth-add request from the client.
			 * 
			 * This function process an auth-add request from the client with the provided parameters.
			 * 
			 * @param parameter The auth-add parameters.
			 */
			void requestAuthAdd(const GameSpy::Parameter& parameter);

			/**
			 * @brief Process a player-invite request from the client.
			 * 
			 * This function process a player-invite request from the client with the provided parameters.
			 * 
			 * @param parameter The player-invite parameters.
			 */
			void requestPlayerInvite(const GameSpy::Parameter& parameter);

			/**
			 * @brief Process a logout request from the client.
			 * 
			 * This function process a logout request from the client with the provided parameters.
			 * 
			 * @param parameter The logout parameters.
			 */
			void requestLogout(const GameSpy::Parameter& parameter);
			
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
			 * @brief Sends a new status to the client.
			 * 
			 * This function sends a new status to the client based on internal status information.
			 */
			void _SendNewStatus() const;

			/**
			 * @brief Synchronizes friends with the client.
			 * 
			 * This function synchronizes the list of friends with the client.
			 */
			void _SyncFriends();
		
		
		public:
			/*
				Static
			*/
			/**
			 * @brief Finds a session by profile ID.
			 * 
			 * @param profileid The profile ID to search for.
			 * @return GPCM::Session The session corresponding to the profile ID, if found.
			 */
			static GPCM::Session findSessionByProfileId(int profileid);

			/**
			 * @brief Finds a session by authentication token.
			 * 
			 * @param authtoken The authentication token to search for.
			 * @return GPCM::Session The session corresponding to the authentication token, if found.
			 */
			static GPCM::Session findSessionByAuthtoken(const std::string& authtoken);

			/**
			 * @brief Sends a buddy message from one profile ID to another.
			 * 
			 * @param profileid The profile ID of the sender.
			 * @param target_profileid The profile ID of the recipient.
			 * @param bm The buddy message code.
			 * @param message The message to send.
			 */
			static void SendBuddyMessage(int profileid, int target_profileid, const std::string& bm, const std::string& message);

			/**
			 * @brief Disconnects a session by profile ID.
			 * 
			 * @param profileid The profile ID of the session to disconnect.
			 */
			static void Disconnect(int profileid);

			/**
			 * @brief Heartbeat function to manage client connections.
			 *
			 * This function periodically checks for client connections and closes
			 * connections that have not sent any data in the last minute.
			 */
			static void Heartbeat();
	};
}

#endif // GPCM_CLIENT_H
