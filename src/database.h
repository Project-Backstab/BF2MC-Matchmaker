#ifndef DATABASE_H
#define DATABASE_H

#include <mutex>
#include <vector>

#include <mysql/mysql.h>

struct DBUser
{
	int profileid = -1;
	int userid = -1;
	std::string nick = "";
	std::string uniquenick = "";
	std::string email = "";
	std::string password = "";
};

struct DBUserFriend
{
	int profileid = -1;
	int target_profileid = -1;
};

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
		// DBUser
		bool queryDBUserByProfileid(DBUser& dbuser, const std::string profileid);
		bool queryDBUserByUniquenick(DBUser& dbuser, const std::string &uniquenick);
		bool queryDBUsersByEmail(std::vector<DBUser>& dbusers, const std::string &email);
		bool queryDBUserNewUserID(int &userid);
		bool insertDBUser(const DBUser& dbuser);
		
		// DBUserFriends
		bool queryDBUsersFriendsByProfileid(std::vector<DBUserFriend>& dbuserfriends, const std::string profileid);
		bool insertDBUserFriend(const DBUserFriend& dbuserfriend);
		bool removeDBUserFriend(const DBUserFriend& dbuserfriend);
		
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
