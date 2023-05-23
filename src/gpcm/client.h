#ifndef GPCM_CLIENT_H
#define GPCM_CLIENT_H

#include <gamespy.h>
#include <net/socket.h>

namespace GPCM
{
	class Client : public Net::Socket
	{
		private:
			mutable int _session_profileid;
		
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
			void requestChallenge() const;
			void requestLogin(const GameSpy::Parameter &parameter) const;
			void requestInviteTo(const GameSpy::Parameter& parameter) const;
			void requestGetProfile(const GameSpy::Parameter& parameter) const;
			void requestStatus(const GameSpy::Parameter& parameter) const;
			void requestBm(const GameSpy::Parameter& parameter) const;
			void requestAddBuddy(const GameSpy::Parameter& parameter) const;
			void requestRevoke(const GameSpy::Parameter& parameter) const;
			void requestDeleteBuddy(const GameSpy::Parameter& parameter) const;
			void requestLogout(const GameSpy::Parameter& parameter) const;
			
		private:
			void _LogTransaction(const std::string &direction, const std::string &response) const;
	};
}

#endif // GPCM_CLIENT_H
