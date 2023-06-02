#include <unistd.h>
#include <iostream>
#include <thread>
#include <iomanip>
#include <map>

#include <server.h>
#include <gamespy.h>
#include <globals.h>
#include <util.h>
#include <browsing/sb_crypt.h>

#include <browsing/client.h>

typedef void (Browsing::Client::*RequestActionFunc)(const GameSpy::Parameter&);

static std::map<std::string, RequestActionFunc> mRequestActions = 
{
	{ "serverlist",   &Browsing::Client::requestServerList   },
};

Browsing::Client::Client(int socket, struct sockaddr_in address)
{
	this->_socket = socket;
	this->_address = address;
}

Browsing::Client::~Client()
{
	this->Disconnect();
}

void Browsing::Client::Listen()
{
	while(true)
	{
		std::string request;
		std::vector<char> buffer(4096, 0);
		
		int v = read(this->_socket, &(buffer[0]), 4096);
		
		// If error or no data is recieved we end the connection
		if(v <= 0)
		{
			break;
		}
		
		// Resize buffer
		buffer.resize(v);
		
		request = Util::Buffer2String(buffer);
		
		this->_LogTransaction("-->", request);
		
		this->onRequest(request);
	}
	
	this->Disconnect();
}

void Browsing::Client::Disconnect()
{
	close(this->_socket);
	g_browsing_server->onClientDisconnect(*this);
}

/*
	Events
*/
void Browsing::Client::onRequest(const std::string &request)
{
	GameSpy::Parameter parameter = GameSpy::Request2Parameter(request);
	
	// Find function name
	//std::string action = parameter[0];
	std::string action = "serverlist";
	
	if (mRequestActions.find(action) != mRequestActions.end())
	{
		// Get Function address
		RequestActionFunc func = mRequestActions[action];
	
		// Execute action function with class object.
		(this->*(func))(parameter);
	}
	else
	{
		std::unique_lock<std::mutex> guard(g_mutex_io); // io lock (read/write)
		
		std::cout << "action \"" << action << "\"not implemented!" << std::endl;
		
		guard.unlock();
		
		this->Disconnect();
	}
}

/*
	
*/
void Browsing::Client::requestServerList(const GameSpy::Parameter& parameter)
{
	std::vector<unsigned char> data = {
		0xe6, 0x21, 0x12, 0x22, 0xc7, 0xbe, 0xbc, 0x2f, 0xa9, 0x35, 0x12, 0xf3, 0x7c, 0x80, 0xe9, 0xd2,
		0xba, 0x93, 0x69, 0x98, 0xe8, 0xac, 0x91, 0xa9, 0xf9, 0xc4, 0x37, 0xf4, 0x08, 0x18, 0x1c, 0x8a,
		0xa5, 0x3e, 0x1c, 0xc7, 0x05, 0xcd, 0x58, 0x78, 0x25, 0x4c, 0xba, 0xba, 0x8a, 0x97, 0x35, 0x7b,
		0xd0, 0xcc, 0x3a, 0xac, 0xd0, 0x2f, 0x1f, 0x13, 0xa3, 0x8e, 0x1c, 0xac, 0xa5, 0x89, 0x34, 0xc0,
		0x9e, 0x7d, 0x90, 0xe2, 0x08, 0x8f, 0x6c, 0x1b, 0xf8, 0xc3, 0xb7, 0x64, 0x4b, 0xfd, 0x66, 0x7b,
		0x24, 0xff, 0x35, 0xea, 0xcb, 0xcc, 0x82, 0x33, 0xc4, 0x35, 0xa6, 0xc2, 0xa4, 0x6a, 0x75, 0x03,
		0x8e, 0x33, 0x97, 0x2c, 0xac, 0x39, 0xea, 0x0a, 0x66, 0x55, 0xf5, 0x6f, 0x7a, 0x84, 0x4f, 0x15,
		0x92, 0xc9, 0xb5, 0x65, 0x3a, 0x74, 0xf8, 0x61, 0x88, 0x9c, 0x53, 0x7c, 0x89, 0xfa, 0x8a, 0x46,
		0x57, 0x9a, 0x83, 0xd8, 0x3b, 0x7c, 0x70, 0xdc, 0x70, 0xf1, 0xa2, 0xd0, 0x76, 0xa4, 0x3a, 0x0f,
		0x87, 0xc7, 0x1b, 0x84, 0xf0, 0x26, 0xfa, 0x31, 0x77, 0xa2, 0x3f, 0xb8, 0x55, 0x8c, 0x6b, 0xcb,
		0x39, 0xbd, 0x0e, 0x2d, 0x4b, 0xbf, 0xa5, 0x76, 0xb3, 0x48, 0x33, 0xa8, 0x99, 0x20, 0xb8, 0x6f,
		0x37, 0xde, 0xf1, 0x17, 0xd6, 0xd1, 0x8f, 0x72, 0xa4, 0x29, 0x1b, 0x17, 0xe4, 0x0c, 0x8b, 0x77,
		0xda, 0x88, 0x4d, 0x7a, 0x4a, 0xf1, 0x65, 0x7a, 0xa1, 0x49, 0x5c, 0xd7, 0x15, 0xcc, 0x4c, 0x1c,
		0x98, 0x79, 0x46, 0x54, 0xef, 0xdd, 0xe3, 0xcd, 0xfc, 0xed, 0x4d, 0x1f, 0x51, 0x32, 0xf7, 0xf7,
		0x60, 0xc8, 0xd4, 0x6a, 0xcd, 0xd1, 0x9c, 0x8e, 0x84, 0xba, 0x6a, 0x05, 0xc9, 0x35, 0xa2, 0x24,
		0x13, 0x1c, 0xe7, 0xae, 0x91, 0x63, 0xd1, 0x7b, 0x71, 0x1e, 0xcf, 0xb1, 0x58, 0x70, 0xaf, 0xd0,
		0x8f, 0x6a, 0x00, 0x61, 0x34, 0x4a, 0x1d, 0xaa, 0x03, 0xe3, 0x1e, 0xc6, 0x5e, 0xfc, 0xcd
	};
	
	std::vector<unsigned char> data2 = {
		0xe6, 0x21, 0x12, 0x22, 0xc7, 0xbe, 0xbc, 0x2f, 0xa9, 0x35, 0x12, 0xf3, 0x7c, 0x80, 0xe9, 0xd2,
		0xba, 0x93, 0x69, 0x98, 0xe8, 0xac, 0x91, 0xa9, 0xf9, 0xc4, 0x37, 0xf4, 0x08, 0x18, 0x1c, 0x8a,
		0xa5, 0x3e, 0x1c, 0xc7, 0x05, 0x77, 0x42, 0x41, 0x5c, 0x7e, 0x87, 0xd9, 0xe6, 0x26, 0x6e, 0x3e,
		0x49, 0xea, 0x43, 0x3a, 0xfb, 0x18, 0x1d, 0x2c, 0xd4, 0x2c, 0xfb, 0x1e, 0x1e, 0xac, 0x1f, 0x24,
		0x09, 0xad, 0x90, 0x39, 0x41, 0x05, 0x6b, 0x07, 0xd2, 0x6b, 0x0e, 0x0d, 0xa0, 0xf4, 0x15, 0x49,
		0x9b, 0x30, 0xfe, 0x54, 0xa6, 0x99, 0x1e, 0xd7, 0x9e, 0x6b, 0x0b, 0x00, 0xb6, 0x78, 0x56, 0xb6,
		0xcd, 0x94, 0xbf, 0x32, 0x7d, 0x01, 0x5a, 0xcc, 0xcf, 0x1b, 0x11, 0xb3, 0xc1, 0xee, 0x26, 0x5d,
		0xfa, 0x2b, 0x36, 0xb5, 0x34, 0xa0, 0xe7, 0xc6, 0x7b, 0x96, 0x26, 0xe7, 0xca, 0x38, 0x3f, 0x69,
		0xc3, 0x25, 0x6e, 0x9d, 0xc0, 0x5e, 0xe9, 0x4c, 0x0b, 0x1d, 0xfa, 0x77, 0xcb, 0x12, 0x75, 0x19,
		0xa9, 0x81, 0x50, 0x4f, 0x12, 0x71, 0xea, 0x0d, 0x5f, 0x07, 0x61, 0xc6, 0x99, 0x3e, 0xf5, 0x45,
		0xed, 0xc7, 0x2d, 0xb8, 0x75, 0x00, 0x12, 0x64, 0x0f, 0xe3, 0xc2, 0xa8, 0x6f, 0x9b, 0x79, 0x18,
		0x59, 0x10, 0x5a, 0xee, 0x98, 0xc9, 0x1f, 0x49, 0x4b, 0x47, 0x6f, 0xbd, 0x31, 0x4b, 0x4c, 0xa1,
		0x87, 0x66, 0xf9, 0xbb, 0x0e, 0x8b, 0x48, 0xc3, 0x03, 0xa4, 0x7c, 0x19, 0xc8, 0xca, 0x22, 0x6a,
		0xe2, 0xc0, 0xf5, 0xfe, 0x24, 0xed, 0x36, 0x78, 0xed, 0x5c, 0xf6, 0x97, 0x1b, 0xbc, 0x71, 0x39,
		0x20, 0x34, 0x36, 0x53, 0xe9, 0x46, 0x4c, 0xb7, 0x98, 0xf7, 0x8a, 0x9a, 0x70
	};
	
	this->Send(data);
	//this->Send(data2);
	
	std::cout << "Send data" << std::endl;
	//this->_LogTransaction("<--", response);
	
	//this->Disconnect();
}

