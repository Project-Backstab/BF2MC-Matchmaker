#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <regex>
#include <thread>

#include <settings.h>
#include <logger.h>
#include <server.h>
#include <globals.h>
#include <util.h>
#include <database.h>
#include <browsing/sb_crypt.h>
#include <browsing/constants.h>

#include <browsing/client.h>

typedef void (Browsing::Client::*RequestActionFunc)(const std::vector<unsigned char>&);

static std::map<uint8_t, RequestActionFunc> mRequestActions = 
{
	{ REQUEST_SERVER_LIST,    &Browsing::Client::requestServerList   },
	{ REQUEST_SERVER_INFO,    &Browsing::Client::requestServerInfo   },
	//{ REQUEST_SEND_MESSAGE,   &Browsing::Client::requestSendMessage  },
	//{ REQUEST_KEEPALIVE,      &Browsing::Client::requestKeepAlive    },
	//{ REQUEST_MAPLOOP,        &Browsing::Client::requestMapLoop      },
	//{ REQUEST_PLAYERSEARCH,   &Browsing::Client::requestPlayerSearch },
};

Browsing::Client::Client(int socket, struct sockaddr_in address)
{
	this->_socket = socket;
	this->_address = address;
	this->UpdateLastRecievedTime();
}

Browsing::Client::~Client()
{
	this->Disconnect();
}

void Browsing::Client::Listen()
{
	while(true)
	{
		std::vector<unsigned char> buffer(4096, 0);
		
		int recv_size = read(this->_socket, &(buffer[0]), 4096);
		
		// If error or no data is recieved we end the connection
		if(recv_size <= 0)
		{
			break;
		}
		
		// Resize buffer
		buffer.resize(recv_size);
		
		this->UpdateLastRecievedTime();

		this->_LogTransaction("-->", Util::Buffer::ToString(buffer));
		
		// Extract requests
		std::vector<std::vector<unsigned char>> requests;
		this->_BufferToRequests(buffer, requests);
		
		// Iterate through requests
		for(const std::vector<unsigned char>& request : requests)
		{
			this->onRequest(request);
		}
		
		// Debug
		//std::stringstream ss;
		//ss.str("");
		//for(int i = 0; i < buffer.size(); i++)
		//{
		//	ss << std::hex << std::setfill('0') << std::setw(2) << (int)(buffer[i]);
		//}
		//Logger::debug("buffer = " + ss.str());
	}
	
	this->Disconnect();
}

void Browsing::Client::Disconnect()
{
	this->Close();
	g_browsing_server->onClientDisconnect(*this);
}

// Events

void Browsing::Client::onRequest(const std::vector<unsigned char>& request)
{
	uint16_t size = static_cast<uint16_t>(request[1]) | (static_cast<uint16_t>(request[0]) << 8);
	uint8_t action = static_cast<uint8_t>(request[2]);
	
	// Find function
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
		Logger::warning("action \"" + std::to_string(action) + "\"not implemented!", Server::Type::Browsing);
		
		this->Disconnect();
	}
}

std::vector<unsigned char> serverListHeaderItems = {
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
};

/*
	filter:
		gamever='V1.31a' and (teamplay=0 and numplayers<=23 and numplayers>=0 and numplayers!=maxplayers and nr<=20 and xr>=20)and (region & 1)=0
		
		Min/Max Players:
			Max:				numplayers<=22
			Min:				numplayers>=1
		Game Mode:
			Conquest:			gc=0 and gametype='conquest'
			CTF:				gc=0 and gametype='capturetheflag'
		Maps: 
			Backstab:			mapname=0 and mc=1
			DeadlyPass:			mapname=1 and mc=1
			BridgeToFar:		mapname=2 and mc=1
			Dammage:			mapname=3 and mc=1
			HarborEdge:			mapname=4 and mc=1
			Honor:				mapname=5 and mc=1
			LittleBigEye:		mapname=6 and mc=1
			MissleCrisis		mapname=7 and mc=1
			ColdFront			mapname=8 and mc=1
			RussianBorder:		mapname=9 and mc=1
			SpecialOp:			mapname=10 and mc=1
			TheBlackGold		mapname=11 and mc=1
			TheNest				mapname=12 and mc=1
			[All Maps Playlist]	mc=2
			[Assualt Maps]		mc=3
			[Incursion Maps]	mc=4
			[Domination Maps]	mc=5
		Ranked:
			On:					(nr>1 or xr<20)
			Off:				nr=1 and xr=20
		Stats Tracking:
			On:					sr=1
			Off:				sr=0
		
		Clan fight:
			gamever='V1.31a' and (teamplay!=0 and (c0=-1 or c1=-1 or c0=19 or c1=19) and maxplayers<=24 and numplayers!=maxplayers and nr<=20 and xr>=20)and (region & 1)=0
			
			gamever='V1.31a' and (teamplay!=0 and (c0=-1 or c1=-1 or c0=21 or c1=21) and maxplayers<=24 and numplayers!=maxplayers and nr<=1 and xr>=1 and sr=1)and (region & 1)=0	
*/

