#ifndef WEBSERVER_CLIENT_H
#define WEBSERVER_CLIENT_H

#include <net/socket.h>
#include <util.h>

// Forward declair
namespace atomizes
{
	class HTTPMessage;
};

namespace Json
{
	class Value;
};

namespace Battlefield
{
	class Clan;
	class Player;
};

namespace Webserver
{
	class Client : public Net::Socket
	{
		public:
			/**
			 * @brief Constructor for Webserver Client.
			 * 
			 * @param socket The socket identifier.
			 * @param address The client's socket address.
			 */
			Client(int socket, struct sockaddr_in address);

			/**
			 * @brief Destructor for Webserver Client.
			 */
			~Client();
			
			/**
			 * @brief Start listening for incoming requests.
			 */
			void Listen();

			/**
			 * @brief Disconnect the client.
			 */
			void Disconnect();

			/**
			 * @brief Send an HTTP response.
			 * 
			 * @param http_response The HTTP response message.
			 */
			void Send(const atomizes::HTTPMessage& http_response) const;

			/**
			 * @brief Send a JSON value.
			 * 
			 * @param value The JSON value to send.
			 */
			void Send(const Json::Value &value) const;
			
			/*
				Events
			*/
			/**
			 * @brief Process incoming HTTP request.
			 * 
			 * @param http_request The HTTP request message.
			 */
			void onRequest(const atomizes::HTTPMessage& http_request);
			
			/*
				Requests
			*/
			// www.easports.com
			/**
			 * @brief Handle a request for a specific file.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for file requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestFile(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			
			// bfmc.gamespy.com
			// Stats
			/**
			 * @brief Handle a request to get player information.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for player info requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestGetPlayerInfo(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

			/**
			 * @brief Handle a request for player statistics.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for stats requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestStats(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			
			// Clan
			/**
			 * @brief Handle a request to get clan information.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for clan info requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestClanInfo(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

			/**
			 * @brief Handle a request to get clan members.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for clan member requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestClanMembers(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

			/**
			 * @brief Handle a request to get the leaderboard.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for leaderboard requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestLeaderboard(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

			/**
			 * @brief Handle a request to create a new clan.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for clan creation requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestCreateClan(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

			/**
			 * @brief Handle a request to update a clan.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for clan update requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestUpdateClan(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

			/**
			 * @brief Handle a request to disband a clan.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for clan disband requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestDisband(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

			/**
			 * @brief Handle a request to change a member's rank in a clan.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for rank change requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestChangeRank(const atomizes::HTTPMessage &http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

			/**
			 * @brief Handle a request to add a member to a clan.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for adding members to a clan.
			 * @param url_variables  Additional URL variables.
			 */
			void requestAddMember(const atomizes::HTTPMessage &http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

			/**
			 * @brief Handle a request to delete a member from a clan.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for deleting members from a clan.
			 * @param url_variables  Additional URL variables.
			 */
			void requestDeleteMember(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

			/**
			 * @brief Handle a request for clan messages.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for clan message requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestClanMessage(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

			/**
			 * @brief Handle a request for a meme.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for meme requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestMeme(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			
			// Empty
			/**
			 * @brief Handle a request for a empty response.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for meme requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestEmpty(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			
			// API
			/**
			 * @brief Handle a request for live servers through the API.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for live API server requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestAPIServersLive(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

			/**
			 * @brief Handle a request for a specific game through the API.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for game API requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestAPIGame(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

			/**
			 * @brief Handle a request for multiple games through the API.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for games API requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestAPIGames(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

			/**
			 * @brief Handle a request for a specific player through the API.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for player API requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestAPIPlayer(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

			/**
			 * @brief Handle a request for a specific clan through the API.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for clan API requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestAPIClan(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

			/**
			 * @brief Handle a request for the leaderboard through the API.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for leaderboard API requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestAPILeaderboard(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

			/**
			 * @brief Handle a request for the clan leaderboard through the API.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for clan leaderboard API requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestAPILeaderboardClan(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

			/**
			 * @brief Handle a request for clan simulation through the API.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for clan simulation API requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestAPIClanSimulation(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			
			// API admin
			/**
			 * @brief Handle a request for admin clients through the API.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for admin clients API requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestAPIAdminClients(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

			/**
			 * @brief Handle a request to kick a client through the API.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for kick client API requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestAPIAdminKick(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

			/**
			 * @brief Handle a request to send a message to a client through the API.
			 * 
			 * @param http_request   The HTTP request message.
			 * @param url_base       The base URL for client message API requests.
			 * @param url_variables  Additional URL variables.
			 */
			void requestAPIAdminMessage(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

			/**
			 * @brief Handles the API request to recalculate player statistics.
			 *
			 * This function processes an HTTP request to recalculate player statistics. It uses the provided
			 * URL base and URL variables to construct the necessary request for the recalculation operation.
			 *
			 * @param http_request The HTTP request object containing details about the API request.
			 * @param url_base The base URL for the API endpoint.
			 * @param url_variables The URL variables to be included in the request.
			 */
			void requestAPIAdminPlayerStatsRecalc(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);

		private:
			/**
			 * @brief Generate the default HTTP response header.
			 * 
			 * @param isPlainText Flag indicating whether the content type is plain text.
			 * @return The generated HTTP response message.
			 */
			atomizes::HTTPMessage _defaultResponseHeader(bool isPlainText = true) const;

			/**
			 * @brief Log a transaction.
			 * 
			 * This function logs a transaction with the specified direction and response.
			 * 
			 * @param direction The direction of the transaction ("<--" or "-->").
			 * @param response The response to log.
			 */
			void _LogTransaction(const std::string& direction, const std::string& response) const;

			/**
			 * @brief Read a file and store its contents in a string.
			 * 
			 * @param file_name The name of the file to read.
			 * @param data Reference to store the file contents.
			 * @return True if the file was read successfully, false otherwise.
			 */
			bool _readFile(const std::string& file_name, std::string& data) const;

			/**
			 * @brief Send a file as an HTTP response.
			 * 
			 * @param file_name The name of the file to send.
			 */
			void _SendFile(const std::string& file_name) const;

			/**
			 * @brief Extract session, player, and clan information from URL variables.
			 * 
			 * This function extracts session, player, and clan information from the provided URL variables.
			 * 
			 * @param url_variables The URL variables containing the information.
			 * @param clan Reference to store the extracted clan information.
			 * @param player Reference to store the extracted player information.
			 */
			void _GetSessionPlayerAndClan(const Util::Url::Variables& url_variables,
					Battlefield::Clan& clan, Battlefield::Player& player) const;
			
			/**
			 * @brief Update clan information based on URL variables.
			 * 
			 * This function updates clan information based on the provided URL variables.
			 * 
			 * @param clan Reference to the clan object to update.
			 * @param url_variables The URL variables containing the update information.
			 * @param is_update Flag indicating whether this is an update operation.
			 *                   If true, only non-name fields are updated.
			 *                   If false, all fields, including name, can be updated.
			 * @return True if the clan information was updated successfully, false otherwise.
			 */
			bool _updateClanInformation(Battlefield::Clan& clan,
					const Util::Url::Variables &url_variables, bool is_update = false);
		
		public:
			/**
			 * @brief Heartbeat function to manage client connections.
			 *
			 * This function periodically checks for client connections and closes
			 * connections that have not sent any data in the last minute.
			 */
			static void Heartbeat();
	};
}

#endif // WEBSERVER_CLIENT_H