/*
	Private functions
*/
void Browsing::Client::_LogTransaction(const std::string &direction, const std::string &response) const
{
	std::lock_guard<std::mutex> guard(g_mutex_io); // io lock (read/write)
	
	std::cout << std::setfill(' ') << std::setw(21) << this->GetAddress() << " " << direction << " " << response << std::endl;
}

std::vector<unsigned char> example_A_request = {
	0x01, 0x5e,                                                                                    // Total Bytes to read
	0x00,                                                                                          // SERVER_LIST_REQUEST
	0x01,                                                                                          // protocol_version
	0x03,                                                                                          // encoding_version
	0x00, 0x00, 0x00, 0x00,                                                                        // game_version
	0x62, 0x66, 0x69, 0x65, 0x6c, 0x64, 0x31, 0x39, 0x34, 0x32, 0x70, 0x73, 0x32, 0x00,            // for_gamename
	0x62, 0x66, 0x69, 0x65, 0x6c, 0x64, 0x31, 0x39, 0x34, 0x32, 0x70, 0x73, 0x32, 0x00,            // from_gamename
	0x25, 0x60, 0x50, 0x27, 0x6c, 0x6d, 0x64, 0x55,                                                // client challenge
	
	// filter: gamever='V1.31a' and (teamplay=0 and numplayers<=23 and numplayers>=0 and numplayers!=maxplayers and nr<=2 and xr>=2)and (region & 1)!=0
	0x67, 0x61, 0x6d, 0x65, 0x76, 0x65, 0x72, 0x3d, 0x27, 0x56, 0x31, 0x2e, 0x33, 0x31, 0x61, 0x27,
	0x20, 0x61, 0x6e, 0x64, 0x20, 0x28, 0x74, 0x65, 0x61, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x3d, 0x30,
	0x20, 0x61, 0x6e, 0x64, 0x20, 0x6e, 0x75, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73, 0x3c,
	0x3d, 0x32, 0x33, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x6e, 0x75, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x65,
	0x72, 0x73, 0x3e, 0x3d, 0x30, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x6e, 0x75, 0x6d, 0x70, 0x6c, 0x61,
	0x79, 0x65, 0x72, 0x73, 0x21, 0x3d, 0x6d, 0x61, 0x78, 0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73,
	0x20, 0x61, 0x6e, 0x64, 0x20, 0x6e, 0x72, 0x3c, 0x3d, 0x32, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x78,
	0x72, 0x3e, 0x3d, 0x32, 0x29, 0x61, 0x6e, 0x64, 0x20, 0x28, 0x72, 0x65, 0x67, 0x69, 0x6f, 0x6e,
	0x20, 0x26, 0x20, 0x31, 0x29, 0x21, 0x3d, 0x30, 0x00,
	
	// key_list: \hostname\gametype\gamever\hostport\timelimit\timeelapsed\mapname\numplayers\maxplayers\
	             teamplay\team_t\ping_\c0\c1\n0\n1\c0c\c1c\nr\xr\ff\mc\gc\rc\sr\ni\xi\qm
	0x5c, 0x68, 0x6f, 0x73, 0x74, 0x6e, 0x61, 0x6d, 0x65, 0x5c, 0x67, 0x61, 0x6d, 0x65, 0x74, 0x79,
	0x70, 0x65, 0x5c, 0x67, 0x61, 0x6d, 0x65, 0x76, 0x65, 0x72, 0x5c, 0x68, 0x6f, 0x73, 0x74, 0x70,
	0x6f, 0x72, 0x74, 0x5c, 0x74, 0x69, 0x6d, 0x65, 0x6c, 0x69, 0x6d, 0x69, 0x74, 0x5c, 0x74, 0x69,
	0x6d, 0x65, 0x65, 0x6c, 0x61, 0x70, 0x73, 0x65, 0x64, 0x5c, 0x6d, 0x61, 0x70, 0x6e, 0x61, 0x6d,
	0x65, 0x5c, 0x6e, 0x75, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73, 0x5c, 0x6d, 0x61, 0x78,
	0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73, 0x5c, 0x74, 0x65, 0x61, 0x6d, 0x70, 0x6c, 0x61, 0x79,
	0x5c, 0x74, 0x65, 0x61, 0x6d, 0x5f, 0x74, 0x5c, 0x70, 0x69, 0x6e, 0x67, 0x5f, 0x5c, 0x63, 0x30,
	0x5c, 0x63, 0x31, 0x5c, 0x6e, 0x30, 0x5c, 0x6e, 0x31, 0x5c, 0x63, 0x30, 0x63, 0x5c, 0x63, 0x31,
	0x63, 0x5c, 0x6e, 0x72, 0x5c, 0x78, 0x72, 0x5c, 0x66, 0x66, 0x5c, 0x6d, 0x63, 0x5c, 0x67, 0x63,
	0x5c, 0x72, 0x63, 0x5c, 0x73, 0x72, 0x5c, 0x6e, 0x69, 0x5c, 0x78, 0x69, 0x5c, 0x71, 0x6d, 0x00,
	
	0x00, 0x00, 0x00, 0x80,                                                                         // options
	0x14, 0x00, 0x00, 0x00                                                                          // options max_results
};

