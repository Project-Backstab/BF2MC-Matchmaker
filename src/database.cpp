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

DBUser Database::queryUserByUniquenick(const std::string &uniquenick)
{
	std::lock_guard<std::mutex> guard(this->_mutex);
	
	DBUser dbuser;
	
	this->_doQuery("SELECT * FROM Users WHERE uniquenick='" + uniquenick + "'");
	
	// Fetch and print the results
    MYSQL_RES* result = mysql_use_result(this->_connection);
	
    if(result && mysql_num_fields(result) == 6)
	{
        MYSQL_ROW row;
		
		while((row = mysql_fetch_row(result)) != nullptr)
		{
			dbuser.profileid = (row[0]) ? std::stoi(row[0]) : -1;
			dbuser.userid = (row[1]) ? std::stoi(row[1]) : -1;
			dbuser.nick = (row[2]) ? row[2] : "";
			dbuser.uniquenick = (row[3]) ? row[3] : "";
			dbuser.email = (row[4]) ? row[4] : "";
			dbuser.password = (row[5]) ? row[5] : "";
        }
		
        mysql_free_result(result);
    }
	
	/*
	// Preperation for more secure sql query handling
	
	// Prepare the statement
    MYSQL_STMT* statement = mysql_stmt_init(this->_connection);
    std::string query = "SELECT * FROM Users WHERE uniquenick='?'";
	
    if (mysql_stmt_prepare(statement, query.c_str(), query.size()) != 0)
	{
        std::cerr << "Failed to prepare the statement: " << mysql_stmt_error(statement) << std::endl;
        return dbuser;
    }
	
	// Bind parameters
    std::string value = "IamLupo";
    MYSQL_BIND bindParam;
    bindParam.buffer_type = MYSQL_TYPE_STRING;
    bindParam.buffer = (char*)&(value[0]);
    bindParam.buffer_length = value.size();
    bindParam.is_null = nullptr;
    bindParam.length = nullptr;
	
	if (mysql_stmt_bind_param(statement, &bindParam) != 0) {
        std::cerr << "Failed to bind the parameter: " << mysql_stmt_error(statement) << std::endl;
        return dbuser;
    }
	
	// Fetch and print the results
    MYSQL_RES* result = mysql_stmt_result_metadata(statement);
    if (result == nullptr)
	{
        std::cerr << "Failed to get result set metadata: " << mysql_stmt_error(statement) << std::endl;
        return dbuser;
    }
	
	
	std::cout << mysql_num_fields(result) << std::endl;
	
	MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != nullptr) {
        // Process the row data
        // ...

        // Example: Print the values
        std::cout << "Column1: " << row[0] << ", Column2: " << row[1] << std::endl;
    }
	*/
	
	return dbuser;
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
