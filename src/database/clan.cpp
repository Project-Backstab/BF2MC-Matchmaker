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

bool Database::queryClansByClanId(Battlefield::Clan& clan1, Battlefield::Clan& clan2)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock
	
	std::string query = "";
	query += "SELECT ";
	query += "	`clanid`, `name`, `tag`, `homepage`, `info`, `region`, `score`, `wins`, `losses`, `draws`, `created_at`, `disable`";
	query += "FROM ";
	query += "	`Clans` ";
	query += "WHERE ";
	query += "	`clanid` IN (?, ?)";

	int input_clanid1 = clan1.GetClanId();
	int input_clanid2 = clan2.GetClanId();
	
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
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_clanid1;
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = &input_clanid2;
	input_bind[1].is_unsigned = false;
	
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

	// Fetch and process rows
	while (true)
	{
		int status = mysql_stmt_fetch(statement);
		
		if (status == 1 || status == MYSQL_NO_DATA)
			break;
		
		Battlefield::Clan* clan;

		if(input_clanid1 == output_clanid)
		{
			clan = &clan1;
		}
		else if(input_clanid2 == output_clanid)
		{
			clan = &clan2;
		}

		clan->SetName(output_name);
		clan->SetTag(output_tag);
		clan->SetHomepage(output_homepage);
		clan->SetInfo(output_info);
		clan->SetRegion(output_region);
		clan->SetScore(output_score);
		clan->SetWins(output_wins);
		clan->SetLosses(output_losses);
		clan->SetDraws(output_draws);
		clan->SetCreatedAt(output_created_at);
		clan->SetDisable(output_disable);
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
	uint8_t      input_region   = clan.GetRegion();
	
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
	input_bind[4].buffer = &input_region;
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
	uint8_t      input_region   = clan.GetRegion();
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
	input_bind[3].buffer = &input_region;
	input_bind[3].is_unsigned = true;
	input_bind[4].buffer_type = MYSQL_TYPE_LONG;
	input_bind[4].buffer = &input_score;
	input_bind[4].is_unsigned = true;
	input_bind[5].buffer_type = MYSQL_TYPE_LONG;
	input_bind[5].buffer = &input_wins;
	input_bind[5].is_unsigned = true;
	input_bind[6].buffer_type = MYSQL_TYPE_LONG;
	input_bind[6].buffer = &input_losses;
	input_bind[6].is_unsigned = true;
	input_bind[7].buffer_type = MYSQL_TYPE_LONG;
	input_bind[7].buffer = &input_draws;
	input_bind[7].is_unsigned = true;
	input_bind[8].buffer_type = MYSQL_TYPE_LONG;
	input_bind[8].buffer = &input_clanid;
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

bool Database::updateClans(const Battlefield::Clan& clan1, const Battlefield::Clan& clan2)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "UPDATE ";
	query += "	`Clans` ";
	query += "SET ";
	query += "	`tag` = CASE ";
	query += "		WHEN `clanid` = ? THEN ?";
	query += "		WHEN `clanid` = ? THEN ?";
	query += "	END,";
	query += "	`homepage` = CASE ";
	query += "		WHEN `clanid` = ? THEN ?";
	query += "		WHEN `clanid` = ? THEN ?";
	query += "	END,";
	query += "	`info` = CASE ";
	query += "		WHEN `clanid` = ? THEN ?";
	query += "		WHEN `clanid` = ? THEN ?";
	query += "	END,";
	query += "	`region` = CASE ";
	query += "		WHEN `clanid` = ? THEN ?";
	query += "		WHEN `clanid` = ? THEN ?";
	query += "	END,";
	query += "	`score` = CASE ";
	query += "		WHEN `clanid` = ? THEN ?";
	query += "		WHEN `clanid` = ? THEN ?";
	query += "	END,";
	query += "	`wins` = CASE ";
	query += "		WHEN `clanid` = ? THEN ?";
	query += "		WHEN `clanid` = ? THEN ?";
	query += "	END,";
	query += "	`losses` = CASE ";
	query += "		WHEN `clanid` = ? THEN ?";
	query += "		WHEN `clanid` = ? THEN ?";
	query += "	END,";
	query += "	`draws` = CASE ";
	query += "		WHEN `clanid` = ? THEN ?";
	query += "		WHEN `clanid` = ? THEN ?";
	query += "	END ";
	query += "WHERE id IN (?, ?);";

	int          input_clanid1   = clan1.GetClanId();
	std::string  input_tag1      = clan1.GetTag();
	std::string  input_homepage1 = clan1.GetHomepage();
	std::string  input_info1     = clan1.GetInfo();
	uint8_t      input_region1   = clan1.GetRegion();
	uint32_t     input_score1    = clan1.GetScore();
	uint32_t     input_wins1     = clan1.GetWins();
	uint32_t     input_losses1   = clan1.GetLosses();
	uint32_t     input_draws1    = clan1.GetDraws();

	int          input_clanid2   = clan2.GetClanId();
	std::string  input_tag2      = clan2.GetTag();
	std::string  input_homepage2 = clan2.GetHomepage();
	std::string  input_info2     = clan2.GetInfo();
	uint8_t      input_region2   = clan2.GetRegion();
	uint32_t     input_score2    = clan2.GetScore();
	uint32_t     input_wins2     = clan2.GetWins();
	uint32_t     input_losses2   = clan2.GetLosses();
	uint32_t     input_draws2    = clan2.GetDraws();

	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(34, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_clanid1;
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_STRING;
	input_bind[1].buffer = const_cast<char*>(&(input_tag1[0]));
	input_bind[1].buffer_length = input_tag1.size();
	input_bind[2].buffer_type = MYSQL_TYPE_LONG;
	input_bind[2].buffer = &input_clanid2;
	input_bind[2].is_unsigned = false;
	input_bind[3].buffer_type = MYSQL_TYPE_STRING;
	input_bind[3].buffer = const_cast<char*>(&(input_tag2[0]));
	input_bind[3].buffer_length = input_tag2.size();
	
	input_bind[4].buffer_type = MYSQL_TYPE_LONG;
	input_bind[4].buffer = &input_clanid1;
	input_bind[4].is_unsigned = false;
	input_bind[5].buffer_type = MYSQL_TYPE_STRING;
	input_bind[5].buffer = const_cast<char*>(&(input_homepage1[0]));
	input_bind[5].buffer_length = input_homepage1.size();
	input_bind[6].buffer_type = MYSQL_TYPE_LONG;
	input_bind[6].buffer = &input_clanid2;
	input_bind[6].is_unsigned = false;
	input_bind[7].buffer_type = MYSQL_TYPE_STRING;
	input_bind[7].buffer = const_cast<char*>(&(input_homepage2[0]));
	input_bind[7].buffer_length = input_homepage2.size();
	
	input_bind[8].buffer_type = MYSQL_TYPE_LONG;
	input_bind[8].buffer = &input_clanid1;
	input_bind[8].is_unsigned = false;
	input_bind[9].buffer_type = MYSQL_TYPE_STRING;
	input_bind[9].buffer = const_cast<char*>(&(input_info1[0]));
	input_bind[9].buffer_length = input_info1.size();
	input_bind[10].buffer_type = MYSQL_TYPE_LONG;
	input_bind[10].buffer = &input_clanid2;
	input_bind[10].is_unsigned = false;
	input_bind[11].buffer_type = MYSQL_TYPE_STRING;
	input_bind[11].buffer = const_cast<char*>(&(input_info2[0]));
	input_bind[11].buffer_length = input_info2.size();

	input_bind[12].buffer_type = MYSQL_TYPE_LONG;
	input_bind[12].buffer = &input_clanid1;
	input_bind[12].is_unsigned = false;
	input_bind[13].buffer_type = MYSQL_TYPE_TINY;
	input_bind[13].buffer = &input_region1;
	input_bind[13].is_unsigned = true;
	input_bind[14].buffer_type = MYSQL_TYPE_LONG;
	input_bind[14].buffer = &input_clanid2;
	input_bind[14].is_unsigned = false;
	input_bind[15].buffer_type = MYSQL_TYPE_TINY;
	input_bind[15].buffer = &input_region2;
	input_bind[15].is_unsigned = true;

	input_bind[16].buffer_type = MYSQL_TYPE_LONG;
	input_bind[16].buffer = &input_clanid1;
	input_bind[16].is_unsigned = false;
	input_bind[17].buffer_type = MYSQL_TYPE_LONG;
	input_bind[17].buffer = &input_score1;
	input_bind[17].is_unsigned = true;
	input_bind[18].buffer_type = MYSQL_TYPE_LONG;
	input_bind[18].buffer = &input_clanid2;
	input_bind[18].is_unsigned = false;
	input_bind[19].buffer_type = MYSQL_TYPE_LONG;
	input_bind[19].buffer = &input_score2;
	input_bind[19].is_unsigned = true;

	input_bind[20].buffer_type = MYSQL_TYPE_LONG;
	input_bind[20].buffer = &input_clanid1;
	input_bind[20].is_unsigned = false;
	input_bind[21].buffer_type = MYSQL_TYPE_LONG;
	input_bind[21].buffer = &input_wins1;
	input_bind[21].is_unsigned = true;
	input_bind[22].buffer_type = MYSQL_TYPE_LONG;
	input_bind[22].buffer = &input_clanid2;
	input_bind[22].is_unsigned = false;
	input_bind[23].buffer_type = MYSQL_TYPE_LONG;
	input_bind[23].buffer = &input_wins2;
	input_bind[23].is_unsigned = true;

	input_bind[24].buffer_type = MYSQL_TYPE_LONG;
	input_bind[24].buffer = &input_clanid1;
	input_bind[24].is_unsigned = false;
	input_bind[25].buffer_type = MYSQL_TYPE_LONG;
	input_bind[25].buffer = &input_losses1;
	input_bind[25].is_unsigned = true;
	input_bind[26].buffer_type = MYSQL_TYPE_LONG;
	input_bind[26].buffer = &input_clanid2;
	input_bind[26].is_unsigned = false;
	input_bind[27].buffer_type = MYSQL_TYPE_LONG;
	input_bind[27].buffer = &input_losses2;
	input_bind[27].is_unsigned = true;

	input_bind[28].buffer_type = MYSQL_TYPE_LONG;
	input_bind[28].buffer = &input_clanid1;
	input_bind[28].is_unsigned = false;
	input_bind[29].buffer_type = MYSQL_TYPE_LONG;
	input_bind[29].buffer = &input_draws1;
	input_bind[29].is_unsigned = true;
	input_bind[30].buffer_type = MYSQL_TYPE_LONG;
	input_bind[30].buffer = &input_clanid2;
	input_bind[30].is_unsigned = false;
	input_bind[31].buffer_type = MYSQL_TYPE_LONG;
	input_bind[31].buffer = &input_draws2;
	input_bind[31].is_unsigned = true;

	input_bind[32].buffer_type = MYSQL_TYPE_LONG;
	input_bind[32].buffer = &input_clanid1;
	input_bind[32].is_unsigned = false;
	input_bind[33].buffer_type = MYSQL_TYPE_LONG;
	input_bind[33].buffer = &input_clanid2;
	input_bind[33].is_unsigned = false;

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
	input_bind[0].buffer = &input_clanid;
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