std::vector<unsigned char> example_A_response = {
	// Crypt challenge
	0xe6,                                                                                           // length
	0xbb, 0x79, 0x76, 0xb5, 0xb9, 0xe1, 0xa6, 0xaf, 0x08, 0x15,                                     // data
	
	// Server challenge
	0xf3,                                                                                           // length // data
	0xc7, 0x63, 0x80, 0x6e, 0xd7, 0x70, 0x7a, 0x11, 0xc4, 0x03, 0x1b, 0xa0, 0x70, 0x13, 0x7e, 0x69, // data 1-16
	0x69, 0x53, 0x33, 0x45, 0x92, 0xee, 0x3e, 0x09, 0xa4,
	
	// Encrypted data
	0xe6, 0x35, 0x83, 0xc4, 0x70, 0x40, 0xdf, 0x4f, 0x14, 0x77, 0x52, 0xb3, 0x4c, 0xf8, 0xca, 0xec,
	0x9f, 0xd7, 0x74, 0xe9, 0x03, 0x49, 0xe8, 0xcb, 0xef, 0xd8, 0xb3, 0x01, 0xfc, 0xe6, 0xe1, 0xd0,
	0x03, 0xbe, 0x61, 0x43, 0x2a, 0x66, 0xfb, 0x13, 0x3d, 0x67, 0xc5, 0x17, 0xd9, 0x53, 0x52, 0xbf,
	0xa8, 0x87, 0xe6, 0x20, 0x2d, 0xd1, 0xe9, 0xb8, 0x82, 0x29, 0xac, 0xb1, 0x93, 0x63, 0x10, 0xf2,
	0xa3, 0x7f, 0x60, 0x61, 0x31, 0xc3, 0x22, 0xa0, 0x7b, 0x88, 0x25, 0xf1, 0xbe, 0xee, 0x5b, 0x44,
	0x43, 0xab, 0xb9, 0x84, 0x58, 0xb4, 0xf4, 0xf3, 0xf6, 0x61, 0x81, 0x31, 0x04, 0x9c, 0xf5, 0xff,
	0xe2, 0x79, 0x65, 0x98, 0x6d, 0x1d, 0x4d, 0x00, 0xa8, 0x2d, 0x1d, 0x4f, 0xfd, 0x68, 0x7e, 0x7e,
	0xb3, 0x0f, 0xda, 0x4c, 0x90, 0xe9, 0x89, 0x64, 0x3d, 0xbe, 0xdf, 0xd3, 0x31, 0x2f, 0x1a, 0x96,
	0x89, 0xef, 0x00, 0x89, 0x7a, 0xe8, 0x56, 0xa1, 0x5a, 0x95, 0x66, 0xd4, 0x20, 0x0b, 0x20, 0xbc,
	0xe7, 0xe6, 0xea, 0x55, 0xdd, 0xd3, 0x33, 0x9c, 0x37, 0x82, 0xcb, 0xc1, 0x8d, 0x40, 0xf3, 0x0f,
	0x33, 0x13, 0x1f, 0xd4, 0x40, 0x65, 0xd1, 0x3f, 0xd1, 0xbb, 0xf6, 0x71, 0x88, 0xfc, 0x9c, 0xa4,
	0x8b, 0xbe, 0x41, 0xf6, 0x11, 0x0b, 0x09, 0x7f, 0x1d, 0x72, 0x04, 0x05, 0x4b, 0xaf, 0xe8, 0x0b,
	0x62, 0x7b, 0xce, 0xcf, 0x27, 0xd0, 0xb6, 0x19, 0x4a, 0x25, 0x94, 0x8f, 0x6a, 0x50, 0xa4, 0xec,
	0x30, 0x19, 0xda, 0x08, 0xc7, 0xa1, 0x90, 0x2e, 0x83, 0x44, 0x16, 0x49, 0x6b, 0x69, 0x6c, 0x63,
	0x17, 0x59, 0x28, 0xc9, 0xa9, 0x6f, 0xcb, 0xce, 0x22, 0x9f
};

std::vector<unsigned char> example_B_request = {
	0x01, 0x5d,                                                                                    // Total Bytes to read
	0x00,                                                                                          // SERVER_LIST_REQUEST
	0x01,                                                                                          // protocol_version
	0x03,                                                                                          // encoding_version
	0x00, 0x00, 0x00, 0x00,                                                                        // game_version
	0x62, 0x66, 0x69, 0x65, 0x6c, 0x64, 0x31, 0x39, 0x34, 0x32, 0x70, 0x73, 0x32, 0x00,            // for_gamename
	0x62, 0x66, 0x69, 0x65, 0x6c, 0x64, 0x31, 0x39, 0x34, 0x32, 0x70, 0x73, 0x32, 0x00,            // from_gamename
	0x3b, 0x6e, 0x32, 0x56, 0x72, 0x4d, 0x2a, 0x30,                                                // client challenge
	
	// filter: gamever='V1.31a' and (teamplay=0 and numplayers<=23 and numplayers>=0 and numplayers!=maxplayers and nr<=2 and xr>=2)and (region & 1)=0
	0x67, 0x61, 0x6d, 0x65, 0x76, 0x65, 0x72, 0x3d, 0x27, 0x56, 0x31, 0x2e, 0x33, 0x31, 0x61, 0x27,
	0x20, 0x61, 0x6e, 0x64, 0x20, 0x28, 0x74, 0x65, 0x61, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x3d, 0x30,
	0x20, 0x61, 0x6e, 0x64, 0x20, 0x6e, 0x75, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73, 0x3c,
	0x3d, 0x32, 0x33, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x6e, 0x75, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x65,
	0x72, 0x73, 0x3e, 0x3d, 0x30, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x6e, 0x75, 0x6d, 0x70, 0x6c, 0x61,
	0x79, 0x65, 0x72, 0x73, 0x21, 0x3d, 0x6d, 0x61, 0x78, 0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73,
	0x20, 0x61, 0x6e, 0x64, 0x20, 0x6e, 0x72, 0x3c, 0x3d, 0x32, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x78,
	0x72, 0x3e, 0x3d, 0x32, 0x29, 0x61, 0x6e, 0x64, 0x20, 0x28, 0x72, 0x65, 0x67, 0x69, 0x6f, 0x6e,
	0x20, 0x26, 0x20, 0x31, 0x29, 0x3d, 0x30, 0x00,
	
	// key_list: \hostname\gametype\gamever\hostport\timelimit\timeelapsed\mapname\numplayers\maxplayers\
	             teamplay\team_t\ping_\c0\c1\n0\n1\c0c\c1c\nr\xr\ff\mc\gc\rc\sr\ni\xi\qm
	0x5c, 0x68, 0x6f, 0x73, 0x74, 0x6e, 0x61, 0x6d, 0x65, 0x5c, 0x67, 0x61, 0x6d, 0x65, 0x74, 0x79,
	0x70, 0x65, 0x5c, 0x67, 0x61, 0x6d, 0x65, 0x76, 0x65, 0x72, 0x5c, 0x68, 0x6f, 0x73, 0x74, 0x70,
	0x6f, 0x72, 0x74, 0x5c, 0x74, 0x69, 0x6d, 0x65, 0x6c, 0x69, 0x6d, 0x69, 0x74, 0x5c, 0x74, 0x69,
	0x6d, 0x65, 0x65, 0x6c, 0x61, 0x70, 0x73, 0x65, 0x64, 0x5c, 0x6d, 0x61, 0x70, 0x6e, 0x61, 0x6d,
	0x65, 0x5c, 0x6e, 0x75, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73, 0x5c, 0x6d, 0x61, 0x78,
	0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73, 0x5c, 0x74, 0x65, 0x61, 0x6d, 0x70, 0x6c, 0x61, 0x79,
	0x5c, 0x74, 0x65, 0x61, 0x6d, 0x5f, 0x74, 0x5c, 0x70, 0x69, 0x6e, 0x67, 0x5f, 0x5c, 0x63, 0x30,
	0x5c, 0x63, 0x31, 0x5c, 0x6e, 0x30, 0x5c, 0x6e, 0x31, 0x5c, 0x63, 0x30, 0x63, 0x5c, 0x63, 0x31,
	0x63, 0x5c, 0x6e, 0x72, 0x5c, 0x78, 0x72, 0x5c, 0x66, 0x66, 0x5c, 0x6d, 0x63, 0x5c, 0x67, 0x63,
	0x5c, 0x72, 0x63, 0x5c, 0x73, 0x72, 0x5c, 0x6e, 0x69, 0x5c, 0x78, 0x69, 0x5c, 0x71, 0x6d, 0x00,
	
	0x00, 0x00, 0x00, 0x80,                                                                         // options
	0x12, 0x00, 0x00, 0x00                                                                          // options max_results
};

