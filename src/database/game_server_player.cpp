#include <iostream>
#include <mysql/mysql.h>

#include <settings.h>
#include <logger.h>
#include <util.h>

#include <database.h>

bool Database::queryGameServerPlayers(Battlefield::GameServer& game_server)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "SELECT ";
	query += "	`id`, `name`, `score`, `skill`, `ping`, `team`, `deaths`, `profileid` ";
	query += "FROM ";
	query += "	`GameServerPlayers` ";
	query += "WHERE ";
	query += "	`gameserverid` = ?";
	
	int      input_gameserverid = game_server.GetId();
	
	int      output_id;
	char     output_name[VARCHAR_LEN(45)];
	int16_t  output_score;
	char     output_skill[VARCHAR_LEN(45)];
	uint8_t  output_ping;
	uint8_t  output_team;
	uint16_t output_deaths;
	int      output_profileid;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_gameserverid;
	input_bind[0].is_unsigned = false;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(8, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_id;
	output_bind[0].is_unsigned = false;	
	output_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[1].buffer = output_name;
	output_bind[1].buffer_length = VARCHAR_LEN(45);	
	output_bind[2].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[2].buffer = &output_score;
	output_bind[2].is_unsigned = false;
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = output_skill;
	output_bind[3].buffer_length = VARCHAR_LEN(45);
	output_bind[4].buffer_type = MYSQL_TYPE_TINY;
	output_bind[4].buffer = &output_ping;
	output_bind[4].is_unsigned = true;
	output_bind[5].buffer_type = MYSQL_TYPE_TINY;
	output_bind[5].buffer = &output_team;
	output_bind[5].is_unsigned = true;
	output_bind[6].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[6].buffer = &output_deaths;
	output_bind[6].is_unsigned = true;
	output_bind[7].buffer_type = MYSQL_TYPE_LONG;
	output_bind[7].buffer = &output_profileid;
	output_bind[7].is_unsigned = false;
	
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
		
		Battlefield::GameServerPlayer gsplayer;
		
		gsplayer.SetId(output_id);
		gsplayer.SetName(output_name);
		gsplayer.SetScore(output_score);
		gsplayer.SetSkill(output_skill);
		gsplayer.SetPing(output_ping);
		gsplayer.SetTeam(output_team);
		gsplayer.SetDeaths(output_deaths);
		gsplayer.SetProfileId(output_profileid);
		
		game_server.AddPlayer(gsplayer);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
	
	return true;
}

bool Database::_insertGameServerPlayer(const Battlefield::GameServer& game_server, Battlefield::GameServerPlayer& gsplayer)
{
	//std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "INSERT INTO `GameServerPlayers` ";
	query += "	(`gameserverid`, `name`, `score`, `skill`, `ping`, `team`, `deaths`, `profileid`) ";
	query += "VALUES ";
	query += "	(?, ?, ?, ?, ?, ?, ?, ?)";
	
	int         input_id        = game_server.GetId();
	std::string input_name      = gsplayer.GetName();
	int16_t     input_score     = gsplayer.GetScore();
	std::string input_skill     = gsplayer.GetSkill();
	uint8_t     input_ping      = gsplayer.GetPing();
	uint8_t     input_team      = gsplayer.GetTeam();
	uint16_t    input_deaths    = gsplayer.GetDeaths();
	int         input_profileid = gsplayer.GetProfileId();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(9, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = const_cast<int*>(&input_id);
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_STRING;
	input_bind[1].buffer = const_cast<char*>(&(input_name[0]));
	input_bind[1].buffer_length = input_name.size();
	input_bind[2].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[2].buffer = &input_score;
	input_bind[2].is_unsigned = false;
	input_bind[3].buffer_type = MYSQL_TYPE_STRING;
	input_bind[3].buffer = const_cast<char*>(&(input_skill[0]));
	input_bind[3].buffer_length = input_skill.size();
	input_bind[4].buffer_type = MYSQL_TYPE_TINY;
	input_bind[4].buffer = &input_ping;
	input_bind[4].is_unsigned = true;
	input_bind[5].buffer_type = MYSQL_TYPE_TINY;
	input_bind[5].buffer = &input_team;
	input_bind[5].is_unsigned = true;
	input_bind[6].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[6].buffer = &input_deaths;
	input_bind[6].is_unsigned = true;
	input_bind[7].buffer_type = MYSQL_TYPE_LONG;
	input_bind[7].buffer = const_cast<int*>(&input_profileid);
	input_bind[7].is_unsigned = false;
	
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
	
	// Update GameServerPlayer id
	int id = mysql_stmt_insert_id(statement);
	gsplayer.SetId(id);
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	
	return true;
}

bool Database::_removeGameServerPlayers(const Battlefield::GameServer& game_server)
{
	//std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "DELETE FROM ";
	query += "	`GameServerPlayers` ";
	query += "WHERE ";
	query += "	`gameserverid` = ?";
	
	int input_id        = game_server.GetId();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_id;
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