void Browsing::Client::requestServerList(const std::vector<unsigned char>& request)
{	
	if(request.size() < 50)
	{
		return;
	}
	
	size_t offset = 9;
	
	// Read for_gamename and from_gamename
	std::string for_gamename, from_gamename;
	if(!Util::Buffer::ReadString(request, offset, for_gamename) ||
		!Util::Buffer::ReadString(request, offset, from_gamename))
	{
		return;
	}
	
	// Copy client_challenge
	this->_client_challenge.assign(request.begin() + offset, request.begin() + offset + CHALLENGE_CLIENT_LEN);
	offset += CHALLENGE_CLIENT_LEN;
	
	// Read filter and key_list
	std::string filter, key_list;
	if(!Util::Buffer::ReadString(request, offset, filter) ||
		!Util::Buffer::ReadString(request, offset, key_list))
	{
		return;
	}
	
	// Debug
	//Logger::debug("for_gamename = " + for_gamename);
	//Logger::debug("from_gamename = " + from_gamename);
	//Logger::debug("filter = " + filter);
	//Logger::debug("key_list = " + key_list);
	
	if(filter == "" or key_list == "")
	{
		return;
	}

	Logger::info(this->GetAddress() + " --> ServerList: " + filter, Server::Type::Browsing);
	
	std::vector<unsigned char> response(CHALLENGE_HEADER_LEN, 0x0);
	
	// Copy client information
	this->_insertClientInfo(response);
	
	// Copy over the header items
	response.insert(response.end(), serverListHeaderItems.begin(), serverListHeaderItems.end());
	
	Battlefield::GameServers game_servers;
	g_database->queryGameServers(game_servers);
	
	// Filter Game servers
	this->_FilterServers(filter, game_servers);
	
	for(Battlefield::GameServer game_server : game_servers)
	{
		//game_server.useExample();
		//game_server.Debug();
		
		if(!game_server.isVerified())
			continue;
		
		this->_insertGameServerFlagIpPort(response, game_server);
	}

	// End data
	response.push_back(0x00); // Empty flag means end of servers
	
	// Add to data to say confirm decryption went correctly
	response.push_back(0xFF);
	response.push_back(0xFF);
	response.push_back(0xFF);
	response.push_back(0xFF);
	
	this->_Encrypt(response);
	
	this->Send(response);
	
	this->_LogTransaction("<--", Util::Buffer::ToString(response));
}