std::vector<unsigned char> example_B_response = {
	// Crypt challenge
	0xe6,                                                                                           // length
	0xe2, 0x7f, 0x2f, 0x6e, 0x42, 0x18, 0x35, 0x59, 0x19, 0x1b,                                     // data
	
	// Server challenge
	0xf3,                                                                                           // length
	0x7b, 0x7a, 0xbd, 0x2d, 0x2e, 0xb3, 0xa5, 0x7e, 0x9f, 0x91, 0x77, 0x74, 0xc0, 0xcd, 0x08, 0xf5, // data 1-16
	0x86, 0x60, 0xc0, 0xf5, 0x4c, 0xa2, 0x74, 0x7b, 0x10,                                           // data 17-25
	
	// Encrypted data
	0x46, 0xb5, 0xa5, 0x93, 0xf3, 0xaa, 0x48, 0xc4, 0x5e, 0xdb, 0x1a, 0x64, 0xf4, 0xee, 0x6f, 0x43,
	0xa7, 0x44, 0x73, 0x4b, 0x98, 0xa7, 0xe6, 0x96, 0xfb, 0x2f, 0xe4, 0xe2, 0xbd, 0xe6, 0x71, 0xae,
	0x7b, 0x76, 0xee, 0x99, 0x36, 0xef, 0x3b, 0x62, 0xfd, 0xd6, 0xbb, 0x55, 0x88, 0xb4, 0x43, 0xe1,
	0xd1, 0x85, 0x30, 0x2d, 0x45, 0xef, 0x08, 0x8f, 0x79, 0x6a, 0x78, 0xaf, 0x37, 0xe4, 0x79, 0x8d,
	0xa8, 0x1d, 0x1b, 0xa5, 0x5e, 0x9a, 0x03, 0xa2, 0x9a, 0xb3, 0xa2, 0x0d, 0xf8, 0xc9, 0xe3, 0xf1,
	0xce, 0xb3, 0x1d, 0x93, 0x6d, 0xc9, 0xf3, 0x25, 0x99, 0xb8, 0x77, 0xfb, 0xd4, 0x6b, 0x7b, 0x2c,
	0x3c, 0xa3, 0xe0, 0x19, 0x8b, 0x6f, 0x2a, 0x79, 0xa7, 0x54, 0xc0, 0x9f, 0xa9, 0x7e, 0xea, 0x90,
	0xe1, 0xd1, 0x22, 0x5c, 0x8f, 0x8a, 0x87, 0x76, 0x5c, 0x21, 0x01, 0x5c, 0x35, 0x0f, 0x20, 0xe4,
	0x8b, 0x7f, 0xfc, 0x0f, 0xec, 0x32, 0x5a, 0x2c, 0xbd, 0x61, 0xdd, 0x11, 0xc6, 0x75, 0x21, 0x62,
	0x89, 0x05, 0x6c, 0x0a, 0xc2, 0xf3, 0xfd, 0xfe, 0x73, 0xea, 0x41, 0x44, 0xb6, 0xb6, 0x1e, 0x2d,
	0x15, 0x24, 0xf4, 0x64, 0x9c, 0x41, 0x06, 0x3f, 0xa8, 0xf8, 0x49, 0x3f, 0x63, 0x2f, 0xe0, 0x74,
	0x31, 0xe8, 0x43, 0xa0, 0x23, 0x3d, 0xae, 0x4d, 0x5e, 0x95, 0xcf, 0xcd, 0x83, 0x3d, 0x25, 0xbd,
	0x22, 0xaf, 0x72, 0x39, 0xc3, 0xea, 0x94, 0x72
};

