#ifndef QR_CLIENT_H
#define QR_CLIENT_H

#include <gamespy.h>
#include <net/socket.h>

namespace QR
{
	class Client : public Net::Socket
	{
		public:
			Client(int socket, struct sockaddr_in address);
			~Client();
			
			/*
				Events
			*/
			void onRequest(const std::vector<unsigned char>& request);
			
			/*
				Requests
			*/
			void requestChallenge(const std::vector<unsigned char>& request) const;
			void requestAvailable(const std::vector<unsigned char>& request) const;
			void requestHeartbeat(const std::vector<unsigned char>& request) const;
			void requestKeepAlive(const std::vector<unsigned char>& request) const;
			
		private:
			void _LogTransaction(const std::string& direction, const std::string& response) const;
	};
}

#endif // QR_CLIENT_H
