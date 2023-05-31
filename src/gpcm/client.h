#ifndef GPCM_CLIENT_H
#define GPCM_CLIENT_H

#include <gamespy.h>
#include <net/socket.h>


namespace GPCM
{
	class Client;
	
	struct Session
	{
		std::string  challenge;
		int          profileid = -1;
		std::string  authtoken;
		Client* client;
	};

	class Client : public Net::Socket
	{
		private:
			GPCM::Session _session;
		
		public:
			Client(int socket, struct sockaddr_in address);
			~Client();
			
			void Listen();
			void Disconnect();
			GPCM::Session GetSession() const;
			
			/*
				Events
			*/
			void onRequest(const std::string &msg);
			
			/*
				Requests
			*/
			void requestChallenge();
			void requestLogin(const GameSpy::Parameter &parameter);
			void requestInviteTo(const GameSpy::Parameter& parameter);
			void requestGetProfile(const GameSpy::Parameter& parameter);
			void requestStatus(const GameSpy::Parameter& parameter);
			void requestBm(const GameSpy::Parameter& parameter);
			void requestAddBuddy(const GameSpy::Parameter& parameter);
			void requestRevoke(const GameSpy::Parameter& parameter);
			void requestDeleteBuddy(const GameSpy::Parameter& parameter);
			void requestAuthAdd(const GameSpy::Parameter& parameter);
			void requestLogout(const GameSpy::Parameter& parameter);
			
		private:
			void _LogTransaction(const std::string &direction, const std::string &response) const;
		
		/*
			Static
		*/
		public:
			static GPCM::Session findSessionByProfileId(int profileid);
			static GPCM::Session findSessionByAuthtoken(const std::string& authtoken);
			static void          SendBuddyMessage(int profileid, int target_profileid, const std::string& msg);
	};
}

#endif // GPCM_CLIENT_H
