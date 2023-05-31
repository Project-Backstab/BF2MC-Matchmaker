#ifndef GPSP_CLIENT_H
#define GPSP_CLIENT_H

#include <gamespy.h>
#include <net/socket.h>

namespace GPSP
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
			void onRequest(const std::string &msg);
			
			/*
				Requests
			*/
			void requestNicks(const GameSpy::Parameter &parameter) const;
			void requestValid(const GameSpy::Parameter &parameter) const;
			void requestNewUser(const GameSpy::Parameter &parameter) const;
			void requestSearch(const GameSpy::Parameter& parameter) const;
			
		private:
			void _LogTransaction(const std::string &direction, const std::string &response) const;
	};
}

#endif // GPSP_CLIENT_H
