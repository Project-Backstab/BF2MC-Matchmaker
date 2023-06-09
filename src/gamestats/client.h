#ifndef GAMESTATS_CLIENT_H
#define GAMESTATS_CLIENT_H

#include <net/socket.h>

namespace GameStats
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
			//void requestServerList(const std::vector<unsigned char>& request);
			
		private:
			void _LogTransaction(const std::string& direction, const std::string& response) const;
	};
}

#endif // GAMESTATS_CLIENT_H