std::vector<unsigned char> example_C_request = {
	0x01, 0x5e,                                                                                    // Total Bytes to read
	0x00,                                                                                          // SERVER_LIST_REQUEST
	0x01,                                                                                          // protocol_version
	0x03,                                                                                          // encoding_version
	0x00, 0x00, 0x00, 0x00,                                                                        // game_version
	0x62, 0x66, 0x69, 0x65, 0x6c, 0x64, 0x31, 0x39, 0x34, 0x32, 0x70, 0x73, 0x32, 0x00,            // for_gamename
	0x62, 0x66, 0x69, 0x65, 0x6c, 0x64, 0x31, 0x39, 0x34, 0x32, 0x70, 0x73, 0x32, 0x00,            // from_gamename
	0x34, 0x68, 0x6b, 0x54, 0x61, 0x5c, 0x2d, 0x6d,                                                // client challenge
	
	// filter: gamever='V1.31a' and (teamplay=0 and numplayers<=23 and numplayers>=0 and numplayers!=maxplayers and nr<=2 and xr>=2)and (region & 1)!=0
	0x67, 0x61, 0x6d, 0x65, 0x76, 0x65, 0x72, 0x3d, 0x27, 0x56, 0x31, 0x2e, 0x33, 0x31, 0x61, 0x27,
	0x20, 0x61, 0x6e, 0x64, 0x20, 0x28, 0x74, 0x65, 0x61, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x3d, 0x30,
	0x20, 0x61, 0x6e, 0x64, 0x20, 0x6e, 0x75, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73, 0x3c,
	0x3d, 0x32, 0x33, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x6e, 0x75, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x65,
	0x72, 0x73, 0x3e, 0x3d, 0x30, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x6e, 0x75, 0x6d, 0x70, 0x6c, 0x61,
	0x79, 0x65, 0x72, 0x73, 0x21, 0x3d, 0x6d, 0x61, 0x78, 0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73,
	0x20, 0x61, 0x6e, 0x64, 0x20, 0x6e, 0x72, 0x3c, 0x3d, 0x32, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x78,
	0x72, 0x3e, 0x3d, 0x32, 0x29, 0x61, 0x6e, 0x64, 0x20, 0x28, 0x72, 0x65, 0x67, 0x69, 0x6f, 0x6e,
	0x20, 0x26, 0x20, 0x31, 0x29, 0x21, 0x3d, 0x30, 0x00,
	
	// key_list: \hostname\gametype\gamever\hostport\timelimit\timeelapsed\mapname\numplayers\maxplayers\
	             teamplay\team_t\ping_\c0\c1\n0\n1\c0c\c1c\nr\xr\ff\mc\gc\rc\sr\ni\xi\qm
	0x5c, 0x68, 0x6f, 0x73, 0x74, 0x6e, 0x61, 0x6d, 0x65, 0x5c, 0x67, 0x61, 0x6d, 0x65, 0x74, 0x79,
	0x70, 0x65, 0x5c, 0x67, 0x61, 0x6d, 0x65, 0x76, 0x65, 0x72, 0x5c, 0x68, 0x6f, 0x73, 0x74, 0x70,
	0x6f, 0x72, 0x74, 0x5c, 0x74, 0x69, 0x6d, 0x65, 0x6c, 0x69, 0x6d, 0x69, 0x74, 0x5c, 0x74, 0x69,
	0x6d, 0x65, 0x65, 0x6c, 0x61, 0x70, 0x73, 0x65, 0x64, 0x5c, 0x6d, 0x61, 0x70, 0x6e, 0x61, 0x6d,
	0x65, 0x5c, 0x6e, 0x75, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73, 0x5c, 0x6d, 0x61, 0x78,
	0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73, 0x5c, 0x74, 0x65, 0x61, 0x6d, 0x70, 0x6c, 0x61, 0x79,
	0x5c, 0x74, 0x65, 0x61, 0x6d, 0x5f, 0x74, 0x5c, 0x70, 0x69, 0x6e, 0x67, 0x5f, 0x5c, 0x63, 0x30,
	0x5c, 0x63, 0x31, 0x5c, 0x6e, 0x30, 0x5c, 0x6e, 0x31, 0x5c, 0x63, 0x30, 0x63, 0x5c, 0x63, 0x31,
	0x63, 0x5c, 0x6e, 0x72, 0x5c, 0x78, 0x72, 0x5c, 0x66, 0x66, 0x5c, 0x6d, 0x63, 0x5c, 0x67, 0x63,
	0x5c, 0x72, 0x63, 0x5c, 0x73, 0x72, 0x5c, 0x6e, 0x69, 0x5c, 0x78, 0x69, 0x5c, 0x71, 0x6d, 0x00,
	
	0x00, 0x00, 0x00, 0x80,                                                                         // options
	0x14, 0x00, 0x00, 0x00                                                                          // options max_results
};

std::vector<unsigned char> example_C_response = {
	// Crypt challenge
	0xe6,                                                                                           // length
	0x48, 0x3c, 0xdb, 0xb6, 0x0b, 0x68, 0x4f, 0x45, 0xb1, 0xcd,                                     // data
	
	// Server challenge
	0xf3,                                                                                           // length
	0x8e, 0x71, 0x43, 0xfa, 0xdb, 0x0c, 0x42, 0xef, 0xaf, 0x68, 0xfb, 0x64, 0x4b, 0x53, 0xac, 0x69, // data 1-16
	0xbf, 0x74, 0xe8, 0x4d, 0x6f, 0x30, 0x8a, 0x4a, 0xe6,                                           // data 17-25
	
	// Encrypted data
	0x92, 0xbd, 0x4c, 0xaf, 0x27, 0x6d, 0x87, 0x87, 0x3a, 0xfa, 0x44, 0x67, 0x16, 0xbc, 0x8e, 0x2e,
	0xe0, 0x9f, 0xfb, 0x0d, 0xdf, 0x37, 0x5b, 0xe4, 0x8a, 0xc4, 0x06, 0x21, 0x7d, 0x55, 0x73, 0xfe,
	0x5b, 0x22, 0x23, 0xd9, 0x9e, 0xd2, 0xe3, 0xbd, 0xd9, 0xd2, 0xcd, 0x2c, 0x6b, 0xc6, 0x2a, 0xe0,
	0x34, 0x7b, 0xfd, 0x6a, 0xed, 0xad, 0xcf, 0xca, 0x88, 0x5e, 0xa3, 0x9e, 0xef, 0xc4, 0x0f, 0x95,
	0x84, 0xf4, 0xaa, 0xd7, 0x04, 0x03, 0xfd, 0xfd, 0x0e, 0xc9, 0xd3, 0xeb, 0x78, 0x0c, 0x39, 0xc7,
	0xbb, 0xe8, 0xa3, 0x8a, 0x49, 0x31, 0xb1, 0xe1, 0xde, 0x74, 0x40, 0x48, 0x37, 0x45, 0x1a, 0x4f,
	0xb3, 0x3a, 0xc8, 0x2c, 0x5d, 0x22, 0xd3, 0x59, 0xb1, 0x40, 0x54, 0x21, 0xdd, 0x20, 0xa4, 0x89,
	0x26, 0xfc, 0x2c, 0x45, 0x12, 0x7e, 0x0e, 0x67, 0xc9, 0x53, 0xb2, 0x05, 0xa6, 0xbc, 0x74, 0xba,
	0x63, 0x21, 0x82, 0x49, 0x68, 0xd7, 0x26, 0x39, 0xc3, 0xdc, 0x56, 0xee, 0x36, 0x24, 0xd6, 0x93,
	0x83, 0xe6, 0x8a, 0x84, 0x53, 0x00, 0x80, 0xb9, 0x5c, 0x19, 0xf4, 0xa6, 0x54, 0xc3, 0xc0, 0xa4,
	0x1a, 0x0b, 0x03, 0x32, 0xb2, 0x5f, 0x6f, 0x83, 0xef, 0x38, 0xed, 0x5a, 0x39, 0xa2, 0xbe, 0x0d,
	0x8b, 0xa8, 0x14, 0xaf, 0xaa, 0x8f, 0x3a, 0x37, 0xc5, 0xab, 0x0a, 0x01, 0x8a, 0x67, 0xc2, 0x62,
	0xac, 0x53, 0x19, 0xd1, 0x51, 0xa2, 0x81, 0x05, 0xc3, 0x6a, 0xde, 0x17, 0xb3, 0x4a, 0x5d, 0xc2,
	0xb4, 0xe6, 0x91, 0x02, 0x4a, 0x0b, 0x75, 0x70, 0x86, 0xe1, 0xe8, 0x93, 0x3f, 0x7c, 0x5a, 0xc8,
	0x55, 0xd7, 0x04, 0x7a, 0x7a, 0x46, 0x4f, 0x5d, 0x88, 0x13
};

