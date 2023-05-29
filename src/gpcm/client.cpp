#include <unistd.h>
#include <iostream>
#include <thread>
#include <iomanip>
#include <map>

#include <server.h>
#include <gamespy.h>
#include <globals.h>
#include <database.h>
#include <util.h>

#include <gpcm/client.h>

typedef void (GPCM::Client::*RequestActionFunc)(const GameSpy::Parameter&) const;

static std::map<std::string, RequestActionFunc> mRequestActions = 
{
	{ "login",      &GPCM::Client::requestLogin },       // Working, but need to do extra investigation for client variable response
	{ "inviteto",   &GPCM::Client::requestInviteTo },    // Done
	{ "getprofile", &GPCM::Client::requestGetProfile },  // Done
	{ "status",     &GPCM::Client::requestStatus },      // Working, but need to do investigation in msg value
	{ "bm",         &GPCM::Client::requestBm },          // Done
	{ "addbuddy",   &GPCM::Client::requestAddBuddy },    // Done
	{ "authadd",    &GPCM::Client::requestAuthAdd },     // Done
	{ "revoke",     &GPCM::Client::requestRevoke },      // Done
	{ "delbuddy",   &GPCM::Client::requestDeleteBuddy }, // Done
	{ "logout",     &GPCM::Client::requestLogout },      // Done
};

GPCM::Client::Client(int socket, struct sockaddr_in address)
{
	this->_socket = socket;
	this->_address = address;
}

GPCM::Client::~Client()
{
	this->Disconnect();
}

void GPCM::Client::Listen()
{	
	// Initialize connection send challenge
	this->requestChallenge();
	
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
		
		std::vector<std::string> requests = GameSpy::RequestToRequests(request);
		
		for(std::string v : requests)
		{
			this->_LogTransaction("-->", v);
		
			this->onRequest(v);
		}
	}
	
	this->Disconnect();
}

void GPCM::Client::Disconnect()
{
	close(this->_socket);
	g_gpcm_server->onClientDisconnect(*this);
}

void GPCM::Client::Send(const std::string &msg) const
{
	send(this->_socket, msg.c_str(), msg.size(), 0);
}

/*
	Events
*/
void GPCM::Client::onRequest(const std::string &request)
{
	GameSpy::Parameter parameter = GameSpy::Request2Parameter(request);
	
	// Find function name
	std::string action = parameter[0];
	
	if (mRequestActions.find(action) != mRequestActions.end())
	{
		// Get Function address
		RequestActionFunc func = mRequestActions[action];
	
		// Execute action function with class object.
		(this->*(func))(parameter);
	}
	else
	{
		std::unique_lock<std::mutex> guard(g_mutex_io);
		
		std::cout << "action \"" << action << "\" not implemented!" << std::endl;
		
		guard.unlock();
		
		this->Disconnect();
	}
}

void GPCM::Client::requestChallenge() const
{
	this->_session_challenge = Util::generateRandomChallenge();
	
	std::string response = GameSpy::Parameter2Response({
		"lc",        "1",
		"challenge", this->_session_challenge,
		"id",        "1",
		"final"
	});
	
	this->Send(response);
	
	this->_LogTransaction("<--", response);
}

/*
	Response:
		\lc\1\challenge\Thc3BZFhfv\id\1\final\
	Request:
		\login\\challenge\otMLwtUZyLCV85ERGzg5mVER8nknWX0B\uniquenick\IamLupo\response\b836c6a19b240bb9fb2a31179b28062b\firewall\1\port\0\productid\10307\gamename\bfield1942ps2\namespaceid\13\id\1\final\
	Response:
		\lc\2\sesskey\1\userid\64679\profileid\10036819\uniquenick\IamLupo\lt\ee5540bbd764b321378ccedd\proof\70eabd6f5b004fc0b42056ac3cef5c7b\id\1\final\
		
	To do:
		- The client sends in the login request parameter "response". We have to figure out how this response is been generated.
*/
void GPCM::Client::requestLogin(const GameSpy::Parameter& parameter) const
{
	if(parameter.size() != 21)
	{
		return;
	}
	
	std::string uniquenick = parameter[5];
	std::string client_challenge = parameter[3];
	std::string id = parameter[19];
	std::string server_challenge = this->_session_challenge;
	
	Battlefield::Player player;
	player.SetUniquenick(uniquenick);
	
	// Get player information
	g_database->queryPlayerByUniquenick(player);
	
	// Generate proof
	std::string proof = GameSpy::LoginProof(player.GetPassword(), player.GetUniquenick(), client_challenge, server_challenge);
	
	// Save session profileid
	this->_session_profileid = player.GetProfileId();
	this->_session_authtoken = Util::generateRandomAuthtoken();
	
	std::string response = GameSpy::Parameter2Response({
		"lc", "2",
		"sesskey", "1",
		"userid", std::to_string(player.GetUserId()),
		"profileid", std::to_string(player.GetProfileId()),
		"uniquenick", player.GetUniquenick(),
		"lt", this->_session_authtoken,
		"proof", proof,
		"id", id,
		"final"
	});
	
	this->Send(response);
	
	this->_LogTransaction("<--", response);
}

