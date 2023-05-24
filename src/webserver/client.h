#ifndef WEBSERVER_CLIENT_H
#define WEBSERVER_CLIENT_H

#include <net/socket.h>
#include <urlrequest.h>

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
			
			/*
				Events
			*/
			void onRequest(const std::string &msg);
			
			/*
				Requests
			*/
			void requestNews(const UrlRequest::Parameter &parameter, const UrlRequest::UrlVariables &url_variables);
			void requestGetPlayerInfo(const UrlRequest::Parameter &parameter, const UrlRequest::UrlVariables &url_variables);
			void requestStats(const UrlRequest::Parameter &parameter, const UrlRequest::UrlVariables &url_variables);
			//Clan			
			void requestClanInfo(const UrlRequest::Parameter& parameter, const UrlRequest::UrlVariables &url_variables);
			void requestClanMembers(const UrlRequest::Parameter& parameter, const UrlRequest::UrlVariables &url_variables);
			void requestLeaderboard(const UrlRequest::Parameter &parameter, const UrlRequest::UrlVariables &url_variables);
			void requestCreateClan(const UrlRequest::Parameter &parameter, const UrlRequest::UrlVariables &url_variables);
			void requestUpdateClan(const UrlRequest::Parameter& parameter, const UrlRequest::UrlVariables &url_variables);
			void requestDisband(const UrlRequest::Parameter& parameter, const UrlRequest::UrlVariables &url_variables);
			
			//Empty
			void requestEmpty(const UrlRequest::Parameter &parameter, const UrlRequest::UrlVariables &url_variables);
		
		private:
			void _LogTransaction(const std::string &direction, const std::string &response) const;
			std::string _readFile(const std::string &file_name) const;
			void _SendFile(const std::string &file_name) const;
	};
}

#endif // WEBSERVER_CLIENT_H
