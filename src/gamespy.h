#ifndef GAMESPY_H
#define GAMESPY_H

#include <string>
#include <vector>

namespace GameSpy
{
	enum class ErrorCode : uint16_t
	{
		// General error codes.
		GP_GENERAL					= 0, // There was an unknown error. 
		GP_PARSE					= 1, // Unexpected data was received from the server. 
		GP_NOT_LOGGED_IN			= 2, // The request cannot be processed because user has not logged in. 
		GP_BAD_SESSKEY				= 3, // The request cannot be processed because of an invalid session key.
		GP_DATABASE					= 4, // There was a database error.
		GP_NETWORK					= 5, // There was an error connecting a network socket.
		GP_FORCED_DISCONNECT		= 6, // This profile has been disconnected by another login.
		GP_CONNECTION_CLOSED		= 7, // The server has closed the connection.
		GP_UDP_LAYER				= 8, // There was a problem with the UDP layer.

		// Error codes that can occur while logging in.
		GP_LOGIN						= 256, // 0x100, There was an error logging in to the GP backend.
		GP_LOGIN_TIMEOUT				= 257, // 0x101, The login attempt timed out.
		GP_LOGIN_BAD_NICK				= 258, // 0x102, The nickname provided was incorrect.
		GP_LOGIN_BAD_EMAIL				= 259, // 0x103, The e-mail address provided was incorrect.
		GP_LOGIN_BAD_PASSWORD			= 260, // 0x104, The password provided was incorrect.
		GP_LOGIN_BAD_PROFILE			= 261, // 0x105, The profile provided was incorrect.
		GP_LOGIN_PROFILE_DELETED		= 262, // 0x106, The profile has been deleted.
		GP_LOGIN_CONNECTION_FAILED		= 263, // 0x107, The server has refused the connection.
		GP_LOGIN_SERVER_AUTH_FAILED		= 264, // 0x108, The server could not be authenticated.
		GP_LOGIN_BAD_UNIQUENICK			= 265, // 0x109, The uniquenick provided was incorrect.
		GP_LOGIN_BAD_PREAUTH			= 266, // 0x10A, There was an error validating the pre-authentication.
		GP_LOGIN_BAD_LOGIN_TICKET		= 267, // 0x10B, The login ticket was unable to be validated.
		GP_LOGIN_EXPIRED_LOGIN_TICKET	= 268, // 0x10C, The login ticket had expired and could not be used.

		// Error codes that can occur while creating a new user.
		GP_NEWUSER						= 512, // 0x200, There was an error creating a new user.
		GP_NEWUSER_BAD_NICK				= 513, // 0x201, A profile with that nick already exists.
		GP_NEWUSER_BAD_PASSWORD			= 514, // 0x202, The password does not match the email address.
		GP_NEWUSER_UNIQUENICK_INVALID	= 515, // 0x203, The uniquenick is invalid.
		GP_NEWUSER_UNIQUENICK_INUSE		= 516, // 0x204, The uniquenick is already in use.

		// Error codes that can occur while updating user information.
		GP_UPDATEUI						= 768, // 0x300, There was an error updating the user information.
		GP_UPDATEUI_BAD_EMAIL			= 769, // 0x301, A user with the email address provided already exists.

		// Error codes that can occur while creating a new profile.
		GP_NEWPROFILE					= 1024, // 0x400, There was an error creating a new profile.
		GP_NEWPROFILE_BAD_NICK			= 1025, // 0x401, The nickname to be replaced does not exist.
		GP_NEWPROFILE_BAD_OLD_NICK		= 1026, // 0x402, A profile with the nickname provided already exists.

		// Error codes that can occur while updating profile information.
		GP_UPDATEPRO					= 1280, // 0x500, There was an error updating the profile information. 
		GP_UPDATEPRO_BAD_NICK			= 1281, // 0x501, A user with the nickname provided already exists.

		// Error codes that can occur while adding someone to your buddy list.
		GP_ADDBUDDY						= 1536, // 0x600, There was an error adding a buddy. 
		GP_ADDBUDDY_BAD_FROM			= 1537, // 0x601, The profile requesting to add a buddy is invalid.
		GP_ADDBUDDY_BAD_NEW				= 1538, // 0x602, The profile requested is invalid.
		GP_ADDBUDDY_ALREADY_BUDDY		= 1539, // 0x603, The profile requested is already a buddy.
		GP_ADDBUDDY_IS_ON_BLOCKLIST		= 1540, // 0x604, The profile requested is on the local profile's block list.
		//DOM-IGNORE-BEGIN 
		GP_ADDBUDDY_IS_BLOCKING			= 1541, // 0x605, Reserved for future use.
		//DOM-IGNORE-END

