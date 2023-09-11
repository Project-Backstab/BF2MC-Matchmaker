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
			Client(int socket, struct sockaddr_in address);
			~Client();
			
			void Listen();
			void Disconnect();
			void Send(const atomizes::HTTPMessage& http_response) const;
			void Send(const Json::Value &value) const;
			
			/*
				Events
			*/
			void onRequest(const atomizes::HTTPMessage& http_request);
			
			/*
				Requests
			*/
			// www.easports.com
			void requestFile(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			
			// bfmc.gamespy.com
			// Stats
			void requestGetPlayerInfo(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			void requestStats(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			
			// Clan	
			void requestClanInfo(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			void requestClanMembers(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			void requestLeaderboard(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			void requestCreateClan(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			void requestUpdateClan(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			void requestDisband(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			void requestChangeRank(const atomizes::HTTPMessage &http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			void requestAddMember(const atomizes::HTTPMessage &http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			void requestDeleteMember(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			void requestClanMessage(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			void requestMeme(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			
			// Empty
			void requestEmpty(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			
			// API
			void requestAPIServersLive(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			void requestAPIGame(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			void requestAPIGames(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			void requestAPIPlayer(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			void requestAPIClan(const atomizes::HTTPMessage& http_request, const std::string& url_base, const Util::Url::Variables& url_variables);
			
		private:
			atomizes::HTTPMessage _defaultResponseHeader() const;
			void _LogTransaction(const std::string& direction, const std::string& response) const;
			std::string _readFile(const std::string& file_name) const;
			void _SendFile(const std::string& file_name) const;
			void _GetSessionPlayerAndClan(const Util::Url::Variables& url_variables,
					Battlefield::Clan& clan, Battlefield::Player& player) const;
			void _updateClanInformation(Battlefield::Clan& clan,
					const Util::Url::Variables &url_variables, bool is_update = false);
	};
}

#endif // WEBSERVER_CLIENT_H
