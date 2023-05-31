#ifndef NET_SOCKET_H
#define NET_SOCKET_H

#include <string>
#include <mutex>
#include <netinet/in.h>

namespace Net
{
	class Socket
	{
		protected:
			int                  _socket;
			struct sockaddr_in   _address;
			mutable std::mutex  _mutex;
		
		public:
			Socket();
			
			std::string GetIP() const;
			int GetPort() const;
			std::string GetAddress() const;
			
			void Send(const std::string &msg) const;
			void Send(const std::vector<unsigned char> &msg) const;
	};
}

#endif // NET_SOCKET_H
