#ifndef BATTLEFIELD_PLAYER_H
#define BATTLEFIELD_PLAYER_H

#include <string>
#include <vector>
#include <map>

#include <battlefield/playerstats.h>

/**
 * @brief Forward declaration of the MYSQL_TIME structure.
 * 
 * This forward declaration allows the use of the MYSQL_TIME structure in contexts where only the structure name needs to be known,
 * such as declaring pointers or references to MYSQL_TIME objects, without the need to include the full definition of the structure.
 * 
 * MYSQL_TIME is typically used to represent date and time values in MySQL-related operations.
 */
struct MYSQL_TIME;

namespace Battlefield
{
	/**
	 * @brief Forward declarations
	 */
	class Player; // Forward declaration of the Player class within the Battlefield namespace

	/**
	 * @brief Type definitions
	 */
	typedef std::vector<Player> Players; // Alias for a vector of Player objects, named Players
	typedef std::map<int, Player> RankPlayers; // Alias for a map with integer keys and Player values, named RankPlayers

	
	/**
	 * @brief Represents a player with extended statistics.
	 */
	class Player : public PlayerStats
	{
		private:
			int                 _profileid     = -1;       /**< Profile ID of the player. */
			int                 _userid        = -1;       /**< User ID of the player. */
			std::string         _nick          = "";       /**< Nickname of the player. */
			std::string         _uniquenick    = "";       /**< Unique nickname of the player. */
			std::string         _email         = "";       /**< Email address of the player. */
			std::string         _password      = "";       /**< Password of the player. */
			std::string         _last_login    = "";       /**< Last login timestamp of the player. */
			std::string         _last_login_ip = "";       /**< IP address of the last login. */
			std::string         _created_at    = "";       /**< Creation timestamp of the player. */
			bool                _verified      = false;    /**< Indicates if the player is verified and can login. */
			bool                _restricted    = false;    /**< Indicates if the player is restricted from progressing stats. */
			std::vector<int>    _friends;                  /**< List of profile IDs of the player's friends. */
		
		public:
			/**
			 * @brief Performs an example operation using the Player class.
			 * 
			 * This member function demonstrates an example operation using the Player class.
			 * It serves as a placeholder for illustrating the usage of the Player class.
			 * 
			 * @note This is just an example function and does not perform any meaningful operation.
			 */
			void useExample();
			
			int              GetProfileId() const   { return this->_profileid;     }
			int              GetUserId() const      { return this->_userid;        }
			std::string      GetNick() const        { return this->_nick;          }
			std::string      GetUniquenick() const  { return this->_uniquenick;    }
			std::string      GetEmail() const       { return this->_email;         }
			std::string      GetPassword() const    { return this->_password;      }
			std::string      GetLastLogin() const   { return this->_last_login;    }
			std::string      GetLastLoginIp() const { return this->_last_login_ip; }
			std::string      GetCreatedAt() const   { return this->_created_at;    }
			std::vector<int> GetFriends() const     { return this->_friends;       }
			bool             isVerified() const     { return this->_verified;      }
			bool             isRestricted() const   { return this->_restricted;      }
			
			bool SetProfileId(int profileid);
			bool SetProfileId(const std::string& str_profileid);
			bool SetUserId(int userid);
			bool SetUserId(const std::string& str_userid);
			bool SetNick(const std::string& nick);
			bool SetUniquenick(const std::string& uniquenick);
			bool SetUniquenickWithoutClanTag(const std::string& uniquenick);
			bool SetEmail(const std::string& email);
			bool SetPassword(const std::string& password);
			bool SetMD5Password(const std::string& password);
			bool SetLastLogin(MYSQL_TIME last_login);
			bool SetLastLoginIp(const std::string& last_login_ip);
			bool SetCreatedAt(MYSQL_TIME created_at);
			bool SetVerified(bool verified);
			bool SetRestricted(bool restricted);
			
			bool AddFriend(int profileid);
	};
}

#endif // BATTLEFIELD_PLAYER_H