/*
	Request:
		\inviteto\\sesskey\1\products\10307\final\
	Response:
		\bdy\7\list\10036271,10036113,10036585,10036492,10036029,10036444,10037053\final\
*/
void GPCM::Client::requestInviteTo(const GameSpy::Parameter& parameter) const
{
	Battlefield::Player player;
	
	player.SetProfileId(this->_session_profileid);
	
	if(!g_database->queryPlayerFriends(player))
	{
		return; // Oeps something went wrong?!
	}
	
	std::vector<int> player_friends = player.GetFriends();
	
	std::string response = GameSpy::Parameter2Response({
		"bdy", std::to_string(player_friends.size()),
		"list", Util::ToString(player_friends),
		"final"
	});
	
	this->Send(response);
	
	this->_LogTransaction("<--", response);
	
	
}

/*
	Request:
		\getprofile\\sesskey\1\profileid\10036819\id\3\final\
		\getprofile\\sesskey\1\profileid\10036029\id\4\final\
		\getprofile\\sesskey\1\profileid\10036113\id\5\final\
		\getprofile\\sesskey\1\profileid\10036271\id\6\final\
		\getprofile\\sesskey\1\profileid\10036444\id\7\final\
		\getprofile\\sesskey\1\profileid\10036492\id\8\final\
		\getprofile\\sesskey\1\profileid\10036585\id\9\final\
		\getprofile\\sesskey\1\profileid\10037053\id\10\final\
	Response:
		\pi\\profileid\10036819\userid\64679\nick\IamLupo@6507BAD7\uniquenick\IamLupo\email\help0001@gmail.com\sex\0\birthday\0\countrycode\\aim\\videocard1string\\videocard2string\\osstring\\id\3\sig\d41d8cd98f00b204e9800998ecf8427e\final\
		\pi\\profileid\10036492\userid\64397\nick\Tha_j0k3r@B9708875\uniquenick\Tha_j0k3r\aim\\videocard1string\\videocard2string\\osstring\\id\8\sig\d41d8cd98f00b204e9800998ecf8427e\final\
		\pi\\profileid\10036271\userid\64222\nick\BostonBrew@7F291349\uniquenick\BostonBrew\aim\\videocard1string\\videocard2string\\osstring\\id\6\sig\d41d8cd98f00b204e9800998ecf8427e\final\
		\pi\\profileid\10036029\userid\64015\nick\Shikamaru@93753F85\uniquenick\Shikamaru\aim\\videocard1string\\videocard2string\\osstring\\id\4\sig\d41d8cd98f00b204e9800998ecf8427e\final\
		\pi\\profileid\10036444\userid\64362\nick\CHURRU-GP@BDFB5921\uniquenick\CHURRU-GP\aim\\videocard1string\\videocard2string\\osstring\\id\7\sig\d41d8cd98f00b204e9800998ecf8427e\final\
		\pi\\profileid\10036113\userid\64089\nick\Scram@E3883FAF\uniquenick\Scram\aim\\videocard1string\\videocard2string\\osstring\\id\5\sig\d41d8cd98f00b204e9800998ecf8427e\final\
		\pi\\profileid\10036585\userid\64473\nick\NPC@A15FEDF3\uniquenick\NPC\aim\\videocard1string\\videocard2string\\osstring\\id\9\sig\d41d8cd98f00b204e9800998ecf8427e\final\
		\pi\\profileid\10037053\userid\64872\nick\-Gh0sTeD-@7DE1959D\uniquenick\-Gh0sTeD-\aim\\videocard1string\\videocard2string\\osstring\\id\10\sig\d41d8cd98f00b204e9800998ecf8427e\final\
*/
void GPCM::Client::requestGetProfile(const GameSpy::Parameter& parameter) const
{
	if(parameter.size() != 9)
	{
		return;
	}
	
	std::string profileid = parameter[5];
	std::string id = parameter[7];
	
	Battlefield::Player player;
	player.SetProfileId(profileid);
	
	// Get player information
	g_database->queryPlayerByProfileid(player);
	
	std::string response;
	
	if(player.GetUserId() != -1)
	{
		response = GameSpy::Parameter2Response({
			"pi", "",
			"profileid", std::to_string(player.GetProfileId()),
			"userid", std::to_string(player.GetUserId()),
			"nick", player.GetNick(),
			"uniquenick", player.GetUniquenick(),
			"email", player.GetEmail(),
			"sex", "0",
			"birthday", "0",
			"countrycode", "",
			"aim", "",
			"videocard1string", "",
			"videocard2string", "",
			"osstring", "",
			"id", id,
			"sig", "d41d8cd98f00b204e9800998ecf8427e",
			"final"
		});
	}
	else	
	{
		response = GameSpy::Parameter2Response({
			"pi", "",
			"profileid", profileid,
			"userid", std::to_string(-1),
			"nick", "<Unknown>",
			"uniquenick", "<Unknown>",
			"email", "<Unknown>",
			"sex", "0",
			"birthday", "0",
			"countrycode", "",
			"aim", "",
			"videocard1string", "",
			"videocard2string", "",
			"osstring", "",
			"id", id,
			"sig", "d41d8cd98f00b204e9800998ecf8427e",
			"final"
		});
	}
	
	this->Send(response);
	
	this->_LogTransaction("<--", response);
}

