#include <iostream>
#include <mysql/mysql.h>

#include <settings.h>
#include <logger.h>
#include <util.h>

#include <database.h>

bool Database::queryLeaderboardType(Battlefield::RankPlayers& rank_players, const std::string& type,
		uint32_t limit, uint32_t offset)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "SELECT ";
	query += "	`rank`, `profileid`, `uniquenick`, `" + type + "` ";
	query += "FROM ";
	query += "	`Leaderboard_" + type + "` ";
	query += "ORDER BY ";
	query += "	`rank` ASC, ";
	query += "	`" + type + "` DESC ";
	query += "LIMIT ? OFFSET ?";
	
	int      output_rank;
	int      output_profileid;
	char     output_uniquenick[VARCHAR_LEN(32)];
	uint32_t output_value;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &limit;
	input_bind[0].is_unsigned = true;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = &offset;
	input_bind[1].is_unsigned = true;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(4, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = const_cast<int*>(&output_rank);
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = const_cast<int*>(&output_profileid);
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_uniquenick;
	output_bind[2].buffer_length = VARCHAR_LEN(32);
	output_bind[3].buffer_type = MYSQL_TYPE_LONG;
	output_bind[3].buffer = const_cast<uint32_t*>(&output_value);
	output_bind[3].is_unsigned = true;
	
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
		
		if(type == "score")
		{
			player.SetScore(static_cast<int32_t>(output_value));
		}
		else
		{
			auto it = Battlefield::PlayerStats::SetterMap.find(type);
			if (it != Battlefield::PlayerStats::SetterMap.end()) {
				it->second(player, output_value);
			}
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

bool Database::queryLeaderboardTypeByProfileid(Battlefield::RankPlayers& rank_players, const std::string& type, int profileid)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "SELECT `rp`.* ";
	query += "FROM ";
	query += "	`Leaderboard_" + type + "` AS `rp` ";
	query += "JOIN ( ";
	query += "	SELECT ";
	query += "		`profileid`, ";
	query += "		`rank` AS `start_rank` ";
	query += "	FROM ";
	query += "		`Leaderboard_" + type + "` ";
	query += "	WHERE ";
	query += "		`profileid` = ? ";
	query += ") AS `start` ON rp.rank >= CASE WHEN start.start_rank <= 4 THEN 1 ELSE start.start_rank - 4 END ";
	query += "JOIN ( ";
	query += "	SELECT ";
	query += "		`profileid`, ";
	query += "		`rank` AS `end_rank` ";
	query += "	FROM ";
	query += "		`Leaderboard_" + type + "` ";
	query += "	WHERE ";
	query += "		`profileid` = ? ";
	query += ") AS `end` ON rp.rank <= CASE WHEN end.end_rank <= 4 THEN 10 ELSE end.end_rank + 5 END ";
	query += "ORDER BY ";
	query += "	`rank` ASC, ";
	query += "	`" + type + "` DESC";
	
	int input_profileid = profileid;
	
	int      output_rank;
	int      output_profileid;
	char     output_uniquenick[VARCHAR_LEN(32)];
	uint32_t output_value;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_profileid;
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = &input_profileid;
	input_bind[1].is_unsigned = false;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(4, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = const_cast<int*>(&output_rank);
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = const_cast<int*>(&output_profileid);
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_uniquenick;
	output_bind[2].buffer_length = VARCHAR_LEN(32);
	output_bind[3].buffer_type = MYSQL_TYPE_LONG;
	output_bind[3].buffer = const_cast<uint32_t*>(&output_value);
	output_bind[3].is_unsigned = true;

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
		
		if(type == "score")
		{
			player.SetScore(static_cast<int32_t>(output_value));
		}
		else
		{
			auto it = Battlefield::PlayerStats::SetterMap.find(type);
			if (it != Battlefield::PlayerStats::SetterMap.end()) {
				it->second(player, output_value);
			}
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

bool Database::queryLeaderboardTypeByFriends(Battlefield::RankPlayers& rank_players, const std::string& type,
		const std::vector<int>& friends)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "SELECT ";
	query += "	ROW_NUMBER() OVER (";
	query += "		ORDER BY ";
	query += "			`" + type + "` DESC ";
	query += "	) AS `rank`, ";
	query += "	Players.profileid  AS `profileid`, ";
	query += "	Players.uniquenick AS `uniquenick`, ";
	query += "	PlayerStats." + type + " AS '" + type + "' ";
	query += "FROM ";
	query += "	`Players`, ";
	query += "	`PlayerStats` ";
	query += "WHERE ";
	query += "	Players.profileid = PlayerStats.profileid ";
	query += "AND ";
	query += "	Players.profileid IN (" + Util::ToString(friends) + ") ";
	query += "ORDER BY ";
	query += "	`rank` ASC, ";
	query += "	`" + type + "` DESC ";
	query += "LIMIT 10;";
	
	int      output_rank;
	int      output_profileid;
	char     output_uniquenick[VARCHAR_LEN(32)];
	uint32_t output_value;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(4, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = const_cast<int*>(&output_rank);
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = const_cast<int*>(&output_profileid);
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_uniquenick;
	output_bind[2].buffer_length = VARCHAR_LEN(32);
	output_bind[3].buffer_type = MYSQL_TYPE_LONG;
	output_bind[3].buffer = const_cast<uint32_t*>(&output_value);
	output_bind[3].is_unsigned = true;

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
		
		if(type == "score")
		{
			player.SetScore(static_cast<int32_t>(output_value));
		}
		else
		{
			auto it = Battlefield::PlayerStats::SetterMap.find(type);
			if (it != Battlefield::PlayerStats::SetterMap.end()) {
				it->second(player, output_value);
			}
		}
		
		rank_players.insert(std::make_pair(output_rank, player));
	}
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(output_bind);
		
	return true;
}
