#ifndef GAMESTATS_CLIENT_H
#define GAMESTATS_CLIENT_H

#include <net/socket.h>
#include <gamespy.h>

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
			void onRequest(const std::string& request);
			
			/*
				Requests
			*/
			void requestChallenge();
			void requestAuth(const GameSpy::Parameter& parameter);
			void requestNewGame(const GameSpy::Parameter& parameter);
			
		private:
			void _LogTransaction(const std::string& direction, const std::string& response) const;
		
		public:
			static void Test();
			static std::vector<unsigned char> Encrypt(const std::string& response);
			static std::string Decrypt(const std::vector<unsigned char>& request);
	};
}

#endif // GAMESTATS_CLIENT_H
