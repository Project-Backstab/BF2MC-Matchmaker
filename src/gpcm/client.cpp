#include <unistd.h>
#include <iostream>
#include <iomanip>

#include <settings.h>
#include <logger.h>
#include <server.h>
#include <globals.h>
#include <database.h>
#include <util.h>

#include <gpcm/client.h>

typedef void (GPCM::Client::*RequestActionFunc)(const GameSpy::Parameter&);

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
		
		// Debug
		//Logger::debug("--- START ----------------------------------------" << std::endl;
		//Logger::debug("size = " + std::to_string(v));
		//Logger::debug("request = " + request);
		
		std::vector<std::string> requests = GameSpy::RequestToRequests(request);
		
		for(std::string v : requests)
		{
			this->_LogTransaction("-->", v);
		
			this->onRequest(v);
			
			//std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		
		//Logger::debug("--- END ----------------------------------------");
	}
	
	this->Disconnect();
}

void GPCM::Client::Disconnect()
{
	this->Close();
	g_gpcm_server->onClientDisconnect(*this);
}

GPCM::Session GPCM::Client::GetSession() const
{
	GPCM::Session copy_session = this->_session;
	
	return copy_session;
}

/*
	Events
*/
void GPCM::Client::onRequest(const std::string& request)
{
	GameSpy::Parameter parameter = GameSpy::Request2Parameter(request);
	
	// Find function name
	std::string action = parameter[0];
	
	auto it = mRequestActions.find(action);
	if (it != mRequestActions.end())
	{
		// Get Function address
		RequestActionFunc func = it->second;
	
		// Execute action function with class object.
		(this->*(func))(parameter);
	}
	else
	{
		Logger::warning("action \"" + action + "\" not implemented!");
		
		this->Disconnect();
	}
}

