#include <iostream>
#include <mysql/mysql.h>
#include <string>

#include <settings.h>
#include <logger.h>
#include <util.h>

#include <database.h>

bool Database::queryLeaderboardRatio(Battlefield::RankPlayers& rank_players, const std::string& k, const std::string& s,
		uint32_t limit, uint32_t offset)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock
	
	std::string query = "";
	query += "SELECT ";
	query += "	`rank`, `profileid`, `uniquenick`, `ratio`, `" + k + "`, `" + s + "` ";
	query += "FROM ";
	query += "	`Leaderboard_ratio_" + k + "_" + s + "` ";
	query += "ORDER BY ";
	query += "	`rank` ASC, ";
	query += "	`ratio` DESC ";
	query += "LIMIT ? OFFSET ?";
	
	int      output_rank;
	int      output_profileid;
	char     output_uniquenick[VARCHAR_LEN(32)];
	int      output_ratio;
	uint32_t output_k;
	uint32_t output_s;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &limit;
	input_bind[0].is_unsigned = true;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = &offset;
	input_bind[1].is_unsigned = true;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(6, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_rank;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = &output_profileid;
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_uniquenick;
	output_bind[2].buffer_length = VARCHAR_LEN(32);
	output_bind[3].buffer_type = MYSQL_TYPE_LONG;
	output_bind[3].buffer = &output_ratio;
	output_bind[3].is_unsigned = false;
	output_bind[4].buffer_type = MYSQL_TYPE_LONG;
	output_bind[4].buffer = &output_k;
	output_bind[4].is_unsigned = true;
	output_bind[5].buffer_type = MYSQL_TYPE_LONG;
	output_bind[5].buffer = &output_s;
	output_bind[5].is_unsigned = true;
	
	// Prepare and execute with binds
	MYSQL_STMT* statement;
	
	if(
		!this->_init(&statement) ||
		!this->_prepare(statement, query, input_bind) ||
		!this->_execute(statement, output_bind)
	)
	{
		// Cleanup
		free(input_bind);
		free(output_bind);
		
		return false;
	}
	
	// Fetch and process rows
	while (true)
	{
		int status = mysql_stmt_fetch(statement);
		
		if (status == 1 || status == MYSQL_NO_DATA)
			break;
		
		Battlefield::Player player;
		
		player.SetProfileId(output_profileid);
		player.SetUniquenick(output_uniquenick);
		
		auto it = Battlefield::PlayerStats::SetterMap.find(k);
		if (it != Battlefield::PlayerStats::SetterMap.end()) {
			it->second(player, output_k);
		}

		it = Battlefield::PlayerStats::SetterMap.find(s);
		if (it != Battlefield::PlayerStats::SetterMap.end()) {
			it->second(player, output_s);
		}
		
		rank_players.insert(std::make_pair(output_rank, player));
	}
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
		
	return true;
}

bool Database::queryLeaderboardRatioByProfileid(Battlefield::RankPlayers& rank_players, int profileid, const std::string& k,
		const std::string& s)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "SELECT `rp`.* ";
	query += "FROM ";
	query += "	`Leaderboard_ratio_" + k + "_" + s + "` AS `rp` ";
	query += "JOIN ( ";
	query += "	SELECT ";
	query += "		`profileid`, ";
	query += "		`rank` AS `start_rank` ";
	query += "	FROM ";
	query += "		`Leaderboard_ratio_" + k + "_" + s + "` ";
	query += "	WHERE ";
	query += "		`profileid` = ? ";
	query += ") AS `start` ON rp.rank >= CASE WHEN start.start_rank <= 4 THEN 1 ELSE start.start_rank - 4 END ";
	query += "JOIN ( ";
	query += "	SELECT ";
	query += "		`profileid`, ";
	query += "		`rank` AS `end_rank` ";
	query += "	FROM ";
	query += "		`Leaderboard_ratio_" + k + "_" + s + "` ";
	query += "	WHERE ";
	query += "		`profileid` = ? ";
	query += ") AS `end` ON rp.rank <= CASE WHEN end.end_rank <= 4 THEN 10 ELSE end.end_rank + 5 END ";
	query += "ORDER BY ";
	query += "	`rank` ASC, ";
	query += "	`ratio` DESC";
	
	int input_profileid = profileid;
	
	int      output_rank;
	int      output_profileid;
	char     output_uniquenick[VARCHAR_LEN(32)];
	int      output_ratio;
	uint32_t output_k;
	uint32_t output_s;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_profileid;
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = &input_profileid;
	input_bind[1].is_unsigned = false;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(6, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_rank;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = &output_profileid;
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_uniquenick;
	output_bind[2].buffer_length = VARCHAR_LEN(32);
	output_bind[3].buffer_type = MYSQL_TYPE_LONG;
	output_bind[3].buffer = &output_ratio;
	output_bind[3].is_unsigned = false;
	output_bind[4].buffer_type = MYSQL_TYPE_LONG;
	output_bind[4].buffer = &output_k;
	output_bind[4].is_unsigned = true;
	output_bind[5].buffer_type = MYSQL_TYPE_LONG;
	output_bind[5].buffer = &output_s;
	output_bind[5].is_unsigned = true;
	
	// Prepare and execute with binds
	MYSQL_STMT* statement;
	
	if(
		!this->_init(&statement) ||
		!this->_prepare(statement, query, input_bind) ||
		!this->_execute(statement, output_bind)
	)
	{
		// Cleanup
		free(input_bind);
		free(output_bind);
		
		return false;
	}
	
	// Fetch and process rows
	while (true)
	{
		int status = mysql_stmt_fetch(statement);
		
		if (status == 1 || status == MYSQL_NO_DATA)
			break;
		
		Battlefield::Player player;
		
		player.SetProfileId(output_profileid);
		player.SetUniquenick(output_uniquenick);
		
		auto it = Battlefield::PlayerStats::SetterMap.find(k);
		if (it != Battlefield::PlayerStats::SetterMap.end()) {
			it->second(player, output_k);
		}

		it = Battlefield::PlayerStats::SetterMap.find(s);
		if (it != Battlefield::PlayerStats::SetterMap.end()) {
			it->second(player, output_s);
		}
		
		rank_players.insert(std::make_pair(output_rank, player));
	}
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
		
	return true;
}