std::vector<unsigned char> ServerInfo_test_response = {
	//0x05, 0xa8,                                                                   // Total Bytes to read
	//0x02,                                                                         // ???
	
	// Server information
	//0xbb,                    // flags: FLAG_UNSOLICITED_UDP | FLAG_PRIVATE_IP | FLAG_ICMP_IP |
	                         //        FLAG_NONSTANDARD_PORT | FLAG_NONSTANDARD_PRIVATE_PORT |
	                         //        FLAG_HAS_FULL_RULES
	//0x56, 0x57, 0x8B, 0xEB,  //                                  wan ip:      78.47.184.23
	//0x0e, 0x4a,              // FLAG_NONSTANDARD_PORT         -> wan port:    3658
	//0x56, 0x57, 0x8B, 0xEB,  // FLAG_PRIVATE_IP               -> localip0 ip: 78.47.184.23
	//0x0e, 0x4a,              // FLAG_NONSTANDARD_PRIVATE_PORT -> localport:   3658
	//0x56, 0x57, 0x8B, 0xEB,  // FLAG_ICMP_IP                  -> icmp ip:     78.47.184.23
	
	// backend_id = bfield1942ps2:1508534:
	0x62, 0x61, 0x63, 0x6b, 0x65, 0x6e, 0x64, 0x5f, 0x69, 0x64, 0x00,
	0x62, 0x66, 0x69, 0x65, 0x6c, 0x64, 0x31, 0x39, 0x34, 0x32, 0x70, 0x73, 0x32, 0x3a,	0x31, 0x35,
	0x30, 0x38, 0x35, 0x33, 0x34, 0x3a, 0x00,
	
	// cl = 100.216335.alpha
	0x63, 0x6c, 0x00,
	0x31, 0x30, 0x30, 0x2e, 0x32, 0x31, 0x36, 0x33, 0x33, 0x35, 0x2e, 0x61, 0x6c, 0x70, 0x68, 0x61,
	0x00,
	
	// country = DE
	0x63, 0x6f, 0x75, 0x6e, 0x74, 0x72, 0x79, 0x00,
	0x44, 0x45, 0x00,
	
	// ff = 1
	0x66, 0x66, 0x00,
	0x31, 0x00,
	
	// fraglimit = 0
	0x66, 0x72, 0x61, 0x67, 0x6c, 0x69, 0x6d, 0x69, 0x74, 0x00, 
	0x30, 0x00,
	
	// gamemode = openplaying
	0x67, 0x61, 0x6d, 0x65, 0x6d, 0x6f, 0x64, 0x65, 0x00,
	0x6f, 0x70, 0x65, 0x6e, 0x70, 0x6c, 0x61, 0x79, 0x69, 0x6e, 0x67, 0x00,
	
	// gamename = bfield1942ps2
	0x67, 0x61, 0x6d, 0x65, 0x6e, 0x61, 0x6d, 0x65, 0x00,
	0x62, 0x66, 0x69, 0x65, 0x6c, 0x64, 0x31, 0x39, 0x34, 0x32, 0x70, 0x73, 0x32, 0x00,
	
	// gametype = conquest
	0x67, 0x61, 0x6d, 0x65, 0x74, 0x79, 0x70, 0x65, 0x00,
	0x63, 0x6f, 0x6e, 0x71, 0x75, 0x65, 0x73, 0x74, 0x00,
	
	// gamevariant = Border
	0x67, 0x61, 0x6d, 0x65, 0x76, 0x61, 0x72, 0x69, 0x61, 0x6e, 0x74, 0x00,
	0x42, 0x6f, 0x72, 0x64, 0x65, 0x72, 0x00,
	
	// gamever = V1.31a
	0x67, 0x61, 0x6d, 0x65, 0x76, 0x65, 0x72, 0x00,
	0x56, 0x31, 0x2e, 0x33, 0x31, 0x61, 0x00,
	
	// gc = 0
	0x67, 0x63, 0x00,
	0x30, 0x00,
	
	// hostname = [CQ]BF2MC-SERVER1 
	//0x68, 0x6f, 0x73, 0x74, 0x6e, 0x61, 0x6d, 0x65, 0x00,
	//0x5b, 0x43, 0x51, 0x5d, 0x42, 0x46, 0x32, 0x4d, 0x43, 0x2d, 0x53, 0x45, 0x52, 0x56, 0x45, 0x52,
	//0x31, 0x00,
	
	// hostname = [CQ]BF2MC-IamLupo
	0x68, 0x6f, 0x73, 0x74, 0x6e, 0x61, 0x6d, 0x65, 0x00,
	0x5B, 0x43, 0x51, 0x5D, 0x42, 0x46, 0x32, 0x4D, 0x43, 0x2D, 0x49, 0x61, 0x6D, 0x4C, 0x75, 0x70,
	0x6F, 0x00,
	
	// hostport = 3658 
	0x68, 0x6f, 0x73, 0x74, 0x70, 0x6f, 0x72, 0x74, 0x00,
	0x33, 0x36, 0x35, 0x38, 0x00,
	
	// localip0 = 78.47.184.23 
	//0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x69, 0x70, 0x30, 0x00,
	//0x37, 0x38, 0x2e, 0x34, 0x37, 0x2e, 0x31, 0x38, 0x34, 0x2e, 0x32, 0x33, 0x00,
	
	// localip0 = 86.87.139.235
	0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x69, 0x70, 0x30, 0x00,
	0x38, 0x36, 0x2E, 0x38, 0x37, 0x2E, 0x31, 0x33, 0x39, 0x2E, 0x32, 0x33, 0x35, 0x00,
	
	// localport = 3658 
	0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x70, 0x6f, 0x72, 0x74, 0x00,
	0x33, 0x36, 0x35, 0x38, 0x00,
	
	// map = backstab 
	0x6d, 0x61, 0x70, 0x00,
	0x62, 0x61, 0x63, 0x6b, 0x73, 0x74, 0x61, 0x62, 0x00,
	
	// mapname = 0 
	0x6d, 0x61, 0x70, 0x6e, 0x61, 0x6d, 0x65, 0x00,
	0x30, 0x00,
	
	// maxplayers = 24 
	0x6d, 0x61, 0x78, 0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73, 0x00,
	0x32, 0x34, 0x00,
	
	// mc = 1 
	0x6d, 0x63, 0x00,
	0x31, 0x00,
	
	// natneg = 1 
	0x6e, 0x61, 0x74, 0x6e, 0x65, 0x67, 0x00,
	0x31, 0x00,
	
	// ni = -2147483648 
	0x6e, 0x69, 0x00,
	0x2d, 0x32, 0x31, 0x34, 0x37, 0x34, 0x38, 0x33, 0x36, 0x34, 0x38, 0x00,
	
	// nr = 1 
	0x6e, 0x72, 0x00,
	0x31, 0x00,
	
	// numplayers = 9 
	0x6e, 0x75, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x65, 0x72, 0x73, 0x00,
	0x39, 0x00,
	
	// numteams = 2 
	0x6e, 0x75, 0x6d, 0x74, 0x65, 0x61, 0x6d, 0x73, 0x00,
	0x32, 0x00,
	
	// password = 0 
	0x70, 0x61, 0x73, 0x73, 0x77, 0x6f, 0x72, 0x64, 0x00,
	0x30, 0x00,
	
	// qm = 0 
	0x71, 0x6d, 0x00,
	0x30, 0x00,
	
	// rc = 0 
	0x72, 0x63, 0x00,
	0x30, 0x00,
	
	// region = 65536 
	0x72, 0x65, 0x67, 0x69, 0x6f, 0x6e, 0x00,
	0x36, 0x35, 0x35, 0x33, 0x36, 0x00,
	
	// rv = retail 
	0x72, 0x76, 0x00,
	0x72, 0x65, 0x74, 0x61, 0x69, 0x6c, 0x00,
	
	// sr = 1
	0x73, 0x72, 0x00,
	0x31, 0x00,
	
	// statechanged = 1 
	0x73, 0x74, 0x61, 0x74, 0x65, 0x63, 0x68, 0x61, 0x6e, 0x67, 0x65, 0x64, 0x00,
	0x31, 0x00,
	
	// teamfraglimit = 0 
	0x74, 0x65, 0x61, 0x6d, 0x66, 0x72, 0x61, 0x67, 0x6c, 0x69, 0x6d, 0x69, 0x74, 0x00,
	0x30, 0x00,
	
	// teamplay = 0 
	0x74, 0x65, 0x61, 0x6d, 0x70, 0x6c, 0x61, 0x79, 0x00,
	0x30, 0x00,
	
	// timeelapsed = 148 
	0x74, 0x69, 0x6d, 0x65, 0x65, 0x6c, 0x61, 0x70, 0x73, 0x65, 0x64, 0x00,
	0x31, 0x34, 0x38, 0x00,
	
	// timelimit = 1200 
	0x74, 0x69, 0x6d, 0x65, 0x6c, 0x69, 0x6d, 0x69, 0x74, 0x00,
	0x31, 0x32, 0x30, 0x30, 0x00,
	
	// xi = 2147483647 
	0x78, 0x69, 0x00,
	0x32, 0x31, 0x34, 0x37, 0x34, 0x38, 0x33, 0x36, 0x34, 0x37, 0x00,
	
	// xr = 20 
	0x78, 0x72, 0x00,
	0x32, 0x30, 0x00,
	
	// score_t0 = 447 
	0x73, 0x63, 0x6f, 0x72, 0x65, 0x5f, 0x74, 0x30, 0x00,
	0x34, 0x34, 0x37, 0x00,
	
	// team_t0 = AC 
	0x74, 0x65, 0x61, 0x6d, 0x5f, 0x74, 0x30, 0x00,
	0x41, 0x43, 0x00,
	
	// score_t1 = 426 
	0x73, 0x63, 0x6f, 0x72, 0x65, 0x5f, 0x74, 0x31, 0x00,
	0x34, 0x32, 0x36, 0x00,
	
	// team_t1 = US 
	0x74, 0x65, 0x61, 0x6d, 0x5f, 0x74, 0x31, 0x00,
	0x55, 0x53, 0x00,
	
	0x00
};

