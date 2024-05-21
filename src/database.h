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
		/**
		 * @brief Pointer to the MySQL database connection.
		 * 
		 * This pointer represents the connection to the MySQL database.
		 * It is used internally by the Database class to perform database operations.
		 */
		MYSQL*              _connection;

		/**
		 * @brief Mutex for thread-safe access to the database connection.
		 * 
		 * This mutable mutex is used to ensure thread safety when accessing
		 * the database connection represented by _connection. It is mutable
		 * to allow locking and unlocking operations even on const member functions.
		 */
		mutable std::mutex  _mutex;
		
	public:
		Database();
		~Database();
		
		/*
			Queries
		*/
		// Player
		/**
		 * @brief Queries a player by their profile ID.
		 * 
		 * This function retrieves information about a player from the Battlefield database
		 * based on their profile ID. It requires that the player's profile ID is set before
		 * executing this call using the SetProfileId function.
		 * 
		 * @param player The player object to query. Requires the profile ID to be set.
		 * @return True if the player information is successfully queried, false otherwise.
		 * 
		 * @pre The player's profile ID must be set using the SetProfileId function.
		 */
		bool queryPlayerByProfileId(Battlefield::Player& player);

		/**
		 * @brief Queries a player by their unique nickname.
		 * 
		 * This function retrieves information about a player from the Battlefield database
		 * based on their unique nickname. It requires that the player's unique nickname
		 * is set before executing this call using the SetUniquenick function.
		 * 
		 * @param player The player object to query. Requires the unique nickname to be set.
		 * @return True if the player information is successfully queried, false otherwise.
		 * 
		 * @pre The player's unique nickname must be set using the SetUniquenick function.
		 */
		bool queryPlayerByUniquenick(Battlefield::Player& player);

		/**
		 * @brief Queries players by their email addresses.
		 * 
		 * This function retrieves information about players from the Battlefield database
		 * based on their email addresses. It populates the provided vector with player objects
		 * matching the specified email address.
		 * 
		 * @param players A vector of player objects to populate with queried players.
		 * @param email The email address to query players by.
		 * @return True if player information is successfully queried, false otherwise.
		 */
		bool queryPlayersByEmail(Battlefield::Players& players, const std::string& email);

		/**
		 * @brief Queries players by their email addresses or unique nicknames.
		 * 
		 * This function retrieves information about players from the Battlefield database
		 * based on either their email addresses or unique nicknames. It populates the provided
		 * vector with player objects matching the specified email address or unique nickname.
		 * 
		 * @param players A vector of player objects to populate with queried players.
		 * @param email The email address to query players by. Can be empty if querying by uniquenick.
		 * @param uniquenick The unique nickname to query players by. Can be empty if querying by email.
		 * @return True if player information is successfully queried, false otherwise.
		 * 
		 * @note Either email or uniquenick must be provided for the query. If both are provided,
		 *       email takes precedence.
		 */
		bool queryPlayersByEmailOrUniquenick(Battlefield::Players& players, const std::string& email, const std::string& uniquenick);

		/**
		 * @brief Queries a new user ID for a player.
		 * 
		 * This function retrieves a new user ID for a player from the Battlefield database.
		 * It calculates the new user ID by taking the highest existing user ID and adding 1.
		 * 
		 * @param player The player object for which to query the new user ID.
		 * @return True if the new user ID is successfully queried, false otherwise.
		 */
		bool queryPlayerNewUserID(Battlefield::Player& player);

		/**
		 * @brief Updates the last login IP address and date for a player.
		 * 
		 * This function saves the last login IP address and date to the database
		 * for the specified player. It requires that the player's profile ID is set
		 * before executing this call using the SetProfileId function.
		 * 
		 * @param player The player object for which to update the last login information.
		 *        Requires the profile ID to be set.
		 * @param ip The IP address from which the player last logged in.
		 * @return True if the last login information is successfully updated, false otherwise.
		 * 
		 * @pre The player's profile ID must be set using the SetProfileId function.
		 */
		bool updatePlayerLastLogin(Battlefield::Player& player, const std::string& ip);

		/**
		 * @brief Inserts a player into the database.
		 * 
		 * This function inserts the specified player into the Battlefield database.
		 * It requires that the player's profile ID is set before executing this call
		 * using the SetProfileId function.
		 * 
		 * @param player The player object to be inserted into the database.
		 *        Requires the profile ID to be set.
		 * @return True if the player is successfully inserted into the database, false otherwise.
		 * 
		 * @pre The player's profile ID must be set using the SetProfileId function.
		 */
		bool insertPlayer(Battlefield::Player& player);
		
		// Player friends
		/**
		 * @brief Queries a player's friends by their profile ID.
		 * 
		 * This function retrieves information about a player's friends from the Battlefield database
		 * based on their profile ID. It populates the player's friend list with the queried friends.
		 * 
		 * @param player The player object whose friends to query. Requires the profile ID to be set.
		 * @return True if the player's friends are successfully queried, false otherwise.
		 * 
		 * @pre The player's profile ID must be set using the SetProfileId function.
		 */
		bool queryPlayerFriendsByProfileId(Battlefield::Player& player);

		/**
		 * @brief Inserts a friendship relation between two players into the database.
		 * 
		 * This function inserts a friendship relation between the specified player and the target player
		 * into the Battlefield database. It represents the player adding the target player as a friend.
		 * 
		 * @param player The player object representing the user initiating the friend request.
		 * @param target_player The player object representing the user being added as a friend.
		 * @return True if the friendship relation is successfully inserted into the database, false otherwise.
		 */
		bool insertPlayerFriend(const Battlefield::Player& player, const Battlefield::Player& target_player);

		/**
		 * @brief Removes a friendship relation between two players from the database.
		 * 
		 * This function removes the friendship relation between the specified player and the target player
		 * from the Battlefield database. It represents the player removing the target player from their friends list.
		 * 
		 * @param player The player object representing the user removing the friend.
		 * @param target_player The player object representing the user being removed from the friend list.
		 * @return True if the friendship relation is successfully removed from the database, false otherwise.
		 */
		bool removePlayerFriend(const Battlefield::Player& player, const Battlefield::Player& target_player);
		
		// Player Stats
		/**
		 * @brief Queries the statistics of a player by their profile ID.
		 * 
		 * This function retrieves statistical information about a player from the Battlefield database
		 * based on their profile ID. It populates the player object with the queried statistics.
		 * 
		 * @param player The player object for which to query the statistics. Requires the profile ID to be set.
		 * @return True if the player statistics are successfully queried, false otherwise.
		 * 
		 * @pre The player's profile ID must be set using the SetProfileId function.
		 */
		bool queryPlayerStatsByProfileId(Battlefield::Player& player);

		/**
		 * @brief Updates the statistics of a player in the database.
		 * 
		 * This function updates the statistical information of the specified player
		 * in the Battlefield database. It represents the player's updated statistics
		 * being stored in the database.
		 * 
		 * @param player The player object whose statistics to update. Must be of type Battlefield::Player, which inherits from Battlefield::PlayerStats.
		 * @return True if the player statistics are successfully updated in the database, false otherwise.
		 */
		bool updatePlayerStats(const Battlefield::Player& player);

		/**
		 * @brief Inserts the statistics of a player into the database.
		 * 
		 * This function inserts the statistical information of the specified player
		 * into the Battlefield database. It represents the initial insertion of the player's
		 * statistics into the database.
		 * 
		 * @param player The player object whose statistics to insert. Must be of type Battlefield::Player, which inherits from Battlefield::PlayerStats.
		 * @return True if the player statistics are successfully inserted into the database, false otherwise.
		 */
		bool insertPlayerStats(const Battlefield::Player& player);
		
		// Clan
		/**
		 * @brief Queries a clan by its clan ID.
		 * 
		 * This function retrieves information about a clan from the Battlefield database
		 * based on its clan ID. It populates the provided clan object with the queried clan data.
		 * 
		 * @param clan The clan object to populate with the queried clan data. Requires the clan ID to be set using SetClanId.
		 * @return True if the clan information is successfully queried, false otherwise.
		 * 
		 * @pre The clan's ID must be set using the SetClanId function.
		 */
		bool queryClanByClanId(Battlefield::Clan& clan);

		/**
		 * @brief Queries a clan by a player's profile ID.
		 * 
		 * This function retrieves information about the clan of a player from the Battlefield database
		 * based on the player's profile ID. It populates the provided clan object with the queried clan data.
		 * 
		 * @param clan The clan object to populate with the queried clan data.
		 * @param player The player object whose clan information to query. Requires the player's profile ID to be set using SetProfileId.
		 * @return True if the clan information is successfully queried, false otherwise.
		 * 
		 * @pre The player's profile ID must be set using the SetProfileId function.
		 */
		bool queryClanByProfileId(Battlefield::Clan& clan, const Battlefield::Player& player);
		
		/**
		 * @brief Queries a clan by its name or tag.
		 * 
		 * This function retrieves information about a clan from the Battlefield database
		 * based on its name or tag. It populates the provided clan object with the queried clan data.
		 * 
		 * @param clan The clan object to populate with the queried clan data.
		 *        Requires either the clan's name or tag to be set using SetName or SetTag.
		 * @return True if the clan information is successfully queried, false otherwise.
		 */
		bool queryClanByNameOrTag(Battlefield::Clan& clan);

		/**
		 * @brief Inserts a clan into the database.
		 * 
		 * This function inserts the specified clan into the Battlefield database.
		 * 
		 * @param clan The clan object to insert into the database.
		 * @return True if the clan is successfully inserted into the database, false otherwise.
		 */
		bool insertClan(Battlefield::Clan& clan);

		/**
		 * @brief Updates a clan in the database.
		 * 
		 * This function updates the specified clan in the Battlefield database.
		 * 
		 * @param clan The clan object to update in the database.
		 * @return True if the clan is successfully updated in the database, false otherwise.
		 */
		bool updateClan(const Battlefield::Clan& clan);

		/**
		 * @brief Removes a clan from the database.
		 * 
		 * This function removes the specified clan from the Battlefield database.
		 * 
		 * @param clan The clan object to remove from the database.
		 * @return True if the clan is successfully removed from the database, false otherwise.
		 */
		bool removeClan(const Battlefield::Clan& clan);
		
		// Clan Rank
		/**
		 * @brief Queries the ranks of a clan by its clan ID.
		 * 
		 * This function retrieves information about the ranks of a clan from the Battlefield database
		 * based on its clan ID. It populates the provided clan object with the queried rank data.
		 * 
		 * @param clan The clan object to populate with the queried rank data. Requires the clan ID to be set using SetClanId.
		 * @return True if the clan rank information is successfully queried, false otherwise.
		 * 
		 * @pre The clan's ID must be set using the SetClanId function.
		 */
		bool queryClanRanksByClanId(Battlefield::Clan& clan);

		/**
		 * @brief Inserts a rank for a player within a clan into the database.
		 * 
		 * This function inserts a rank for the specified player within the specified clan
		 * into the Battlefield database.
		 * 
		 * @param clan The clan object representing the clan to which the player belongs.
		 *        Requires the clan ID to be set using SetClanId.
		 * @param player The player object representing the player for whom to insert the rank.
		 *        Requires the player's profile ID to be set using SetProfileId.
		 * @param rank The rank to insert for the player within the clan.
		 * @return True if the rank is successfully inserted into the database, false otherwise.
		 */
		bool insertClanRank(const Battlefield::Clan& clan, const Battlefield::Player& player, Battlefield::Clan::Ranks rank);

		/**
		 * @brief Updates the rank of a player within a clan in the database.
		 * 
		 * This function updates the rank of the specified player within the specified clan
		 * in the Battlefield database.
		 * 
		 * @param clan The clan object representing the clan to which the player belongs.
		 *        Requires the clan ID to be set using SetClanId.
		 * @param player The player object representing the player for whom to update the rank.
		 *        Requires the player's profile ID to be set using SetProfileId.
		 * @param rank The new rank to assign to the player within the clan.
		 * @return True if the rank is successfully updated in the database, false otherwise.
		 */
		bool updateClanRank(const Battlefield::Clan& clan, const Battlefield::Player& player, Battlefield::Clan::Ranks rank);

		/**
		 * @brief Removes the rank of a player within a clan from the database.
		 * 
		 * This function removes the rank of the specified player within the specified clan
		 * from the Battlefield database.
		 * 
		 * @param clan The clan object representing the clan from which to remove the rank.
		 *        Requires the clan ID to be set using SetClanId.
		 * @param player The player object representing the player for whom to remove the rank.
		 *        Requires the player's profile ID to be set using SetProfileId.
		 * @return True if the rank is successfully removed from the database, false otherwise.
		 */
		bool removeClanRank(const Battlefield::Clan& clan, const Battlefield::Player& player);
		
		/**
		 * @brief Removes all ranks associated with a clan from the database.
		 * 
		 * This function removes all ranks associated with the specified clan
		 * from the Battlefield database.
		 * 
		 * @param clan The clan object representing the clan for which to remove all ranks.
		 *        Requires the clan ID to be set using SetClanId.
		 * @return True if all ranks associated with the clan are successfully removed from the database, false otherwise.
		 */
		bool removeClanRanksByClanId(const Battlefield::Clan& clan);
		
		//Game Server
		/**
		 * @brief Queries a game server by its IP address.
		 * 
		 * This function retrieves information about a game server from the Battlefield database
		 * based on its IP address. It populates the provided game server object with the queried data.
		 * 
		 * @param game_server The game server object to populate with the queried data. 
		 *        Requires the IP address to be set using SetIp.
		 * @return True if the game server information is successfully queried, false otherwise.
		 */
		bool queryGameServerByIp(Battlefield::GameServer& game_server);

		/**
		 * @brief Queries a game server by its IP address and port.
		 * 
		 * This function retrieves information about a game server from the Battlefield database
		 * based on its IP address and port. It populates the provided game server object with the queried data.
		 * 
		 * @param game_server The game server object to populate with the queried data. 
		 *        Requires the IP address to be set using SetIp and the port to be set using SetPort.
		 * @return True if the game server information is successfully queried, false otherwise.
		 */
		bool queryGameServerByIpAndPort(Battlefield::GameServer& game_server);

		/**
		 * @brief Queries all game servers from the database.
		 * 
		 * This function retrieves information about all game servers from the Battlefield database.
		 * It populates the provided vector of game servers with the queried data.
		 * 
		 * @param game_servers A vector of game server objects to populate with the queried data.
		 * @return True if the game servers information is successfully queried, false otherwise.
		 */
		bool queryGameServers(Battlefield::GameServers& game_servers);

		/**
		 * @brief Inserts a game server into the database.
		 * 
		 * This function inserts the specified game server into the Battlefield database.
		 * 
		 * @param game_server The game server object to insert into the database.
		 * @return True if the game server is successfully inserted into the database, false otherwise.
		 */
		bool insertGameServer(Battlefield::GameServer& game_server);

		/**
		 * @brief Updates a game server in the database.
		 * 
		 * This function updates the specified game server in the Battlefield database.
		 * 
		 * @param game_server The game server object to update in the database.
		 * @return True if the game server is successfully updated in the database, false otherwise.
		 */
		bool updateGameServer(const Battlefield::GameServer& game_server);
		
		//Game Server Player
		/**
		 * @brief Queries the players connected to a game server.
		 * 
		 * This function retrieves information about the players connected to the specified game server
		 * from the Battlefield database. It populates the provided game server object with the queried player data.
		 * 
		 * @param game_server The game server object representing the server whose players to query.
		 *        Requires the server ID to be set using SetId.
		 * @return True if the player information is successfully queried, false otherwise.
		 */
		bool queryGameServerPlayers(Battlefield::GameServer& game_server);
	
	private:
		/**
		 * @brief Inserts a game server player record into the database.
		 * 
		 * This function inserts a game server player record into the database based on the provided game server information.
		 * The `game_server` object must have its ID set using the SetId method before calling this function. Upon successful 
		 * insertion, the ID generated by the database will be set inside the `gsplayer` object.
		 * 
		 * @param game_server The game server object containing information about the game server.
		 *        Requires the game server ID to be set using the SetId method.
		 * @param gsplayer The game server player object to be inserted into the database.
		 * @return True if the game server player record is successfully inserted into the database, false otherwise.
		 */
		bool _insertGameServerPlayer(const Battlefield::GameServer& game_server, Battlefield::GameServerPlayer& gsplayer);

		/**
		 * @brief Removes game server players associated with a specific game server.
		 * 
		 * This function removes all game server player records associated with the specified game server from the database.
		 * The `game_server` object must have its ID set using the SetId method before calling this function.
		 * 
		 * @param game_server The game server object for which to remove associated game server players.
		 *        Requires the game server ID to be set using the SetId method.
		 * @return True if the game server players are successfully removed from the database, false otherwise.
		 */
		bool _removeGameServerPlayers(const Battlefield::GameServer& game_server);
	
	public:
		// Game Stat
		/**
		 * @brief Queries game statistics by date.
		 * 
		 * This function retrieves game statistics from the Battlefield database for a specific date.
		 * The date format must be in "dd-MM-yyyy".
		 * 
		 * @param game_stats A vector of game statistics objects to populate with the queried data.
		 * @param date The date for which to query the game statistics. Must be in "dd-MM-yyyy" format.
		 * @return True if the game statistics are successfully queried, false otherwise.
		 */
		bool queryGameStatsByDate(Battlefield::GameStats& game_stats, const std::string date);

		/**
		 * @brief Queries a game statistic by its ID.
		 * 
		 * This function retrieves information about a specific game statistic from the Battlefield database
		 * based on its ID. It populates the provided game statistic object with the queried data.
		 * 
		 * @param game_stat The game statistic object to populate with the queried data.
		 *        Requires the ID to be set using SetId.
		 * @return True if the game statistic information is successfully queried, false otherwise.
		 */
		bool queryGameStatById(Battlefield::GameStat& game_stat);

		/**
		 * @brief Inserts a game statistic into the database.
		 * 
		 * This function inserts the specified game statistic into the Battlefield database.
		 * 
		 * @param game_stat The game statistic object to insert into the database.
		 * @return True if the game statistic is successfully inserted into the database, false otherwise.
		 */
		bool insertGameStat(Battlefield::GameStat& game_stat);
	
		// Game Stat Player
		/**
		 * @brief Queries the players associated with a game statistic.
		 * 
		 * This function retrieves information about the players associated with the specified game statistic
		 * from the Battlefield database. It populates the provided game statistic object with the queried player data.
		 * 
		 * @param game_stat The game statistic object representing the statistic whose players to query.
		 *        Requires the statistic ID to be set using SetId.
		 * @return True if the player information is successfully queried, false otherwise.
		 */
		bool queryGameStatPlayers(Battlefield::GameStat& game_stat);
		
		/**
		 * @brief Queries game statistics for a player by their profile ID.
		 *
		 * This function retrieves game statistics for a player identified by their profile ID.
		 *
		 * @param player The player object containing the profile ID to query.
		 * @param gsplayers[out] Reference to a Battlefield::GameStatPlayers object where the result will be stored.
		 * @return bool True if the query is successful and game statistics are found for the player, false otherwise.
		 */
		bool queryGameStatPlayersByProfileId(const Battlefield::Player& player, Battlefield::GameStatPlayers& gsplayers);

	private:
		/**
		 * @brief Inserts a game stat player record into the database.
		 * 
		 * This function inserts a game stat player record into the database based on the provided game stat information.
		 * Upon successful insertion, the ID generated by the database will be set inside the `gsplayer` object.
		 * 
		 * @param game_stat The game stat object containing information about the game.
		 * @param gsplayer The game stat player object to be inserted into the database.
		 * @return True if the game stat player record is successfully inserted into the database, false otherwise.
		 */
		bool _insertGameStatPlayer(const Battlefield::GameStat& game_stat, Battlefield::GameStatPlayer& gsplayer);
	
	public:
		// Leaderboard Rank
		/**
		 * @brief Queries the leaderboard rank of players.
		 * 
		 * This function retrieves information about the leaderboard rank of players from the Battlefield database.
		 * It populates the provided vector of rank players with the queried data.
		 * 
		 * @param rank_players A vector of rank player objects to populate with the queried data.
		 * @param limit The maximum number of rank players to retrieve (default is 10).
		 * @param offset The offset for pagination (default is 0).
		 * @return True if the leaderboard rank information is successfully queried, false otherwise.
		 */
		bool queryLeaderboardRank(Battlefield::RankPlayers& rank_players,
				uint32_t limit = 10, uint32_t offset = 0);
		
		/**
		 * @brief Queries the leaderboard rank of players by profile ID.
		 * 
		 * This function retrieves information about the leaderboard rank of players with the specified profile ID
		 * from the Battlefield database. It populates the provided vector of rank players with the queried data.
		 * The function always returns 10 players, with the specified profile ID located in the middle of the list if possible.
		 * 
		 * @param rank_players A vector of rank player objects to populate with the queried data.
		 * @param profileid The profile ID of the player whose leaderboard rank to query.
		 * @return True if the leaderboard rank information is successfully queried, false otherwise.
		 */
		bool queryLeaderboardRankByProfileId(Battlefield::RankPlayers& rank_players, int profileid);

		/**
		 * @brief Queries the leaderboard rank of players by their friends' profile IDs.
		 * 
		 * This function retrieves information about the leaderboard rank of players who are friends
		 * of the specified list of profile IDs from the Battlefield database. It populates the provided
		 * vector of rank players with the queried data.
		 * 
		 * @param rank_players A vector of rank player objects to populate with the queried data.
		 * @param friends A vector containing the profile IDs of the friends whose leaderboard ranks to query.
		 * @return True if the leaderboard rank information is successfully queried, false otherwise.
		 */
		bool queryLeaderboardRankByFriends(Battlefield::RankPlayers& rank_players, const std::vector<int>& friends);
		
		
		// Leaderboard Type
		/**
		 * @brief Queries the leaderboard rank of players by player stat type.
		 * 
		 * This function retrieves information about the leaderboard rank of players based on a specific
		 * player stat type from the Battlefield database. It populates the provided vector of rank players
		 * with the queried data.
		 * 
		 * @param rank_players A vector of rank player objects to populate with the queried data.
		 * @param type The player stat property to filter on.
		 * @param limit The maximum number of rank players to retrieve (default is 10).
		 * @param offset The offset for pagination (default is 0).
		 * @return True if the leaderboard rank information is successfully queried, false otherwise.
		 */
		bool queryLeaderboardType(Battlefield::RankPlayers& rank_players, const std::string& type,
				uint32_t limit = 10, uint32_t offset = 0);
		
		/**
		 * @brief Queries the leaderboard rank of players by player stat type with a profile ID in the middle.
		 * 
		 * This function retrieves information about the leaderboard rank of players based on a specific
		 * player stat type from the Battlefield database. It populates the provided vector of rank players
		 * with the queried data. The function ensures that the specified profile ID is located in the middle
		 * of the returned list if possible.
		 * 
		 * @param rank_players A vector of rank player objects to populate with the queried data.
		 * @param type The player stat property to filter on.
		 * @param profileid The profile ID of the player to position in the middle of the list.
		 * @return True if the leaderboard rank information is successfully queried, false otherwise.
		 */
		bool queryLeaderboardTypeByProfileid(Battlefield::RankPlayers& rank_players, const std::string& type, int profileid);
		
		/**
		 * @brief Queries the leaderboard rank of players by player stat type filtered by friends.
		 * 
		 * This function retrieves information about the leaderboard rank of players based on a specific
		 * player stat type from the Battlefield database. It populates the provided vector of rank players
		 * with the queried data. The results are filtered based on the profile IDs of the specified friends.
		 * 
		 * @param rank_players A vector of rank player objects to populate with the queried data.
		 * @param type The player stat property to filter on.
		 * @param friends A vector containing the profile IDs of the friends to filter the results.
		 * @return True if the leaderboard rank information is successfully queried, false otherwise.
		 */
		bool queryLeaderboardTypeByFriends(Battlefield::RankPlayers& rank_players, const std::string& type,
				const std::vector<int>& friends);
		
		// Leaderboard Ratio
		/**
		 * @brief Queries the leaderboard rank of players by kills-to-spawns ratio.
		 * 
		 * This function retrieves information about the leaderboard rank of players based on the ratio
		 * of kills to total spawns from the Battlefield database. It populates the provided vector of rank players
		 * with the queried data. The ratio is calculated as kills divided by total spawns.
		 * 
		 * @param rank_players A vector of rank player objects to populate with the queried data.
		 * @param k The player stat property representing kills.
		 * @param s The player stat property representing total spawns.
		 * @param limit The maximum number of rank players to retrieve (default is 10).
		 * @param offset The offset for pagination (default is 0).
		 * @return True if the leaderboard rank information is successfully queried, false otherwise.
		 */
		bool queryLeaderboardRatio(Battlefield::RankPlayers& rank_players, const std::string& k, const std::string& s, 
				uint32_t limit = 10, uint32_t offset = 0);
		
		/**
		 * @brief Queries the leaderboard rank of players by kills-to-spawns ratio with a specified profile ID.
		 * 
		 * This function retrieves information about the leaderboard rank of players based on the ratio
		 * of kills to total spawns from the Battlefield database. It populates the provided vector of rank players
		 * with the queried data. The ratio is calculated as kills divided by total spawns. The specified profile ID
		 * is used to position the player in the middle of the list if possible.
		 * 
		 * @param rank_players A vector of rank player objects to populate with the queried data.
		 * @param profileid The profile ID of the player whose leaderboard rank to query.
		 * @param k The player stat property representing kills.
		 * @param s The player stat property representing total spawns.
		 * @return True if the leaderboard rank information is successfully queried, false otherwise.
		 */
		bool queryLeaderboardRatioByProfileid(Battlefield::RankPlayers& rank_players, int profileid, const std::string& k,
				const std::string& s);
		
		/**
		 * @brief Queries the leaderboard rank of players by kills-to-spawns ratio filtered by friends.
		 * 
		 * This function retrieves information about the leaderboard rank of players based on the ratio
		 * of kills to total spawns from the Battlefield database. It populates the provided vector of rank players
		 * with the queried data. The ratio is calculated as kills divided by total spawns. The results are filtered
		 * based on the profile IDs of the specified friends.
		 * 
		 * @param rank_players A vector of rank player objects to populate with the queried data.
		 * @param friends A vector containing the profile IDs of the friends to filter the results.
		 * @param k The player stat property representing kills.
		 * @param s The player stat property representing total spawns.
		 * @return True if the leaderboard rank information is successfully queried, false otherwise.
		 */
		bool queryLeaderboardRatioByFriends(Battlefield::RankPlayers& rank_players, const std::vector<int>& friends,
				const std::string& k, const std::string& s);
		

		/**
		 * @brief Creates leaderboard tables with ranked players.
		 * 
		 * This function generates leaderboard tables in the Battlefield database, populated with all ranked players.
		 * 
		 * @return True if the leaderboard tables are successfully created, false otherwise.
		 */
		bool createLeaderboards();
		
		// Leaderboard Clan
		/**
		 * @brief Queries clan leaderboard.
		 * 
		 * This function retrieves information about the clan leaderboard from the Battlefield database.
		 * It populates the provided vector of ranked clans with the queried data.
		 * 
		 * @param rank_clans A vector of ranked clan objects to populate with the queried data.
		 * @param limit The maximum number of ranked clans to retrieve (default is 10).
		 * @param offset The offset for pagination (default is 0).
		 * @return True if the clan leaderboard information is successfully queried, false otherwise.
		 */
		bool queryLeaderboardClan(Battlefield::RankClans& rank_clans, uint32_t limit = 10, uint32_t offset = 0);

		/**
		 * @brief Queries clan leaderboard by clan ID with the clan in the middle.
		 * 
		 * This function retrieves information about the clan leaderboard for a specific clan ID from the Battlefield database.
		 * It populates the provided vector of ranked clans with the queried data. The function ensures that the specified clan's
		 * rank is positioned in the middle of the list if possible.
		 * 
		 * @param rank_clans A vector of ranked clan objects to populate with the queried data.
		 * @param clan The clan object representing the clan whose leaderboard information to query.
		 *        Requires the clan ID to be set using SetClanId.
		 * @return True if the clan leaderboard information is successfully queried, false otherwise.
		 */
		bool queryLeaderboardClanByClanId(Battlefield::RankClans& rank_clans, const Battlefield::Clan& clan);

		// Chat
		/**
		 * @brief Inserts a chat message into the database.
		 * 
		 * This function inserts a chat message into the database, including the sender's profile ID, IP address,
		 * the recipient's profile ID, and the message content.
		 * 
		 * @param profileid The profile ID of the sender.
		 * @param ip The IP address of the sender.
		 * @param target_profileid The profile ID of the recipient.
		 * @param msg The content of the chat message.
		 * @return True if the chat message is successfully inserted into the database, false otherwise.
		 */
		bool insertChat(int profileid, const std::string& ip, int target_profileid, const std::string& msg);

		/*
			Events
		*/
		/**
		 * @brief Event function triggered when the database is started.
		 * 
		 * This function serves as an event handler that triggers when the database is started.
		 * 
		 * @note This function does not return any value.
		 */
		void OnDatabaseStart();
	
	private:
		/**
		 * @brief Establishes a connection to the database.
		 * 
		 * This function establishes a connection to the database using the database settings stored in the global settings object.
		 * It initializes the MySQL connection object and attempts to connect to the database server. If the connection is successful,
		 * the connection object `_connection` is updated with the connection information.
		 * 
		 * @return True if the connection to the database is successfully established, false otherwise.
		 */
		bool _connect();

		/**
		 * @brief Initializes a MySQL statement object.
		 * 
		 * This function initializes a MySQL statement object using the provided pointer to a statement object.
		 * If the MySQL connection is no longer alive, it reconnects to the database before initializing the statement.
		 * 
		 * @param statement A pointer to a MySQL statement object to be initialized.
		 * @return True if the statement object is successfully initialized, false otherwise.
		 */
		bool _init(MYSQL_STMT** statement);

		/**
		 * @brief Prepares a MySQL statement with a query.
		 * 
		 * This function prepares a MySQL statement with the provided query string using the specified statement object.
		 * If the preparation of the statement fails, an error message is logged, and any associated result set is freed.
		 * 
		 * @param statement The MySQL statement object to be prepared.
		 * @param query The query string to be prepared.
		 * @return True if the statement is successfully prepared with the query, false otherwise.
		 */
		bool _prepare(MYSQL_STMT* statement, const std::string query);

		/**
		 * @brief Prepares a MySQL statement with a query and binds input parameters.
		 * 
		 * This function prepares a MySQL statement with the provided query string using the specified statement object.
		 * It also binds input parameters specified in the `input_bind` array to the prepared statement.
		 * If the preparation of the statement or binding of input parameters fails, an error message is logged, and the statement is closed.
		 * 
		 * @param statement The MySQL statement object to be prepared and bound.
		 * @param query The query string to be prepared.
		 * @param input_bind An array of MySQL bind structures representing the input parameters to be bound to the statement.
		 * @return True if the statement is successfully prepared, and input parameters are bound, false otherwise.
		 */
		bool _prepare(MYSQL_STMT* statement, const std::string query, MYSQL_BIND* input_bind);
		
		/**
		 * @brief Executes a prepared MySQL statement.
		 * 
		 * This function executes a prepared MySQL statement using the specified statement object.
		 * If the execution of the statement fails, an error message is logged, and any associated result set is freed.
		 * 
		 * @param statement The MySQL statement object to be executed.
		 * @return True if the statement is successfully executed, false otherwise.
		 */
		bool _execute(MYSQL_STMT* statement);

		/**
		 * @brief Executes a prepared MySQL statement with output binding.
		 * 
		 * This function executes a prepared MySQL statement using the specified statement object.
		 * It also binds output parameters specified in the `output_bind` array to the executed statement.
		 * If the execution of the statement or binding of output parameters fails, an error message is logged,
		 * and the statement is closed.
		 * 
		 * @param statement The MySQL statement object to be executed and bound with output parameters.
		 * @param output_bind An array of MySQL bind structures representing the output parameters to be bound to the statement.
		 * @return True if the statement is successfully executed and output parameters are bound, false otherwise.
		 */
		bool _execute(MYSQL_STMT* statement, MYSQL_BIND* output_bind);
};

#endif // DATABASE_H
