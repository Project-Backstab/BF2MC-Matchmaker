#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H

#include <net/socket.h>
#include <util.h>

// Forward declair
namespace atomizes
{
	class HTTPMessage;
};

namespace Json
{
	class Value;
};

/*
	https://en.wikipedia.org/wiki/WebSocket
*/
namespace Websocket
{
	class Client : public Net::Socket
	{
		public:
			Client(int socket, struct sockaddr_in address);
			~Client();
			
			void Listen();
			void Disconnect();
			void Send(const atomizes::HTTPMessage& http_response) const;
			void Send(const Json::Value &value) const;
			
			/*
				Events
			*/
			void onRequest(const std::vector<char>& buffer);
			
			/*
				Requests
			*/
			
		
		private:
			void _LogTransaction(const std::string& direction, const std::string& response) const;
			std::string _GetSocketAcceptKey(const std::string& websocket_key);
			std::vector<char> _UnmaskPayload(const std::vector<char>& key, const std::vector<char>& payload);
	};
}

#endif // WEBSOCKET_CLIENT_H