		// Error codes that can occur while being authorized to add someone to your buddy list.
		GP_AUTHADD						= 1792, // 0x700, There was an error authorizing an add buddy request.
		GP_AUTHADD_BAD_FROM				= 1793, // 0x701, The profile being authorized is invalid. 
		GP_AUTHADD_BAD_SIG				= 1794, // 0x702, The signature for the authorization is invalid.
		GP_AUTHADD_IS_ON_BLOCKLIST		= 1795, // 0x703, The profile requesting authorization is on a block list.
		//DOM-IGNORE-BEGIN 
		GP_AUTHADD_IS_BLOCKING			= 1796, // 0x704, Reserved for future use.
		//DOM-IGNORE-END

		// Error codes that can occur with status messages.
		GP_STATUS						= 2048, // 0x800, There was an error with the status string.

		// Error codes that can occur while sending a buddy message.
		GP_BM							= 2304, // 0x900, There was an error sending a buddy message.
		GP_BM_NOT_BUDDY					= 2305, // 0x901, The profile the message was to be sent to is not a buddy.
		GP_BM_EXT_INFO_NOT_SUPPORTED	= 2306, // 0x902, The profile does not support extended info keys.
		GP_BM_BUDDY_OFFLINE				= 2307, // 0x903, The buddy to send a message to is offline.

		// Error codes that can occur while getting profile information.
		GP_GETPROFILE					= 2560, // 0xA00, There was an error getting profile info. 
		GP_GETPROFILE_BAD_PROFILE		= 2561, // 0xA01, The profile info was requested on is invalid.

		// Error codes that can occur while deleting a buddy.
		GP_DELBUDDY						= 2816, // 0xB00, There was an error deleting the buddy.
		GP_DELBUDDY_NOT_BUDDY			= 2817, // 0xB01, The buddy to be deleted is not a buddy. 

		// Error codes that can occur while deleting your profile.
		GP_DELPROFILE					= 3072, // 0xC00, There was an error deleting the profile.
		GP_DELPROFILE_LAST_PROFILE		= 3073, // 0xC01, The last profile cannot be deleted.

		// Error codes that can occur while searching for a profile.
		GP_SEARCH						= 3328, // 0xD00, There was an error searching for a profile.
		GP_SEARCH_CONNECTION_FAILED		= 3329, // 0xD01, The search attempt failed to connect to the server.
		GP_SEARCH_TIMED_OUT				= 3330, // 0XD02, The search did not return in a timely fashion.

		// Error codes that can occur while checking whether a user exists.
		GP_CHECK						= 3584, // 0xE00, There was an error checking the user account.
		GP_CHECK_BAD_EMAIL				= 3585, // 0xE01, No account exists with the provided e-mail address.
		GP_CHECK_BAD_NICK				= 3586,	// 0xE02, No such profile exists for the provided e-mail address.
		GP_CHECK_BAD_PASSWORD			= 3587, // 0xE03, The password is incorrect.

		// Error codes that can occur while revoking buddy status.
		GP_REVOKE						= 3840, // 0xF00, There was an error revoking the buddy.
		GP_REVOKE_NOT_BUDDY				= 3841, // 0xF01, You are not a buddy of the profile.

		// Error codes that can occur while registering a new unique nick.
		GP_REGISTERUNIQUENICK				= 4096, // 0x1000, There was an error registering the uniquenick.
		GP_REGISTERUNIQUENICK_TAKEN			= 4097, // 0x1001, The uniquenick is already taken.
		GP_REGISTERUNIQUENICK_RESERVED		= 4098, // 0x1002, The uniquenick is reserved. 
		GP_REGISTERUNIQUENICK_BAD_NAMESPACE	= 4099, // 0x1003, Tried to register a nick with no namespace set. 

		// Error codes that can occur while registering a CD key.
		GP_REGISTERCDKEY				= 4352, // 0x1100, There was an error registering the cdkey.
		GP_REGISTERCDKEY_BAD_KEY		= 4353, // 0x1101, The cdkey is invalid. 
		GP_REGISTERCDKEY_ALREADY_SET	= 4354, // 0x1102, The profile has already been registered with a different cdkey.
		GP_REGISTERCDKEY_ALREADY_TAKEN	= 4355, // 0x1103, The cdkey has already been registered to another profile. 

		// Error codes that can occur while adding someone to your block list.
		GP_ADDBLOCK						= 4608, // 0x1200, There was an error adding the player to the blocked list. 
		GP_ADDBLOCK_ALREADY_BLOCKED		= 4609, // 0x1201, The profile specified is already blocked.

		// Error codes that can occur while removing someone from your block list.
		GP_REMOVEBLOCK					= 4864, // 0x1300, There was an error removing the player from the blocked list. 
		GP_REMOVEBLOCK_NOT_BLOCKED		= 4865  // 0x1301, The profile specified was not a member of the blocked list.
	};

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
