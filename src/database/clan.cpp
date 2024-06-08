#include <iostream>
#include <mysql/mysql.h>

#include <settings.h>
#include <logger.h>
#include <util.h>

#include <database.h>

bool Database::queryClanByClanId(Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock
	
	std::string query = "";
	query += "SELECT ";
	query += "	`name`, `tag`, `homepage`, `info`, `region`, `score`, `wins`, `losses`, `draws`, `created_at`, `disable`";
	query += "FROM ";
	query += "	`Clans` ";
	query += "WHERE ";
	query += "	`clanid` = ?";

	int input_clanid = clan.GetClanId();
	
	char       output_name[VARCHAR_LEN(32)];
	char       output_tag[VARCHAR_LEN(3)];
	char       output_homepage[VARCHAR_LEN(256)];
	char       output_info[VARCHAR_LEN(1024)];
	uint8_t    output_region;
	uint32_t   output_score;
	uint32_t   output_wins;
	uint32_t   output_losses;
	uint32_t   output_draws;
	MYSQL_TIME output_created_at;
	uint8_t    output_disable;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_clanid;
	input_bind[0].is_unsigned = false;	
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(11, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[0].buffer = &output_name;
	output_bind[0].buffer_length = VARCHAR_LEN(32);
	output_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[1].buffer = &output_tag;
	output_bind[1].buffer_length = VARCHAR_LEN(3);
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_homepage;
	output_bind[2].buffer_length = VARCHAR_LEN(256);
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = &output_info;
	output_bind[3].buffer_length = VARCHAR_LEN(1024);
	output_bind[4].buffer_type = MYSQL_TYPE_TINY;
	output_bind[4].buffer = &output_region;
	output_bind[4].is_unsigned = true;
	output_bind[5].buffer_type = MYSQL_TYPE_LONG;
	output_bind[5].buffer = &output_score;
	output_bind[5].is_unsigned = true;
	output_bind[6].buffer_type = MYSQL_TYPE_LONG;
	output_bind[6].buffer = &output_wins;
	output_bind[6].is_unsigned = true;
	output_bind[7].buffer_type = MYSQL_TYPE_LONG;
	output_bind[7].buffer = &output_losses;
	output_bind[7].is_unsigned = true;
	output_bind[8].buffer_type = MYSQL_TYPE_LONG;
	output_bind[8].buffer = &output_draws;
	output_bind[8].is_unsigned = true;
	output_bind[9].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[9].buffer = &output_created_at;
	output_bind[10].buffer_type = MYSQL_TYPE_TINY;
	output_bind[10].buffer = &output_disable;
	output_bind[10].is_unsigned = true;

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

	int status = mysql_stmt_fetch(statement);

	if (status != 1 && status != MYSQL_NO_DATA)
	{
		clan.SetName(output_name);
		clan.SetTag(output_tag);
		clan.SetHomepage(output_homepage);
		clan.SetInfo(output_info);
		clan.SetRegion(output_region);
		clan.SetScore(output_score);
		clan.SetWins(output_wins);
		clan.SetLosses(output_losses);
		clan.SetDraws(output_draws);
		clan.SetCreatedAt(output_created_at);
		clan.SetDisable(output_disable);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
	
	return true;
}

bool Database::queryClanByProfileId(Battlefield::Clan& clan, const Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "SELECT ";
	query += "	Clans.clanid as `clanid`, `name`, `tag`, `homepage`, `info`, `region`, `score`, `wins`, `losses`, `draws`, `created_at`, `disable` ";
	query += "FROM ";
	query += "	`Clans`, `ClanRanks` ";
	query += "WHERE ";
	query += "	Clans.clanid = ClanRanks.clanid  ";
	query += "AND ";
	query += "	ClanRanks.profileid = ?";
	
	int input_profileid = player.GetProfileId();
	
	int        output_clanid;
	char       output_name[VARCHAR_LEN(32)];
	char       output_tag[VARCHAR_LEN(3)];
	char       output_homepage[VARCHAR_LEN(256)];
	char       output_info[VARCHAR_LEN(1024)];
	uint8_t    output_region;
	uint32_t   output_score;
	uint32_t   output_wins;
	uint32_t   output_losses;
	uint32_t   output_draws;
	MYSQL_TIME output_created_at;
	uint8_t    output_disable;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_profileid;
	input_bind[0].is_unsigned = false;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(12, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_clanid;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[1].buffer = &output_name;
	output_bind[1].buffer_length = VARCHAR_LEN(32);
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_tag;
	output_bind[2].buffer_length = VARCHAR_LEN(3);
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = &output_homepage;
	output_bind[3].buffer_length = VARCHAR_LEN(256);
	output_bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[4].buffer = &output_info;
	output_bind[4].buffer_length = VARCHAR_LEN(1024);
	output_bind[5].buffer_type = MYSQL_TYPE_TINY;
	output_bind[5].buffer = &output_region;
	output_bind[5].is_unsigned = true;
	output_bind[6].buffer_type = MYSQL_TYPE_LONG;
	output_bind[6].buffer = &output_score;
	output_bind[6].is_unsigned = true;
	output_bind[7].buffer_type = MYSQL_TYPE_LONG;
	output_bind[7].buffer = &output_wins;
	output_bind[7].is_unsigned = true;
	output_bind[8].buffer_type = MYSQL_TYPE_LONG;
	output_bind[8].buffer = &output_losses;
	output_bind[8].is_unsigned = true;
	output_bind[9].buffer_type = MYSQL_TYPE_LONG;
	output_bind[9].buffer = &output_draws;
	output_bind[9].is_unsigned = true;
	output_bind[10].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[10].buffer = &output_created_at;
	output_bind[11].buffer_type = MYSQL_TYPE_TINY;
	output_bind[11].buffer = &output_disable;
	output_bind[11].is_unsigned = true;

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

	int status = mysql_stmt_fetch(statement);

	if (status != 1 && status != MYSQL_NO_DATA)
	{		
		clan.SetClanId(output_clanid);
		clan.SetName(output_name);
		clan.SetTag(output_tag);
		clan.SetHomepage(output_homepage);
		clan.SetInfo(output_info);
		clan.SetRegion(output_region);
		clan.SetScore(output_score);
		clan.SetWins(output_wins);
		clan.SetLosses(output_losses);
		clan.SetDraws(output_draws);
		clan.SetCreatedAt(output_created_at);
		clan.SetDisable(output_disable);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
	
	return true;
}

bool Database::queryClanByNameOrTag(Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock
	
	std::string query = "";
	query += "SELECT ";
	query += "	`clanid`, `name`, `tag`, `homepage`, `info`, `region`, `score`, `wins`, `losses`, `draws`, `created_at` ";
	query += "FROM ";
	query += "	`Clans` ";
	query += "WHERE ";
	query += "(	`name` = ? ";
	query += "OR ";
	query += "	`tag` = ? ";
	query += ") AND ";
	query += "	`disable` = 0";
	
	std::string input_name = clan.GetName();
	std::string input_tag = clan.GetTag();
	
	int        output_clanid;
	char       output_name[VARCHAR_LEN(32)];
	char       output_tag[VARCHAR_LEN(3)];
	char       output_homepage[VARCHAR_LEN(256)];
	char       output_info[VARCHAR_LEN(1024)];
	uint8_t    output_region;
	uint32_t   output_score;
	uint32_t   output_wins;
	uint32_t   output_losses;
	uint32_t   output_draws;
	MYSQL_TIME output_created_at;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	input_bind[0].buffer = const_cast<char*>(&(input_name[0]));
	input_bind[0].buffer_length = input_name.size();
	input_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	input_bind[1].buffer = const_cast<char*>(&(input_tag[0]));
	input_bind[1].buffer_length = input_tag.size();
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(11, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_clanid;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[1].buffer = &output_name;
	output_bind[1].buffer_length = VARCHAR_LEN(32);
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_tag;
	output_bind[2].buffer_length = VARCHAR_LEN(3);
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = &output_homepage;
	output_bind[3].buffer_length = VARCHAR_LEN(256);
	output_bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[4].buffer = &output_info;
	output_bind[4].buffer_length = VARCHAR_LEN(1024);
	output_bind[5].buffer_type = MYSQL_TYPE_TINY;
	output_bind[5].buffer = &output_region;
	output_bind[5].is_unsigned = true;
	output_bind[6].buffer_type = MYSQL_TYPE_LONG;
	output_bind[6].buffer = &output_score;
	output_bind[6].is_unsigned = true;
	output_bind[7].buffer_type = MYSQL_TYPE_LONG;
	output_bind[7].buffer = &output_wins;
	output_bind[7].is_unsigned = true;
	output_bind[8].buffer_type = MYSQL_TYPE_LONG;
	output_bind[8].buffer = &output_losses;
	output_bind[8].is_unsigned = true;
	output_bind[9].buffer_type = MYSQL_TYPE_LONG;
	output_bind[9].buffer = &output_draws;
	output_bind[9].is_unsigned = true;
	output_bind[10].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[10].buffer = &output_created_at;

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

	int status = mysql_stmt_fetch(statement);

	if (status != 1 && status != MYSQL_NO_DATA)
	{		
		clan.SetClanId(output_clanid);
		clan.SetName(output_name);
		clan.SetTag(output_tag);
		clan.SetHomepage(output_homepage);
		clan.SetInfo(output_info);
		clan.SetRegion(output_region);
		clan.SetScore(output_score);
		clan.SetWins(output_wins);
		clan.SetLosses(output_losses);
		clan.SetDraws(output_draws);
		clan.SetCreatedAt(output_created_at);
		clan.SetDisable(false);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
	
	return true;
}

bool Database::insertClan(Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "INSERT INTO `Clans` ";
	query += "	(`name`, `tag`, `homepage`, `info`, `region`) ";
	query += "VALUES ";
	query += "	(?, ?, ?, ?, ?)";
	
	std::string  input_name     = clan.GetName();
	std::string  input_tag      = clan.GetTag();
	std::string  input_homepage = clan.GetHomepage();
	std::string  input_info     = clan.GetInfo();
	uint8_t      input_region   = static_cast<uint8_t>(clan.GetRegion());
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(5, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_STRING;
	input_bind[0].buffer = const_cast<char*>(&(input_name[0]));
	input_bind[0].buffer_length = input_name.size();
	input_bind[1].buffer_type = MYSQL_TYPE_STRING;
	input_bind[1].buffer = const_cast<char*>(&(input_tag[0]));
	input_bind[1].buffer_length = input_tag.size();
	input_bind[2].buffer_type = MYSQL_TYPE_STRING;
	input_bind[2].buffer = const_cast<char*>(&(input_homepage[0]));
	input_bind[2].buffer_length = input_homepage.size();
	input_bind[3].buffer_type = MYSQL_TYPE_STRING;
	input_bind[3].buffer = const_cast<char*>(&(input_info[0]));
	input_bind[3].buffer_length = input_info.size();
	input_bind[4].buffer_type = MYSQL_TYPE_TINY;
	input_bind[4].buffer = const_cast<uint8_t*>(&input_region);
	input_bind[4].is_unsigned = true;

	// Prepare and execute with binds
	MYSQL_STMT* statement;
	
	if(
		!this->_init(&statement) ||
		!this->_prepare(statement, query, input_bind) ||
		!this->_execute(statement)
	)
	{
		// Cleanup
		free(input_bind);
		
		return false;
	}
	
	// Update Clan id
	int clanid = mysql_stmt_insert_id(statement);
	clan.SetClanId(clanid);
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	
	return true;
}

bool Database::updateClan(const Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "UPDATE ";
	query += "	`Clans` ";
	query += "SET ";
	query += "	`tag` = ?, ";
	query += "	`homepage` = ?, ";
	query += "	`info` = ?, ";
	query += "	`region` = ?, ";
	query += "	`score` = ?, ";
	query += "	`wins` = ?, ";
	query += "	`losses` = ?, ";
	query += "	`draws` = ? ";
	query += "WHERE ";
	query += "	`clanid` = ?";
	
	int          input_clanid   = clan.GetClanId();
	std::string  input_tag      = clan.GetTag();
	std::string  input_homepage = clan.GetHomepage();
	std::string  input_info     = clan.GetInfo();
	uint8_t      input_region   = static_cast<uint8_t>(clan.GetRegion());
	uint32_t     input_score    = clan.GetScore();
	uint32_t     input_wins     = clan.GetWins();
	uint32_t     input_losses   = clan.GetLosses();
	uint32_t     input_draws    = clan.GetDraws();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(9, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_STRING;
	input_bind[0].buffer = const_cast<char*>(&(input_tag[0]));
	input_bind[0].buffer_length = input_tag.size();
	input_bind[1].buffer_type = MYSQL_TYPE_STRING;
	input_bind[1].buffer = const_cast<char*>(&(input_homepage[0]));
	input_bind[1].buffer_length = input_homepage.size();
	input_bind[2].buffer_type = MYSQL_TYPE_STRING;
	input_bind[2].buffer = const_cast<char*>(&(input_info[0]));
	input_bind[2].buffer_length = input_info.size();
	input_bind[3].buffer_type = MYSQL_TYPE_TINY;
	input_bind[3].buffer = const_cast<uint8_t*>(&input_region);
	input_bind[3].is_unsigned = true;
	input_bind[4].buffer_type = MYSQL_TYPE_LONG;
	input_bind[4].buffer = const_cast<uint32_t*>(&input_score);
	input_bind[4].is_unsigned = true;
	input_bind[5].buffer_type = MYSQL_TYPE_LONG;
	input_bind[5].buffer = const_cast<uint32_t*>(&input_wins);
	input_bind[5].is_unsigned = true;
	input_bind[6].buffer_type = MYSQL_TYPE_LONG;
	input_bind[6].buffer = const_cast<uint32_t*>(&input_losses);
	input_bind[6].is_unsigned = true;
	input_bind[7].buffer_type = MYSQL_TYPE_LONG;
	input_bind[7].buffer = const_cast<uint32_t*>(&input_draws);
	input_bind[7].is_unsigned = true;
	input_bind[8].buffer_type = MYSQL_TYPE_LONG;
	input_bind[8].buffer = const_cast<int*>(&input_clanid);
	input_bind[8].is_unsigned = false;

	// Prepare and execute with binds
	MYSQL_STMT* statement;
	
	if(
		!this->_init(&statement) ||
		!this->_prepare(statement, query, input_bind) ||
		!this->_execute(statement)
	)
	{
		// Cleanup
		free(input_bind);
		
		return false;
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	
	return true;
}

bool Database::disableClan(const Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "UPDATE ";
	query += "	`Clans` ";
	query += "SET ";
	query += "	`disable` = 1 ";
	query += "WHERE ";
	query += "	`clanid` = ?";
	
	int input_clanid = clan.GetClanId();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = const_cast<int*>(&input_clanid);
	input_bind[0].is_unsigned = false;

	// Prepare and execute with binds
	MYSQL_STMT* statement;
	
	if(
		!this->_init(&statement) ||
		!this->_prepare(statement, query, input_bind) ||
		!this->_execute(statement)
	)
	{
		// Cleanup
		free(input_bind);
		
		return false;
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	
	return true;
}
