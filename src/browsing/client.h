#ifndef BROWSING_CLIENT_H
#define BROWSING_CLIENT_H

#include <gamespy.h>
#include <net/socket.h>

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
