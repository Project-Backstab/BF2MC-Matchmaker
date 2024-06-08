#include <iostream>
#include <mysql/mysql.h>

#include <settings.h>
#include <logger.h>
#include <util.h>

#include <database.h>

bool Database::queryPlayerFriendsByProfileId(Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "SELECT ";
	query += "	`profileid`, `target_profileid` ";
	query += "FROM ";
	query += "	`PlayerFriends` ";
	query += "WHERE ";
	query += "	`profileid` = ? ";
	query += "OR ";
	query += "	`target_profileid` = ?";
	
	int input_profileid = player.GetProfileId();
	
	int output_profileid;
	int output_target_profileid;

	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = const_cast<int*>(&input_profileid);
	input_bind[0].is_unsigned = false;	
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = const_cast<int*>(&input_profileid);
	input_bind[1].is_unsigned = false;	

	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_profileid;
	output_bind[0].is_unsigned = false;	
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = &output_target_profileid;
	output_bind[1].is_unsigned = false;

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
			break;
		
		int friend_profileid = (output_profileid == input_profileid) ? output_target_profileid : output_profileid;
		
		player.AddFriend(friend_profileid);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
		
	return true;
}

bool Database::insertPlayerFriend(const Battlefield::Player& player, const Battlefield::Player& target_player)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "INSERT INTO `PlayerFriends` ";
	query += "	(`profileid`, `target_profileid`) ";
	query += "VALUES ";
	query += "	(?, ?)";
	
	int input_profileid        = player.GetProfileId();
	int input_target_profileid = target_player.GetProfileId();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = const_cast<int*>(&input_profileid);
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = const_cast<int*>(&input_target_profileid);
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

bool Database::removePlayerFriend(const Battlefield::Player& player, const Battlefield::Player& target_player)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "DELETE FROM ";
	query += "	`PlayerFriends` ";
	query += "WHERE ";
	query += "	(`profileid` = ? AND `target_profileid` = ?) ";
	query += "OR ";
	query += "	(`target_profileid` = ? AND `profileid` = ?)";
	
	int input_profileid        = player.GetProfileId();
	int input_target_profileid = target_player.GetProfileId();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(4, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = const_cast<int*>(&input_profileid);
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = const_cast<int*>(&input_target_profileid);
	input_bind[1].is_unsigned = false;
	input_bind[2].buffer_type = MYSQL_TYPE_LONG;
	input_bind[2].buffer = const_cast<int*>(&input_profileid);
	input_bind[2].is_unsigned = false;
	input_bind[3].buffer_type = MYSQL_TYPE_LONG;
	input_bind[3].buffer = const_cast<int*>(&input_target_profileid);
	input_bind[3].is_unsigned = false;

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