void GPCM::Client::requestChallenge()
{
	this->_session.challenge = Util::generateRandomChallenge();
	
	std::string response = GameSpy::Parameter2Response({
		"lc",        "1",
		"challenge", this->_session.challenge,
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
void GPCM::Client::requestLogin(const GameSpy::Parameter& parameter)
{
	if(parameter.size() != 21)
	{
		return;
	}
	
	std::string uniquenick = parameter[5];
	std::string client_challenge = parameter[3];
	std::string id = parameter[19];
	std::string server_challenge = this->_session.challenge;
	
	Battlefield::Player player;
	player.SetUniquenick(uniquenick);
	
	// Get player information
	g_database->queryPlayerByUniquenick(player);
	
	// Generate proof
	std::string proof = GameSpy::LoginProof(player.GetPassword(), player.GetUniquenick(), client_challenge, server_challenge);
	
	// Save session profileid
	this->_session.profileid = player.GetProfileId();
	this->_session.authtoken = Util::generateRandomAuthtoken();
	
	std::string response = GameSpy::Parameter2Response({
		"lc", "2",
		"sesskey", "1",
		"userid", std::to_string(player.GetUserId()),
		"profileid", std::to_string(player.GetProfileId()),
		"uniquenick", player.GetUniquenick(),
		"lt", this->_session.authtoken,
		"proof", proof,
		"id", id,
		"final"
	});
	
	Logger::info("User \"" + player.GetUniquenick() + "\" logged in from " + this->GetAddress());
	
	this->Send(response);
	
	this->_LogTransaction("<--", response);
}

/*
	Request:
		\inviteto\\sesskey\1\products\10307\final\
	Response:
		\bdy\7\list\10036271,10036113,10036585,10036492,10036029,10036444,10037053\final\
*/
void GPCM::Client::requestInviteTo(const GameSpy::Parameter& parameter)
{
	if(parameter.size() != 7)
	{
		return;
	}
	
	Battlefield::Player player;
	
	player.SetProfileId(this->_session.profileid);
	
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
void GPCM::Client::requestGetProfile(const GameSpy::Parameter& parameter)
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
	
	this->_SyncFriends();
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
void GPCM::Client::requestStatus(const GameSpy::Parameter& parameter)
{
	if(parameter.size() != 9)
	{
		return;
	}
	
	Battlefield::Player player;
	
	player.SetProfileId(this->_session.profileid);
	
	if(!g_database->queryPlayerFriends(player))
	{
		return; // Oeps something went wrong?!
	}
	
	this->_sync_friends = player.GetFriends();
	this->_SyncFriends();
}

/*
	Request:
		\bm\1\sesskey\1\t\10036271\msg\aaa\final\
	
	Response to target:
		\bm\1\f\10037318\msg\awesome\final\
*/
void GPCM::Client::requestBm(const GameSpy::Parameter& parameter)
{
	if(parameter.size() != 9)
	{
		return;
	}
	
	std::string profileid = parameter[5];
	std::string msg = parameter[7];
	
	Battlefield::Player target_player;
	target_player.SetProfileId(profileid);
	
	GPCM::Session session = GPCM::Client::findSessionByProfileId(target_player.GetProfileId());
	
	if(session.profileid != -1)
	{
		std::string response = GameSpy::Parameter2Response({
			"bm", "1",
			"f", std::to_string(this->_session.profileid),
			"msg", msg,
			"final"
		});
		
		session.client->Send(response);
		
		session.client->_LogTransaction("<--", response);
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
void GPCM::Client::requestAddBuddy(const GameSpy::Parameter& parameter)
{
	if(parameter.size() != 9 || parameter[7].find("BFMCB-AUTO") != std::string::npos)
	{
		return;
	}
	
	std::string target_profileid = parameter[5];
	
	Battlefield::Player target_player;
	target_player.SetProfileId(target_profileid);
		
	GPCM::Session session = GPCM::Client::findSessionByProfileId(target_player.GetProfileId());
	
	if(session.profileid != -1)
	{
		std::string response = GameSpy::Parameter2Response({
			"bm", "2",
			"f", std::to_string(this->_session.profileid),
			"msg", "|signed|d41d8cd98f00b204e9800998ecf8427e",
			"final"
		});
		
		session.client->Send(response);
		
		session.client->_LogTransaction("<--", response);
	}
}

/*
	Request:
		\authadd\\sesskey\1\fromprofileid\10037049\sig\d41d8cd98f00b204e9800998ecf8427e\final\
	Response:
		\bm\4\f\10037318\msg\I have authorized your request to add me to your list|signed|d41d8cd98f00b204e9800998ecf8427e\final\

*/
void GPCM::Client::requestAuthAdd(const GameSpy::Parameter& parameter)
{
	if(parameter.size() != 9)
	{
		return;
	}
	
	std::string target_profileid = parameter[5];
	std::string sig = parameter[7];
	
	Battlefield::Player player;
	Battlefield::Player target_player;
	
	// Set profileid
	player.SetProfileId(this->_session.profileid);
	target_player.SetProfileId(target_profileid);
	
	// Save friendship
	g_database->insertPlayerFriend(player, target_player);
	
	GPCM::Session session = GPCM::Client::findSessionByProfileId(target_player.GetProfileId());
	
	if(session.profileid != -1)
	{
		// Send friendlist update to friendship sender
		std::string response = GameSpy::Parameter2Response({
			"bm", "100",
			"f", std::to_string(player.GetProfileId()),
			"msg", "|s|2|ss|Playing|ls|bfield1942ps2:/[EU]CTF-SERVER1@78.47.184.23:3659|ip|3115326802|p|710",
			"final"
		});
		
		session.client->Send(response);
		
		session.client->_LogTransaction("<--", response);
		
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

/*
	Request:
		\revoke\\sesskey\1\profileid\10036819\final\
	Response:
		\bm\6\f\10036819\msg\I have revoked you from my list.|signed|d41d8cd98f00b204e9800998ecf8427e\final\
		\bm\6\f\10037049\msg\I have revoked you from my list.|signed|d41d8cd98f00b204e9800998ecf8427e\final\
*/
void GPCM::Client::requestRevoke(const GameSpy::Parameter& parameter)
{
	if(parameter.size() != 7)
	{
		return;
	}
	
	std::string target_profileid = parameter[5];
	
	Battlefield::Player player;
	Battlefield::Player target_player;
	
	// Set profileid
	player.SetProfileId(this->_session.profileid);
	target_player.SetProfileId(target_profileid);
	
	// Remove friendship
	g_database->removePlayerFriend(player, target_player);
	
	GPCM::Session session = GPCM::Client::findSessionByProfileId(target_player.GetProfileId());
	
	if(session.profileid != -1)
	{
		// Send friendlist update to friendship sender
		std::string response = GameSpy::Parameter2Response({
			"bm", "100",
			"f", std::to_string(player.GetProfileId()),
			"msg", "|s|0|ss|Offline",
			"final"
		});
		
		session.client->Send(response);
		
		session.client->_LogTransaction("<--", response);
	}
}

/*
	Request:
		\delbuddy\\sesskey\1\delprofileid\10036819\final\
*/
void GPCM::Client::requestDeleteBuddy(const GameSpy::Parameter& parameter)
{
	//if(parameter.size() != 7)
	//{
	//	return;
	//}
}

/*
	Request:
		\logout\\sesskey\1\final\
*/
void GPCM::Client::requestLogout(const GameSpy::Parameter& parameter)
{
	//if(parameter.size() != 5)
	//{
	//	return;
	//}
	
	Battlefield::Player player;
	
	player.SetProfileId(this->_session.profileid);
	
	g_database->queryPlayerByProfileid(player);
	
	Logger::info("User \"" + player.GetUniquenick() + "\" logged out");
}

/*
	Private functions
*/
void GPCM::Client::_LogTransaction(const std::string& direction, const std::string& response) const
{
	std::shared_lock<std::shared_mutex> guard2(g_mutex_settings); // settings lock  (read)
	
	bool show_console = (g_settings["gpcm"]["show_requests"].asBool() && direction == "-->") ||
						(g_settings["gpcm"]["show_responses"].asBool() && direction == "<--");
	
	Logger::info(this->GetAddress() + " " + direction + " " + response, show_console);
}

void GPCM::Client::_SyncFriends()
{
	if(this->_sync_friends.size() >= 1)
	{
		int id = this->_sync_friends.size() - 1;
		int profileid = this->_sync_friends[id];
		
		std::string response = GameSpy::Parameter2Response({
			"bm", "100",
			"f", std::to_string(profileid),
			"msg", "|s|2|ss|Offline",
			"final"
		});
		
		this->Send(response);

		this->_LogTransaction("<--", response);
		
		// Erase last profileid from list
		this->_sync_friends.pop_back();
	}
}

/*
	Static
*/
GPCM::Session GPCM::Client::findSessionByProfileId(int profileid)
{
	GPCM::Session session;
	
	for(Net::Socket* client : g_gpcm_server->_clients)
	{
		GPCM::Client* gpcm_client = static_cast<GPCM::Client*>(client);
		
		// Find session
		session = gpcm_client->GetSession();
		
		if(session.profileid == profileid)
		{
			session.client = gpcm_client;
			return session;
		}
	}
	
	return {};
}

GPCM::Session GPCM::Client::findSessionByAuthtoken(const std::string& authtoken)
{
	GPCM::Session session;
	
	for(Net::Socket* client : g_gpcm_server->_clients)
	{
		GPCM::Client* gpcm_client = static_cast<GPCM::Client*>(client);
		
		// Find session
		session = gpcm_client->GetSession();
		
		if(session.authtoken == authtoken)
		{
			session.client = gpcm_client;
			return session;
		}
	}
	
	return {};
}

void GPCM::Client::SendBuddyMessage(int profileid, int target_profileid, const std::string& msg)
{
	GPCM::Session session = findSessionByProfileId(target_profileid);
	
	if(session.profileid != -1)
	{
		std::string response = GameSpy::Parameter2Response({
			"bm", "1",
			"f", std::to_string(profileid),
			"msg", msg,
			"final"
		});
		
		session.client->Send(response);
		
		session.client->_LogTransaction("<--", response);
	}
}

