#ifndef NET_SOCKET_H
#define NET_SOCKET_H

#include <string>
#include <mutex>
#include <netinet/in.h>
#include <chrono>

namespace Net
{
	class Socket
	{
		protected:
			int                                   _socket;
			struct sockaddr_in                    _address;
			std::chrono::system_clock::time_point _recieved_time;
			mutable std::mutex                    _mutex;

		
		public:
			Socket();
			
			void Close();
			
			std::string GetIP() const;
			void GetIpArray(uint8_t* ip) const;
			
			uint16_t GetPort() const;
			std::string GetAddress() const;
			std::string GetSocketType() const;
			std::chrono::system_clock::time_point GetLastRecievedTime() const;

			void Send(const std::string& msg) const;
			void Send(const std::vector<unsigned char>& msg) const;
			
			void UDPSend(const std::string& msg) const;
			void UDPSend(const std::vector<unsigned char>& msg) const;
			
			void UpdateLastRecievedTime();

			// ChatGPT:
			// In C++, you cannot directly cast a base class pointer (std::shared_ptr<Net::Socket>) to a derived class pointer
			// (std::shared_ptr<GPCM::Client>) using static_cast when the base class is not polymorphic (i.e., it doesn't have at
			// least one virtual function). This is because static_cast requires a polymorphic base class for a safe cast.
			virtual void WTF_WHY_AM_I_HERE_1337() { /* Empty virtual function */ }
	};
}

#endif // NET_SOCKET_H