std::vector<unsigned char> example_D_request = {
	0x01, 0x5d,                                                                                    // Total Bytes to read
	0x00,                                                                                          // SERVER_LIST_REQUEST
	0x01,                                                                                          // protocol_version
	0x03,                                                                                          // encoding_version
	0x00, 0x00, 0x00, 0x00,                                                                        // game_version
	0x62, 0x66, 0x69, 0x65, 0x6c, 0x64, 0x31, 0x39, 0x34, 0x32, 0x70, 0x73, 0x32, 0x00,            // for_gamename
	0x62, 0x66, 0x69, 0x65, 0x6c, 0x64, 0x31, 0x39, 0x34, 0x32, 0x70, 0x73, 0x32, 0x00,            // from_gamename
	0x25, 0x70, 0x24, 0x56, 0x7c, 0x29, 0x3e, 0x5d,                                                // client challenge
	
	// filter: gamever='V1.31a' and (teamplay=0 and numplayers<=23 and numplayers>=0 and numplayers!=maxplayers and nr<=2 and xr>=2)and (region & 1)=0
	0x67, 0x61, 0x6d, 0x65, 0x76, 0x65, 0x72, 0x3d, 0x27, 0x56, 0x31, 0x2e, 0x33, 0x31, 0x61, 0x27,
	0x20, 0x61, 0x6e, 0x64, 0x20, 0x28, 0x74, 0x65, 0x61, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x3d, 0x30,
	0x20, 0x61, 0x6e, 0x64, 0x20, 0x6e, 0x75, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73, 0x3c,
	0x3d, 0x32, 0x33, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x6e, 0x75, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x65,
	0x72, 0x73, 0x3e, 0x3d, 0x30, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x6e, 0x75, 0x6d, 0x70, 0x6c, 0x61,
	0x79, 0x65, 0x72, 0x73, 0x21, 0x3d, 0x6d, 0x61, 0x78, 0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73,
	0x20, 0x61, 0x6e, 0x64, 0x20, 0x6e, 0x72, 0x3c, 0x3d, 0x32, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x78,
	0x72, 0x3e, 0x3d, 0x32, 0x29, 0x61, 0x6e, 0x64, 0x20, 0x28, 0x72, 0x65, 0x67, 0x69, 0x6f, 0x6e,
	0x20, 0x26, 0x20, 0x31, 0x29, 0x3d, 0x30, 0x00,
	
	// key_list
	0x5c, 0x68, 0x6f, 0x73, 0x74, 0x6e, 0x61, 0x6d, 0x65, 0x5c, 0x67, 0x61, 0x6d, 0x65, 0x74, 0x79,
	0x70, 0x65, 0x5c, 0x67, 0x61, 0x6d, 0x65, 0x76, 0x65, 0x72, 0x5c, 0x68, 0x6f, 0x73, 0x74, 0x70,
	0x6f, 0x72, 0x74, 0x5c, 0x74, 0x69, 0x6d, 0x65, 0x6c, 0x69, 0x6d, 0x69, 0x74, 0x5c, 0x74, 0x69,
	0x6d, 0x65, 0x65, 0x6c, 0x61, 0x70, 0x73, 0x65, 0x64, 0x5c, 0x6d, 0x61, 0x70, 0x6e, 0x61, 0x6d,
	0x65, 0x5c, 0x6e, 0x75, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73, 0x5c, 0x6d, 0x61, 0x78,
	0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73, 0x5c, 0x74, 0x65, 0x61, 0x6d, 0x70, 0x6c, 0x61, 0x79,
	0x5c, 0x74, 0x65, 0x61, 0x6d, 0x5f, 0x74, 0x5c, 0x70, 0x69, 0x6e, 0x67, 0x5f, 0x5c, 0x63, 0x30,
	0x5c, 0x63, 0x31, 0x5c, 0x6e, 0x30, 0x5c, 0x6e, 0x31, 0x5c, 0x63, 0x30, 0x63, 0x5c, 0x63, 0x31,
	0x63, 0x5c, 0x6e, 0x72, 0x5c, 0x78, 0x72, 0x5c, 0x66, 0x66, 0x5c, 0x6d, 0x63, 0x5c, 0x67, 0x63,
	0x5c, 0x72, 0x63, 0x5c, 0x73, 0x72, 0x5c, 0x6e, 0x69, 0x5c, 0x78, 0x69, 0x5c, 0x71, 0x6d, 0x00,
	
	0x00, 0x00, 0x00, 0x80,                                                                         // options
	0x12, 0x00, 0x00, 0x00                                                                          // options max_results
};

std::vector<unsigned char> example_D_response = {
	// Crypt challenge
	0xe6,                                                                                           // length
	0x48, 0x3c, 0xdb, 0xb6, 0x0b, 0x68, 0x4f, 0x45, 0xb1, 0xcd,                                     // data
	
	// Server challenge
	0xf3,                                                                                           // length
	0x8e, 0x71, 0x43, 0xfa, 0xdb, 0x0c, 0x42, 0xef, 0xaf, 0x68, 0xfb, 0x64, 0x4b, 0x53, 0xac, 0x69, // data 1-16
	0xbf, 0x74, 0xe8, 0x4d, 0x6f, 0x30, 0x8a, 0x4a, 0xe6,                                           // data 17-25
	
	// Encrypted data
	0xa6, 0x4e, 0x0e, 0xe7, 0xbc, 0xdf, 0xa6, 0xc1, 0x47, 0x28, 0x28, 0x44, 0xeb, 0x9e, 0xac, 0x94,
	0x71, 0x36, 0xcf, 0xed, 0x49, 0x98, 0x2d, 0xa9, 0x44, 0xbf, 0xe6, 0xc2, 0x6d, 0xa9, 0x15, 0x00,
	0x83, 0xa6, 0x30, 0xe6, 0x63, 0x9a, 0xcb, 0x85, 0x91, 0xad, 0xfe, 0x22, 0x1b, 0xac, 0x27, 0xe4,
	0xee, 0x4a, 0xe6, 0x31, 0x03, 0x18, 0x30, 0x31, 0x2b, 0xb5, 0xca, 0x60, 0xd7, 0xc6, 0x64, 0x25,
	0x0c, 0x20, 0x08, 0x47, 0x95, 0x50, 0x98, 0xb5, 0xd3, 0xa8, 0x6c, 0x4d, 0x1e, 0x27, 0x43, 0xc0,
	0x8e, 0xf3, 0x9d, 0xfd, 0x83, 0xb6, 0x4d, 0x4c, 0x6b, 0x39, 0x72, 0x0d, 0x1a, 0x97, 0x55, 0x4c,
	0x7f, 0xe5, 0xf4, 0x32, 0x82, 0xd4, 0x29, 0xe1, 0x29, 0x4a, 0x4a, 0x68, 0x28, 0x62, 0xe6, 0xbe,
	0xed, 0xfa, 0xd2, 0x63, 0xc1, 0x1d, 0x54, 0xaf, 0xcc, 0x68, 0x11, 0x1e, 0x3e, 0xda, 0xb3, 0x65,
	0x9f, 0x1a, 0x61, 0x99, 0xcd, 0x3d, 0x2f, 0x47, 0xaf, 0xdb, 0x52, 0xb4, 0xa2, 0x62, 0xd5, 0xee,
	0x5b, 0xb1, 0x32, 0xd9, 0xea, 0x82, 0xac, 0x43, 0xab, 0x5c, 0x77, 0xd9, 0xab, 0x4b, 0xf0, 0x07,
	0x58, 0x6f, 0x9d, 0xf3, 0x64, 0xd2, 0x37, 0x8d, 0x2e, 0x72, 0x45, 0x84, 0x89, 0x18, 0x98, 0x9c,
	0x13, 0x7e, 0x1a, 0x89, 0x29, 0x6b, 0x5b, 0x0b, 0x08, 0x0b, 0x7f, 0x11, 0x41, 0x50, 0x1e, 0x55,
	0x6e, 0xa9, 0x13, 0x99, 0x88, 0xcc, 0xf0, 0x76
};

