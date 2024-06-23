#include <iostream>
#include <mysql/mysql.h>

#include <settings.h>
#include <logger.h>
#include <util.h>

#include <database.h>

bool Database::queryLeaderboardClan(Battlefield::RankClans& rank_clans, uint32_t limit, uint32_t offset)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "SELECT ";
	query += "	`rank`, `clanid`, `name`, `tag`, `score`, `wins`, `losses`, `draws` ";
	query += "FROM ";
	query += "	`Leaderboard_clan` ";
	query += "ORDER BY ";
	query += "	`rank` ASC, ";
	query += "	`score` DESC, ";
	query += "	`wins` DESC, ";
	query += "	`losses` DESC, ";
	query += "	`draws` DESC  ";
	query += "LIMIT ? OFFSET ?;";
	
	int      output_rank;
	int      output_clanid;
	char     output_name[VARCHAR_LEN(32)];
	char     output_tag[VARCHAR_LEN(3)];
	uint32_t output_score;
	uint32_t output_wins;
	uint32_t output_losses;
	uint32_t output_draws;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &limit;
	input_bind[0].is_unsigned = true;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = &offset;
	input_bind[1].is_unsigned = true;

	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(8, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_rank;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = &output_clanid;
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_name;
	output_bind[2].buffer_length = VARCHAR_LEN(32);
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = &output_tag;
	output_bind[3].buffer_length = VARCHAR_LEN(3);
	output_bind[4].buffer_type = MYSQL_TYPE_LONG;
	output_bind[4].buffer = &output_score;
	output_bind[4].is_unsigned = true;
	output_bind[5].buffer_type = MYSQL_TYPE_LONG;
	output_bind[5].buffer = &output_wins;
	output_bind[5].is_unsigned = true;
	output_bind[6].buffer_type = MYSQL_TYPE_LONG;
	output_bind[6].buffer = &output_losses;
	output_bind[6].is_unsigned = true;
	output_bind[7].buffer_type = MYSQL_TYPE_LONG;
	output_bind[7].buffer = &output_draws;
	output_bind[7].is_unsigned = true;
	
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
		
		Battlefield::Clan clan;
		
		clan.SetClanId(output_clanid);
		clan.SetName(output_name);
		clan.SetTag(output_tag);
		clan.SetScore(output_score);
		clan.SetWins(output_wins);
		clan.SetLosses(output_losses);
		clan.SetDraws(output_draws);

		rank_clans.insert(std::make_pair(output_rank, clan));
	}
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
		
	return true;
}

bool Database::queryLeaderboardClanByClanId(Battlefield::RankClans& rank_clans, const Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "SELECT `rp`.* ";
	query += "FROM ";
	query += "	`Leaderboard_clan` AS `rp` ";
	query += "JOIN ( ";
	query += "	SELECT ";
	query += "		`clanid`, ";
	query += "		`rank` AS `start_rank` ";
	query += "	FROM ";
	query += "		`Leaderboard_clan` ";
	query += "	WHERE ";
	query += "		`clanid` = ? ";
	query += ") AS `start` ON rp.rank >= CASE WHEN start.start_rank <= 4 THEN 1 ELSE start.start_rank - 4 END ";
	query += "JOIN ( ";
	query += "	SELECT ";
	query += "		`clanid`, ";
	query += "		`rank` AS `end_rank` ";
	query += "	FROM ";
	query += "		`Leaderboard_clan` ";
	query += "	WHERE ";
	query += "		`clanid` = ? ";
	query += ") AS `end` ON rp.rank <= CASE WHEN end.end_rank <= 4 THEN 10 ELSE end.end_rank + 5 END ";
	query += "ORDER BY ";
	query += "	`rank` ASC";
	
	int      input_clanid = clan.GetClanId();
	
	int      output_rank;
	int      output_clanid;
	char     output_name[VARCHAR_LEN(32)];
	char     output_tag[VARCHAR_LEN(3)];
	uint32_t output_score;
	uint32_t output_wins;
	uint32_t output_losses;
	uint32_t output_draws;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_clanid;
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = &input_clanid;
	input_bind[1].is_unsigned = false;

	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(8, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_rank;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = &output_clanid;
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_name;
	output_bind[2].buffer_length = VARCHAR_LEN(32);
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = &output_tag;
	output_bind[3].buffer_length = VARCHAR_LEN(3);
	output_bind[4].buffer_type = MYSQL_TYPE_LONG;
	output_bind[4].buffer = &output_score;
	output_bind[4].is_unsigned = true;
	output_bind[5].buffer_type = MYSQL_TYPE_LONG;
	output_bind[5].buffer = &output_wins;
	output_bind[5].is_unsigned = true;
	output_bind[6].buffer_type = MYSQL_TYPE_LONG;
	output_bind[6].buffer = &output_losses;
	output_bind[6].is_unsigned = true;
	output_bind[7].buffer_type = MYSQL_TYPE_LONG;
	output_bind[7].buffer = &output_draws;
	output_bind[7].is_unsigned = true;
	
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
		
		Battlefield::Clan clan;
		
		clan.SetClanId(output_clanid);
		clan.SetName(output_name);
		clan.SetTag(output_tag);
		clan.SetScore(output_score);
		clan.SetWins(output_wins);
		clan.SetLosses(output_losses);
		clan.SetDraws(output_draws);

		rank_clans.insert(std::make_pair(output_rank, clan));
	}
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
		
	return true;
}

bool Database::createLeaderboards()
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "CALL CreateLeaderboards()";

	// Prepare and execute with binds
	MYSQL_STMT* statement;
	
	if(
		!this->_init(&statement) ||
		!this->_prepare(statement, query) ||
		!this->_execute(statement)
	)
	{
		return false;
	}
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
		
	return true;
}