void Browsing::Client::requestServerInfo(const std::vector<unsigned char>& request)
{
	if(request.size() < 9)
	{
		return;
	}

	uint8_t ip[4] = { request[3], request[4], request[5], request[6] };
	std::string str_ip = std::to_string(ip[0]) + "." + std::to_string(ip[1]) + "." +
	                     std::to_string(ip[2]) + "." + std::to_string(ip[3]);
	uint16_t port = static_cast<uint16_t>(request[8]) | (static_cast<uint16_t>(request[7]) << 8);
	
	// Debug
	//Logger::debug("ip = " + str_ip);
	//Logger::debug("port = " + std::to_string(port));
	
	Logger::info(this->GetAddress() + " --> ServerInfo: " + str_ip + ":" + std::to_string(port), Server::Type::Browsing);

	Battlefield::GameServer game_server;
	game_server.SetIp(str_ip);
	game_server.SetPort(port);
	
	// Query the database
	g_database->queryGameServerByIpAndPort(game_server);
	
	// No results found
	if(game_server.GetId() == -1)
	{
		return; // Server not found
	}
	
	std::vector<unsigned char> response(CHALLENGE_HEADER_LEN, 0x0);
	
	// Copy client information
	this->_insertClientInfo(response);
	
	// Push unknown byte size. Needs to be patched later on
	response.push_back(0x00);
	response.push_back(0x00);
	
	// Push response code? MAGIC!!!
	response.push_back(0x02);
	
	this->_insertGameServerFlagIpPort(response, game_server);
	
	//response.push_back(0x00);
	
	// Copy over test response
	response.insert(response.end(), ServerInfo_test_response.begin(), ServerInfo_test_response.end());
	
	// Patch bytes to read
	uint16_t response_size = ServerInfo_test_response.size() + 20;
	response[CHALLENGE_HEADER_LEN + 6] = response_size / 256;
	response[CHALLENGE_HEADER_LEN + 7] = response_size % 256;
	
	this->_Encrypt(response);
	
	this->Send(response);
	
	this->_LogTransaction("<--", Util::Buffer::ToString(response));
}