std::vector<unsigned char> example_C_decrypt_data = {
	// Request
	0x56, 0x57, 0x8b, 0xeb,                                                       // Request ip: 86.87.139.235
	0x19, 0x64,                                                                   // Requested: port 6500

	// ?? Header information ??
	0x1c, 0x00,                                                                   // Total header items: 28
	0x68, 0x6f, 0x73, 0x74, 0x6e, 0x61, 0x6d, 0x65, 0x00, 0x00,                   // hostname
	0x67, 0x61, 0x6d, 0x65, 0x74, 0x79, 0x70, 0x65, 0x00, 0x00,                   // gametype
	0x67, 0x61, 0x6d, 0x65, 0x76, 0x65, 0x72, 0x00, 0x00,                         // gamever
	0x68, 0x6f, 0x73, 0x74, 0x70, 0x6f, 0x72, 0x74, 0x00, 0x00,                   // hostport
	0x74, 0x69, 0x6d, 0x65, 0x6c, 0x69, 0x6d, 0x69, 0x74, 0x00, 0x00,             // timelimit
	0x74, 0x69, 0x6d, 0x65, 0x65, 0x6c, 0x61, 0x70, 0x73, 0x65, 0x64, 0x00, 0x00, // timeelapsed
	0x6d, 0x61, 0x70, 0x6e, 0x61, 0x6d, 0x65, 0x00, 0x00,                         // mapname
	0x6e, 0x75, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73, 0x00, 0x00,       // numplayers
	0x6d, 0x61, 0x78, 0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73, 0x00, 0x00,       // maxplayers
	0x74, 0x65, 0x61, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x00, 0x00,                   // teamplay
	0x74, 0x65, 0x61, 0x6d, 0x5f, 0x74, 0x00, 0x00,                               // team_t
	0x70, 0x69, 0x6e, 0x67, 0x5f, 0x00, 0x00,                                     // ping_
	0x63, 0x30, 0x00, 0x00,                                                       // c0
	0x63, 0x31, 0x00, 0x00,                                                       // c1
	0x6e, 0x30, 0x00, 0x00,                                                       // n0
	0x6e, 0x31, 0x00, 0x00,                                                       // n1
	0x63, 0x30, 0x63, 0x00, 0x00,                                                 // c0c
	0x63, 0x31, 0x63, 0x00, 0x00,                                                 // c1c
	0x6e, 0x72, 0x00, 0x00,                                                       // nr
	0x78, 0x72, 0x00, 0x00,                                                       // xr
	0x66, 0x66, 0x00, 0x00,                                                       // ff
	0x6d, 0x63, 0x00, 0x00,                                                       // mc
	0x67, 0x63, 0x00, 0x00,                                                       // gc
	0x72, 0x63, 0x00, 0x00,                                                       // rc
	0x73, 0x72, 0x00, 0x00,                                                       // sr
	0x6e, 0x69, 0x00, 0x00,                                                       // ni
	0x78, 0x69, 0x00, 0x00,                                                       // xi
	0x71, 0x6d, 0x00, 0x00,                                                       // qm

	// New server
	0x3b,                            // flags: UNSOLICITED_UDP_FLAG | PRIVATE_IP_FLAG | ICMP_IP_FLAG | NONSTANDARD_PORT_FLAG | NONSTANDARD_PRIVATE_PORT_FLAG
	0xa8, 0x77, 0xbd, 0x95,          //                                  wan ip:      168.119.189.149
	0x0e, 0x4a,                      // NONSTANDARD_PORT_FLAG         -> wan port:    3658
	0xa8, 0x77, 0xbd, 0x95,          // PRIVATE_IP_FLAG               -> localip0 ip: 168.119.189.149
	0x0e, 0x4a,                      // NONSTANDARD_PRIVATE_PORT_FLAG -> localport:   3658
	0xa8, 0x77, 0xbd, 0x95,          // ICMP_IP_FLAG                  -> icmp ip:     168.119.189.149

	// New server
	0x3b,                            // flags: UNSOLICITED_UDP_FLAG | PRIVATE_IP_FLAG | ICMP_IP_FLAG | NONSTANDARD_PORT_FLAG | NONSTANDARD_PRIVATE_PORT_FLAG
	0x4e, 0x2f, 0xb8, 0x17,          //                                  wan ip:      78.47.184.23
	0x0e, 0x4a,                      // NONSTANDARD_PORT_FLAG         -> wan port:    3658
	0x4e, 0x2f, 0xb8, 0x17,          // PRIVATE_IP_FLAG               -> localip0 ip: 78.47.184.23
	0x0e, 0x4a,                      // NONSTANDARD_PRIVATE_PORT_FLAG -> localport:   3658
	0x4e, 0x2f, 0xb8, 0x17,          // ICMP_IP_FLAG                  -> icmp ip:     78.47.184.23

	// End decrypted data
	0x00, 0xff, 0xff, 0xff, 0xff
};

