#ifndef BROWSING_CLIENT_H
#define BROWSING_CLIENT_H

#include <gamespy.h>
#include <net/socket.h>

//message types for outgoing requests
#define SERVER_LIST_REQUEST   0
#define SERVER_INFO_REQUEST   1
#define SEND_MESSAGE_REQUEST  2
#define KEEPALIVE_REPLY       3
#define MAPLOOP_REQUEST       4
#define PLAYERSEARCH_REQUEST  5

#define CRYPTCHAL_LEN         10
#define SERVCHAL_LEN          25
#define LIST_CHALLENGE_LEN    8

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
			void onRequest(const std::vector<unsigned char>& msg);
			
			/*
				Requests
			*/
			void requestServerList(const std::vector<unsigned char>& msg);
			
		private:
			void _LogTransaction(const std::string &direction, const std::string &response) const;
		
		public:
			static void Test();
	};
}

#endif // BROWSING_CLIENT_H