/*
	Request:
		\status\1\sesskey\1\statstring\Resting\locstring\bfield1942ps2:/\final\
		\status\2\sesskey\1\statstring\Playing\locstring\bfield1942ps2:/[EU]CTF-SERVER1@78.47.184.23:3659\final\
		\status\1\sesskey\1\statstring\Online\locstring\bfield1942ps2:/\final\
	
	Response:
		\bm\100\f\10036271\msg\|s|0|ss|Offline\final\
		\bm\100\f\10036029\msg\|s|0|ss|Offline\final\
		\bm\100\f\10037810\msg\|s|2|ss|Playing|ls|bfield1942ps2:/[EU]CTF-SERVER1@78.47.184.23:3659|ip|2970240317|p|44223\final\
*/
void GPCM::Client::requestStatus(const GameSpy::Parameter& parameter) const
{
	if(parameter.size() != 9)
	{
		return;
	}
	
	Battlefield::Player player;
	
	player.SetProfileId(this->_session_profileid);
	
	if(!g_database->queryPlayerFriends(player))
	{
		return; // Oeps something went wrong?!
	}
	
	std::vector<int> player_friends = player.GetFriends();
	
	for(int friend_profileid : player_friends)
	{
		std::string response;
		
		response += GameSpy::Parameter2Response({
			"bm", "100",
			"f", std::to_string(friend_profileid),
			"msg", "|s|2|ss|Offline",
			//"msg", "|s|2|ss|Playing|ls|bfield1942ps2:/[EU]CTF-SERVER1@78.47.184.23:3659|ip|3115326802|p|710",
			"final"
		});
		
		this->Send(response);

		this->_LogTransaction("<--", response);
	}
	
	/*
	// Stress test: Test a lot of friends
	if(profileids.size())
	{
		for(int i = 0; i < 100; i++)
		{
			std::string response;
			
			response += GameSpy::Parameter2Response({
				"bm", "100",
				"f", std::to_string(profileids[profileids.size() - 1] + i + 1),
				"msg", "|s|2|ss|Offline",
				"final"
			});
			
			this->Send(response);

			this->_LogTransaction("<--", response);
		}
	}
	*/
}

/*
	Request:
		\bm\1\sesskey\1\t\10036271\msg\aaa\final\
	
	Response to target:
		\bm\1\f\10037318\msg\awesome\final\
*/
void GPCM::Client::requestBm(const GameSpy::Parameter& parameter) const
{
	if(parameter.size() != 9)
	{
		return;
	}
	
	std::string profileid = parameter[5];
	std::string msg = parameter[7];
	
	Battlefield::Player target_player;
	target_player.SetProfileId(profileid);
	
	for(Net::Socket* client : g_gpcm_server->_clients)
	{
		GPCM::Client* gpcm_client = reinterpret_cast<GPCM::Client*>(client);
		
		if(gpcm_client->_session_profileid == target_player.GetProfileId())
		{
			std::string response = GameSpy::Parameter2Response({
				"bm", "1",
				"f", std::to_string(this->_session_profileid),
				"msg", msg,
				"final"
			});
			
			gpcm_client->Send(response);
			
			gpcm_client->_LogTransaction("<--", response);
		}
	}
}

