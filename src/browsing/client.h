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
			void Send(const std::string &msg) const;
			void Send(const std::vector<unsigned char> &msg) const;
			
			/*
				Events
			*/
			void onRequest(const std::string &msg);
			
			/*
				Requests
			*/
			void requestServerList(const GameSpy::Parameter &parameter);
			
		private:
			void _LogTransaction(const std::string &direction, const std::string &response) const;
	};
}

#endif // BROWSING_CLIENT_H
