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
	{ "login",      &GPCM::Client::requestLogin },
	{ "inviteto",   &GPCM::Client::requestInviteTo },
	{ "getprofile", &GPCM::Client::requestGetProfile },
	{ "status",     &GPCM::Client::requestStatus },
	{ "bm",         &GPCM::Client::requestBm },
	{ "logout",     &GPCM::Client::requestLogout },
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
		
		std::cout << "action \"" << action << "\"not implemented!" << std::endl;
		
		guard.unlock();
		
		this->Disconnect();
	}
}

void GPCM::Client::requestChallenge() const
{
	std::string response = GameSpy::Parameter2Response({
		"lc",        "1",
		"challenge", "Thc3BZFhfv",
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
*/
void GPCM::Client::requestLogin(const GameSpy::Parameter& parameter) const
{
	if(parameter.size() != 21)
	{
		return;
	}
	
	std::string uniquenick = parameter[5];
	std::string client_challenge = parameter[3];

	// Query Database user
	DBUser dbuser;
	if(!g_database->queryDBUserByUniquenick(dbuser, uniquenick))
	{
		// No Database user found with uniquenick
		return;
	}
	
	// Generate proof
	std::string proof = GameSpy::LoginProof(dbuser.password, uniquenick, client_challenge, "Thc3BZFhfv");
	
	std::string response = GameSpy::Parameter2Response({
		"lc", "2",
		"sesskey", "1",
		"userid", std::to_string(dbuser.userid),
		"profileid", std::to_string(dbuser.profileid),
		"uniquenick", dbuser.uniquenick,
		"lt", "ee5540bbd764b321378ccedd",
		"proof", proof,
		"id", "1",
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
	std::string response = GameSpy::Parameter2Response({
		"bdy", "1",
		"list", "10037049",
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
	std::string response;
	
	DBUser dbuser;
	
	std::cout << "profileid = " << profileid << std::endl;
	
	if(!g_database->queryDBUserByProfileid(dbuser, profileid))
	{
		return; // No Database user found with uniquenick
	}
	
	if(dbuser.profileid != -1)
	{
		response = GameSpy::Parameter2Response({
			"pi", "",
			"profileid", std::to_string(dbuser.profileid),
			"userid", std::to_string(dbuser.userid),
			"nick", dbuser.nick,
			"uniquenick", dbuser.uniquenick,
			"email", dbuser.email,
			"sex", "0",
			"birthday", "0",
			"countrycode", "",
			"aim", "",
			"videocard1string", "",
			"videocard2string", "",
			"osstring", "",
			"id", parameter[7],
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
			"id", parameter[7],
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
	Response:
		\bm\100\f\10036271\msg\|s|0|ss|Offline\final\
		\bm\100\f\10036029\msg\|s|0|ss|Offline\final\
		\bm\100\f\10036113\msg\|s|0|ss|Offline\final\
		\bm\100\f\10036444\msg\|s|0|ss|Offline\final\
		\bm\100\f\10036492\msg\|s|0|ss|Offline\final\
		\bm\100\f\10036585\msg\|s|0|ss|Offline\final\
		\bm\100\f\10037053\msg\|s|0|ss|Offline\final\
*/
void GPCM::Client::requestStatus(const GameSpy::Parameter& parameter) const
{
	if(parameter.size() != 9)
	{
		return;
	}
	
	std::string response = GameSpy::Parameter2Response({
		"bm", "100",
		"f", "10037049",
		"msg", "|s|0|ss|Offline",
		"final"
	});
	
	this->Send(response);
}

/*
	\bm\1\sesskey\1\t\10036271\msg\aaa\final\
*/
void GPCM::Client::requestBm(const GameSpy::Parameter& parameter) const
{
	
}

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
