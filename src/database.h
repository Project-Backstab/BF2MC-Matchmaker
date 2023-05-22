#ifndef DATABASE_H
#define DATABASE_H

#include <mutex>
#include <vector>

#include <mysql/mysql.h>

struct DBUser {
	int profileid;
	int userid;
	std::string nick;
	std::string uniquenick;
	std::string email;
	std::string password;
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
		DBUser queryUserByUniquenick(const std::string &uniquenick);
		
		/*
			Events
		*/
		void OnDatabaseStart();
	
	private:
		void _doQuery(const std::string &query);
};

#endif // DATABASE_H