// Private functions

void Browsing::Client::_BufferToRequests(const std::vector<unsigned char>& buffer,
		std::vector<std::vector<unsigned char>>& requests)
{
	uint16_t request_size;
	size_t offset = 0;
	
	while(offset < buffer.size())
	{
		std::vector<unsigned char> request;
		
		request_size = static_cast<uint16_t>(buffer[offset + 1]) | (static_cast<uint16_t>(buffer[offset + 0]) << 8);
		
		if(offset + request_size <= buffer.size())
		{
			request.insert(request.end(), buffer.begin() + offset, buffer.begin() + offset + request_size);
			
			requests.push_back(request);
		}
		
		offset += request_size;
	}
}

void Browsing::Client::_insertClientInfo(std::vector<unsigned char>& response)
{
	uint8_t client_ip[4];
	uint16_t client_port = 6500;
	
	this->GetIpArray(client_ip);
	
	response.insert(response.end(), client_ip, client_ip + 4);
	response.push_back(client_port / 256);
	response.push_back(client_port % 256);
}

void Browsing::Client::_insertGameServerFlagIpPort(std::vector<unsigned char>& response,
		const Battlefield::GameServer& game_server)
{
	uint8_t ip[4];
	uint16_t port;
	uint8_t flag;
	
	// Get values
	game_server.GetIpArray(ip);
	port = game_server.GetPort();
	flag = game_server.GetFlag();
	
	// Server flag
	response.push_back(flag);
	
	// wan ip
	response.insert(response.end(), ip, ip + 4);
	
	if(flag & FLAG_NONSTANDARD_PORT)         // -> wan port
	{
		response.push_back(port / 256);
		response.push_back(port % 256);
	}
	
	if(flag & FLAG_PRIVATE_IP)               // -> localip0 ip
	{              
		response.insert(response.end(), ip, ip + 4);
	}
	
	if(flag & FLAG_NONSTANDARD_PRIVATE_PORT) // -> localport
	{
		response.push_back(port / 256);
		response.push_back(port % 256);
	}
	
	if(flag & FLAG_ICMP_IP)                  // -> icmp ip
	{
		response.insert(response.end(), ip, ip + 4);
	}
}

