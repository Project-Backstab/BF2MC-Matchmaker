#ifndef NET_SOCKET_H
#define NET_SOCKET_H

#include <netinet/in.h>
#include <string>

namespace Net
{
	class Socket
	{
		protected:
			int                  _socket;
			struct sockaddr_in   _address;
		
		public:
			Socket();
			
			std::string GetIP() const;
			int GetPort() const;
			std::string GetAddress() const;
	};
}

#endif // NET_SOCKET_H