bool Database::queryLeaderboardRatioByFriends(Battlefield::RankPlayers& rank_players, const std::vector<int>& friends,
		const std::string& k, const std::string& s)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "SELECT ";
	query += "	ROW_NUMBER() OVER (";
	query += "		ORDER BY ";
	query += "			PlayerStats." + k + " / PlayerStats." + s + " DESC ";
	query += "	) AS `rank`, ";
	query += "	Players.profileid                             AS `profileid`, ";
	query += "	Players.uniquenick                            AS `uniquenick`, ";
	query += "	PlayerStats." + k + " / PlayerStats." + s + " AS `ratio`, ";
	query += "	PlayerStats." + k + "                         AS `" + k + "`, ";
	query += "	PlayerStats." + s + "                         AS `" + s + "` ";
	query += "FROM ";
	query += "	`Players`, ";
	query += "	`PlayerStats` ";
	query += "WHERE ";
	query += "	Players.profileid = PlayerStats.profileid  AND ";
	query += "	PlayerStats." + k + " != 0 AND ";
	query += "	PlayerStats." + s + " != 0 ";
	query += "AND ";
	query += "	Players.profileid IN (" + Util::ToString(friends) + ") ";
	query += "ORDER BY ";
	query += "	`rank` ASC, ";
	query += "	`ratio` DESC ";
	query += "LIMIT 10;";
	
	int      output_rank;
	int      output_profileid;
	char     output_uniquenick[VARCHAR_LEN(32)];
	int      output_ratio;
	uint32_t output_k;
	uint32_t output_s;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(6, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_rank;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = &output_profileid;
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_uniquenick;
	output_bind[2].buffer_length = VARCHAR_LEN(32);
	output_bind[3].buffer_type = MYSQL_TYPE_LONG;
	output_bind[3].buffer = &output_ratio;
	output_bind[3].is_unsigned = false;
	output_bind[4].buffer_type = MYSQL_TYPE_LONG;
	output_bind[4].buffer = &output_k;
	output_bind[4].is_unsigned = true;
	output_bind[5].buffer_type = MYSQL_TYPE_LONG;
	output_bind[5].buffer = &output_s;
	output_bind[5].is_unsigned = true;

	// Prepare and execute with binds
	MYSQL_STMT* statement;
	
	if(
		!this->_init(&statement) ||
		!this->_prepare(statement, query) ||
		!this->_execute(statement, output_bind)
	)
	{
		// Cleanup
		free(output_bind);
		
		return false;
	}
	
	// Fetch and process rows
	while (true)
	{
		int status = mysql_stmt_fetch(statement);
		
		if (status == 1 || status == MYSQL_NO_DATA)
			break;
		
		Battlefield::Player player;
		
		player.SetProfileId(output_profileid);
		player.SetUniquenick(output_uniquenick);
		
		auto it = Battlefield::PlayerStats::SetterMap.find(k);
		if (it != Battlefield::PlayerStats::SetterMap.end()) {
			it->second(player, output_k);
		}

		it = Battlefield::PlayerStats::SetterMap.find(s);
		if (it != Battlefield::PlayerStats::SetterMap.end()) {
			it->second(player, output_s);
		}
		
		rank_players.insert(std::make_pair(output_rank, player));
	}
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(output_bind);
		
	return true;
}