std::vector<unsigned char> example_D_decrypt_data = {
	// Request
	0x56, 0x57, 0x8b, 0xeb,
	
	// ?? Header information ??
	0x19, 0x64, 0x1c, 0x00,
	0x68, 0x6f, 0x73, 0x74, 0x6e, 0x61, 0x6d, 0x65, 0x00, 0x00,                   // hostname
	0x67, 0x61, 0x6d, 0x65, 0x74, 0x79, 0x70, 0x65, 0x00, 0x00,                   // gametype
	0x67, 0x61, 0x6d, 0x65, 0x76, 0x65, 0x72, 0x00, 0x00,                         // gamever
	0x68, 0x6f, 0x73, 0x74, 0x70, 0x6f, 0x72, 0x74, 0x00, 0x00,                   // hostport
	0x74, 0x69, 0x6d, 0x65, 0x6c, 0x69, 0x6d, 0x69, 0x74, 0x00, 0x00,             // timelimit
	0x74, 0x69, 0x6d, 0x65, 0x65, 0x6c, 0x61, 0x70, 0x73, 0x65, 0x64, 0x00, 0x00, // timeelapsed
	0x6d, 0x61, 0x70, 0x6e, 0x61, 0x6d, 0x65, 0x00, 0x00,                         // mapname
	0x6e, 0x75, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73, 0x00, 0x00,       // numplayers
	0x6d, 0x61, 0x78, 0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73, 0x00, 0x00,       // maxplayers
	0x74, 0x65, 0x61, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x00, 0x00,                   // teamplay
	0x74, 0x65, 0x61, 0x6d, 0x5f, 0x74, 0x00, 0x00,                               // team_t
	0x70, 0x69, 0x6e, 0x67, 0x5f, 0x00, 0x00,                                     // ping_
	0x63, 0x30, 0x00, 0x00,                                                       // c0
	0x63, 0x31, 0x00, 0x00,                                                       // c1
	0x6e, 0x30, 0x00, 0x00,                                                       // n0
	0x6e, 0x31, 0x00, 0x00,                                                       // n1
	0x63, 0x30, 0x63, 0x00, 0x00,                                                 // c0c
	0x63, 0x31, 0x63, 0x00, 0x00,                                                 // c1c
	0x6e, 0x72, 0x00, 0x00,                                                       // nr
	0x78, 0x72, 0x00, 0x00,                                                       // xr
	0x66, 0x66, 0x00, 0x00,                                                       // ff
	0x6d, 0x63, 0x00, 0x00,                                                       // mc
	0x67, 0x63, 0x00, 0x00,                                                       // gc
	0x72, 0x63, 0x00, 0x00,                                                       // rc
	0x73, 0x72, 0x00, 0x00,                                                       // sr
	0x6e, 0x69, 0x00, 0x00,                                                       // ni
	0x78, 0x69, 0x00, 0x00,                                                       // xi
	0x71, 0x6d, 0x00, 0x00,                                                       // qm
	
	// End decrypted data
	0x00, 0xff, 0xff, 0xff, 0xff
};

void Browsing::Client::Test()
{
	std::lock_guard<std::mutex> guard(g_mutex_io); // io lock (read/write)
	
	int response_offset = 0;
	std::stringstream ss;
	
	#define CRYPTCHAL_LEN 10
	#define SERVCHAL_LEN 25
	#define LIST_CHALLENGE_LEN 8
	
	//std::vector<unsigned char> request = example_A_request;
	//std::vector<unsigned char> response = example_A_response;
	// std::vector<unsigned char> request = example_B_request;
	// std::vector<unsigned char> response = example_B_response;
	std::vector<unsigned char> request = example_C_request;
	std::vector<unsigned char> response = example_C_response;
	//std::vector<unsigned char> request = example_D_request;
	//std::vector<unsigned char> response = example_D_response;
	
	uint32_t cryptlen = CRYPTCHAL_LEN;
	uint8_t cryptchal[CRYPTCHAL_LEN];
	uint32_t servchallen = SERVCHAL_LEN;
	uint8_t servchal[SERVCHAL_LEN];
	uint8_t m_challenge[LIST_CHALLENGE_LEN];
	
	// First conclusion:
	// buffer.WriteByte((uint8_t)(cryptlen ^ 0xEC));
	// buffer.WriteBuffer((uint8_t *)&cryptchal, cryptlen);
	std::cout << std::hex << (cryptlen ^ 0xEC) << std::endl; // Returns e6
	response_offset++;
	
	// copy crypt challenge
	ss.str("");
	for(int i = 0; i < CRYPTCHAL_LEN; i++)
	{
		cryptchal[i] = response[response_offset + i];
		ss << std::hex << std::setfill('0') << std::setw(2) << (int)(cryptchal[i]);
	}
	response_offset += CRYPTCHAL_LEN;
	std::cout << "cryptchal = " << ss.str() << std::endl;
	
	// Second conclusion
	// buffer.WriteByte((uint8_t)(servchallen ^ 0xEA));
	// buffer.WriteBuffer((uint8_t *)&servchal, servchallen);
	std::cout << std::hex << (servchallen ^ 0xEA) << std::endl; // Returns f3
	response_offset++;
	
	// copy crypt challenge
	ss.str("");
	for(int i = 0; i < SERVCHAL_LEN; i++)
	{
		servchal[i] = response[response_offset + i];
		ss << std::hex << std::setfill('0') << std::setw(2) << (int)(servchal[i]);
	}
	response_offset += SERVCHAL_LEN;
	std::cout << "servchal = " << ss.str() << std::endl;
	
	// Third: Find secret key
	// Found online:
	// https://github.com/devzspy/GameSpy-Openspy-Core/blob/master/Gamemaster.sql
	// size_t seckeylen = m_game.secretkey.length();
    // const char *seckey = m_game.secretkey.c_str();
	size_t seckeylen = 6;
	const char *seckey = "HpWx9z";
	
	// Get the challenge from the clients request
	ss.str("");
	for(int i = 0; i < LIST_CHALLENGE_LEN; i++)
	{
		m_challenge[i] = request[37 + i];
		ss << std::hex << std::setfill('0') << std::setw(2) << (int)(m_challenge[i]);
	}
	std::cout << "m_challenge = " << ss.str() << std::endl;
	
	// Update challenge
	for (uint32_t i = 0 ; i < servchallen ; i++)
	{
		m_challenge[(i *  seckey[i % seckeylen]) % LIST_CHALLENGE_LEN] ^= (char)((m_challenge[i % LIST_CHALLENGE_LEN] ^ servchal[i]) & 0xFF);
	}
	
	GOACryptState m_crypt_state;
	int encrypted_data_position = 1 + CRYPTCHAL_LEN + 1 + SERVCHAL_LEN;
	
	ss.str("");
	for(int i = encrypted_data_position; i < response.size(); i++)
	{
		ss << std::hex << std::setfill('0') << std::setw(2) << (int)(response[i]);
	}
	std::cout << "encrypted data = " << ss.str() << std::endl;
	
	// Initialization GOA crypt
	GOACryptInit(&(m_crypt_state), (unsigned char *)(&m_challenge), LIST_CHALLENGE_LEN);	
	
	// Decrypt data
	GOADecrypt(&(m_crypt_state), (&response[encrypted_data_position]), response.size() - encrypted_data_position);
	
	ss.str("");
	for(int i = encrypted_data_position; i < response.size(); i++)
	{
		ss << std::hex << std::setfill('0') << std::setw(2) << (int)(response[i]);
	}
	std::cout << "decrypted data = " << ss.str() << std::endl;
	
	/*
	// Encrypt the data again to confirm its the right encrypted data
	GOACryptState m_crypt_state2;
	GOACryptInit(&(m_crypt_state2), (unsigned char *)(&m_challenge), LIST_CHALLENGE_LEN);	
	GOAEncrypt(&(m_crypt_state2), (&response[encrypted_data_position]), response.size() - encrypted_data_position);
	
	ss.str("");
	for(int i = encrypted_data_position; i < response.size(); i++)
	{
		ss << std::hex << (int)(response[i]);
	}
	std::cout << "encrypt data = " << ss.str() << std::endl;
	*/
}

