#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <regex>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#include <settings.h>
#include <logger.h>
#include <server.h>
#include <gpcm/client.h>
#include <globals.h>
#include <database.h>
#include <util.h>
#include <atomizes.hpp>
#include <service/file_system.h>

#include <websocket/client.h>

using namespace atomizes;

typedef void (Websocket::Client::*RequestActionFunc)(const atomizes::HTTPMessage&, const std::string&, const Util::Url::Variables&);

static std::map<std::string, RequestActionFunc> mRequestActions = 
{
	
};

Websocket::Client::Client(int socket, struct sockaddr_in address)
{
	this->_socket = socket;
	this->_address = address;
	this->UpdateLastRecievedTime();
}

Websocket::Client::~Client()
{
	this->Disconnect();
}

void Websocket::Client::Listen()
{
	while(true)
	{
		std::vector<char> buffer(4096, 0);
		
		// Read socket
		int recv_size = read(this->_socket, &(buffer[0]), 4096);
		
		// If error or no data is recieved we end the connection
		if(recv_size <= 0)
		{
			this->Disconnect();

			return;
		}
		
		// Resize buffer
		buffer.resize(recv_size);
		
		this->UpdateLastRecievedTime();

		// Trigger onRequest event
		this->onRequest(buffer);
	}

	this->Disconnect();
}

void Websocket::Client::Disconnect()
{
	this->Close();
	g_webserver_server->onClientDisconnect(*this);
}

void Websocket::Client::Send(const atomizes::HTTPMessage &http_response) const
{
	this->Net::Socket::Send(http_response.ToString());
}

/*
	Events
*/
void Websocket::Client::onRequest(const std::vector<char>& buffer)
{
	if(Util::Buffer::ToString(buffer).substr(0, 3) == "GET")
	{
		HTTPMessage http_response;
		HTTPMessageParser http_parser;
		HTTPMessage http_request;

		http_parser.Parse(&http_request, &(buffer[0]));

		std::string websocket_key = http_request.GetHeader("Sec-WebSocket-Key");
		std::string websocket_accept_key = this->_GetSocketAcceptKey(websocket_key);
		
		Logger::debug("websocket_key = " + websocket_key);
		Logger::debug("websocket_accept_key = " + websocket_accept_key);

		http_response.SetStatusCode(101);
		http_response.SetHeader("Upgrade", "websocket");
		http_response.SetHeader("Connection", "Upgrade");
		http_response.SetHeader("Sec-WebSocket-Accept", websocket_accept_key);
		// No compression support
		//http_response.SetHeader("Sec-WebSocket-Extensions", "permessage-deflate");
		http_response.SetHeader("Server", "BF2MC-Matchmaker");

		this->Send(http_response);
	
		this->_LogTransaction("<--", "HTTP/1.1 101 Switching Protocols");
	}
	else
	{
		std::vector<char> key(buffer.begin() + 4, buffer.begin() + 8);
		std::vector<char> payload(buffer.begin() + 8, buffer.end());

		std::vector<char> unmask_data = this->_UnmaskPayload(key,  payload);

		Logger::debug("unmask_data = " + Util::Buffer::ToString(unmask_data));
		
		/*
		auto it = mRequestActions.find(action);
		if (it != mRequestActions.end())
		{
			// Get Function address
			RequestActionFunc func = it->second;
		
			// Execute action function with class object.
			//(this->*(func))(http_request, url_base, url_variables);
		}
		else
		{		
			Logger::warning("action \"" + url_base + "\" not implemented!", Server::Type::Websocket);
		}
		*/
	}
}

/*
	Private functions
*/
void Websocket::Client::_LogTransaction(const std::string& direction, const std::string& response) const
{
	std::shared_lock<std::shared_mutex> guard(g_settings_mutex); // settings lock (read)
	
	if ((g_logger_mode & Logger::Mode::Development) == 0)
	{
		return;
	}
	
	bool show_console = (g_settings["websocket"]["show_requests"].asBool() && direction == "-->") ||
						(g_settings["websocket"]["show_responses"].asBool() && direction == "<--");
	
	Logger::info(this->GetAddress() + " " + direction + " " + response,
			Server::Type::Websocket, show_console);
}

std::string Websocket::Client::_GetSocketAcceptKey(const std::string& websocket_key)
{
    std::string GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    std::string concatenated = websocket_key + GUID;

    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(concatenated.c_str()), concatenated.length(), hash);

    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO* bio = BIO_new(BIO_s_mem());
    BIO_push(b64, bio);
    BIO_write(b64, hash, SHA_DIGEST_LENGTH);
    BIO_flush(b64);

    BUF_MEM* bptr;
    BIO_get_mem_ptr(b64, &bptr);

    std::string websocket_accept_key(bptr->data, bptr->length - 1); // Exclude the newline character

    BIO_free_all(b64);

    return websocket_accept_key + "=";
}

std::vector<char> Websocket::Client::_UnmaskPayload(const std::vector<char>& key, const std::vector<char>& payload)
{
	std::vector<char> unmask_data;

	// Unmask the payload data starting from the 9th byte
	for (size_t i = 0; i < payload.size(); ++i)
	{	
		unmask_data.push_back(payload[i] ^ key[i % 4]);
	}

	return unmask_data;
}

