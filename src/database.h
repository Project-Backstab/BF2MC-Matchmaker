#ifndef DATABASE_H
#define DATABASE_H

#include <mutex>

#include <battlefield/player.h>
#include <battlefield/clan.h>
#include <battlefield/gameserver.h>
#include <battlefield/gamestat.h>

class MYSQL;
class MYSQL_STMT;
class MYSQL_BIND;

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
		bool queryPlayerByProfileid(Battlefield::Player& player);
		bool queryPlayerByUniquenick(Battlefield::Player& player);
		bool queryPlayersByEmail(Battlefield::Players& players, const std::string& email);
		bool queryPlayersByEmailAndUniquenick(Battlefield::Players& players, const std::string& email, const std::string& uniquenick);
		bool queryPlayerNewUserID(Battlefield::Player& player);
		bool updatePlayerLastLogin(Battlefield::Player& player, const std::string& ip);
		bool insertPlayer(Battlefield::Player& player);
		
		// Player friends
		bool queryPlayerFriends(Battlefield::Player& player);
		bool insertPlayerFriend(const Battlefield::Player& player, const Battlefield::Player& target_player);
		bool removePlayerFriend(const Battlefield::Player& player, const Battlefield::Player& target_player);
		
		// Player Stats
		bool queryPlayerStats(Battlefield::Player& player);
		bool updatePlayerStats(const Battlefield::Player& player);
		bool insertPlayerStats(const Battlefield::Player& player);
		
		// Clan
		bool queryClanByClanId(Battlefield::Clan& clan);
		bool queryClanByNameOrTag(Battlefield::Clan& clan);
		bool queryClanByPlayer(Battlefield::Clan& clan, const Battlefield::Player& player);
		bool insertClan(Battlefield::Clan& clan);
		bool updateClan(const Battlefield::Clan& clan);
		bool removeClan(const Battlefield::Clan& clan);
		
		// Clan Rank
		bool queryClanRanksByClanId(Battlefield::Clan& clan);
		bool insertClanRank(const Battlefield::Clan& clan, const Battlefield::Player& player, Battlefield::Clan::Ranks rank);
		bool updateClanRank(const Battlefield::Clan& clan, const Battlefield::Player& player, Battlefield::Clan::Ranks rank);
		bool removeClanRank(const Battlefield::Clan& clan, const Battlefield::Player& player);
		bool removeClanRanksByClanId(const Battlefield::Clan& clan);
		
		//Game Server
		bool queryGameServerByIp(Battlefield::GameServer& game_server);
		bool queryGameServerByIpAndPort(Battlefield::GameServer& game_server);
		bool queryGameServers(Battlefield::GameServers& game_servers);
		bool insertGameServer(Battlefield::GameServer& game_server);
		bool updateGameServer(const Battlefield::GameServer& game_server);
		
		//Game Server Player
		bool queryGameServerPlayers(Battlefield::GameServer& game_server);
		
	private:
		bool _insertGameServerPlayer(const Battlefield::GameServer& game_server, Battlefield::GameServerPlayer& gsplayer);
		bool _removeGameServerPlayers(const Battlefield::GameServer& game_server);
	
	public:
		// Game Stat
		bool queryGameStatsByDate(Battlefield::GameStats& game_stats, const std::string date);
		bool queryGameStatById(Battlefield::GameStat& game_stat);
		bool insertGameStat(Battlefield::GameStat& game_stat);
	
		// Game Stat Player
		bool queryGameStatPlayers(Battlefield::GameStat& game_stat);
	private:
		bool _insertGameStatPlayer(const Battlefield::GameStat& game_stat, Battlefield::GameStatPlayer& gsplayer);
	
	public:
		// Leaderboard - Top
		bool queryLeaderboardTopByRank(Battlefield::RankPlayers& rank_players,
				uint32_t limit = 10, uint32_t offset = 0);
		bool queryLeaderboardTopByType(Battlefield::RankPlayers& rank_players, const std::string& type,
				uint32_t limit = 10, uint32_t offset = 0);
		bool queryLeaderboardTopByRatio(Battlefield::RankPlayers& rank_players, const std::string& k, const std::string& s, 
				uint32_t limit = 10, uint32_t offset = 0);
		
		// Leaderboard - Self
		bool queryLeaderboardSelfByRank(Battlefield::RankPlayers& rank_players, int profileid);
		bool queryLeaderboardSelfByType(Battlefield::RankPlayers& rank_players, const std::string& type, int profileid);
		bool queryLeaderboardSelfByRatio(Battlefield::RankPlayers& rank_players, int profileid, const std::string& k,
				const std::string& s);
		
		// Leaderboard - Top Friends
		bool queryLeaderboardTopFriendsByRank(Battlefield::RankPlayers& rank_players, const std::vector<int>& friends);
		bool queryLeaderboardTopFriendsByType(Battlefield::RankPlayers& rank_players, const std::string& type,
				const std::vector<int>& friends);
		bool queryLeaderboardTopFriendsByRatio(Battlefield::RankPlayers& rank_players, const std::vector<int>& friends,
				const std::string& k, const std::string& s);
		
		bool createLeaderboards();
		
		/*
			Events
		*/
		void OnDatabaseStart();
	
	private:
		bool _connect();
		bool _init(MYSQL_STMT** statement);
		bool _prepare(MYSQL_STMT* statement, const std::string query);
		bool _prepare(MYSQL_STMT* statement, const std::string query, MYSQL_BIND* input_bind);
		bool _execute(MYSQL_STMT* statement);
		bool _execute(MYSQL_STMT* statement, MYSQL_BIND* output_bind);
};

#endif // DATABASE_H
