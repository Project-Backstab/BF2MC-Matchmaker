#ifndef GAMESPY_H
#define GAMESPY_H

#include <string>
#include <vector>

namespace GameSpy
{
	typedef std::vector<std::string> Parameter;
	
	std::vector<std::string> RequestToRequests(const std::string &request);
	std::vector<std::string> RequestToRequests(const std::string &request);
	GameSpy::Parameter Request2Parameter(const std::string &request);
	std::string Parameter2Response(const GameSpy::Parameter &parameter);
	
	std::string LoginProof(const std::string &password, const std::string &username, const std::string &client_challenge, const std::string &server_challenge);
}

#endif // GAMESPY_H
