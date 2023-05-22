#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <string>

#include <net/socket.h>

//forward declare
namespace GPSP
{
	class Client;
}

class Server : public Net::Socket
{
	public:
		enum Type
		{
			GPSP,			// gpsp.gamespy.com
			GPCM,			// gpcm.gamespy.com
			EASports,		// www.easports.com
			Browsing,
		};
	
	private:
		std::vector<Net::Socket*> _clients;
		Server::Type              _type;
	
	public:
		Server(Server::Type type, int port);
		void Listen();
		void Close();
		
		// Events
		void onServerListen() const;
		void onServerShutdown() const;
		void onClientConnect(const Net::Socket &client) const;
		void onClientDisconnect(const Net::Socket &client) const;
};

#endif // SERVER_H