/*
	Request:
		\addbuddy\\sesskey\1\newprofileid\10037827\reason\\final\
	
	Response:
		\bm\2\f\10037049\msg\|signed|d41d8cd98f00b204e9800998ecf8427e\final\
	
	Extra:
		\error\\err\1539\errmsg\The profile requested is already a buddy.\final\
*/
void GPCM::Client::requestAddBuddy(const GameSpy::Parameter& parameter) const
{
	if(parameter.size() != 9 || parameter[7] == "BFMCB-AUTO.")
	{
		return;
	}
	
	std::string target_profileid = parameter[5];
	
	Battlefield::Player target_player;
	target_player.SetProfileId(target_profileid);
		
	for(Net::Socket* client : g_gpcm_server->_clients)
	{
		GPCM::Client* gpcm_client = reinterpret_cast<GPCM::Client*>(client);
		
		if(gpcm_client->_session_profileid == target_player.GetProfileId())
		{
			std::string response = GameSpy::Parameter2Response({
				"bm", "2",
				"f", std::to_string(this->_session_profileid),
				"msg", "|signed|d41d8cd98f00b204e9800998ecf8427e",
				"final"
			});
			
			gpcm_client->Send(response);
			
			gpcm_client->_LogTransaction("<--", response);
		}
	}
}

/*
	Request:
		\authadd\\sesskey\1\fromprofileid\10037049\sig\d41d8cd98f00b204e9800998ecf8427e\final\
	Response:
		\bm\4\f\10037318\msg\I have authorized your request to add me to your list|signed|d41d8cd98f00b204e9800998ecf8427e\final\

*/
void GPCM::Client::requestAuthAdd(const GameSpy::Parameter& parameter) const
{
	if(parameter.size() != 9)
	{
		return;
	}
	
	std::string target_profileid = parameter[5];
	std::string sig = parameter[7];
	
	Battlefield::Player target_player;
	target_player.SetProfileId(target_profileid);
	
	if(!g_database->insertPlayerFriend(this->_session_profileid, target_player.GetProfileId()))
	{
		return;
	}
	
	for(Net::Socket* client : g_gpcm_server->_clients)
	{
		GPCM::Client* gpcm_client = reinterpret_cast<GPCM::Client*>(client);
		
		if(gpcm_client->_session_profileid == target_player.GetProfileId())
		{
			// Send friendlist update to friendship sender
			std::string response = GameSpy::Parameter2Response({
				"bm", "100",
				"f", std::to_string(this->_session_profileid),
				"msg", "|s|2|ss|Playing|ls|bfield1942ps2:/[EU]CTF-SERVER1@78.47.184.23:3659|ip|3115326802|p|710",
				"final"
			});
			
			gpcm_client->Send(response);
			
			gpcm_client->_LogTransaction("<--", response);
			
			// Send friendlist update to friendship reciever
			response = GameSpy::Parameter2Response({
				"bm", "100",
				"f", target_profileid,
				"msg", "|s|2|ss|Playing|ls|bfield1942ps2:/[EU]CTF-SERVER1@78.47.184.23:3659|ip|3115326802|p|710",
				"final"
			});
			
			this->Send(response);
			
			this->_LogTransaction("<--", response);
		}
	}
}

/*
	Request:
		\revoke\\sesskey\1\profileid\10036819\final\
	Response:
		\bm\6\f\10036819\msg\I have revoked you from my list.|signed|d41d8cd98f00b204e9800998ecf8427e\final\
		\bm\6\f\10037049\msg\I have revoked you from my list.|signed|d41d8cd98f00b204e9800998ecf8427e\final\
*/
void GPCM::Client::requestRevoke(const GameSpy::Parameter& parameter) const
{
	if(parameter.size() != 7)
	{
		return;
	}
	
	std::string target_profileid = parameter[5];
	
	Battlefield::Player target_player;
	target_player.SetProfileId(target_profileid);
	
	if(!g_database->removePlayerFriend(this->_session_profileid, target_player.GetProfileId()))
	{
		return;
	}
	
	for(Net::Socket* client : g_gpcm_server->_clients)
	{
		GPCM::Client* gpcm_client = reinterpret_cast<GPCM::Client*>(client);
		
		if(gpcm_client->_session_profileid == target_player.GetProfileId())
		{
			// Send friendlist update to friendship sender
			std::string response = GameSpy::Parameter2Response({
				"bm", "100",
				"f", std::to_string(this->_session_profileid),
				"msg", "|s|0|ss|Offline",
				"final"
			});
			
			gpcm_client->Send(response);
			
			gpcm_client->_LogTransaction("<--", response);
		}
	}
}

/*
	Request:
		\delbuddy\\sesskey\1\delprofileid\10036819\final\
*/
void GPCM::Client::requestDeleteBuddy(const GameSpy::Parameter& parameter) const
{
	
}

/*
	Request:
		\logout\\sesskey\1\final\
*/
void GPCM::Client::requestLogout(const GameSpy::Parameter& parameter) const
{
	
}

/*
	Private functions
*/
void GPCM::Client::_LogTransaction(const std::string &direction, const std::string &response) const
{
	std::lock_guard<std::mutex> guard(g_mutex_io);
	
	std::cout << std::setfill(' ') << std::setw(21) << this->GetAddress() << " " << direction << " " << response << std::endl;
}

