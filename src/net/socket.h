#ifndef NET_SOCKET_H
#define NET_SOCKET_H

#include <string>
#include <mutex>
#include <netinet/in.h>
#include <chrono>

namespace Net
{
	/**
	 * @brief A base class representing a network socket.
	 */
	class Socket
	{
		protected:
			int                                   _socket;        /**< The socket file descriptor. */
			struct sockaddr_in                    _address;       /**< The socket address information. */
			std::chrono::system_clock::time_point _recieved_time; /**< Time when data was last received. */
			mutable std::mutex                    _mutex;         /**< Mutex for thread safety. */

		public:
			Socket();
			
			/**
			 * @brief Closes the socket.
			 */
			void Close();
			
			/**
			 * @brief Gets the IP address associated with the socket.
			 * @return The IP address as a string.
			 */
			std::string GetIP() const;
			
			/**
			 * @brief Gets the IP address associated with the socket as an array of bytes.
			 * @param ip Pointer to an array where the IP will be stored.
			 */
			void GetIpArray(uint8_t* ip) const;
			
			/**
			 * @brief Gets the port number associated with the socket.
			 * @return The port number.
			 */
			uint16_t GetPort() const;
			
			/**
			 * @brief Gets the full address (IP:Port) associated with the socket.
			 * @return The address as a string in the format "IP:Port".
			 */
			std::string GetAddress() const;

			/**
			 * @brief Gets the socket type.
			 * @return The socket type as a string.
			 */
			std::string GetSocketType() const;
			
			/**
			 * @brief Gets the time when the socket last received data.
			 * @return The last received time as a system_clock::time_point.
			 */
			std::chrono::system_clock::time_point GetLastRecievedTime() const;

			/**
			 * @brief Sends a message over the socket.
			 * @param msg The message to send as a string.
			 */
			void Send(const std::string& msg) const;
			
			/**
			 * @brief Sends a message over the socket.
			 * @param msg The message to send as a vector of unsigned chars.
			 */
			void Send(const std::vector<unsigned char>& msg) const;
			
			/**
			 * @brief Sends a UDP message over the socket.
			 * @param msg The message to send as a string.
			 */
			void UDPSend(const std::string& msg) const;
			
			/**
			 * @brief Sends a UDP message over the socket.
			 * @param msg The message to send as a vector of unsigned chars.
			 */
			void UDPSend(const std::vector<unsigned char>& msg) const;
			
			/**
			 * @brief Updates the last received time to the current system time.
			 */
			void UpdateLastRecievedTime();

			/**
			 * @brief Empty virtual function required for static_cast in C++.
			 *
			 * In C++, you cannot directly cast a base class pointer (std::shared_ptr<Net::Socket>) to a derived class pointer
			 * (std::shared_ptr<GPCM::Client>) using static_cast when the base class is not polymorphic (i.e., it doesn't have at
			 * least one virtual function). This is because static_cast requires a polymorphic base class for a safe cast.
			 */
			virtual void WTF_WHY_AM_I_HERE_1337() { /* Empty virtual function */ }
	};
}

#endif // NET_SOCKET_H
