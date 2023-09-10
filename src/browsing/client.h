#ifndef BROWSING_CLIENT_H
#define BROWSING_CLIENT_H

#include <gamespy.h>
#include <net/socket.h>
#include <battlefield/gameserver.h>


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
			void _LogTransaction(const std::string& direction, const std::string& response) const;
			
			void _FilterServers(const std::string& filter, Battlefield::GameServers& game_servers);
			bool _FilterServerGameVersion(const std::string& filter, const Battlefield::GameServer& game_server);
			bool _FilterServerRegion(const std::string& filter, const Battlefield::GameServer& game_server);
			bool _FilterServerNumPlayers(const std::string& filter, const Battlefield::GameServer& game_server);
			bool _FilterServerGameType(const std::string& filter, const Battlefield::GameServer& game_server);
			bool _FilterServerMapName(const std::string& filter, const Battlefield::GameServer& game_server);
			bool _FilterServerStatsTracking(const std::string& filter, const Battlefield::GameServer& game_server);
			bool _FilterServerReconfigurable(const std::string& filter, const Battlefield::GameServer& game_server);
			bool _FilterServerClan(const std::string& filter, const Battlefield::GameServer& game_server);
			bool _FilterServerTeamplay(const std::string& filter, const Battlefield::GameServer& game_server);
			
		public:
			static void Test();
			static void Crack();
	};
}

#endif // BROWSING_CLIENT_H
