#ifndef BATTLEFIELD_PLAYER_H
#define BATTLEFIELD_PLAYER_H

#include <string>
#include <vector>
#include <map>

#include <battlefield/playerstats.h>

namespace Battlefield
{
	/*
		Forward declair
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
			int                 _profileid = -1;
			int                 _userid = -1;
			std::string         _nick = "";
			std::string         _uniquenick = "";
			std::string         _email = "";
			std::string         _password = "";
			std::vector<int>    _friends;
		
		public:
			void useExample();
			
			int                    GetProfileId() const  { return this->_profileid;  }
			int                    GetUserId() const     { return this->_userid;     }
			const std::string      GetNick() const       { return this->_nick;       }
			const std::string      GetUniquenick() const { return this->_uniquenick; }
			const std::string      GetEmail() const      { return this->_email;      }
			const std::string      GetPassword() const   { return this->_password;   }
			const std::vector<int> GetFriends() const    { return this->_friends;    }
			
			bool SetProfileId(int profileid);
			bool SetProfileId(const std::string& str_profileid);
			bool SetUserId(int userid);
			bool SetNick(const std::string& nick);
			bool SetUniquenick(const std::string& uniquenick);
			bool SetEmail(const std::string& email);
			bool SetPassword(const std::string& password);
			bool SetMD5Password(const std::string& password);
			bool AddFriend(int profileid);
	};
}

#endif // BATTLEFIELD_PLAYER_H