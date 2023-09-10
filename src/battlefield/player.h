#ifndef BATTLEFIELD_PLAYER_H
#define BATTLEFIELD_PLAYER_H

#include <string>
#include <vector>
#include <map>

#include <battlefield/playerstats.h>

/*
	Forward declarations
*/
struct MYSQL_TIME;

namespace Battlefield
{
	/*
		Forward declarations
	*/
	class Player;
	
	/*
		Type definitions
	*/
	typedef std::vector<Battlefield::Player> Players;
	typedef std::map<int, Battlefield::Player> RankPlayers;
	
	/*
		Player
	*/
	class Player : public PlayerStats
	{
		private:
			int                 _profileid     = -1;
			int                 _userid        = -1;
			std::string         _nick          = "";
			std::string         _uniquenick    = "";
			std::string         _email         = "";
			std::string         _password      = "";
			std::string         _last_login    = "";
			std::string         _last_login_ip = "";
			std::string         _created_at    = "";
			std::vector<int>    _friends;
		
		public:
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
			
			bool SetProfileId(int profileid);
			bool SetProfileId(const std::string& str_profileid);
			bool SetUserId(int userid);
			bool SetUserId(const std::string& str_userid);
			bool SetNick(const std::string& nick);
			bool SetUniquenick(const std::string& uniquenick);
			bool SetEmail(const std::string& email);
			bool SetPassword(const std::string& password);
			bool SetMD5Password(const std::string& password);
			bool SetLastLogin(MYSQL_TIME last_login);
			bool SetLastLoginIp(const std::string& last_login_ip);
			bool SetCreatedAt(MYSQL_TIME created_at);
			bool AddFriend(int profileid);
	};
}

#endif // BATTLEFIELD_PLAYER_H