#ifndef GAMESPY_H
#define GAMESPY_H

#include <string>
#include <vector>

namespace GameSpy
{
	/*
		Type definitions
	*/
	typedef std::vector<std::string> Parameter;
	
	/**
	 * @brief Convert a request string to a vector of strings.
	 * 
	 * Splits the request string into individual parameters and returns them as a vector.
	 * 
	 * @param request The request string to convert.
	 * @return A vector of strings, each element representing a parameter from the request.
	 */
	std::vector<std::string> RequestToRequests(const std::string& request);

	/**
	 * @brief Convert a request string to a vector of strings.
	 * 
	 * Splits the request string into individual parameters and returns them as a vector.
	 * 
	 * @param request The request string to convert.
	 * @return A vector of strings, each element representing a parameter from the request.
	 */
	GameSpy::Parameter Request2Parameter(const std::string& request);

	/**
	 * @brief Convert a vector of strings to a response string.
	 * 
	 * Concatenates the elements of the parameter vector into a response string.
	 * 
	 * @param parameter The vector of strings to convert.
	 * @return A single string representing the response.
	 */
	std::string Parameter2Response(const GameSpy::Parameter& parameter);

	/**
	 * @brief Generate a login response.
	 * 
	 * Generates a login response based on the provided parameters.
	 * 
	 * @param password The password for the login.
	 * @param uniquenick The unique nickname for the login.
	 * @param client_challenge The client challenge for the login.
	 * @param server_challenge The server challenge for the login.
	 * @return The login response string.
	 */
	std::string LoginResponse(const std::string& password, const std::string& uniquenick,
		const std::string& client_challenge, const std::string& server_challenge);

	/**
	 * @brief Generate a login proof.
	 * 
	 * Generates a login proof based on the provided parameters.
	 * 
	 * @param password The password for the login.
	 * @param uniquenick The unique nickname for the login.
	 * @param client_challenge The client challenge for the login.
	 * @param server_challenge The server challenge for the login.
	 * @return The login proof string.
	 */
	std::string LoginProof(const std::string& password, const std::string& uniquenick,
			const std::string& client_challenge, const std::string& server_challenge);
}

#endif // GAMESPY_H