void Browsing::Client::_Encrypt(std::vector<unsigned char>& response)
{
	uint8_t server_challenge[CHALLENGE_SERVER_LEN];
	uint8_t secret[CHALLENGE_CLIENT_LEN];
	
	GOACryptState m_crypt_state;
	
	// Push Crypt Challenge
	response[0] = 0xe6; // CHALLENGE_CRYPT_LEN ^ 0xec = 0xe6
	for(int i = 0; i < CHALLENGE_CRYPT_LEN; i++)
	{
		response[1 + i] = 0xff; // Emptry crypto challenge
	}
	
	// Push Server challenge
	response[CHALLENGE_CRYPT_LEN + 1] = 0xf3; // CHALLENGE_SERVER_LEN ^ 0xea = 0xf3
	for(int i = 0; i < CHALLENGE_SERVER_LEN; i++)
	{
		server_challenge[i] = 0x0;
		response[CHALLENGE_CRYPT_LEN + 2 + i] = server_challenge[i];
	}
	
	// Copy client challenge to secret
	for(int i = 0; i < CHALLENGE_CLIENT_LEN; i++)
	{
		secret[i] = this->_client_challenge[i];
	}
	
	// Generate secret encryption/decryption key
	for (uint32_t i = 0 ; i < CHALLENGE_SERVER_LEN; i++)
	{
		uint8_t index = (i *  SECRET_KEY[i % SECRET_KEY_LEN]) % CHALLENGE_CLIENT_LEN;
		uint8_t value = (secret[i % CHALLENGE_CLIENT_LEN] ^ server_challenge[i]) & 0xFF;
		
		secret[index] ^= value;
	}
	
	// Encrypt data
	GOACryptInit(&(m_crypt_state), (unsigned char *)(&secret), CHALLENGE_CLIENT_LEN);
	GOAEncrypt(&(m_crypt_state), &response[CHALLENGE_HEADER_LEN], response.size() - CHALLENGE_HEADER_LEN);
}

void Browsing::Client::_LogTransaction(const std::string& direction, const std::string& response) const
{
	std::shared_lock<std::shared_mutex> guard2(g_settings_mutex); // settings lock (read)
	
	if ((g_logger_mode & Logger::Mode::Development) == 0)
	{
		return;
	}

	bool show_console = (g_settings["browsing"]["show_requests"].asBool() && direction == "-->") ||
						(g_settings["browsing"]["show_responses"].asBool() && direction == "<--");
	
	Logger::info(this->GetAddress() + " " + direction + " " + response,
			Server::Type::Browsing, show_console);
}

void Browsing::Client::_FilterServers(const std::string& filter, Battlefield::GameServers& game_servers)
{
	Battlefield::GameServers::const_iterator game_server_it;

	for(game_server_it = game_servers.begin(); game_server_it != game_servers.end(); ++game_server_it)
	{
		// Check if server needs to be removed
		if(
			this->_FilterServerGameVersion(filter, *game_server_it) ||
			this->_FilterServerRegion(filter, *game_server_it) ||
			this->_FilterServerNumPlayers(filter, *game_server_it) ||
			this->_FilterServerGameType(filter, *game_server_it) ||
			this->_FilterServerMapName(filter, *game_server_it) ||
			this->_FilterServerStatsTracking(filter, *game_server_it) ||
			this->_FilterServerReconfigurable(filter, *game_server_it) ||
			this->_FilterServerClan(filter, *game_server_it) ||
			this->_FilterServerTeamplay(filter, *game_server_it)
		)
		{
			// Remove the game server out of the list
			game_servers.erase(game_server_it);
			
			// Go back one item value
			game_server_it--;
		}
	}
}

