#include <iostream>
#include <mysql/mysql.h>

#include <settings.h>
#include <logger.h>
#include <util.h>

#include <database.h>

bool Database::queryClanRanksByClanId(Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "SELECT ";
	query += "	`profileid`, `rank` ";
	query += "FROM ";
	query += "	`ClanRanks` ";
	query += "WHERE ";
	query += "	`clanid` = ?";
	
	int input_clanid = clan.GetClanId();
	
	int     output_profileid;
	uint8_t output_rank;

	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_clanid;
	input_bind[0].is_unsigned = false;	
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_profileid;
	output_bind[0].is_unsigned = false;	
	output_bind[1].buffer_type = MYSQL_TYPE_TINY;
	output_bind[1].buffer = &output_rank;
	output_bind[1].is_unsigned = true;	

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

	while(true)
	{
		int status = mysql_stmt_fetch(statement);

		if (status == 1 || status == MYSQL_NO_DATA)
		{
			break;
		}
		
		clan.AddRank(output_profileid, output_rank);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
	
	return true;
}

bool Database::insertClanRank(const Battlefield::Clan& clan, const Battlefield::Player& player, Battlefield::Clan::Ranks rank)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "INSERT INTO `ClanRanks` ";
	query += "	(`clanid`, `profileid`, `rank`) ";
	query += "VALUES ";
	query += "	(?, ?, ?)";
	
	int     input_clanid    = clan.GetClanId();
	int     input_profileid = player.GetProfileId();
	uint8_t input_rank      = static_cast<uint8_t>(rank);
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(3, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_clanid;
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = &input_profileid;
	input_bind[1].is_unsigned = false;
	input_bind[2].buffer_type = MYSQL_TYPE_TINY;
	input_bind[2].buffer = &input_rank;
	input_bind[2].is_unsigned = true;

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

bool Database::updateClanRank(const Battlefield::Clan& clan, const Battlefield::Player& player, Battlefield::Clan::Ranks rank)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock
	
	std::string query = "";
	query += "UPDATE ";
	query += "	`ClanRanks` ";
	query += "SET ";
	query += "	`rank` = ? ";
	query += "WHERE ";
	query += "	`clanid` = ? ";
	query += "AND ";
	query += "	`profileid` = ?";
	 
	uint8_t input_rank      = static_cast<uint8_t>(rank);
	int     input_clanid    = clan.GetClanId();
	int     input_profileid = player.GetProfileId();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(3, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_TINY;
	input_bind[0].buffer = &input_rank;
	input_bind[0].is_unsigned = true;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = &input_clanid;
	input_bind[1].is_unsigned = false;
	input_bind[2].buffer_type = MYSQL_TYPE_LONG;
	input_bind[2].buffer = &input_profileid;
	input_bind[2].is_unsigned = false;

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

bool Database::removeClanRank(const Battlefield::Clan& clan, const Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "DELETE FROM ";
	query += "	`ClanRanks` ";
	query += "WHERE ";
	query += "	`clanid` = ? ";
	query += "AND ";
	query += "	`profileid` = ?";
	
	int input_clanid     = clan.GetClanId();
	int input_profileid  = player.GetProfileId();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_clanid;
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = &input_profileid;
	input_bind[1].is_unsigned = false;

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

bool Database::removeClanRanksByClanId(const Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "DELETE FROM ";
	query += "	`ClanRanks` ";
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
