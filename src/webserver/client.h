#ifndef WEBSERVER_CLIENT_H
#define WEBSERVER_CLIENT_H

#include <net/socket.h>
#include <urlrequest.h>

// Forward declair
namespace atomizes
{
	class HTTPMessage;
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
			
			/*
				Events
			*/
			void onRequest(const atomizes::HTTPMessage& http_request);
			void onUnimplementedAction(const std::string& action);
			
			/*
				Requests
			*/
			// www.easports.com
			// News
			void requestNews(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			
			// Licenses
			void requestLicenseGameSpy(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestLicenseEA(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestLicensePunkBuster(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			
			// Settings
			void requestSettingsGameModes(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestSettingsQuickMatch(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestSettingsPlayList(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestSettingsClanMatch(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			
			// Locale
			void requestLocaleLanguage(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestLocaleEnglish(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestLocaleFrench(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestLocaleGerman(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestLocaleItalian(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestLocaleJapanese(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestLocaleSpanish(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestLocaleSwedish(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			
			// Advertisements
			void requestAdvertSux(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestAdvertTxt(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			
			// bfmc.gamespy.com
			// Stats
			void requestGetPlayerInfo(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestStats(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			
			// Clan	
			void requestClanInfo(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestClanMembers(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestLeaderboard(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestCreateClan(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestUpdateClan(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestDisband(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestChangeRank(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables& url_variables);
			void requestAddMember(const atomizes::HTTPMessage &http_request, const UrlRequest::UrlVariables& url_variables);
			void requestDeleteMember(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestClanMessage(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			void requestMeme(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
			
			// Empty
			void requestEmpty(const atomizes::HTTPMessage& http_request, const UrlRequest::UrlVariables& url_variables);
		
		private:
			atomizes::HTTPMessage _defaultResponseHeader() const;
			void           _LogTransaction(const std::string& direction, const std::string& response) const;
			std::string    _readFile(const std::string& file_name) const;
			void           _SendFile(const std::string& file_name) const;
			void           _GetSessionPlayerAndClan(const UrlRequest::UrlVariables& url_variables, Battlefield::Clan& clan, Battlefield::Player& player) const;
	};
}

#endif // WEBSERVER_CLIENT_H
