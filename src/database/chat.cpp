#include <iostream>
#include <mysql/mysql.h>

#include <settings.h>
#include <logger.h>
#include <util.h>

#include <database.h>

bool Database::insertChat(int profileid, const std::string& ip, int target_profileid, const std::string& message)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "INSERT INTO `Chat` ";
	query += "	(`profileid`, `ip`, `target_profileid`, `message`) ";
	query += "VALUES ";
	query += "	(?, ?, ?, ?)";
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(4, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &profileid;
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_STRING;
	input_bind[1].buffer = const_cast<char*>(&(ip[0]));
	input_bind[1].buffer_length = ip.size();
	input_bind[2].buffer_type = MYSQL_TYPE_LONG;
	input_bind[2].buffer = &target_profileid;
	input_bind[2].is_unsigned = false;
	input_bind[3].buffer_type = MYSQL_TYPE_STRING;
	input_bind[3].buffer = const_cast<char*>(&(message[0]));
	input_bind[3].buffer_length = message.size();
	
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
