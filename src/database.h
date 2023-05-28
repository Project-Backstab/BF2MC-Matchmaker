#ifndef DATABASE_H
#define DATABASE_H

#include <mutex>
#include <vector>

#include <mysql/mysql.h>
#include <battlefield/player.h>
#include <battlefield/clan.h>

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
		
		// Player friends
		bool queryPlayerFriends(Battlefield::Player& player);
		bool insertPlayerFriend(int profileid, int target_profileid);
		bool removePlayerFriend(int profileid, int target_profileid);
		
		// Player Stats
		bool queryPlayerStats(Battlefield::Player& player);
		bool updatePlayerStats(const Battlefield::Player& player);
		bool insertPlayerStats(const Battlefield::Player& player);
		
		// Clan
		bool queryClanByClanId(Battlefield::Clan& clan);
		bool queryClanByNameOrTag(Battlefield::Clan& clan);
		bool queryClanByPlayer(Battlefield::Clan& clan, const Battlefield::Player& player);
		bool queryClanRanksByClanId(Battlefield::Clan& clan);
		bool insertClan(const Battlefield::Clan& clan);
		bool updateClan(const Battlefield::Clan& clan);
		bool removeClan(const Battlefield::Clan& clan);
		
		// Clan Rank
		bool insertClanRank(const Battlefield::Clan& clan, const Battlefield::Player& player, int rank);
		bool updateClanRank(const Battlefield::Clan& clan, const Battlefield::Player& player, int rank);
		bool removeClanRank(const Battlefield::Clan& clan, const Battlefield::Player& player);
		bool removeClanRanksByClanId(const Battlefield::Clan& clan);
		
		/*
			Events
		*/
		void OnDatabaseStart();
	
	private:
		bool _prepare(MYSQL_STMT* statement, const std::string query);
		bool _prepare(MYSQL_STMT* statement, const std::string query, MYSQL_BIND* input_bind);
		bool _execute(MYSQL_STMT* statement);
		bool _execute(MYSQL_STMT* statement, MYSQL_BIND* output_bind);
};

#endif // DATABASE_H