bool Browsing::Client::_FilterServerGameVersion(const std::string& filter, const Battlefield::GameServer& game_server)
{
	std::regex pattern;
	std::smatch matches;
	
	// Find: gamever='V1.31a'
	pattern = std::regex(R"(gamever='([^']+))");
	if (std::regex_search(filter, matches, pattern) && matches.size() >= 2)
	{
		std::string gamever = matches[1];
		
		if(!(game_server.GetGameVersion() == gamever))
			return true; // Remove server
	}

	return false; // Don't remove server
}

bool Browsing::Client::_FilterServerRegion(const std::string& filter, const Battlefield::GameServer& game_server)
{
	std::regex pattern;
	std::smatch matches;
	
	// Find: (region & 1)!=0
	pattern = std::regex(R"(\(region \& (\d+)\)!=0)");
	if (std::regex_search(filter, matches, pattern) && matches.size() >= 2)
	{
		uint64_t v = std::stoull(matches[1]);
		
		if(!((game_server.GetRegion() & v) != 0))
			return true; // Remove server
	}
	
	// Find: (region & 1)=0
	pattern = std::regex(R"(\(region \& (\d+)\)=0)");
	if (std::regex_search(filter, matches, pattern) && matches.size() >= 2)
	{
		uint64_t v = std::stoull(matches[1]);
		
		if(!((game_server.GetRegion() & v) == 0))
			return true; // Remove server
	}

	return false; // Don't remove server
}

bool Browsing::Client::_FilterServerNumPlayers(const std::string& filter, const Battlefield::GameServer& game_server)
{
	std::regex pattern;
	std::smatch matches;
	
	// Find: numplayers>=0
	pattern = std::regex(R"(numplayers>=(\d+))");
	if (std::regex_search(filter, matches, pattern) && matches.size() >= 2)
	{
		uint8_t numplayers = std::stoul(matches[1]);
		
		if(!(game_server.GetNumPlayers() >= numplayers))
			return true; // Remove server
	}
	
	// Find: numplayers<=23
	pattern = std::regex(R"(numplayers<=(\d+))");
	if (std::regex_search(filter, matches, pattern) && matches.size() >= 2)
	{
		uint8_t numplayers = std::stoul(matches[1]);
		
		if(!(game_server.GetNumPlayers() <= numplayers))
			return true; // Remove server
	}
	
	return false; // Don't remove server
}

bool Browsing::Client::_FilterServerGameType(const std::string& filter, const Battlefield::GameServer& game_server)
{
	std::regex pattern;
	std::smatch matches;
	
	// Find: gametype='conquest'
	pattern = std::regex(R"(gametype='([^']+))");
	if (std::regex_search(filter, matches, pattern) && matches.size() >= 2)
	{
		std::string gametype = matches[1];
		
		if(!(game_server.GetGameType() == gametype))
			return true; // Remove server
	}

	return false; // Don't remove server
}

bool Browsing::Client::_FilterServerMapName(const std::string& filter, const Battlefield::GameServer& game_server)
{
	std::regex pattern;
	std::smatch matches;
	
	// Find: mapname=2
	pattern = std::regex(R"(mapname=(\d+))");
	if (std::regex_search(filter, matches, pattern) && matches.size() >= 2)
	{
		uint8_t mapname = std::stoul(matches[1]);
		
		if(!(game_server.GetMapName() == mapname))
			return true; // Remove server
	}
	
	return false; // Don't remove server
}

bool Browsing::Client::_FilterServerStatsTracking(const std::string& filter, const Battlefield::GameServer& game_server)
{
	std::regex pattern;
	std::smatch matches;
	
	// Find: mapname=2
	pattern = std::regex(R"(sr=(\d+))");
	if (std::regex_search(filter, matches, pattern) && matches.size() >= 2)
	{
		uint8_t sr = std::stoul(matches[1]);
		
		if(!(game_server.GetStatsTracking() == sr))
			return true; // remove server
	}
	
	return false;
}

