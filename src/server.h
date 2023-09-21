#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <memory>

#include <net/socket.h>

class Server : public Net::Socket
{
	public:
		enum Type
		{
			QR,        // udp:27900 domain: bfield1942ps2.master.bfmcspy.net
			GPSP,      // tcp:29901 domain: gpsp.gamespy.com
			GPCM,      // tcp:29900 domain: gpcm.gamespy.com
			Webserver, // tcp:80    domain: www.easports.com and bfmc.gamespy.com
			Browsing,  // tcp:28910 domain: bfield1942ps2.available.gamespy.com and bfield1942ps2.ms7.gamespy.com
			GameStats, // tcp:29920 domain: bfield1942ps2.gamestats.bfmcspy.net
			None,
		};
	
	private:
		std::vector<std::shared_ptr<Net::Socket>> _clients;
		Server::Type        _type;
		mutable std::mutex  _mutex;
	
	public:
		Server(Server::Type type);
		
		std::vector<std::shared_ptr<Net::Socket>> GetClients();
		
		void Listen();
		void UDPListen();
		void DisconnectAllClients();
		void Close();
		
		// Events
		void onServerListen() const;
		void onServerShutdown() const;
		void onClientConnect(const Net::Socket& client) const;
		void onClientConnect(const std::shared_ptr<Net::Socket>& client) const;
		void onClientDisconnect(const Net::Socket& client);
};

#endif // SERVER_H
