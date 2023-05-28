#ifndef WEBSERVER_CLIENT_H
#define WEBSERVER_CLIENT_H

#include <net/socket.h>
#include <urlrequest.h>
#include <atomizes.hpp>
#include <battlefield/clan.h>
#include <battlefield/player.h>

namespace Webserver
{
	class Client : public Net::Socket
	{
		public:
			Client(int socket, struct sockaddr_in address);
			~Client();
			
			void Listen();
			void Disconnect();
			void Send(const std::string &msg) const;
			void Send(const atomizes::HTTPMessage &http_response) const;
			
			/*
				Events
			*/
			void onRequest(const atomizes::HTTPMessage &http_request);
			
			/*
				Requests
			*/
			void requestNews(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables);
			
			// Stats
			void requestGetPlayerInfo(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables);
			void requestStats(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables);
			
			// Clan			
			void requestClanInfo(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables);
			void requestClanMembers(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables);
			void requestLeaderboard(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables);
			void requestCreateClan(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables);
			void requestUpdateClan(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables);
			void requestDisband(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables);
			void requestChangeRank(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables);
			void requestAddMember(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables);
			void requestDeleteMember(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables);
			void requestClanMessage(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables);
			
			// Empty
			void requestEmpty(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables &url_variables);
		
		private:
			atomizes::HTTPMessage _defaultResponseHeader() const;
			void _LogTransaction(const std::string &direction, const std::string &response) const;
			std::string _readFile(const std::string &file_name) const;
			void _SendFile(const std::string &file_name) const;
			
			int _GetSessionProfileId(const UrlRequest::UrlVariables &url_variables) const;
			void _GetSessionPlayerAndClan(const UrlRequest::UrlVariables &url_variables, Battlefield::Clan& clan, Battlefield::Player& player) const;
	};
}

#endif // WEBSERVER_CLIENT_H