bool Browsing::Client::_FilterServerReconfigurable(const std::string& filter, const Battlefield::GameServer& game_server)
{
	std::regex pattern;
	std::smatch matches;
	
	// Find: (rc=1 or teamplay!=0)
	pattern = std::regex(R"(rc=(\d+) or teamplay!=(\d+))");
	if (std::regex_search(filter, matches, pattern) && matches.size() >= 2)
	{
		
	}
	else
	{
		// Find: rc=2
		pattern = std::regex(R"(rc=(\d+))");
		if (std::regex_search(filter, matches, pattern) && matches.size() >= 2)
		{
			uint8_t rc = std::stoul(matches[1]);
			
			if(!(game_server.GetReconfigurable() == rc))
				return true; // Remove server
		}
	}
	
	return false; // Don't remove server
}

bool Browsing::Client::_FilterServerTeamplay(const std::string& filter, const Battlefield::GameServer& game_server)
{
	std::regex pattern;
	std::smatch matches;
	
	// Find: (rc=1 or teamplay!=0)
	pattern = std::regex(R"(rc=(\d+) or teamplay!=(\d+))");
	if (std::regex_search(filter, matches, pattern) && matches.size() >= 2)
	{
		uint8_t rc = std::stoul(matches[1]);
		uint8_t teamplay = std::stoul(matches[2]);
		
		if(!(game_server.GetReconfigurable() == rc || game_server.GetTeamplay() != teamplay))
			return true; // Remove server
	}
	else
	{
		// Find: teamplay=0
		pattern = std::regex(R"(teamplay=(\d+))");
		if (std::regex_search(filter, matches, pattern) && matches.size() >= 2)
		{
			uint8_t teamplay = std::stoul(matches[1]);
			
			if(!(game_server.GetTeamplay() == teamplay))
				return true; // Remove server
		}
		
		// Find: teamplay!=0
		pattern = std::regex(R"(teamplay!=(\d+))");
		if (std::regex_search(filter, matches, pattern) && matches.size() >= 2)
		{
			uint8_t teamplay = std::stoul(matches[1]);
			
			if(!(game_server.GetTeamplay() != teamplay))
				return true; // Remove server
		}
	}

	return false; // Don't remove server
}

bool Browsing::Client::_FilterServerClan(const std::string& filter, const Battlefield::GameServer& game_server)
{
	std::regex pattern;
	std::smatch matches;
	
	// Find: (c0=33 or c1=33)
	pattern = std::regex(R"(\(c0=(\d+) or c1=(\d+)\))");
	if (std::regex_search(filter, matches, pattern) && matches.size() >= 2)
	{
		int32_t c0 = std::stoi(matches[1]);
		int32_t c1 = std::stoi(matches[2]);
		
		if(!(game_server.GetClan1Id() == c0 || game_server.GetClan2Id() == c1))
			return true; // Remove server
	}
	
	//Find: (c0=-1 or c1=-1 or c0=33 or c1=33)
	pattern = std::regex(R"(\(c0=-1 or c1=-1 or c0=(\d+) or c1=(\d+)\))");
	if (std::regex_search(filter, matches, pattern) && matches.size() >= 2)
	{
		int32_t c0 = std::stoi(matches[1]);
		int32_t c1 = std::stoi(matches[2]);
		
		if(!(game_server.GetClan1Id() == -1 || game_server.GetClan2Id() == -1 ||
			game_server.GetClan1Id() == c0 || game_server.GetClan2Id() == c1))
			return true; // Remove server
	}
	
	return false; // Don't remove server
}

// Static functions

void Browsing::Client::Heartbeat()
{
	Logger::info("Heartbeat started", Server::Browsing);

	while(true)
	{
		// Sleep for 60 seconds
		std::this_thread::sleep_for (std::chrono::seconds(60));

		// Calculate the target time (1 minute ago)
		auto target_time = std::chrono::system_clock::now() - std::chrono::minutes(1);
		
		// Iterate through all connected clients
		for(std::shared_ptr<Net::Socket> client : g_browsing_server->GetClients())
		{
			// Get the last received time of the client
			std::chrono::system_clock::time_point last_recieved = client.get()->GetLastRecievedTime();

			// Check if the last received time is older than the target time
			if (last_recieved <= target_time)
			{
				// Close the client connection
				client.get()->Close();
			}
		}
	}
}
