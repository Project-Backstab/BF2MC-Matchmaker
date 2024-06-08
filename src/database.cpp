#include <iostream>
#include <mysql/mysql.h>

#include <settings.h>
#include <logger.h>
#include <util.h>

#include <database.h>

Database::Database()
{
	if(!this->_connect())
	{
		exit(EXIT_FAILURE);
	}
	
	this->OnDatabaseStart();
}

// Private functions

bool Database::_connect()
{	
	std::shared_lock<std::shared_mutex> guard(g_settings_mutex); // settings lock (read)

	this->_connection = mysql_init(nullptr);

	if (!mysql_real_connect(
			this->_connection,
			g_settings["database"]["host"].asString().c_str(),
			g_settings["database"]["username"].asString().c_str(),
			g_settings["database"]["password"].asString().c_str(),
			g_settings["database"]["database_name"].asString().c_str(),
			0, nullptr, 0))
	{
		Logger::error("Failed to connect to the database: " + std::string(mysql_error(this->_connection)));
		Logger::error("Database::Database() at mysql_real_connect");
		
		return false;
	}
	
	return true;
}

bool Database::_init(MYSQL_STMT** statement)
{
	// Check if MySQL connection is still alive
	if (mysql_ping(this->_connection) != 0) {
		// In case disconnected, reconnect!
		this->_connect();
	}
	
	// Initialize statement
	*statement = mysql_stmt_init(this->_connection);
	
	return true;
}

bool Database::_prepare(MYSQL_STMT* statement, const std::string query)
{
	if (mysql_stmt_prepare(statement, query.c_str(), query.size()) != 0)
	{
		Logger::error("Failed to prepare the statement: " + std::string(mysql_stmt_error(statement)));

		// Cleanup
		mysql_stmt_free_result(statement);
		
		return false;
	}
	
	return true;
}

bool Database::_prepare(MYSQL_STMT* statement, const std::string query, MYSQL_BIND* input_bind)
{
	if(!this->_prepare(statement, query))
	{
		return false;
	}
	
	if (mysql_stmt_bind_param(statement, input_bind) != 0)
	{
		Logger::error("Failed to bind the input parameter: " + std::string(mysql_stmt_error(statement)));

		// Cleanup
		mysql_stmt_close(statement);

		return false;
	}

	return true;
}

bool Database::_execute(MYSQL_STMT* statement)
{
	if(mysql_stmt_execute(statement))
	{
		Logger::error("Failed to execute the statement: " + std::string(mysql_stmt_error(statement)));
		
		// Cleanup
		mysql_stmt_free_result(statement);
		mysql_stmt_close(statement);

		return false;
	}

	return true;
}

bool Database::_execute(MYSQL_STMT* statement, MYSQL_BIND* output_bind)
{
	if(!this->_execute(statement))
	{
		return false;
	}

	// Bind the result buffers to the statement
	if (mysql_stmt_bind_result(statement, output_bind))
	{
		Logger::error("Failed to bind the output parameter: " + std::string(mysql_stmt_error(statement)));

		// Cleanup
		mysql_stmt_free_result(statement);
		mysql_stmt_close(statement);

		return false;
	}

	return true;
}

// Events

void Database::OnDatabaseStart()
{
	Logger::info("MySQL Database connected");
}

