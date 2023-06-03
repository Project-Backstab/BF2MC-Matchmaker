#ifndef SERVER_H
#define SERVER_H

#include <vector>

#include <net/socket.h>

class Server : public Net::Socket
{
	public:
		enum Type
		{
			GPSP,			// gpsp.gamespy.com
			GPCM,			// gpcm.gamespy.com
			Webserver,		// www.easports.com and bfmc.gamespy.com
			Browsing,       // bfield1942ps2.available.gamespy.com and bfield1942ps2.ms7.gamespy.com
		};
	
	public:
		std::vector<Net::Socket*> _clients;
	
	private:
		Server::Type              _type;
	
	public:
		Server(Server::Type type, int port);
		void Listen();
		void DisconnectAllClients();
		void Close();
		
		// Events
		void onServerListen() const;
		void onServerShutdown() const;
		void onClientConnect(const Net::Socket& client) const;
		void onClientDisconnect(const Net::Socket& client);
};

#endif // SERVER_H
