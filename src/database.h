#ifndef DATABASE_H
#define DATABASE_H

#include <mutex>
#include <vector>

#include <mysql/mysql.h>
#include <battlefield/player.h>

class Database
{
	private:
		MYSQL*              _connection;
		mutable std::mutex  _mutex;
		
	public:
		Database();
		~Database();
		
		/*
			Queries
		*/
		// Player
		bool queryPlayerByProfileid(Battlefield::Player& player, const std::string profileid);
		bool queryPlayerByUniquenick(Battlefield::Player& player, const std::string &uniquenick);
		bool queryPlayersByEmail(Battlefield::Players& players, const std::string &email);
		bool queryPlayerNewUserID(Battlefield::Player& player);
		bool insertPlayer(const Battlefield::Player& player);
		bool queryPlayerFriends(Battlefield::Player& player);
		bool insertPlayerFriend(int profileid, int target_profileid);
		bool removePlayerFriend(int profileid, int target_profileid);
		
		/*
			Events
		*/
		void OnDatabaseStart();
	
	private:
		void _doQuery(const std::string &query);
		bool _prepare(MYSQL_STMT* statement, const std::string query);
		bool _prepare(MYSQL_STMT* statement, const std::string query, MYSQL_BIND* input_bind);
		bool _execute(MYSQL_STMT* statement);
		bool _execute(MYSQL_STMT* statement, MYSQL_BIND* output_bind);
};

#endif // DATABASE_H
