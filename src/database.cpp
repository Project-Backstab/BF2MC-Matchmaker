#include <iostream>

#include <globals.h>
#include <database.h>

Database::Database()
{
	this->_connection = mysql_init(nullptr);
	
	if (!mysql_real_connect(this->_connection, "localhost", "root", "", "BF2MC", 0, nullptr, 0))
	{
        std::cerr << "[Error] Failed to connect to the database: " << mysql_error(this->_connection) << std::endl;
		std::cerr << "[Error] Database::Database() at mysql_real_connect" << std::endl;
		
		exit(EXIT_FAILURE);
    }
	
	this->OnDatabaseStart();
}

bool Database::queryDBUserByUniquenick(DBUser& dbuser, const std::string &uniquenick)
{
	std::lock_guard<std::mutex> guard(this->_mutex);
	
	unsigned long output_profileid = 0;
	unsigned long output_userid = 0;
	char          output_nick[46];
	char          output_uniquenick[46];
	char          output_email[46];
	char          output_password[46];
	
    std::string query = "SELECT * FROM Users WHERE uniquenick = ?";
	
	// Allocate input binds
    MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
    input_bind[0].buffer_type = MYSQL_TYPE_STRING;
    input_bind[0].buffer = const_cast<char*>(&(uniquenick[0]));
    input_bind[0].buffer_length = uniquenick.size();
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(6, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
    output_bind[0].buffer = &output_profileid;
    output_bind[0].is_unsigned = false;	
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
    output_bind[1].buffer = &output_userid;
    output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
    output_bind[2].buffer = output_nick;
    output_bind[2].buffer_length = 46;
    output_bind[2].length = nullptr;
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
    output_bind[3].buffer = output_uniquenick;
    output_bind[3].buffer_length = 46;
    output_bind[3].length = nullptr;
	output_bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
    output_bind[4].buffer = output_email;
    output_bind[4].buffer_length = 46;
    output_bind[4].length = nullptr;
	output_bind[5].buffer_type = MYSQL_TYPE_VAR_STRING;
    output_bind[5].buffer = output_password;
    output_bind[5].buffer_length = 46;
    output_bind[5].length = nullptr;
	
	// Prepare the statement
    MYSQL_STMT* statement = mysql_stmt_init(this->_connection);
	
	// Prepare and execute with binds
    if(
		!this->_prepare(statement, query, input_bind) ||
		!this->_execute(statement, output_bind)
	)
	{
		return false;
	}
	
	// Fetch and process rows
	while (true)
	{
		int status = mysql_stmt_fetch(statement);
		
		if (status == 1 || status == MYSQL_NO_DATA)
			break;
		
		dbuser.profileid  = output_profileid;
		dbuser.userid     = output_userid;
		dbuser.nick       = output_nick;
		dbuser.uniquenick = output_uniquenick;
		dbuser.email      = output_email;
		dbuser.password   = output_password;
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	
	return true;
}

bool Database::queryDBUsersByEmail(std::vector<DBUser>& dbusers, const std::string &email)
{
	std::lock_guard<std::mutex> guard(this->_mutex);
	
	unsigned long output_profileid = 0;
	unsigned long output_userid = 0;
	char          output_nick[46];
	char          output_uniquenick[46];
	char          output_email[46];
	char          output_password[46];
	
    std::string query = "SELECT * FROM Users WHERE email = ?";
	
	// Allocate input binds
    MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
    input_bind[0].buffer_type = MYSQL_TYPE_STRING;
    input_bind[0].buffer = const_cast<char*>(&(email[0]));
    input_bind[0].buffer_length = email.size();
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(6, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
    output_bind[0].buffer = &output_profileid;
    output_bind[0].is_unsigned = false;	
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
    output_bind[1].buffer = &output_userid;
    output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
    output_bind[2].buffer = output_nick;
    output_bind[2].buffer_length = 46;
    output_bind[2].length = nullptr;
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
    output_bind[3].buffer = output_uniquenick;
    output_bind[3].buffer_length = 46;
    output_bind[3].length = nullptr;
	output_bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
    output_bind[4].buffer = output_email;
    output_bind[4].buffer_length = 46;
    output_bind[4].length = nullptr;
	output_bind[5].buffer_type = MYSQL_TYPE_VAR_STRING;
    output_bind[5].buffer = output_password;
    output_bind[5].buffer_length = 46;
    output_bind[5].length = nullptr;
	
	// Prepare the statement
    MYSQL_STMT* statement = mysql_stmt_init(this->_connection);
	
	// Prepare and execute with binds
    if(
		!this->_prepare(statement, query, input_bind) ||
		!this->_execute(statement, output_bind)
	)
	{
		return false;
	}
	
	// Fetch and process rows
	while (true)
	{
		int status = mysql_stmt_fetch(statement);
		
		if (status == 1 || status == MYSQL_NO_DATA)
			break;
		
		DBUser dbuser;
		dbuser.profileid  = output_profileid;
		dbuser.userid     = output_userid;
		dbuser.nick       = output_nick;
		dbuser.uniquenick = output_uniquenick;
		dbuser.email      = output_email;
		dbuser.password   = output_password;
		
		dbusers.push_back(dbuser);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	
	return true;
}

bool Database::insertDBUser(const DBUser& dbuser, int &profileid)
{
	std::lock_guard<std::mutex> guard(this->_mutex);
	
    std::string query = "INSERT INTO `Users` (userid, nick, uniquenick, email, password) VALUES (?, ?, ?, ?, ?);";
	
	// Allocate input binds
    MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(5, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
    input_bind[0].buffer = const_cast<int*>(&(dbuser.userid));
    input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    input_bind[1].buffer = const_cast<char*>(&(dbuser.nick[0]));
    input_bind[1].buffer_length = dbuser.nick.size();
    input_bind[1].length = nullptr;
	input_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
    input_bind[2].buffer = const_cast<char*>(&(dbuser.uniquenick[0]));
    input_bind[2].buffer_length = dbuser.uniquenick.size();
    input_bind[2].length = nullptr;
	input_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
    input_bind[3].buffer = const_cast<char*>(&(dbuser.email[0]));
    input_bind[3].buffer_length = dbuser.email.size();
    input_bind[3].length = nullptr;
	input_bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
    input_bind[4].buffer = const_cast<char*>(&(dbuser.password[0]));
    input_bind[4].buffer_length = dbuser.password.size();
    input_bind[4].length = nullptr;
	
	// Prepare the statement
    MYSQL_STMT* statement = mysql_stmt_init(this->_connection);
	
	// Prepare and execute with binds
    if(
		!this->_prepare(statement, query, input_bind) ||
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

bool Database::_prepare(MYSQL_STMT* statement, const std::string query)
{
	if (mysql_stmt_prepare(statement, query.c_str(), query.size()) != 0)
	{
        std::cerr << "Failed to prepare the statement: " << mysql_stmt_error(statement) << std::endl;
		
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
        std::cerr << "Failed to bind the input parameter: " << mysql_stmt_error(statement) << std::endl;
		
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
		std::cerr << "Failed to execute the statement: " << mysql_stmt_error(statement) << std::endl;
		
		// Cleanup
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
		std::cerr << "Failed to bind the output parameter: " << mysql_stmt_error(statement) << std::endl;
        
		// Cleanup
		mysql_stmt_free_result(statement);
		mysql_stmt_close(statement);

		return false;
	}
	
	return true;
}

void Database::_doQuery(const std::string &query)
{
	// Execute a query
    if (mysql_query(this->_connection, query.c_str()))
	{
		std::cerr << "Failed to execute the query: " << mysql_error(this->_connection) << std::endl;
		std::cerr << "[Error] Database::_doQuery() at mysql_query with query '" << query << "'" << std::endl;
		
		exit(EXIT_FAILURE);
    }
}

void Database::OnDatabaseStart()
{
	std::lock_guard<std::mutex> guard(g_mutex_io);
	
	std::cout << "Database started" << std::endl;
}
