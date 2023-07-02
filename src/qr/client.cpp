#include <map>
#include <vector>

#include <logger.h>
#include <util.h>
#include <settings.h>
#include <qr/constants.h>

#include <qr/client.h>

typedef void (QR::Client::*RequestActionFunc)(const std::vector<unsigned char>&) const;

static std::map<int, RequestActionFunc> mRequestActions = 
{
	{ REQUEST_CHALLENGE,   &QR::Client::requestChallenge   }, // Done
	{ REQUEST_AVAILABLE,   &QR::Client::requestAvailable   }, // Done
	{ REQUEST_HEARTBEAT,   &QR::Client::requestHeartbeat   }, // Done
	{ REQUEST_KEEPALIVE,   &QR::Client::requestKeepAlive   }, // Done
};

QR::Client::Client(int socket, struct sockaddr_in address)
{
	this->_socket = socket;
	this->_address = address;
}

QR::Client::~Client()
{
	
}

/*
	Events
*/
void QR::Client::onRequest(const std::vector<unsigned char>& request)
{
	this->_LogTransaction("-->", Util::Buffer2String(request));
	
	int action = request[0];
	
	// Find function name
	auto it = mRequestActions.find(action);
	if (it != mRequestActions.end())
	{
		// Get Function address
		RequestActionFunc func = it->second;
	
		// Execute action function with class object.
		(this->*(func))(request);
	}
	else
	{
		Logger::warning("action \"" + std::to_string(action) + "\" not implemented!", Server::Type::QR);
	}
}

/*
	
*/
void QR::Client::requestChallenge(const std::vector<unsigned char>& request) const
{
	std::vector<unsigned char> response = {
		RESPONSE_MAGIC_1, RESPONSE_MAGIC_2,				// Magic
		RESPONSE_CHALLENGE,								// Challenge response action
		request[1], request[2], request[3], request[4],	// Instance key
	};
	
	this->UDPSend(response);
	
	this->_LogTransaction("<--", Util::Buffer2String(response));
}

/*
	
*/
void QR::Client::requestAvailable(const std::vector<unsigned char>& request) const
{
	std::vector<unsigned char> response = {
		RESPONSE_MAGIC_1, RESPONSE_MAGIC_2,
		RESPONSE_AVAILABLE,
		0x0, 0x0, 0x0, 0x0
	};
	
	this->UDPSend(response);
	
	this->_LogTransaction("<--", Util::Buffer2String(response));
}

/*
	Response:
		0020                                 fe fd 01 ad b2 25             .....%
		0030   10 51 56 46 63 55 7a 34 37 34 45 32 46 42 38 31   .QVFcUz474E2FB81
		0040   37 30 45 34 41 00                                 70E4A.
*/
void QR::Client::requestHeartbeat(const std::vector<unsigned char>& request) const
{
	std::vector<unsigned char> response = {
		RESPONSE_MAGIC_1, RESPONSE_MAGIC_2,				// Magic
		RESPONSE_HEARTBEAT,								// Challenge response action
		request[1], request[2], request[3], request[4],	// Instance key
		0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, //   - 8 random characters
		0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,	//   - client ip in hex
		0x30, 0x30, 0x30, 0x30,							//   - client port in hex
		0x00
	};
	
	this->UDPSend(response);
	
	this->_LogTransaction("<--", Util::Buffer2String(response));
}

/*

*/
void QR::Client::requestKeepAlive(const std::vector<unsigned char>& request) const
{
	std::vector<unsigned char> response = {
		RESPONSE_MAGIC_1, RESPONSE_MAGIC_2,				// Magic
		RESPONSE_KEEPALIVE,								// Challenge response action
		request[1], request[2], request[3], request[4],	// Instance key
		0x00, 0x00, 0x00, 0x00,							// time
	};
	
	this->UDPSend(response);
	
	this->_LogTransaction("<--", Util::Buffer2String(response));
}

/*
	Private functions
*/
void QR::Client::_LogTransaction(const std::string& direction, const std::string& response) const
{
	std::shared_lock<std::shared_mutex> guard(g_mutex_settings); // settings lock  (read)
	
	bool show_console = (g_settings["qr"]["show_requests"].asBool() && direction == "-->") ||
						(g_settings["qr"]["show_responses"].asBool() && direction == "<--");
	
	Logger::info(this->GetAddress() + " " + direction + " " + response,
			Server::Type::QR, show_console);
}

