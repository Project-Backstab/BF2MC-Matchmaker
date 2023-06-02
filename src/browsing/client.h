#ifndef BROWSING_CLIENT_H
#define BROWSING_CLIENT_H

#include <gamespy.h>
#include <net/socket.h>

//message types for outgoing requests
#define REQUEST_SERVER_LIST   0
#define REQUEST_SERVER_INFO   1
#define REQUEST_SEND_MESSAGE  2
#define REQUEST_KEEPALIVE     3
#define REQUEST_MAPLOOP       4
#define REQUEST_PLAYERSEARCH  5

#define CRYPT_CHALLENGE_LEN   10
#define SERVER_CHALLENGE_LEN  25
#define CLIENT_CHALLENGE_LEN  8
#define BROWSING_HEADER_LEN   (1 + CRYPT_CHALLENGE_LEN + 1 + SERVER_CHALLENGE_LEN)

#define SECRET_KEY            "HpWx9z"
#define SECRET_KEY_LEN        6

namespace Browsing
{
	class Client : public Net::Socket
	{	
		public:
			Client(int socket, struct sockaddr_in address);
			~Client();
			
			void Listen();
			void Disconnect();
			
			/*
				Events
			*/
			void onRequest(const std::vector<unsigned char>& request);
			
			/*
				Requests
			*/
			void requestServerList(const std::vector<unsigned char>& request);
			
		private:
			void _Encrypt(const std::vector<unsigned char>& request, std::vector<unsigned char>& response);
			void _LogTransaction(const std::string &direction, const std::string &response) const;
		
		public:
			static void Test();
	};
}

#endif // BROWSING_CLIENT_H
