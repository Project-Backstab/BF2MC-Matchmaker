#include <iostream>
#include <sstream>
#include <util.h>

#include <gamespy.h>

std::vector<std::string> GameSpy::RequestToRequests(const std::string &request)
{
	std::string t = request;
	std::vector<std::string> requests;
	
	size_t found = t.find("final\\");
	
	//std::cout << t << std::endl;
	
	while(found != std::string::npos)
	{
		//std::cout << found << std::endl;
		
		requests.push_back(t.substr(0, found + 6));
		
		t = t.substr(found + 6, t.size());
		
		found = t.find("final\\");
		
		//std::cout << t << std::endl;
	}
	
	return requests;
}

GameSpy::Parameter GameSpy::Request2Parameter(const std::string &request)
{
	std::stringstream input;
	std::string value;
	GameSpy::Parameter parameter;
	
	input.str(request);
	
	// Skip first backslash
	input.ignore(1, '\\');
	
	// Split between backslashes
	while(std::getline(input, value, '\\'))
	{
	   parameter.push_back(value);
	}
	
	return parameter;
}

std::string GameSpy::Parameter2Response(const GameSpy::Parameter &parameter)
{
	std::string response;
	
	response += "\\";
	
	for(std::string value : parameter)
	{
		response += value + "\\";
	}
	
	return response;
}

std::string GameSpy::LoginProof(const std::string &password, const std::string &username, const std::string &client_challenge, const std::string &server_challenge)
{
	std::string password_hash;
	const std::string space = "                                                ";
	
	// Generate password hash
	password_hash = Util::MD5hash(password);
	
	// Generate proof
	return Util::MD5hash(password_hash + space + username + server_challenge + client_challenge + password_hash);
}

