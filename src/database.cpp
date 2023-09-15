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

// Player
bool Database::queryPlayerByProfileid(Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "SELECT ";
	query += "	`userid`, `nick`, `uniquenick`, `email`, `password`, `last_login`, `last_login_ip`, `created_at` ";
	query += "FROM ";
	query += "	`Players` ";
	query += "WHERE ";
	query += "	`profileid` = ?";

	int        input_profileid = player.GetProfileId();
	
	int        output_userid = 0;
	char       output_nick[42];
	char       output_uniquenick[33];
	char       output_email[51];
	char       output_md5password[33];
	MYSQL_TIME output_last_login;
	char       output_last_login_ip[16];
	MYSQL_TIME output_created_at;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_profileid;
	input_bind[0].is_unsigned = false;

	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(8, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_userid;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[1].buffer = output_nick;
	output_bind[1].buffer_length = 42;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = output_uniquenick;
	output_bind[2].buffer_length = 33;
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = output_email;
	output_bind[3].buffer_length = 51;
	output_bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[4].buffer = output_md5password;
	output_bind[4].buffer_length = 33;
	output_bind[5].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[5].buffer = &output_last_login;
	output_bind[6].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[6].buffer = output_last_login_ip;
	output_bind[6].buffer_length = 16;
	output_bind[7].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[7].buffer = &output_created_at;
	
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
		player.SetUserId(output_userid);
		player.SetNick(output_nick);
		player.SetUniquenick(output_uniquenick);
		player.SetEmail(output_email);
		player.SetMD5Password(output_md5password);
		player.SetLastLogin(output_last_login);
		player.SetLastLoginIp(output_last_login_ip);
		player.SetCreatedAt(output_created_at);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);

	return true;
}

bool Database::queryPlayerByUniquenick(Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)
	
	std::string query = "";
	query += "SELECT ";
	query += "	`profileid`, `userid`, `nick`, `email`, `password`, `last_login`, `last_login_ip`, `created_at` ";
	query += "FROM ";
	query += "	`Players` ";
	query += "WHERE ";
	query += "	`uniquenick` = ?";
	
	std::string input_uniquenick = player.GetUniquenick();
	
	int        output_profileid = 0;
	int        output_userid = 0;
	char       output_nick[42];
	char       output_email[51];
	char       output_md5password[33];
	MYSQL_TIME output_last_login;
	char       output_last_login_ip[16];
	MYSQL_TIME output_created_at;

	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_STRING;
	input_bind[0].buffer = const_cast<char*>(&(input_uniquenick[0]));
	input_bind[0].buffer_length = input_uniquenick.size();

	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(8, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_profileid;
	output_bind[0].is_unsigned = false;	
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = &output_userid;
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = output_nick;
	output_bind[2].buffer_length = 42;
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = output_email;
	output_bind[3].buffer_length = 51;
	output_bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[4].buffer = output_md5password;
	output_bind[4].buffer_length = 33;
	output_bind[5].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[5].buffer = &output_last_login;
	output_bind[6].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[6].buffer = output_last_login_ip;
	output_bind[6].buffer_length = 16;
	output_bind[7].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[7].buffer = &output_created_at;
	
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
		player.SetProfileId(output_profileid);
		player.SetUserId(output_userid);
		player.SetNick(output_nick);
		player.SetEmail(output_email);
		player.SetMD5Password(output_md5password);
		player.SetLastLogin(output_last_login);
		player.SetLastLoginIp(output_last_login_ip);
		player.SetCreatedAt(output_created_at);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
		
	return true;
}

bool Database::queryPlayersByEmail(Battlefield::Players& players, const std::string& email)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "SELECT ";
	query += "	`profileid`, `userid`, `nick`, `uniquenick`, `email`, `password`, `last_login`, `last_login_ip`, `created_at` ";
	query += "FROM ";
	query += "	`Players` ";
	query += "WHERE ";
	query += "	`email` = ?";
	
	int        output_profileid = 0;
	int        output_userid = 0;
	char       output_nick[42];
	char       output_uniquenick[33];
	char       output_email[51];
	char       output_md5password[33];
	MYSQL_TIME output_last_login;
	char       output_last_login_ip[16];
	MYSQL_TIME output_created_at;

	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_STRING;
	input_bind[0].buffer = const_cast<char*>(&(email[0]));
	input_bind[0].buffer_length = email.size();

	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(9, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_profileid;
	output_bind[0].is_unsigned = false;	
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = &output_userid;
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = output_nick;
	output_bind[2].buffer_length = 42;
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = output_uniquenick;
	output_bind[3].buffer_length = 33;
	output_bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[4].buffer = output_email;
	output_bind[4].buffer_length = 51;
	output_bind[5].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[5].buffer = output_md5password;
	output_bind[5].buffer_length = 33;
	output_bind[6].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[6].buffer = &output_last_login;
	output_bind[7].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[7].buffer = output_last_login_ip;
	output_bind[7].buffer_length = 16;
	output_bind[8].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[8].buffer = &output_created_at;
	
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
		
		Battlefield::Player player;
		
		player.SetProfileId(output_profileid);
		player.SetUserId(output_userid);
		player.SetNick(output_nick);
		player.SetUniquenick(output_uniquenick);
		player.SetEmail(output_email);
		player.SetMD5Password(output_md5password);
		player.SetLastLogin(output_last_login);
		player.SetLastLoginIp(output_last_login_ip);
		player.SetCreatedAt(output_created_at);
		
		players.push_back(player);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
	
	return true;
}

bool Database::queryPlayersByEmailAndUniquenick(Battlefield::Players& players, const std::string& email, const std::string& uniquenick)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "SELECT ";
	query += "	`profileid`, `userid`, `nick`, `uniquenick`, `email`, `password`, `last_login`, `last_login_ip`, `created_at` ";
	query += "FROM ";
	query += "	`Players` ";
	query += "WHERE ";
	query += "	`email` = ? ";
	query += "OR ";
	query += "	`uniquenick` = ?";
	
	int        output_profileid = 0;
	int        output_userid = 0;
	char       output_nick[42];
	char       output_uniquenick[33];
	char       output_email[51];
	char       output_md5password[33];
	MYSQL_TIME output_last_login;
	char       output_last_login_ip[16];
	MYSQL_TIME output_created_at;

	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_STRING;
	input_bind[0].buffer = const_cast<char*>(&(email[0]));
	input_bind[0].buffer_length = email.size();
	input_bind[1].buffer_type = MYSQL_TYPE_STRING;
	input_bind[1].buffer = const_cast<char*>(&(uniquenick[0]));
	input_bind[1].buffer_length = uniquenick.size();
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(9, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_profileid;
	output_bind[0].is_unsigned = false;	
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = &output_userid;
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = output_nick;
	output_bind[2].buffer_length = 42;
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = output_uniquenick;
	output_bind[3].buffer_length = 33;
	output_bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[4].buffer = output_email;
	output_bind[4].buffer_length = 51;
	output_bind[5].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[5].buffer = output_md5password;
	output_bind[5].buffer_length = 33;
	output_bind[6].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[6].buffer = &output_last_login;
	output_bind[7].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[7].buffer = output_last_login_ip;
	output_bind[7].buffer_length = 16;
	output_bind[8].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[8].buffer = &output_created_at;
	
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
		
		Battlefield::Player player;
		
		player.SetProfileId(output_profileid);
		player.SetUserId(output_userid);
		player.SetNick(output_nick);
		player.SetUniquenick(output_uniquenick);
		player.SetEmail(output_email);
		player.SetMD5Password(output_md5password);
		player.SetLastLogin(output_last_login);
		player.SetLastLoginIp(output_last_login_ip);
		player.SetCreatedAt(output_created_at);
		
		players.push_back(player);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
	
	return true;
}

bool Database::queryPlayerNewUserID(Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "Select ";
	query += "	MAX(userid) + 1 as `maxuserid` ";
	query += "FROM ";
	query += "	`Players`";
	
	int output_userid;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_userid;
	output_bind[0].is_unsigned = false;	

	// Prepare and execute with binds
	MYSQL_STMT* statement;
	
	if(
		!this->_init(&statement) ||
		!this->_prepare(statement, query) ||
		!this->_execute(statement, output_bind)
	)
	{
		// Cleanup
		free(output_bind);
		
		return false;
	}
	
	// Fetch and process rows
	while (true)
	{
		int status = mysql_stmt_fetch(statement);
		
		if (status == 1 || status == MYSQL_NO_DATA)
			break;
		
		player.SetUserId(output_userid);
	}
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(output_bind);
	
	return true;
}

bool Database::updatePlayerLastLogin(Battlefield::Player& player, const std::string& ip)
{
	std::string query = "";
	query += "UPDATE ";
	query += "	`Players` ";
	query += "SET ";
	query += "	`last_login` = DATE_FORMAT(NOW(), '%Y-%m-%d %H:%i:%s'), ";
	query += "	`last_login_ip` = ? ";
	query += "WHERE ";
	query += "	`profileid` = ?";
	
	int input_profileid = player.GetProfileId();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	input_bind[0].buffer = const_cast<char*>(&(ip[0]));
	input_bind[0].buffer_length = ip.size();
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = const_cast<int*>(&input_profileid);
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

bool Database::insertPlayer(Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)
	
	std::string query;
	query += "INSERT INTO `Players` ";
	query += "	(`userid`, `nick`, `uniquenick`, `email`, `password`, `created_at`) ";
	query += "VALUES ";
	query += "	(?, ?, ?, ?, ?, DATE_FORMAT(NOW(), '%Y-%m-%d %H:%i:%s'))";
	
	int          input_userid      = player.GetUserId();
	std::string  input_nick        = player.GetNick();
	std::string  input_uniquenick  = player.GetUniquenick();
	std::string  input_email       = player.GetEmail();
	std::string  input_md5password = player.GetPassword();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(5, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = const_cast<int*>(&input_userid);
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	input_bind[1].buffer = const_cast<char*>(&(input_nick[0]));
	input_bind[1].buffer_length = input_nick.size();
	input_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	input_bind[2].buffer = const_cast<char*>(&(input_uniquenick[0]));
	input_bind[2].buffer_length = input_uniquenick.size();
	input_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	input_bind[3].buffer = const_cast<char*>(&(input_email[0]));
	input_bind[3].buffer_length = input_email.size();
	input_bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	input_bind[4].buffer = const_cast<char*>(&(input_md5password[0]));
	input_bind[4].buffer_length = input_md5password.size();

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
	
	// Update Profile id
	int profileid = mysql_stmt_insert_id(statement);
	player.SetProfileId(profileid);
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	
	return true;
}

// Player Friends
bool Database::queryPlayerFriends(Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

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
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

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
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "DELETE FROM ";
	query += "	`PlayerFriends` ";
	query += "WHERE ";
	query += "	(`profileid` = ? AND `target_profileid` = ?) ";
	query += "OR ";
	query += "	(`profileid` = ? AND `target_profileid` = ?)";
	
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
	input_bind[2].buffer = const_cast<int*>(&input_target_profileid);
	input_bind[2].is_unsigned = false;
	input_bind[3].buffer_type = MYSQL_TYPE_LONG;
	input_bind[3].buffer = const_cast<int*>(&input_profileid);
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

// Player Stats
bool Database::queryPlayerStats(Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "SELECT ";
	query += "	`score`, `ran`, `pph`, `kills`, `deaths`, `suicides`, `time`, ";
	query += "	`vehicles`, `lavd`, `mavd`, `havd`, `hed`, `bod`, ";
	query += "	`k1`, `s1`, `k2`, `s2`, `k3`, `s3`, `k4`, `s4`, `k5`, `s5`, ";
	query += "	`medals`, `ttb`, `mv`, `ngp` ";
	query += "FROM ";
	query += "	`PlayerStats` ";
	query += "WHERE ";
	query += "	`profileid` = ?";
	
	int input_profileid = player.GetProfileId();
	
	int output_score;
	int output_ran;
	int output_pph;
	int output_kills;
	int output_deaths;
	int output_suicides;
	int output_time;
	int output_vehicles;
	int output_lavd;
	int output_mavd;
	int output_havd;
	int output_hed;
	int output_bod;
	int output_k1;
	int output_s1;
	int output_k2;
	int output_s2;
	int output_k3;
	int output_s3;
	int output_k4;
	int output_s4;
	int output_k5;
	int output_s5;
	int output_medals;
	int output_ttb;
	int output_mv;
	int output_ngp;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = const_cast<int*>(&input_profileid);
	input_bind[0].is_unsigned = false;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(27, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = const_cast<int*>(&output_score);
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = const_cast<int*>(&output_ran);
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_LONG;
	output_bind[2].buffer = const_cast<int*>(&output_pph);
	output_bind[2].is_unsigned = false;
	output_bind[3].buffer_type = MYSQL_TYPE_LONG;
	output_bind[3].buffer = const_cast<int*>(&output_kills);
	output_bind[3].is_unsigned = false;
	output_bind[4].buffer_type = MYSQL_TYPE_LONG;
	output_bind[4].buffer = const_cast<int*>(&output_deaths);
	output_bind[4].is_unsigned = false;
	output_bind[5].buffer_type = MYSQL_TYPE_LONG;
	output_bind[5].buffer = const_cast<int*>(&output_suicides);
	output_bind[5].is_unsigned = false;
	output_bind[6].buffer_type = MYSQL_TYPE_LONG;
	output_bind[6].buffer = const_cast<int*>(&output_time);
	output_bind[6].is_unsigned = false;
	output_bind[7].buffer_type = MYSQL_TYPE_LONG;
	output_bind[7].buffer = const_cast<int*>(&output_vehicles);
	output_bind[7].is_unsigned = false;
	output_bind[8].buffer_type = MYSQL_TYPE_LONG;
	output_bind[8].buffer = const_cast<int*>(&output_lavd);
	output_bind[8].is_unsigned = false;
	output_bind[9].buffer_type = MYSQL_TYPE_LONG;
	output_bind[9].buffer = const_cast<int*>(&output_mavd);
	output_bind[9].is_unsigned = false;
	output_bind[10].buffer_type = MYSQL_TYPE_LONG;
	output_bind[10].buffer = const_cast<int*>(&output_havd);
	output_bind[10].is_unsigned = false;
	output_bind[11].buffer_type = MYSQL_TYPE_LONG;
	output_bind[11].buffer = const_cast<int*>(&output_hed);
	output_bind[11].is_unsigned = false;
	output_bind[12].buffer_type = MYSQL_TYPE_LONG;
	output_bind[12].buffer = const_cast<int*>(&output_bod);
	output_bind[12].is_unsigned = false;
	output_bind[13].buffer_type = MYSQL_TYPE_LONG;
	output_bind[13].buffer = const_cast<int*>(&output_k1);
	output_bind[13].is_unsigned = false;
	output_bind[14].buffer_type = MYSQL_TYPE_LONG;
	output_bind[14].buffer = const_cast<int*>(&output_s1);
	output_bind[14].is_unsigned = false;
	output_bind[15].buffer_type = MYSQL_TYPE_LONG;
	output_bind[15].buffer = const_cast<int*>(&output_k2);
	output_bind[15].is_unsigned = false;
	output_bind[16].buffer_type = MYSQL_TYPE_LONG;
	output_bind[16].buffer = const_cast<int*>(&output_s2);
	output_bind[16].is_unsigned = false;
	output_bind[17].buffer_type = MYSQL_TYPE_LONG;
	output_bind[17].buffer = const_cast<int*>(&output_k3);
	output_bind[17].is_unsigned = false;
	output_bind[18].buffer_type = MYSQL_TYPE_LONG;
	output_bind[18].buffer = const_cast<int*>(&output_s3);
	output_bind[18].is_unsigned = false;
	output_bind[19].buffer_type = MYSQL_TYPE_LONG;
	output_bind[19].buffer = const_cast<int*>(&output_k4);
	output_bind[19].is_unsigned = false;
	output_bind[20].buffer_type = MYSQL_TYPE_LONG;
	output_bind[20].buffer = const_cast<int*>(&output_s4);
	output_bind[20].is_unsigned = false;
	output_bind[21].buffer_type = MYSQL_TYPE_LONG;
	output_bind[21].buffer = const_cast<int*>(&output_k5);
	output_bind[21].is_unsigned = false;
	output_bind[22].buffer_type = MYSQL_TYPE_LONG;
	output_bind[22].buffer = const_cast<int*>(&output_s5);
	output_bind[22].is_unsigned = false;
	output_bind[23].buffer_type = MYSQL_TYPE_LONG;
	output_bind[23].buffer = const_cast<int*>(&output_medals);
	output_bind[23].is_unsigned = false;
	output_bind[24].buffer_type = MYSQL_TYPE_LONG;
	output_bind[24].buffer = const_cast<int*>(&output_ttb);
	output_bind[24].is_unsigned = false;
	output_bind[25].buffer_type = MYSQL_TYPE_LONG;
	output_bind[25].buffer = const_cast<int*>(&output_mv);
	output_bind[25].is_unsigned = false;
	output_bind[26].buffer_type = MYSQL_TYPE_LONG;
	output_bind[26].buffer = const_cast<int*>(&output_ngp);
	output_bind[26].is_unsigned = false;

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
		
		player.SetScore(output_score);
		player.SetRank(output_ran);
		player.SetPPH(output_pph);
		player.SetKills(output_kills);
		player.SetDeaths(output_deaths);
		player.SetSuicides(output_suicides);
		player.SetTime(output_time);
		player.SetLAVsDestroyed(output_lavd);
		player.SetMAVsDestroyed(output_mavd);
		player.SetHAVsDestroyed(output_havd);
		player.SetHelicoptersDestroyed(output_hed);
		player.SetBoatsDestroyed(output_bod);
		player.SetKillsAssualtKit(output_k1);
		player.SetDeathsAssualtKit(output_s1);
		player.SetKillsSniperKit(output_k2);
		player.SetDeathsSniperKit(output_s2);
		player.SetKillsSpecialOpKit(output_k3);
		player.SetDeathsSpecialOpKit(output_s3);
		player.SetKillsCombatEngineerKit(output_k4);
		player.SetDeathsCombatEngineerKit(output_s4);
		player.SetKillsSupportKit(output_k5);
		player.SetDeathsSupportKit(output_s5);
		player.SetMedals(output_medals);
		player.SetTotalTopPlayer(output_ttb);
		player.SetTotalVictories(output_mv);
		player.SetTotalGameSessions(output_ngp);
	}
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
		
	return true;
}

bool Database::updatePlayerStats(const Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "UPDATE ";
	query += "	`PlayerStats` ";
	query += "SET ";
	query += "	`score` = ?, ";
	query += "	`ran` = ?, ";
	query += "	`pph` = ?, ";
	query += "	`kills` = ?, ";
	query += "	`deaths` = ?, ";
	query += "	`suicides` = ?, ";
	query += "	`time` = ?, ";
	query += "	`vehicles` = ?, ";
	query += "	`lavd` = ?, ";
	query += "	`mavd` = ?, ";
	query += "	`havd` = ?, ";
	query += "	`hed` = ?, ";
	query += "	`bod` = ?, ";
	query += "	`k1` = ?, ";
	query += "	`s1` = ?, ";
	query += "	`k2` = ?, ";
	query += "	`s2` = ?, ";
	query += "	`k3` = ?, ";
	query += "	`s3` = ?, ";
	query += "	`k4` = ?, ";
	query += "	`s4` = ?, ";
	query += "	`k5` = ?, ";
	query += "	`s5` = ?, ";
	query += "	`medals` = ?, ";
	query += "	`ttb` = ?, ";
	query += "	`mv` = ?, ";
	query += "	`ngp` = ? ";
	query += "WHERE ";
	query += "	`profileid` = ?";
	
	int input_score     = player.GetScore();
	int input_ran       = player.GetRank();
	int input_pph       = player.GetPPH();
	int input_kills     = player.GetKills();
	int input_deaths    = player.GetDeaths();
	int input_suicides  = player.GetSuicides();
	int input_time      = player.GetTime();
	int input_vehicles  = player.GetVehiclesDestroyed();
	int input_lavd      = player.GetLAVsDestroyed();
	int input_mavd      = player.GetMAVsDestroyed();
	int input_havd      = player.GetHAVsDestroyed();
	int input_hed       = player.GetHelicoptersDestroyed();
	int input_bod       = player.GetBoatsDestroyed();
	int input_k1        = player.GetKillsAssualtKit();
	int input_s1        = player.GetDeathsAssualtKit();
	int input_k2        = player.GetKillsSniperKit();
	int input_s2        = player.GetDeathsSniperKit();
	int input_k3        = player.GetKillsSpecialOpKit();
	int input_s3        = player.GetDeathsSpecialOpKit();
	int input_k4        = player.GetKillsCombatEngineerKit();
	int input_s4        = player.GetDeathsCombatEngineerKit();
	int input_k5        = player.GetKillsSupportKit();
	int input_s5        = player.GetDeathsSupportKit();
	int input_medals    = player.GetMedals();
	int input_ttb       = player.GetTotalTopPlayer();
	int input_mv        = player.GetTotalVictories();
	int input_ngp       = player.GetTotalGameSessions();
	int input_profileid = player.GetProfileId();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(28, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = const_cast<int*>(&input_score);
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = const_cast<int*>(&input_ran);
	input_bind[1].is_unsigned = false;
	input_bind[2].buffer_type = MYSQL_TYPE_LONG;
	input_bind[2].buffer = const_cast<int*>(&input_pph);
	input_bind[2].is_unsigned = false;
	input_bind[3].buffer_type = MYSQL_TYPE_LONG;
	input_bind[3].buffer = const_cast<int*>(&input_kills);
	input_bind[3].is_unsigned = false;
	input_bind[4].buffer_type = MYSQL_TYPE_LONG;
	input_bind[4].buffer = const_cast<int*>(&input_deaths);
	input_bind[4].is_unsigned = false;
	input_bind[5].buffer_type = MYSQL_TYPE_LONG;
	input_bind[5].buffer = const_cast<int*>(&input_suicides);
	input_bind[5].is_unsigned = false;
	input_bind[6].buffer_type = MYSQL_TYPE_LONG;
	input_bind[6].buffer = const_cast<int*>(&input_time);
	input_bind[6].is_unsigned = false;
	input_bind[7].buffer_type = MYSQL_TYPE_LONG;
	input_bind[7].buffer = const_cast<int*>(&input_vehicles);
	input_bind[7].is_unsigned = false;
	input_bind[8].buffer_type = MYSQL_TYPE_LONG;
	input_bind[8].buffer = const_cast<int*>(&input_lavd);
	input_bind[8].is_unsigned = false;
	input_bind[9].buffer_type = MYSQL_TYPE_LONG;
	input_bind[9].buffer = const_cast<int*>(&input_mavd);
	input_bind[9].is_unsigned = false;
	input_bind[10].buffer_type = MYSQL_TYPE_LONG;
	input_bind[10].buffer = const_cast<int*>(&input_havd);
	input_bind[10].is_unsigned = false;
	input_bind[11].buffer_type = MYSQL_TYPE_LONG;
	input_bind[11].buffer = const_cast<int*>(&input_hed);
	input_bind[11].is_unsigned = false;
	input_bind[12].buffer_type = MYSQL_TYPE_LONG;
	input_bind[12].buffer = const_cast<int*>(&input_bod);
	input_bind[12].is_unsigned = false;
	input_bind[13].buffer_type = MYSQL_TYPE_LONG;
	input_bind[13].buffer = const_cast<int*>(&input_k1);
	input_bind[13].is_unsigned = false;
	input_bind[14].buffer_type = MYSQL_TYPE_LONG;
	input_bind[14].buffer = const_cast<int*>(&input_s1);
	input_bind[14].is_unsigned = false;
	input_bind[15].buffer_type = MYSQL_TYPE_LONG;
	input_bind[15].buffer = const_cast<int*>(&input_k2);
	input_bind[15].is_unsigned = false;
	input_bind[16].buffer_type = MYSQL_TYPE_LONG;
	input_bind[16].buffer = const_cast<int*>(&input_s2);
	input_bind[16].is_unsigned = false;
	input_bind[17].buffer_type = MYSQL_TYPE_LONG;
	input_bind[17].buffer = const_cast<int*>(&input_k3);
	input_bind[17].is_unsigned = false;
	input_bind[18].buffer_type = MYSQL_TYPE_LONG;
	input_bind[18].buffer = const_cast<int*>(&input_s3);
	input_bind[18].is_unsigned = false;
	input_bind[19].buffer_type = MYSQL_TYPE_LONG;
	input_bind[19].buffer = const_cast<int*>(&input_k4);
	input_bind[19].is_unsigned = false;
	input_bind[20].buffer_type = MYSQL_TYPE_LONG;
	input_bind[20].buffer = const_cast<int*>(&input_s4);
	input_bind[20].is_unsigned = false;
	input_bind[21].buffer_type = MYSQL_TYPE_LONG;
	input_bind[21].buffer = const_cast<int*>(&input_k5);
	input_bind[21].is_unsigned = false;
	input_bind[22].buffer_type = MYSQL_TYPE_LONG;
	input_bind[22].buffer = const_cast<int*>(&input_s5);
	input_bind[22].is_unsigned = false;
	input_bind[23].buffer_type = MYSQL_TYPE_LONG;
	input_bind[23].buffer = const_cast<int*>(&input_medals);
	input_bind[23].is_unsigned = false;
	input_bind[24].buffer_type = MYSQL_TYPE_LONG;
	input_bind[24].buffer = const_cast<int*>(&input_ttb);
	input_bind[24].is_unsigned = false;
	input_bind[25].buffer_type = MYSQL_TYPE_LONG;
	input_bind[25].buffer = const_cast<int*>(&input_mv);
	input_bind[25].is_unsigned = false;
	input_bind[26].buffer_type = MYSQL_TYPE_LONG;
	input_bind[26].buffer = const_cast<int*>(&input_ngp);
	input_bind[26].is_unsigned = false;
	input_bind[27].buffer_type = MYSQL_TYPE_LONG;
	input_bind[27].buffer = const_cast<int*>(&input_profileid);
	input_bind[27].is_unsigned = false;
	
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

bool Database::insertPlayerStats(const Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "INSERT INTO `PlayerStats` ";
	query += "	(`profileid`) ";
	query += "VALUES ";
	query += "	(?)";
	
	int input_profileid = player.GetProfileId();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = const_cast<int*>(&input_profileid);
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

// Clan
bool Database::queryClanByClanId(Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)
	
	std::string query = "";
	query += "SELECT ";
	query += "	`name`, `tag`, `homepage`, `info`, `region`, `created_at`";
	query += "FROM ";
	query += "	`Clans` ";
	query += "WHERE ";
	query += "	`clanid` = ?";

	int input_clanid = clan.GetClanId();
	
	char       output_name[33];
	char       output_tag[4];
	char       output_homepage[257];
	char       output_info[1025];
	uint8_t    output_region;
	MYSQL_TIME output_created_at;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_clanid;
	input_bind[0].is_unsigned = false;	
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(6, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[0].buffer = &output_name;
	output_bind[0].buffer_length = 33;
	output_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[1].buffer = &output_tag;
	output_bind[1].buffer_length = 4;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_homepage;
	output_bind[2].buffer_length = 257;
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = &output_info;
	output_bind[3].buffer_length = 1025;
	output_bind[4].buffer_type = MYSQL_TYPE_TINY;
	output_bind[4].buffer = &output_region;
	output_bind[4].is_unsigned = true;	
	output_bind[5].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[5].buffer = &output_created_at;
	
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
		clan.SetCreatedAt(output_created_at);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
	
	return true;
}

bool Database::queryClanByPlayer(Battlefield::Clan& clan, const Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)
	
	std::string query = "";
	query += "SELECT ";
	query += "	Clans.clanid as `clanid`, `name`, `tag`, `homepage`, `info`, `region`, `created_at` ";
	query += "FROM ";
	query += "	`Clans`, `ClanRanks` ";
	query += "WHERE ";
	query += "	Clans.clanid = ClanRanks.clanid  ";
	query += "AND ";
	query += "	ClanRanks.profileid = ?";
	
	int input_profileid = player.GetProfileId();
	
	int        output_clanid;
	char       output_name[33];
	char       output_tag[4];
	char       output_homepage[257];
	char       output_info[1025];
	uint8_t    output_region;
	MYSQL_TIME output_created_at;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_profileid;
	input_bind[0].is_unsigned = false;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(7, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_clanid;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[1].buffer = &output_name;
	output_bind[1].buffer_length = 33;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_tag;
	output_bind[2].buffer_length = 4;
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = &output_homepage;
	output_bind[3].buffer_length = 257;
	output_bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[4].buffer = &output_info;
	output_bind[4].buffer_length = 1025;
	output_bind[5].buffer_type = MYSQL_TYPE_TINY;
	output_bind[5].buffer = &output_region;
	output_bind[5].is_unsigned = true;	
	output_bind[6].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[6].buffer = &output_created_at;
	
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
		clan.SetCreatedAt(output_created_at);
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
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)
	
	std::string query = "";
	query += "SELECT ";
	query += "	`clanid`, `name`, `tag`, `homepage`, `info`, `region`, `created_at` ";
	query += "FROM ";
	query += "	`Clans` ";
	query += "WHERE ";
	query += "	`name` = ? ";
	query += "OR ";
	query += "	`tag` = ?";
	
	std::string input_name = clan.GetName();
	std::string input_tag = clan.GetTag();
	
	int        output_clanid;
	char       output_name[33];
	char       output_tag[4];
	char       output_homepage[257];
	char       output_info[1025];
	uint8_t    output_region;
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
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(7, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_clanid;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[1].buffer = &output_name;
	output_bind[1].buffer_length = 33;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_tag;
	output_bind[2].buffer_length = 4;
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = &output_homepage;
	output_bind[3].buffer_length = 257;
	output_bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[4].buffer = &output_info;
	output_bind[4].buffer_length = 1025;
	output_bind[5].buffer_type = MYSQL_TYPE_TINY;
	output_bind[5].buffer = &output_region;
	output_bind[5].is_unsigned = true;	
	output_bind[6].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[6].buffer = &output_created_at;	

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
		clan.SetCreatedAt(output_created_at);
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
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

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
	input_bind[4].is_unsigned = false;

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
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "UPDATE ";
	query += "	`Clans` ";
	query += "SET ";
	query += "	`tag` = ?, ";
	query += "	`homepage` = ?, ";
	query += "	`info` = ?, ";
	query += "	`region` = ? ";
	query += "WHERE ";
	query += "	`clanid` = ?";
	
	std::string  input_tag      = clan.GetTag();
	std::string  input_homepage = clan.GetHomepage();
	std::string  input_info     = clan.GetInfo();
	uint8_t      input_region   = static_cast<uint8_t>(clan.GetRegion());
	int          input_clanid   = clan.GetClanId();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(5, sizeof(MYSQL_BIND));
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
	input_bind[3].is_unsigned = false;
	input_bind[4].buffer_type = MYSQL_TYPE_LONG;
	input_bind[4].buffer = const_cast<int*>(&input_clanid);
	input_bind[4].is_unsigned = false;

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

bool Database::removeClan(const Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "DELETE FROM ";
	query += "	`Clans` ";
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

// Clan Rank
bool Database::queryClanRanksByClanId(Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

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
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

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
	input_bind[0].buffer = const_cast<int*>(&input_clanid);
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = const_cast<int*>(&input_profileid);
	input_bind[1].is_unsigned = false;
	input_bind[2].buffer_type = MYSQL_TYPE_TINY;
	input_bind[2].buffer = const_cast<uint8_t*>(&input_rank);
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
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)
	
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
	input_bind[0].buffer = const_cast<uint8_t*>(&input_rank);
	input_bind[0].is_unsigned = true;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = const_cast<int*>(&input_clanid);
	input_bind[1].is_unsigned = false;
	input_bind[2].buffer_type = MYSQL_TYPE_LONG;
	input_bind[2].buffer = const_cast<int*>(&input_profileid);
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
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

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
	input_bind[0].buffer = const_cast<int*>(&input_clanid);
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = const_cast<int*>(&input_profileid);
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
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "DELETE FROM ";
	query += "	`ClanRanks` ";
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

// Game Server
bool Database::queryGameServerByIp(Battlefield::GameServer& game_server)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "SELECT ";
	query += "	`id`, `flag`, `localip0`, `localport`, `natneg`, `gamename`, ";
	query += "	`hostname`, `hostport`, `gamever`, `cl`, `rv`, `map`, `mc`, `mapname`, `gc`, ";
	query += "	`gametype`, `gamevariant`, `numplayers`, `maxplayers`, `numteams`, `gamemode`, ";
	query += "	`teamplay`, `fraglimit`, `teamfraglimit`, `timelimit`, `timeelapsed`, `password`, ";
	query += "	`nr`, `xr`, `ff`, `sr`, `rc`, `ni`, `xi`, `qm`, `region`, ";
	query += "	`c0`, `c1`, `n0`, `n1`, `c0c`, `c1c`, ";
	query += "	`team0`, `team1`, `score0`, `score1`, `updated_at`, `verified`";
	query += "FROM ";
	query += "	`GameServers`";
	query += "WHERE ";
	query += "	`ip` = ? ";
	
	std::string input_ip   = game_server.GetIp();
	uint16_t    input_port = game_server.GetPort();
	
	int        output_id;
	uint8_t    output_flag;
	char       output_localip0[16];
	uint16_t   output_localport;
	uint8_t    output_natneg;
	char       output_gamename[46];
	char       output_hostname[46];
	uint16_t   output_hostport;
	char       output_gamever[21];
	char       output_cl[21];
	char       output_rv[21];
	char       output_map[21];
	uint8_t    output_mc;
	uint8_t    output_mapname;
	uint8_t    output_gc;
	char       output_gametype[21];
	char       output_gamevariant[21];
	uint8_t    output_numplayers;
	uint8_t    output_maxplayers;
	uint8_t    output_numteams;
	char       output_gamemode[16];
	uint8_t    output_teamplay;
	uint8_t    output_fraglimit;
	uint8_t    output_teamfraglimit;
	uint16_t   output_timelimit;
	uint16_t   output_timeelapsed;
	uint8_t    output_password;
	uint8_t    output_nr;
	uint8_t    output_xr;
	uint8_t    output_ff;
	uint8_t    output_sr;
	uint8_t    output_rc;
	int64_t    output_ni;
	int64_t    output_xi;
	uint8_t    output_qm;
	uint64_t   output_region;
	int8_t     output_c0;
	int8_t     output_c1;
	char       output_n0[33];
	char       output_n1[33];
	uint8_t    output_c0c;
	uint8_t    output_c1c;
	char       output_team0[6];
	char       output_team1[6];
	int16_t    output_score0;
	int16_t    output_score1;
	MYSQL_TIME output_updated_at;
	uint8_t    output_verified;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_STRING;
	input_bind[0].buffer = const_cast<char*>(&(input_ip[0]));
	input_bind[0].buffer_length = input_ip.size();
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(48, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_id;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_TINY;
	output_bind[1].buffer = &output_flag;
	output_bind[1].is_unsigned = true;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = output_localip0;
	output_bind[2].buffer_length = 16;
	output_bind[3].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[3].buffer = &output_localport;
	output_bind[3].is_unsigned = true;
	output_bind[4].buffer_type = MYSQL_TYPE_TINY;
	output_bind[4].buffer = &output_natneg;
	output_bind[4].is_unsigned = true;
	output_bind[5].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[5].buffer = output_gamename;
	output_bind[5].buffer_length = 46;
	output_bind[6].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[6].buffer = output_hostname;
	output_bind[6].buffer_length = 46;
	output_bind[7].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[7].buffer = &output_hostport;
	output_bind[7].is_unsigned = true;
	output_bind[8].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[8].buffer = output_gamever;
	output_bind[8].buffer_length = 21;
	output_bind[9].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[9].buffer = output_cl;
	output_bind[9].buffer_length = 21;
	output_bind[10].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[10].buffer = output_rv;
	output_bind[10].buffer_length = 21;
	output_bind[11].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[11].buffer = output_map;
	output_bind[11].buffer_length = 21;
	output_bind[12].buffer_type = MYSQL_TYPE_TINY;
	output_bind[12].buffer = &output_mc;
	output_bind[12].is_unsigned = true;
	output_bind[13].buffer_type = MYSQL_TYPE_TINY;
	output_bind[13].buffer = &output_mapname;
	output_bind[13].is_unsigned = true;
	output_bind[14].buffer_type = MYSQL_TYPE_TINY;
	output_bind[14].buffer = &output_gc;
	output_bind[14].is_unsigned = true;
	output_bind[15].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[15].buffer = output_gametype;
	output_bind[15].buffer_length = 21;
	output_bind[16].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[16].buffer = output_gamevariant;
	output_bind[16].buffer_length = 21;
	output_bind[17].buffer_type = MYSQL_TYPE_TINY;
	output_bind[17].buffer = &output_numplayers;
	output_bind[17].is_unsigned = true;
	output_bind[18].buffer_type = MYSQL_TYPE_TINY;
	output_bind[18].buffer = &output_maxplayers;
	output_bind[18].is_unsigned = true;
	output_bind[19].buffer_type = MYSQL_TYPE_TINY;
	output_bind[19].buffer = &output_numteams;
	output_bind[19].is_unsigned = true;
	output_bind[20].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[20].buffer = output_gamemode;
	output_bind[20].buffer_length = 16;
	output_bind[21].buffer_type = MYSQL_TYPE_TINY;
	output_bind[21].buffer = &output_teamplay;
	output_bind[21].is_unsigned = true;
	output_bind[22].buffer_type = MYSQL_TYPE_TINY;
	output_bind[22].buffer = &output_fraglimit;
	output_bind[22].is_unsigned = true;
	output_bind[23].buffer_type = MYSQL_TYPE_TINY;
	output_bind[23].buffer = &output_teamfraglimit;
	output_bind[23].is_unsigned = true;
	output_bind[24].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[24].buffer = &output_timelimit;
	output_bind[24].is_unsigned = true;
	output_bind[25].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[25].buffer = &output_timeelapsed;
	output_bind[25].is_unsigned = true;
	output_bind[26].buffer_type = MYSQL_TYPE_TINY;
	output_bind[26].buffer = &output_password;
	output_bind[26].is_unsigned = true;
	output_bind[27].buffer_type = MYSQL_TYPE_TINY;
	output_bind[27].buffer = &output_nr;
	output_bind[27].is_unsigned = true;
	output_bind[28].buffer_type = MYSQL_TYPE_TINY;
	output_bind[28].buffer = &output_xr;
	output_bind[28].is_unsigned = true;
	output_bind[29].buffer_type = MYSQL_TYPE_TINY;
	output_bind[29].buffer = &output_ff;
	output_bind[29].is_unsigned = true;
	output_bind[30].buffer_type = MYSQL_TYPE_TINY;
	output_bind[30].buffer = &output_sr;
	output_bind[30].is_unsigned = true;
	output_bind[31].buffer_type = MYSQL_TYPE_TINY;
	output_bind[31].buffer = &output_rc;
	output_bind[31].is_unsigned = true;
	output_bind[32].buffer_type = MYSQL_TYPE_LONGLONG;
	output_bind[32].buffer = &output_ni;
	output_bind[32].is_unsigned = false;
	output_bind[33].buffer_type = MYSQL_TYPE_LONGLONG;
	output_bind[33].buffer = &output_xi;
	output_bind[33].is_unsigned = false;
	output_bind[34].buffer_type = MYSQL_TYPE_TINY;
	output_bind[34].buffer = &output_qm;
	output_bind[34].is_unsigned = true;
	output_bind[35].buffer_type = MYSQL_TYPE_LONGLONG;
	output_bind[35].buffer = &output_region;
	output_bind[35].is_unsigned = true;
	output_bind[36].buffer_type = MYSQL_TYPE_TINY;
	output_bind[36].buffer = &output_c0;
	output_bind[36].is_unsigned = false;
	output_bind[37].buffer_type = MYSQL_TYPE_TINY;
	output_bind[37].buffer = &output_c1;
	output_bind[37].is_unsigned = false;
	output_bind[38].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[38].buffer = output_n0;
	output_bind[38].buffer_length = 21;
	output_bind[39].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[39].buffer = output_n1;
	output_bind[39].buffer_length = 21;
	output_bind[40].buffer_type = MYSQL_TYPE_TINY;
	output_bind[40].buffer = &output_c0c;
	output_bind[40].is_unsigned = true;
	output_bind[41].buffer_type = MYSQL_TYPE_TINY;
	output_bind[41].buffer = &output_c1c;
	output_bind[41].is_unsigned = true;
	output_bind[42].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[42].buffer = output_team0;
	output_bind[42].buffer_length = 6;
	output_bind[43].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[43].buffer = output_team1;
	output_bind[43].buffer_length = 6;
	output_bind[44].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[44].buffer = &output_score0;
	output_bind[44].is_unsigned = false;
	output_bind[45].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[45].buffer = &output_score1;
	output_bind[45].is_unsigned = false;
	output_bind[46].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[46].buffer = &output_updated_at;
	output_bind[47].buffer_type = MYSQL_TYPE_TINY;
	output_bind[47].buffer = &output_verified;
	output_bind[47].is_unsigned = true;
	
	// Prepare and execute with binds
	MYSQL_STMT* statement;
	
	if(
		!this->_init(&statement) ||
		!this->_prepare(statement, query, input_bind) ||
		!this->_execute(statement, output_bind)
	)
	{
		// Cleanup
		free(output_bind);
		
		return false;
	}

	// Fetch and process rows
	while (true)
	{
		int status = mysql_stmt_fetch(statement);
		
		if (status == 1 || status == MYSQL_NO_DATA)
			break;
		
		game_server.SetId(output_id);
		game_server.SetFlag(output_flag);
		game_server.SetLocalIp(output_localip0);
		game_server.SetLocalPort(output_localport);
		game_server.SetNatNeg(output_natneg);
		game_server.SetGameName(output_gamename);
		game_server.SetHostName(output_hostname);
		game_server.SetHostPort(output_hostport);
		game_server.SetGameVersion(output_gamever);
		game_server.SetClientVersion(output_cl);
		game_server.SetRV(output_rv);
		game_server.SetMap(output_map);
		game_server.SetMapCycling(output_mc);
		game_server.SetMapName(output_mapname);
		game_server.SetGC(output_gc);
		game_server.SetGameType(output_gametype);
		game_server.SetGameVariant(output_gamevariant);
		game_server.SetNumPlayers(output_numplayers);
		game_server.SetMaxPlayers(output_maxplayers);
		game_server.SetNumTeams(output_numteams);
		game_server.SetGameMode(output_gamemode);
		game_server.SetTeamplay(output_teamplay);
		game_server.SetFlagLimit(output_fraglimit);
		game_server.SetTeamFragLimit(output_fraglimit);
		game_server.SetTimeLimit(output_timelimit);
		game_server.SetTimeElapsed(output_timeelapsed);
		game_server.SetPassword(output_password);
		game_server.SetMinRank(output_nr);
		game_server.SetMaxRank(output_xr);
		game_server.SetFriendlyFire(output_ff);
		game_server.SetStatsTracking(output_sr);
		game_server.SetReconfigurable(output_rc);
		game_server.SetMinIpRange(output_ni);
		game_server.SetMaxIpRange(output_xi);
		game_server.SetQM(output_qm);
		game_server.SetRegion(output_region);
		game_server.SetClan1Id(output_c0);
		game_server.SetClan2Id(output_c1);
		game_server.SetClan1Name(output_n0);
		game_server.SetClan2Name(output_n1);
		game_server.SetClan1Claimed(output_c0c);
		game_server.SetClan2Claimed(output_c1c);
		game_server.SetTeam1Name(output_team0);
		game_server.SetTeam2Name(output_team1);
		game_server.SetTeam1Score(output_score0);
		game_server.SetTeam2Score(output_score1);
		game_server.SetUpdatedAt(output_updated_at);
		game_server.SetVerified(output_verified == 1);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(output_bind);
	
	return true;
}

bool Database::queryGameServerByIpAndPort(Battlefield::GameServer& game_server)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "SELECT ";
	query += "	`id`, `flag`, `localip0`, `localport`, `natneg`, `gamename`, ";
	query += "	`hostname`, `hostport`, `gamever`, `cl`, `rv`, `map`, `mc`, `mapname`, `gc`, ";
	query += "	`gametype`, `gamevariant`, `numplayers`, `maxplayers`, `numteams`, `gamemode`, ";
	query += "	`teamplay`, `fraglimit`, `teamfraglimit`, `timelimit`, `timeelapsed`, `password`, ";
	query += "	`nr`, `xr`, `ff`, `sr`, `rc`, `ni`, `xi`, `qm`, `region`, ";
	query += "	`c0`, `c1`, `n0`, `n1`, `c0c`, `c1c`, ";
	query += "	`team0`, `team1`, `score0`, `score1`, `updated_at`, `verified`";
	query += "FROM ";
	query += "	`GameServers`";
	query += "WHERE ";
	query += "	`ip` = ? ";
	query += "AND";
	query += "	`port` = ? ";
	
	std::string input_ip   = game_server.GetIp();
	uint16_t    input_port = game_server.GetPort();
	
	int        output_id;
	uint8_t    output_flag;
	char       output_localip0[16];
	uint16_t   output_localport;
	uint8_t    output_natneg;
	char       output_gamename[46];
	char       output_hostname[46];
	uint16_t   output_hostport;
	char       output_gamever[21];
	char       output_cl[21];
	char       output_rv[21];
	char       output_map[21];
	uint8_t    output_mc;
	uint8_t    output_mapname;
	uint8_t    output_gc;
	char       output_gametype[21];
	char       output_gamevariant[21];
	uint8_t    output_numplayers;
	uint8_t    output_maxplayers;
	uint8_t    output_numteams;
	char       output_gamemode[16];
	uint8_t    output_teamplay;
	uint8_t    output_fraglimit;
	uint8_t    output_teamfraglimit;
	uint16_t   output_timelimit;
	uint16_t   output_timeelapsed;
	uint8_t    output_password;
	uint8_t    output_nr;
	uint8_t    output_xr;
	uint8_t    output_ff;
	uint8_t    output_sr;
	uint8_t    output_rc;
	int64_t    output_ni;
	int64_t    output_xi;
	uint8_t    output_qm;
	uint64_t   output_region;
	int8_t     output_c0;
	int8_t     output_c1;
	char       output_n0[33];
	char       output_n1[33];
	uint8_t    output_c0c;
	uint8_t    output_c1c;
	char       output_team0[6];
	char       output_team1[6];
	int16_t    output_score0;
	int16_t    output_score1;
	MYSQL_TIME output_updated_at;
	uint8_t    output_verified;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_STRING;
	input_bind[0].buffer = const_cast<char*>(&(input_ip[0]));
	input_bind[0].buffer_length = input_ip.size();
	input_bind[1].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[1].buffer = &input_port;
	input_bind[1].is_unsigned = true;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(48, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_id;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_TINY;
	output_bind[1].buffer = &output_flag;
	output_bind[1].is_unsigned = true;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = output_localip0;
	output_bind[2].buffer_length = 16;
	output_bind[3].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[3].buffer = &output_localport;
	output_bind[3].is_unsigned = true;
	output_bind[4].buffer_type = MYSQL_TYPE_TINY;
	output_bind[4].buffer = &output_natneg;
	output_bind[4].is_unsigned = true;
	output_bind[5].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[5].buffer = output_gamename;
	output_bind[5].buffer_length = 46;
	output_bind[6].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[6].buffer = output_hostname;
	output_bind[6].buffer_length = 46;
	output_bind[7].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[7].buffer = &output_hostport;
	output_bind[7].is_unsigned = true;
	output_bind[8].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[8].buffer = output_gamever;
	output_bind[8].buffer_length = 21;
	output_bind[9].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[9].buffer = output_cl;
	output_bind[9].buffer_length = 21;
	output_bind[10].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[10].buffer = output_rv;
	output_bind[10].buffer_length = 21;
	output_bind[11].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[11].buffer = output_map;
	output_bind[11].buffer_length = 21;
	output_bind[12].buffer_type = MYSQL_TYPE_TINY;
	output_bind[12].buffer = &output_mc;
	output_bind[12].is_unsigned = true;
	output_bind[13].buffer_type = MYSQL_TYPE_TINY;
	output_bind[13].buffer = &output_mapname;
	output_bind[13].is_unsigned = true;
	output_bind[14].buffer_type = MYSQL_TYPE_TINY;
	output_bind[14].buffer = &output_gc;
	output_bind[14].is_unsigned = true;
	output_bind[15].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[15].buffer = output_gametype;
	output_bind[15].buffer_length = 21;
	output_bind[16].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[16].buffer = output_gamevariant;
	output_bind[16].buffer_length = 21;
	output_bind[17].buffer_type = MYSQL_TYPE_TINY;
	output_bind[17].buffer = &output_numplayers;
	output_bind[17].is_unsigned = true;
	output_bind[18].buffer_type = MYSQL_TYPE_TINY;
	output_bind[18].buffer = &output_maxplayers;
	output_bind[18].is_unsigned = true;
	output_bind[19].buffer_type = MYSQL_TYPE_TINY;
	output_bind[19].buffer = &output_numteams;
	output_bind[19].is_unsigned = true;
	output_bind[20].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[20].buffer = output_gamemode;
	output_bind[20].buffer_length = 16;
	output_bind[21].buffer_type = MYSQL_TYPE_TINY;
	output_bind[21].buffer = &output_teamplay;
	output_bind[21].is_unsigned = true;
	output_bind[22].buffer_type = MYSQL_TYPE_TINY;
	output_bind[22].buffer = &output_fraglimit;
	output_bind[22].is_unsigned = true;
	output_bind[23].buffer_type = MYSQL_TYPE_TINY;
	output_bind[23].buffer = &output_teamfraglimit;
	output_bind[23].is_unsigned = true;
	output_bind[24].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[24].buffer = &output_timelimit;
	output_bind[24].is_unsigned = true;
	output_bind[25].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[25].buffer = &output_timeelapsed;
	output_bind[25].is_unsigned = true;
	output_bind[26].buffer_type = MYSQL_TYPE_TINY;
	output_bind[26].buffer = &output_password;
	output_bind[26].is_unsigned = true;
	output_bind[27].buffer_type = MYSQL_TYPE_TINY;
	output_bind[27].buffer = &output_nr;
	output_bind[27].is_unsigned = true;
	output_bind[28].buffer_type = MYSQL_TYPE_TINY;
	output_bind[28].buffer = &output_xr;
	output_bind[28].is_unsigned = true;
	output_bind[29].buffer_type = MYSQL_TYPE_TINY;
	output_bind[29].buffer = &output_ff;
	output_bind[29].is_unsigned = true;
	output_bind[30].buffer_type = MYSQL_TYPE_TINY;
	output_bind[30].buffer = &output_sr;
	output_bind[30].is_unsigned = true;
	output_bind[31].buffer_type = MYSQL_TYPE_TINY;
	output_bind[31].buffer = &output_rc;
	output_bind[31].is_unsigned = true;
	output_bind[32].buffer_type = MYSQL_TYPE_LONGLONG;
	output_bind[32].buffer = &output_ni;
	output_bind[32].is_unsigned = false;
	output_bind[33].buffer_type = MYSQL_TYPE_LONGLONG;
	output_bind[33].buffer = &output_xi;
	output_bind[33].is_unsigned = false;
	output_bind[34].buffer_type = MYSQL_TYPE_TINY;
	output_bind[34].buffer = &output_qm;
	output_bind[34].is_unsigned = true;
	output_bind[35].buffer_type = MYSQL_TYPE_LONGLONG;
	output_bind[35].buffer = &output_region;
	output_bind[35].is_unsigned = true;
	output_bind[36].buffer_type = MYSQL_TYPE_TINY;
	output_bind[36].buffer = &output_c0;
	output_bind[36].is_unsigned = false;
	output_bind[37].buffer_type = MYSQL_TYPE_TINY;
	output_bind[37].buffer = &output_c1;
	output_bind[37].is_unsigned = false;
	output_bind[38].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[38].buffer = output_n0;
	output_bind[38].buffer_length = 21;
	output_bind[39].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[39].buffer = output_n1;
	output_bind[39].buffer_length = 21;
	output_bind[40].buffer_type = MYSQL_TYPE_TINY;
	output_bind[40].buffer = &output_c0c;
	output_bind[40].is_unsigned = true;
	output_bind[41].buffer_type = MYSQL_TYPE_TINY;
	output_bind[41].buffer = &output_c1c;
	output_bind[41].is_unsigned = true;
	output_bind[42].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[42].buffer = output_team0;
	output_bind[42].buffer_length = 6;
	output_bind[43].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[43].buffer = output_team1;
	output_bind[43].buffer_length = 6;
	output_bind[44].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[44].buffer = &output_score0;
	output_bind[44].is_unsigned = false;
	output_bind[45].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[45].buffer = &output_score1;
	output_bind[45].is_unsigned = false;
	output_bind[46].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[46].buffer = &output_updated_at;
	output_bind[47].buffer_type = MYSQL_TYPE_TINY;
	output_bind[47].buffer = &output_verified;
	output_bind[47].is_unsigned = true;
	
	// Prepare and execute with binds
	MYSQL_STMT* statement;
	
	if(
		!this->_init(&statement) ||
		!this->_prepare(statement, query, input_bind) ||
		!this->_execute(statement, output_bind)
	)
	{
		// Cleanup
		free(output_bind);
		
		return false;
	}

	// Fetch and process rows
	while (true)
	{
		int status = mysql_stmt_fetch(statement);
		
		if (status == 1 || status == MYSQL_NO_DATA)
			break;
		
		game_server.SetId(output_id);
		game_server.SetFlag(output_flag);
		game_server.SetLocalIp(output_localip0);
		game_server.SetLocalPort(output_localport);
		game_server.SetNatNeg(output_natneg);
		game_server.SetGameName(output_gamename);
		game_server.SetHostName(output_hostname);
		game_server.SetHostPort(output_hostport);
		game_server.SetGameVersion(output_gamever);
		game_server.SetClientVersion(output_cl);
		game_server.SetRV(output_rv);
		game_server.SetMap(output_map);
		game_server.SetMapCycling(output_mc);
		game_server.SetMapName(output_mapname);
		game_server.SetGC(output_gc);
		game_server.SetGameType(output_gametype);
		game_server.SetGameVariant(output_gamevariant);
		game_server.SetNumPlayers(output_numplayers);
		game_server.SetMaxPlayers(output_maxplayers);
		game_server.SetNumTeams(output_numteams);
		game_server.SetGameMode(output_gamemode);
		game_server.SetTeamplay(output_teamplay);
		game_server.SetFlagLimit(output_fraglimit);
		game_server.SetTeamFragLimit(output_fraglimit);
		game_server.SetTimeLimit(output_timelimit);
		game_server.SetTimeElapsed(output_timeelapsed);
		game_server.SetPassword(output_password);
		game_server.SetMinRank(output_nr);
		game_server.SetMaxRank(output_xr);
		game_server.SetFriendlyFire(output_ff);
		game_server.SetStatsTracking(output_sr);
		game_server.SetReconfigurable(output_rc);
		game_server.SetMinIpRange(output_ni);
		game_server.SetMaxIpRange(output_xi);
		game_server.SetQM(output_qm);
		game_server.SetRegion(output_region);
		game_server.SetClan1Id(output_c0);
		game_server.SetClan2Id(output_c1);
		game_server.SetClan1Name(output_n0);
		game_server.SetClan2Name(output_n1);
		game_server.SetClan1Claimed(output_c0c);
		game_server.SetClan2Claimed(output_c1c);
		game_server.SetTeam1Name(output_team0);
		game_server.SetTeam2Name(output_team1);
		game_server.SetTeam1Score(output_score0);
		game_server.SetTeam2Score(output_score1);
		game_server.SetUpdatedAt(output_updated_at);
		game_server.SetVerified(output_verified == 1);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(output_bind);
	
	return true;
}

bool Database::queryGameServers(Battlefield::GameServers& game_servers)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "SELECT ";
	query += "	`id`, `ip`, `port`, `flag`, `localip0`, `localport`, `natneg`, `gamename`, ";
	query += "	`hostname`, `hostport`, `gamever`, `cl`, `rv`, `map`, `mc`, `mapname`, `gc`, ";
	query += "	`gametype`, `gamevariant`, `numplayers`, `maxplayers`, `numteams`, `gamemode`, ";
	query += "	`teamplay`, `fraglimit`, `teamfraglimit`, `timelimit`, `timeelapsed`, `password`, ";
	query += "	`nr`, `xr`, `ff`, `sr`, `rc`, `ni`, `xi`, `qm`, `region`, ";
	query += "	`c0`, `c1`, `n0`, `n1`, `c0c`, `c1c`, ";
	query += "	`team0`, `team1`, `score0`, `score1`, `updated_at`, `verified`";
	query += "FROM ";
	query += "	`GameServers`";
	
	int        output_id;
	char       output_ip[16];
	uint16_t   output_port;
	uint8_t    output_flag;
	char       output_localip0[16];
	uint16_t   output_localport;
	uint8_t    output_natneg;
	char       output_gamename[46];
	char       output_hostname[46];
	uint16_t   output_hostport;
	char       output_gamever[21];
	char       output_cl[21];
	char       output_rv[21];
	char       output_map[21];
	uint8_t    output_mc;
	uint8_t    output_mapname;
	uint8_t    output_gc;
	char       output_gametype[21];
	char       output_gamevariant[21];
	uint8_t    output_numplayers;
	uint8_t    output_maxplayers;
	uint8_t    output_numteams;
	char       output_gamemode[16];
	uint8_t    output_teamplay;
	uint8_t    output_fraglimit;
	uint8_t    output_teamfraglimit;
	uint16_t   output_timelimit;
	uint16_t   output_timeelapsed;
	uint8_t    output_password;
	uint8_t    output_nr;
	uint8_t    output_xr;
	uint8_t    output_ff;
	uint8_t    output_sr;
	uint8_t    output_rc;
	int64_t    output_ni;
	int64_t    output_xi;
	uint8_t    output_qm;
	uint64_t   output_region;
	int8_t     output_c0;
	int8_t     output_c1;
	char       output_n0[33];
	char       output_n1[33];
	uint8_t    output_c0c;
	uint8_t    output_c1c;
	char       output_team0[6];
	char       output_team1[6];
	int16_t    output_score0;
	int16_t    output_score1;
	MYSQL_TIME output_updated_at;
	uint8_t    output_verified;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(50, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_id;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[1].buffer = output_ip;
	output_bind[1].buffer_length = 16;
	output_bind[2].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[2].buffer = &output_port;
	output_bind[2].is_unsigned = true;
	output_bind[3].buffer_type = MYSQL_TYPE_TINY;
	output_bind[3].buffer = &output_flag;
	output_bind[3].is_unsigned = true;
	output_bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[4].buffer = output_localip0;
	output_bind[4].buffer_length = 16;
	output_bind[5].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[5].buffer = &output_localport;
	output_bind[5].is_unsigned = true;
	output_bind[6].buffer_type = MYSQL_TYPE_TINY;
	output_bind[6].buffer = &output_natneg;
	output_bind[6].is_unsigned = true;
	output_bind[7].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[7].buffer = output_gamename;
	output_bind[7].buffer_length = 46;
	output_bind[8].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[8].buffer = output_hostname;
	output_bind[8].buffer_length = 46;
	output_bind[9].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[9].buffer = &output_hostport;
	output_bind[9].is_unsigned = true;
	output_bind[10].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[10].buffer = output_gamever;
	output_bind[10].buffer_length = 21;
	output_bind[11].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[11].buffer = output_cl;
	output_bind[11].buffer_length = 21;
	output_bind[12].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[12].buffer = output_rv;
	output_bind[12].buffer_length = 21;
	output_bind[13].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[13].buffer = output_map;
	output_bind[13].buffer_length = 21;
	output_bind[14].buffer_type = MYSQL_TYPE_TINY;
	output_bind[14].buffer = &output_mc;
	output_bind[14].is_unsigned = true;
	output_bind[15].buffer_type = MYSQL_TYPE_TINY;
	output_bind[15].buffer = &output_mapname;
	output_bind[15].is_unsigned = true;
	output_bind[16].buffer_type = MYSQL_TYPE_TINY;
	output_bind[16].buffer = &output_gc;
	output_bind[16].is_unsigned = true;
	output_bind[17].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[17].buffer = output_gametype;
	output_bind[17].buffer_length = 21;
	output_bind[18].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[18].buffer = output_gamevariant;
	output_bind[18].buffer_length = 21;
	output_bind[19].buffer_type = MYSQL_TYPE_TINY;
	output_bind[19].buffer = &output_numplayers;
	output_bind[19].is_unsigned = true;
	output_bind[20].buffer_type = MYSQL_TYPE_TINY;
	output_bind[20].buffer = &output_maxplayers;
	output_bind[20].is_unsigned = true;
	output_bind[21].buffer_type = MYSQL_TYPE_TINY;
	output_bind[21].buffer = &output_numteams;
	output_bind[21].is_unsigned = true;
	output_bind[22].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[22].buffer = output_gamemode;
	output_bind[22].buffer_length = 16;
	output_bind[23].buffer_type = MYSQL_TYPE_TINY;
	output_bind[23].buffer = &output_teamplay;
	output_bind[23].is_unsigned = true;
	output_bind[24].buffer_type = MYSQL_TYPE_TINY;
	output_bind[24].buffer = &output_fraglimit;
	output_bind[24].is_unsigned = true;
	output_bind[25].buffer_type = MYSQL_TYPE_TINY;
	output_bind[25].buffer = &output_teamfraglimit;
	output_bind[25].is_unsigned = true;
	output_bind[26].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[26].buffer = &output_timelimit;
	output_bind[26].is_unsigned = true;
	output_bind[27].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[27].buffer = &output_timeelapsed;
	output_bind[27].is_unsigned = true;
	output_bind[28].buffer_type = MYSQL_TYPE_TINY;
	output_bind[28].buffer = &output_password;
	output_bind[28].is_unsigned = true;
	output_bind[29].buffer_type = MYSQL_TYPE_TINY;
	output_bind[29].buffer = &output_nr;
	output_bind[29].is_unsigned = true;
	output_bind[30].buffer_type = MYSQL_TYPE_TINY;
	output_bind[30].buffer = &output_xr;
	output_bind[30].is_unsigned = true;
	output_bind[31].buffer_type = MYSQL_TYPE_TINY;
	output_bind[31].buffer = &output_ff;
	output_bind[31].is_unsigned = true;
	output_bind[32].buffer_type = MYSQL_TYPE_TINY;
	output_bind[32].buffer = &output_sr;
	output_bind[32].is_unsigned = true;
	output_bind[33].buffer_type = MYSQL_TYPE_TINY;
	output_bind[33].buffer = &output_rc;
	output_bind[33].is_unsigned = true;
	output_bind[34].buffer_type = MYSQL_TYPE_LONGLONG;
	output_bind[34].buffer = &output_ni;
	output_bind[34].is_unsigned = false;
	output_bind[35].buffer_type = MYSQL_TYPE_LONGLONG;
	output_bind[35].buffer = &output_xi;
	output_bind[35].is_unsigned = false;
	output_bind[36].buffer_type = MYSQL_TYPE_TINY;
	output_bind[36].buffer = &output_qm;
	output_bind[36].is_unsigned = true;
	output_bind[37].buffer_type = MYSQL_TYPE_LONGLONG;
	output_bind[37].buffer = &output_region;
	output_bind[37].is_unsigned = true;
	output_bind[38].buffer_type = MYSQL_TYPE_TINY;
	output_bind[38].buffer = &output_c0;
	output_bind[38].is_unsigned = false;
	output_bind[39].buffer_type = MYSQL_TYPE_TINY;
	output_bind[39].buffer = &output_c1;
	output_bind[39].is_unsigned = false;
	output_bind[40].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[40].buffer = output_n0;
	output_bind[40].buffer_length = 21;
	output_bind[41].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[41].buffer = output_n1;
	output_bind[41].buffer_length = 21;
	output_bind[42].buffer_type = MYSQL_TYPE_TINY;
	output_bind[42].buffer = &output_c0c;
	output_bind[42].is_unsigned = true;
	output_bind[43].buffer_type = MYSQL_TYPE_TINY;
	output_bind[43].buffer = &output_c1c;
	output_bind[43].is_unsigned = true;
	output_bind[44].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[44].buffer = output_team0;
	output_bind[44].buffer_length = 6;
	output_bind[45].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[45].buffer = output_team1;
	output_bind[45].buffer_length = 6;
	output_bind[46].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[46].buffer = &output_score0;
	output_bind[46].is_unsigned = false;
	output_bind[47].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[47].buffer = &output_score1;
	output_bind[47].is_unsigned = false;
	output_bind[48].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[48].buffer = &output_updated_at;
	output_bind[49].buffer_type = MYSQL_TYPE_TINY;
	output_bind[49].buffer = &output_verified;
	output_bind[49].is_unsigned = true;
	
	// Prepare and execute with binds
	MYSQL_STMT* statement;
	
	if(
		!this->_init(&statement) ||
		!this->_prepare(statement, query) ||
		!this->_execute(statement, output_bind)
	)
	{
		// Cleanup
		free(output_bind);
		
		return false;
	}

	// Fetch and process rows
	while (true)
	{
		int status = mysql_stmt_fetch(statement);
		
		if (status == 1 || status == MYSQL_NO_DATA)
			break;
		
		Battlefield::GameServer game_server;

		game_server.SetId(output_id);
		game_server.SetIp(output_ip);
		game_server.SetPort(output_port);
		game_server.SetFlag(output_flag);
		game_server.SetLocalIp(output_localip0);
		game_server.SetLocalPort(output_localport);
		game_server.SetNatNeg(output_natneg);
		game_server.SetGameName(output_gamename);
		game_server.SetHostName(output_hostname);
		game_server.SetHostPort(output_hostport);
		game_server.SetGameVersion(output_gamever);
		game_server.SetClientVersion(output_cl);
		game_server.SetRV(output_rv);
		game_server.SetMap(output_map);
		game_server.SetMapCycling(output_mc);
		game_server.SetMapName(output_mapname);
		game_server.SetGC(output_gc);
		game_server.SetGameType(output_gametype);
		game_server.SetGameVariant(output_gamevariant);
		game_server.SetNumPlayers(output_numplayers);
		game_server.SetMaxPlayers(output_maxplayers);
		game_server.SetNumTeams(output_numteams);
		game_server.SetGameMode(output_gamemode);
		game_server.SetTeamplay(output_teamplay);
		game_server.SetFlagLimit(output_fraglimit);
		game_server.SetTeamFragLimit(output_fraglimit);
		game_server.SetTimeLimit(output_timelimit);
		game_server.SetTimeElapsed(output_timeelapsed);
		game_server.SetPassword(output_password);
		game_server.SetMinRank(output_nr);
		game_server.SetMaxRank(output_xr);
		game_server.SetFriendlyFire(output_ff);
		game_server.SetStatsTracking(output_sr);
		game_server.SetReconfigurable(output_rc);
		game_server.SetMinIpRange(output_ni);
		game_server.SetMaxIpRange(output_xi);
		game_server.SetQM(output_qm);
		game_server.SetRegion(output_region);
		game_server.SetClan1Id(output_c0);
		game_server.SetClan2Id(output_c1);
		game_server.SetClan1Name(output_n0);
		game_server.SetClan2Name(output_n1);
		game_server.SetClan1Claimed(output_c0c);
		game_server.SetClan2Claimed(output_c1c);
		game_server.SetTeam1Name(output_team0);
		game_server.SetTeam2Name(output_team1);
		game_server.SetTeam1Score(output_score0);
		game_server.SetTeam2Score(output_score1);
		game_server.SetUpdatedAt(output_updated_at);
		game_server.SetVerified(output_verified == 1);
		
		game_servers.push_back(game_server);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(output_bind);
	
	return true;
}

bool Database::insertGameServer(Battlefield::GameServer& game_server)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "INSERT INTO `GameServers` ";
	query += "	(`ip`, `port`, `flag`, `localip0`, `localport`, `natneg`, `gamename`, `hostname`, `hostport`, ";
	query += "	`gamever`, `cl`, `rv`, `map`, `mc`, `mapname`, `gc`, `gametype`, `gamevariant`, `numplayers`, ";
	query += "	`maxplayers`, `numteams`, `gamemode`, `teamplay`, `fraglimit`, `teamfraglimit`, `timelimit`, ";
	query += "	`timeelapsed`, `password`, `nr`, `xr`, `ff`, `sr`, `rc`, `ni`, `xi`, `qm`, `region`, ";
	query += "	`c0`, `c1`, `n0`, `n1`, `c0c`, `c1c`, `team0`, `team1`, `score0`, `score1`) ";
	query += "VALUES ";
	query += "		(?, ?, ?, ?, ?, ?, ?, ?, ?, ";
	query += "		?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ";
	query += "		?, ?, ?, ?, ?, ?, ?, ";
	query += "		?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ";
	query += "		?, ?, ?, ?, ?, ?, ?, ?, ?, ?) ";
	
	std::string input_ip            = game_server.GetIp();
	uint16_t    input_port          = game_server.GetPort();
	uint8_t     input_flag          = game_server.GetFlag();
	std::string input_localip       = game_server.GetLocalIp();
	uint16_t    input_localport     = game_server.GetLocalPort();
	uint8_t     input_natneg        = game_server.GetNatNeg();
	std::string input_gamename      = game_server.GetGameName();
	std::string input_hostname      = game_server.GetHostName();
	uint16_t    input_hostport      = game_server.GetHostPort();
	std::string input_gamever       = game_server.GetGameVersion();
	std::string input_cl            = game_server.GetClientVersion();
	std::string input_rv            = game_server.GetRV();
	std::string input_map           = game_server.GetMap();
	uint8_t     input_mc            = game_server.GetMapCycling();
	uint8_t     input_mapname       = game_server.GetMapName();
	uint8_t     input_gc            = game_server.GetGC();
	std::string input_gametype      = game_server.GetGameType();
	std::string input_gamevariant   = game_server.GetGameVariant();
	uint8_t     input_numplayers    = game_server.GetNumPlayers();
	uint8_t     input_maxplayers    = game_server.GetMaxPlayers();
	uint8_t     input_numteams      = game_server.GetNumTeams();
	std::string input_gamemode      = game_server.GetGameMode();
	uint8_t     input_teamplay      = game_server.GetTeamplay();
	uint8_t     input_fraglimit     = game_server.GetFlagLimit();
	uint8_t     input_teamfraglimit = game_server.GetTeamFragLimit();
	uint16_t    input_timelimit     = game_server.GetTimeLimit();
	uint16_t    input_timeelapsed   = game_server.GetTimeElapsed();
	uint8_t     input_password      = game_server.GetPassword();
	uint8_t     input_nr            = game_server.GetMinRank();
	uint8_t     input_xr            = game_server.GetMaxRank();
	uint8_t     input_ff            = game_server.GetFriendlyFire();
	uint8_t     input_sr            = game_server.GetStatsTracking();
	uint8_t     input_rc            = game_server.GetReconfigurable();
	int64_t     input_ni            = game_server.GetMinIpRange();
	int64_t     input_xi            = game_server.GetMaxIpRange();
	uint8_t     input_qm            = game_server.GetQM();
	uint64_t    input_region        = game_server.GetRegion();
	// Clan
	int32_t     input_c0            = game_server.GetClan1Id();
	int32_t     input_c1            = game_server.GetClan2Id();
	std::string input_n0            = game_server.GetClan1Name();
	std::string input_n1            = game_server.GetClan2Name();
	uint8_t     input_c0c           = game_server.GetClan1Claimed();
	uint8_t     input_c1c           = game_server.GetClan2Claimed();
	// Team
	std::string input_team0         = game_server.GetTeam1Name();
	std::string input_team1         = game_server.GetTeam2Name();
	int16_t     input_score0        = game_server.GetTeam1Score();
	int16_t     input_score1        = game_server.GetTeam2Score();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(47, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_STRING;
	input_bind[0].buffer = const_cast<char*>(&(input_ip[0]));
	input_bind[0].buffer_length = input_ip.size();
	input_bind[1].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[1].buffer = &input_port;
	input_bind[1].is_unsigned = true;
	input_bind[2].buffer_type = MYSQL_TYPE_TINY;
	input_bind[2].buffer = &input_flag;
	input_bind[2].is_unsigned = true;
	input_bind[3].buffer_type = MYSQL_TYPE_STRING;
	input_bind[3].buffer = const_cast<char*>(&(input_localip[0]));
	input_bind[3].buffer_length = input_localip.size();
	input_bind[4].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[4].buffer = &input_localport;
	input_bind[4].is_unsigned = true;
	input_bind[5].buffer_type = MYSQL_TYPE_TINY;
	input_bind[5].buffer = &input_natneg;
	input_bind[5].is_unsigned = true;
	input_bind[6].buffer_type = MYSQL_TYPE_STRING;
	input_bind[6].buffer = const_cast<char*>(&(input_gamename[0]));
	input_bind[6].buffer_length = input_gamename.size();
	input_bind[7].buffer_type = MYSQL_TYPE_STRING;
	input_bind[7].buffer = const_cast<char*>(&(input_hostname[0]));
	input_bind[7].buffer_length = input_hostname.size();
	input_bind[8].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[8].buffer = &input_hostport;
	input_bind[8].is_unsigned = true;
	input_bind[9].buffer_type = MYSQL_TYPE_STRING;
	input_bind[9].buffer = const_cast<char*>(&(input_gamever[0]));
	input_bind[9].buffer_length = input_gamever.size();
	input_bind[10].buffer_type = MYSQL_TYPE_STRING;
	input_bind[10].buffer = const_cast<char*>(&(input_cl[0]));
	input_bind[10].buffer_length = input_cl.size();
	input_bind[11].buffer_type = MYSQL_TYPE_STRING;
	input_bind[11].buffer = const_cast<char*>(&(input_rv[0]));
	input_bind[11].buffer_length = input_rv.size();
	input_bind[12].buffer_type = MYSQL_TYPE_STRING;
	input_bind[12].buffer = const_cast<char*>(&(input_map[0]));
	input_bind[12].buffer_length = input_map.size();
	input_bind[13].buffer_type = MYSQL_TYPE_TINY;
	input_bind[13].buffer = &input_mc;
	input_bind[13].is_unsigned = true;
	input_bind[14].buffer_type = MYSQL_TYPE_TINY;
	input_bind[14].buffer = &input_mapname;
	input_bind[14].is_unsigned = true;
	input_bind[15].buffer_type = MYSQL_TYPE_TINY;
	input_bind[15].buffer = &input_gc;
	input_bind[15].is_unsigned = true;
	input_bind[16].buffer_type = MYSQL_TYPE_STRING;
	input_bind[16].buffer = const_cast<char*>(&(input_gametype[0]));
	input_bind[16].buffer_length = input_gametype.size();
	input_bind[17].buffer_type = MYSQL_TYPE_STRING;
	input_bind[17].buffer = const_cast<char*>(&(input_gamevariant[0]));
	input_bind[17].buffer_length = input_gamevariant.size();
	input_bind[18].buffer_type = MYSQL_TYPE_TINY;
	input_bind[18].buffer = &input_numplayers;
	input_bind[18].is_unsigned = true;
	input_bind[19].buffer_type = MYSQL_TYPE_TINY;
	input_bind[19].buffer = &input_maxplayers;
	input_bind[19].is_unsigned = true;
	input_bind[20].buffer_type = MYSQL_TYPE_TINY;
	input_bind[20].buffer = &input_numteams;
	input_bind[20].is_unsigned = true;
	input_bind[21].buffer_type = MYSQL_TYPE_STRING;
	input_bind[21].buffer = const_cast<char*>(&(input_gamemode[0]));
	input_bind[21].buffer_length = input_gamemode.size();
	input_bind[22].buffer_type = MYSQL_TYPE_TINY;
	input_bind[22].buffer = &input_teamplay;
	input_bind[22].is_unsigned = true;
	input_bind[23].buffer_type = MYSQL_TYPE_TINY;
	input_bind[23].buffer = &input_fraglimit;
	input_bind[23].is_unsigned = true;
	input_bind[24].buffer_type = MYSQL_TYPE_TINY;
	input_bind[24].buffer = &input_teamfraglimit;
	input_bind[24].is_unsigned = true;
	input_bind[25].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[25].buffer = &input_timelimit;
	input_bind[25].is_unsigned = true;
	input_bind[26].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[26].buffer = &input_timeelapsed;
	input_bind[26].is_unsigned = true;
	input_bind[27].buffer_type = MYSQL_TYPE_TINY;
	input_bind[27].buffer = &input_password;
	input_bind[27].is_unsigned = true;
	input_bind[28].buffer_type = MYSQL_TYPE_TINY;
	input_bind[28].buffer = &input_nr;
	input_bind[28].is_unsigned = true;
	input_bind[29].buffer_type = MYSQL_TYPE_TINY;
	input_bind[29].buffer = &input_xr;
	input_bind[29].is_unsigned = true;
	input_bind[30].buffer_type = MYSQL_TYPE_TINY;
	input_bind[30].buffer = &input_ff;
	input_bind[30].is_unsigned = true;
	input_bind[31].buffer_type = MYSQL_TYPE_TINY;
	input_bind[31].buffer = &input_sr;
	input_bind[31].is_unsigned = true;
	input_bind[32].buffer_type = MYSQL_TYPE_TINY;
	input_bind[32].buffer = &input_rc;
	input_bind[32].is_unsigned = true;
	input_bind[33].buffer_type = MYSQL_TYPE_LONGLONG;
	input_bind[33].buffer = &input_ni;
	input_bind[33].is_unsigned = false;
	input_bind[34].buffer_type = MYSQL_TYPE_LONGLONG;
	input_bind[34].buffer = &input_xi;
	input_bind[34].is_unsigned = false;
	input_bind[35].buffer_type = MYSQL_TYPE_TINY;
	input_bind[35].buffer = &input_qm;
	input_bind[35].is_unsigned = true;
	input_bind[36].buffer_type = MYSQL_TYPE_LONGLONG;
	input_bind[36].buffer = &input_region;
	input_bind[36].is_unsigned = true;
	input_bind[37].buffer_type = MYSQL_TYPE_LONG;
	input_bind[37].buffer = &input_c0;
	input_bind[37].is_unsigned = false;
	input_bind[38].buffer_type = MYSQL_TYPE_LONG;
	input_bind[38].buffer = &input_c1;
	input_bind[38].is_unsigned = false;
	input_bind[39].buffer_type = MYSQL_TYPE_STRING;
	input_bind[39].buffer = const_cast<char*>(&(input_n0[0]));
	input_bind[39].buffer_length = input_n0.size();
	input_bind[40].buffer_type = MYSQL_TYPE_STRING;
	input_bind[40].buffer = const_cast<char*>(&(input_n1[0]));
	input_bind[40].buffer_length = input_n1.size();
	input_bind[41].buffer_type = MYSQL_TYPE_TINY;
	input_bind[41].buffer = &input_c0c;
	input_bind[41].is_unsigned = true;
	input_bind[42].buffer_type = MYSQL_TYPE_TINY;
	input_bind[42].buffer = &input_c1c;
	input_bind[42].is_unsigned = true;
	input_bind[43].buffer_type = MYSQL_TYPE_STRING;
	input_bind[43].buffer = const_cast<char*>(&(input_team0[0]));
	input_bind[43].buffer_length = input_team0.size();
	input_bind[44].buffer_type = MYSQL_TYPE_STRING;
	input_bind[44].buffer = const_cast<char*>(&(input_team1[0]));
	input_bind[44].buffer_length = input_team1.size();
	input_bind[45].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[45].buffer = &input_score0;
	input_bind[45].is_unsigned = false;
	input_bind[46].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[46].buffer = &input_score1;
	input_bind[46].is_unsigned = false;
	
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
	
	// Update GameStat id
	int id = mysql_stmt_insert_id(statement);
	game_server.SetId(id);
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	
	// Update players to database
	this->_removeGameServerPlayers(game_server);
	for(Battlefield::GameServerPlayer gsplayer : game_server.GetPlayers())
	{
		this->_insertGameServerPlayer(game_server, gsplayer);
	}
	
	return true;
}

bool Database::updateGameServer(const Battlefield::GameServer& game_server)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "UPDATE ";
	query += "	`GameServers` ";
	query += "SET ";
	query += "	`flag` = ?, ";
	query += "	`localip0` = ?, ";
	query += "	`localport` = ?, ";
	query += "	`natneg` = ?, ";
	query += "	`gamename` = ?, ";
	query += "	`hostname` = ?, ";
	query += "	`hostport` = ?, ";
	query += "	`gamever` = ?, ";
	query += "	`cl` = ?, ";
	query += "	`rv` = ?, ";
	query += "	`map` = ?, ";
	query += "	`mc` = ?, ";
	query += "	`mapname` = ?, ";
	query += "	`gc` = ?, ";
	query += "	`gametype` = ?, ";
	query += "	`gamevariant` = ?, ";
	query += "	`numplayers` = ?, ";
	query += "	`maxplayers` = ?, ";
	query += "	`numteams` = ?, ";
	query += "	`gamemode` = ?, ";
	query += "	`teamplay` = ?, ";
	query += "	`fraglimit` = ?, ";
	query += "	`teamfraglimit` = ?, ";
	query += "	`timelimit` = ?, ";
	query += "	`timeelapsed` = ?, ";
	query += "	`password` = ?, ";
	query += "	`nr` = ?, ";
	query += "	`xr` = ?, ";
	query += "	`ff` = ?, ";
	query += "	`sr` = ?, ";
	query += "	`rc` = ?, ";
	query += "	`ni` = ?, ";
	query += "	`xi` = ?, ";
	query += "	`qm` = ?, ";
	query += "	`region` = ?, ";
	query += "	`c0` = ?, ";
	query += "	`c1` = ?, ";
	query += "	`n0` = ?, ";
	query += "	`n1` = ?, ";
	query += "	`c0c` = ?, ";
	query += "	`c1c` = ?, ";
	query += "	`team0` = ?, ";
	query += "	`team1` = ?, ";
	query += "	`score0` = ?, ";
	query += "	`score1` = ?, ";
	query += "	`updated_at` = DATE_FORMAT(NOW(), '%Y-%m-%d %H:%i:%s') ";
	query += "WHERE ";
	query += "	`id` = ?";
	
	int         input_id            = game_server.GetId();
	uint8_t     input_flag          = game_server.GetFlag();
	std::string input_localip       = game_server.GetLocalIp();
	uint16_t    input_localport     = game_server.GetLocalPort();
	uint8_t     input_natneg        = game_server.GetNatNeg();
	std::string input_gamename      = game_server.GetGameName();
	std::string input_hostname      = game_server.GetHostName();
	uint16_t    input_hostport      = game_server.GetHostPort();
	std::string input_gamever       = game_server.GetGameVersion();
	std::string input_cl            = game_server.GetClientVersion();
	std::string input_rv            = game_server.GetRV();
	std::string input_map           = game_server.GetMap();
	uint8_t     input_mc            = game_server.GetMapCycling();
	uint8_t     input_mapname       = game_server.GetMapName();
	uint8_t     input_gc            = game_server.GetGC();
	std::string input_gametype      = game_server.GetGameType();
	std::string input_gamevariant   = game_server.GetGameVariant();
	uint8_t     input_numplayers    = game_server.GetNumPlayers();
	uint8_t     input_maxplayers    = game_server.GetMaxPlayers();
	uint8_t     input_numteams      = game_server.GetNumTeams();
	std::string input_gamemode      = game_server.GetGameMode();
	uint8_t     input_teamplay      = game_server.GetTeamplay();
	uint8_t     input_fraglimit     = game_server.GetFlagLimit();
	uint8_t     input_teamfraglimit = game_server.GetTeamFragLimit();
	uint16_t    input_timelimit     = game_server.GetTimeLimit();
	uint16_t    input_timeelapsed   = game_server.GetTimeElapsed();
	uint8_t     input_password      = game_server.GetPassword();
	uint8_t     input_nr            = game_server.GetMinRank();
	uint8_t     input_xr            = game_server.GetMaxRank();
	uint8_t     input_ff            = game_server.GetFriendlyFire();
	uint8_t     input_sr            = game_server.GetStatsTracking();
	uint8_t     input_rc            = game_server.GetReconfigurable();
	int64_t     input_ni            = game_server.GetMinIpRange();
	int64_t     input_xi            = game_server.GetMaxIpRange();
	uint8_t     input_qm            = game_server.GetQM();
	uint64_t    input_region        = game_server.GetRegion();
	// Clan
	int32_t     input_c0            = game_server.GetClan1Id();
	int32_t     input_c1            = game_server.GetClan2Id();
	std::string input_n0            = game_server.GetClan1Name();
	std::string input_n1            = game_server.GetClan2Name();
	uint8_t     input_c0c           = game_server.GetClan1Claimed();
	uint8_t     input_c1c           = game_server.GetClan2Claimed();
	// Team
	std::string input_team0         = game_server.GetTeam1Name();
	std::string input_team1         = game_server.GetTeam2Name();
	int16_t     input_score0        = game_server.GetTeam1Score();
	int16_t     input_score1        = game_server.GetTeam2Score();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(46, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_TINY;
	input_bind[0].buffer = &input_flag;
	input_bind[0].is_unsigned = true;
	input_bind[1].buffer_type = MYSQL_TYPE_STRING;
	input_bind[1].buffer = const_cast<char*>(&(input_localip[0]));
	input_bind[1].buffer_length = input_localip.size();
	input_bind[2].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[2].buffer = &input_localport;
	input_bind[2].is_unsigned = true;
	input_bind[3].buffer_type = MYSQL_TYPE_TINY;
	input_bind[3].buffer = &input_natneg;
	input_bind[3].is_unsigned = true;
	input_bind[4].buffer_type = MYSQL_TYPE_STRING;
	input_bind[4].buffer = const_cast<char*>(&(input_gamename[0]));
	input_bind[4].buffer_length = input_gamename.size();
	input_bind[5].buffer_type = MYSQL_TYPE_STRING;
	input_bind[5].buffer = const_cast<char*>(&(input_hostname[0]));
	input_bind[5].buffer_length = input_hostname.size();
	input_bind[6].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[6].buffer = &input_hostport;
	input_bind[6].is_unsigned = true;
	input_bind[7].buffer_type = MYSQL_TYPE_STRING;
	input_bind[7].buffer = const_cast<char*>(&(input_gamever[0]));
	input_bind[7].buffer_length = input_gamever.size();
	input_bind[8].buffer_type = MYSQL_TYPE_STRING;
	input_bind[8].buffer = const_cast<char*>(&(input_cl[0]));
	input_bind[8].buffer_length = input_cl.size();
	input_bind[9].buffer_type = MYSQL_TYPE_STRING;
	input_bind[9].buffer = const_cast<char*>(&(input_rv[0]));
	input_bind[9].buffer_length = input_rv.size();
	input_bind[10].buffer_type = MYSQL_TYPE_STRING;
	input_bind[10].buffer = const_cast<char*>(&(input_map[0]));
	input_bind[10].buffer_length = input_map.size();
	input_bind[11].buffer_type = MYSQL_TYPE_TINY;
	input_bind[11].buffer = &input_mc;
	input_bind[11].is_unsigned = true;
	input_bind[12].buffer_type = MYSQL_TYPE_TINY;
	input_bind[12].buffer = &input_mapname;
	input_bind[12].is_unsigned = true;
	input_bind[13].buffer_type = MYSQL_TYPE_TINY;
	input_bind[13].buffer = &input_gc;
	input_bind[13].is_unsigned = true;
	input_bind[14].buffer_type = MYSQL_TYPE_STRING;
	input_bind[14].buffer = const_cast<char*>(&(input_gametype[0]));
	input_bind[14].buffer_length = input_gametype.size();
	input_bind[15].buffer_type = MYSQL_TYPE_STRING;
	input_bind[15].buffer = const_cast<char*>(&(input_gamevariant[0]));
	input_bind[15].buffer_length = input_gamevariant.size();
	input_bind[16].buffer_type = MYSQL_TYPE_TINY;
	input_bind[16].buffer = &input_numplayers;
	input_bind[16].is_unsigned = true;
	input_bind[17].buffer_type = MYSQL_TYPE_TINY;
	input_bind[17].buffer = &input_maxplayers;
	input_bind[17].is_unsigned = true;
	input_bind[18].buffer_type = MYSQL_TYPE_TINY;
	input_bind[18].buffer = &input_numteams;
	input_bind[18].is_unsigned = true;
	input_bind[19].buffer_type = MYSQL_TYPE_STRING;
	input_bind[19].buffer = const_cast<char*>(&(input_gamemode[0]));
	input_bind[19].buffer_length = input_gamemode.size();
	input_bind[20].buffer_type = MYSQL_TYPE_TINY;
	input_bind[20].buffer = &input_teamplay;
	input_bind[20].is_unsigned = true;
	input_bind[21].buffer_type = MYSQL_TYPE_TINY;
	input_bind[21].buffer = &input_fraglimit;
	input_bind[21].is_unsigned = true;
	input_bind[22].buffer_type = MYSQL_TYPE_TINY;
	input_bind[22].buffer = &input_teamfraglimit;
	input_bind[22].is_unsigned = true;
	input_bind[23].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[23].buffer = &input_timelimit;
	input_bind[23].is_unsigned = true;
	input_bind[24].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[24].buffer = &input_timeelapsed;
	input_bind[24].is_unsigned = true;
	input_bind[25].buffer_type = MYSQL_TYPE_TINY;
	input_bind[25].buffer = &input_password;
	input_bind[25].is_unsigned = true;
	input_bind[26].buffer_type = MYSQL_TYPE_TINY;
	input_bind[26].buffer = &input_nr;
	input_bind[26].is_unsigned = true;
	input_bind[27].buffer_type = MYSQL_TYPE_TINY;
	input_bind[27].buffer = &input_xr;
	input_bind[27].is_unsigned = true;
	input_bind[28].buffer_type = MYSQL_TYPE_TINY;
	input_bind[28].buffer = &input_ff;
	input_bind[28].is_unsigned = true;
	input_bind[29].buffer_type = MYSQL_TYPE_TINY;
	input_bind[29].buffer = &input_sr;
	input_bind[29].is_unsigned = true;
	input_bind[30].buffer_type = MYSQL_TYPE_TINY;
	input_bind[30].buffer = &input_rc;
	input_bind[30].is_unsigned = true;
	input_bind[31].buffer_type = MYSQL_TYPE_LONGLONG;
	input_bind[31].buffer = &input_ni;
	input_bind[31].is_unsigned = false;
	input_bind[32].buffer_type = MYSQL_TYPE_LONGLONG;
	input_bind[32].buffer = &input_xi;
	input_bind[32].is_unsigned = false;
	input_bind[33].buffer_type = MYSQL_TYPE_TINY;
	input_bind[33].buffer = &input_qm;
	input_bind[33].is_unsigned = true;
	input_bind[34].buffer_type = MYSQL_TYPE_LONGLONG;
	input_bind[34].buffer = &input_region;
	input_bind[34].is_unsigned = true;
	input_bind[35].buffer_type = MYSQL_TYPE_LONG;
	input_bind[35].buffer = &input_c0;
	input_bind[35].is_unsigned = false;
	input_bind[36].buffer_type = MYSQL_TYPE_LONG;
	input_bind[36].buffer = &input_c1;
	input_bind[36].is_unsigned = false;
	input_bind[37].buffer_type = MYSQL_TYPE_STRING;
	input_bind[37].buffer = const_cast<char*>(&(input_n0[0]));
	input_bind[37].buffer_length = input_n0.size();
	input_bind[38].buffer_type = MYSQL_TYPE_STRING;
	input_bind[38].buffer = const_cast<char*>(&(input_n1[0]));
	input_bind[38].buffer_length = input_n1.size();
	input_bind[39].buffer_type = MYSQL_TYPE_TINY;
	input_bind[39].buffer = &input_c0c;
	input_bind[39].is_unsigned = true;
	input_bind[40].buffer_type = MYSQL_TYPE_TINY;
	input_bind[40].buffer = &input_c1c;
	input_bind[40].is_unsigned = true;
	input_bind[41].buffer_type = MYSQL_TYPE_STRING;
	input_bind[41].buffer = const_cast<char*>(&(input_team0[0]));
	input_bind[41].buffer_length = input_team0.size();
	input_bind[42].buffer_type = MYSQL_TYPE_STRING;
	input_bind[42].buffer = const_cast<char*>(&(input_team1[0]));
	input_bind[42].buffer_length = input_team1.size();
	input_bind[43].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[43].buffer = &input_score0;
	input_bind[43].is_unsigned = false;
	input_bind[44].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[44].buffer = &input_score1;
	input_bind[44].is_unsigned = false;
	input_bind[45].buffer_type = MYSQL_TYPE_LONG;
	input_bind[45].buffer = &input_id;
	input_bind[45].is_unsigned = false;
	
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
	
	// Update players to database
	this->_removeGameServerPlayers(game_server);
	for(Battlefield::GameServerPlayer gsplayer : game_server.GetPlayers())
	{
		this->_insertGameServerPlayer(game_server, gsplayer);
	}
	
	return true;
}

// Game Server Player
bool Database::queryGameServerPlayers(Battlefield::GameServer& game_server)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "SELECT ";
	query += "	`id`, `name`, `score`, `skill`, `ping`, `team`, `deaths`, `profileid` ";
	query += "FROM ";
	query += "	`GameServerPlayers` ";
	query += "WHERE ";
	query += "	`gameserverid` = ?";
	
	int      input_gameserverid = game_server.GetId();
	
	int      output_id;
	char     output_name[46];
	int16_t  output_score;
	char     output_skill[46];
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
	output_bind[1].buffer_length = 46;	
	output_bind[2].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[2].buffer = &output_score;
	output_bind[2].is_unsigned = false;
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = output_skill;
	output_bind[3].buffer_length = 46;
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
	//std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

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
	//std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

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

// Game Stat
bool Database::queryGameStatsByDate(Battlefield::GameStats& game_stats, const std::string date)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "SELECT ";
	query += "	`id`, `gametype`, `gamver`, `hostname`, `mapid`, `numplayers`, `pplayers`, `tplayed`, ";
	query += "	`clanid_t0`, `clanid_t1`, `country_t0`, `country_t1`, `victory_t0`, `victory_t1`, ";
	query += "	`created_at` ";
	query += "FROM ";
	query += "	`GameStats` ";
	query += "WHERE ";
	query += "	DATE(`created_at`) = ?";
	
	int        output_id;
	uint8_t    output_gametype;
	char       output_gamever[21];
	char       output_hostname[46];
	uint8_t    output_mapid;
	uint8_t    output_numplayers;
	uint8_t    output_pplayers;
	uint16_t   output_tplayed;
	uint8_t    output_clanid_t0;
	uint8_t    output_clanid_t1;
	uint8_t    output_country_t0;
	uint8_t    output_country_t1;
	uint8_t    output_victory_t0;
	uint8_t    output_victory_t1;
	MYSQL_TIME output_created_at;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_STRING;
	input_bind[0].buffer = const_cast<char*>(&(date[0]));
	input_bind[0].buffer_length = date.size();
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(15, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_id;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_TINY;
	output_bind[1].buffer = &output_gametype;
	output_bind[1].is_unsigned = true;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = output_gamever;
	output_bind[2].buffer_length = 21;
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = output_hostname;
	output_bind[3].buffer_length = 46;
	output_bind[4].buffer_type = MYSQL_TYPE_TINY;
	output_bind[4].buffer = &output_mapid;
	output_bind[4].is_unsigned = true;
	output_bind[5].buffer_type = MYSQL_TYPE_TINY;
	output_bind[5].buffer = &output_numplayers;
	output_bind[5].is_unsigned = true;
	output_bind[6].buffer_type = MYSQL_TYPE_TINY;
	output_bind[6].buffer = &output_pplayers;
	output_bind[6].is_unsigned = true;
	output_bind[7].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[7].buffer = &output_tplayed;
	output_bind[7].is_unsigned = true;
	output_bind[8].buffer_type = MYSQL_TYPE_TINY;
	output_bind[8].buffer = &output_clanid_t0;
	output_bind[8].is_unsigned = true;
	output_bind[9].buffer_type = MYSQL_TYPE_TINY;
	output_bind[9].buffer = &output_clanid_t1;
	output_bind[9].is_unsigned = true;
	output_bind[10].buffer_type = MYSQL_TYPE_TINY;
	output_bind[10].buffer = &output_country_t0;
	output_bind[10].is_unsigned = true;
	output_bind[11].buffer_type = MYSQL_TYPE_TINY;
	output_bind[11].buffer = &output_country_t1;
	output_bind[11].is_unsigned = true;
	output_bind[12].buffer_type = MYSQL_TYPE_TINY;
	output_bind[12].buffer = &output_victory_t0;
	output_bind[12].is_unsigned = true;
	output_bind[13].buffer_type = MYSQL_TYPE_TINY;
	output_bind[13].buffer = &output_victory_t1;
	output_bind[13].is_unsigned = true;
	output_bind[14].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[14].buffer = &output_created_at;
	
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
		
		Battlefield::GameStat game_stat;
		
		game_stat.SetId(output_id);
		game_stat.SetGameType(output_gametype);
		game_stat.SetGameVersion(output_gamever);
		game_stat.SetHostName(output_hostname);
		game_stat.SetMapId(output_mapid);
		game_stat.SetNumPlayers(output_numplayers);
		game_stat.SetPPlayers(output_pplayers);
		game_stat.SetTimePlayed(output_tplayed);
		game_stat.SetTeam1ClanId(output_clanid_t0);
		game_stat.SetTeam2ClanId(output_clanid_t1);
		game_stat.SetTeam1Country(output_country_t0);
		game_stat.SetTeam2Country(output_country_t1);
		game_stat.SetTeam1Victory(output_victory_t0);
		game_stat.SetTeam2Victory(output_victory_t1);
		game_stat.SetCreatedAt(output_created_at);
		
		game_stats.push_back(game_stat);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
	
	return true;
}

bool Database::queryGameStatById(Battlefield::GameStat& game_stat)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "SELECT ";
	query += "	`gametype`, `gamver`, `hostname`, `mapid`, `numplayers`, `pplayers`, `tplayed`, ";
	query += "	`clanid_t0`, `clanid_t1`, `country_t0`, `country_t1`, `victory_t0`, `victory_t1`, ";
	query += "	`created_at` ";
	query += "FROM ";
	query += "	`GameStats` ";
	query += "WHERE ";
	query += "	`id` = ?";
	
	int        input_id = game_stat.GetId();
	
	uint8_t    output_gametype;
	char       output_gamever[21];
	char       output_hostname[46];
	uint8_t    output_mapid;
	uint8_t    output_numplayers;
	uint8_t    output_pplayers;
	uint16_t   output_tplayed;
	uint8_t    output_clanid_t0;
	uint8_t    output_clanid_t1;
	uint8_t    output_country_t0;
	uint8_t    output_country_t1;
	uint8_t    output_victory_t0;
	uint8_t    output_victory_t1;
	MYSQL_TIME output_created_at;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_id;
	input_bind[0].is_unsigned = false;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(14, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_TINY;
	output_bind[0].buffer = &output_gametype;
	output_bind[0].is_unsigned = true;
	output_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[1].buffer = output_gamever;
	output_bind[1].buffer_length = 21;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = output_hostname;
	output_bind[2].buffer_length = 46;
	output_bind[3].buffer_type = MYSQL_TYPE_TINY;
	output_bind[3].buffer = &output_mapid;
	output_bind[3].is_unsigned = true;
	output_bind[4].buffer_type = MYSQL_TYPE_TINY;
	output_bind[4].buffer = &output_numplayers;
	output_bind[4].is_unsigned = true;
	output_bind[5].buffer_type = MYSQL_TYPE_TINY;
	output_bind[5].buffer = &output_pplayers;
	output_bind[5].is_unsigned = true;
	output_bind[6].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[6].buffer = &output_tplayed;
	output_bind[6].is_unsigned = true;
	output_bind[7].buffer_type = MYSQL_TYPE_TINY;
	output_bind[7].buffer = &output_clanid_t0;
	output_bind[7].is_unsigned = true;
	output_bind[8].buffer_type = MYSQL_TYPE_TINY;
	output_bind[8].buffer = &output_clanid_t1;
	output_bind[8].is_unsigned = true;
	output_bind[9].buffer_type = MYSQL_TYPE_TINY;
	output_bind[9].buffer = &output_country_t0;
	output_bind[9].is_unsigned = true;
	output_bind[10].buffer_type = MYSQL_TYPE_TINY;
	output_bind[10].buffer = &output_country_t1;
	output_bind[10].is_unsigned = true;
	output_bind[11].buffer_type = MYSQL_TYPE_TINY;
	output_bind[11].buffer = &output_victory_t0;
	output_bind[11].is_unsigned = true;
	output_bind[12].buffer_type = MYSQL_TYPE_TINY;
	output_bind[12].buffer = &output_victory_t1;
	output_bind[12].is_unsigned = true;
	output_bind[13].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[13].buffer = &output_created_at;
	
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
		
		game_stat.SetGameType(output_gametype);
		game_stat.SetGameVersion(output_gamever);
		game_stat.SetHostName(output_hostname);
		game_stat.SetMapId(output_mapid);
		game_stat.SetNumPlayers(output_numplayers);
		game_stat.SetPPlayers(output_pplayers);
		game_stat.SetTimePlayed(output_tplayed);
		game_stat.SetTeam1ClanId(output_clanid_t0);
		game_stat.SetTeam2ClanId(output_clanid_t1);
		game_stat.SetTeam1Country(output_country_t0);
		game_stat.SetTeam2Country(output_country_t1);
		game_stat.SetTeam1Victory(output_victory_t0);
		game_stat.SetTeam2Victory(output_victory_t1);
		game_stat.SetCreatedAt(output_created_at);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
	
	return true;
}

bool Database::insertGameStat(Battlefield::GameStat& game_stat)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "INSERT INTO `GameStats` ";
	query += "	(`gametype`, `gamver`, `hostname`, `mapid`, `numplayers`, `pplayers`, `tplayed`, ";
	query += "	`clanid_t0`, `clanid_t1`, `country_t0`, `country_t1`, `victory_t0`, `victory_t1`) ";
	query += "VALUES ";
	query += "	(?, ?, ?, ?, ?, ?, ?, ";
	query += "	?, ?, ?, ?, ?, ?)";
	
	uint8_t     input_gametype   = game_stat.GetGameType();
	std::string input_gamver     = game_stat.GetGameVersion();
	std::string input_hostname   = game_stat.GetHostName();
	uint8_t     input_mapid      = game_stat.GetMapId();
	uint8_t     input_numplayers = game_stat.GetNumPlayers();
	uint8_t     input_pplayers   = game_stat.GetPPlayers();
	uint16_t    input_tplayed    = game_stat.GetTimePlayed();
	uint8_t     input_clanid_t0  = game_stat.GetTeam1ClanId();
	uint8_t     input_clanid_t1  = game_stat.GetTeam2ClanId();
	uint8_t     input_country_t0 = game_stat.GetTeam1Country();
	uint8_t     input_country_t1 = game_stat.GetTeam2Country();
	uint8_t     input_victory_t0 = game_stat.GetTeam1Victory();
	uint8_t     input_victory_t1 = game_stat.GetTeam2Victory();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(13, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_TINY;
	input_bind[0].buffer = &input_gametype;
	input_bind[0].is_unsigned = true;
	input_bind[1].buffer_type = MYSQL_TYPE_STRING;
	input_bind[1].buffer = const_cast<char*>(&(input_gamver[0]));
	input_bind[1].buffer_length = input_gamver.size();
	input_bind[2].buffer_type = MYSQL_TYPE_STRING;
	input_bind[2].buffer = const_cast<char*>(&(input_hostname[0]));
	input_bind[2].buffer_length = input_hostname.size();
	input_bind[3].buffer_type = MYSQL_TYPE_TINY;
	input_bind[3].buffer = &input_mapid;
	input_bind[3].is_unsigned = true;
	input_bind[4].buffer_type = MYSQL_TYPE_TINY;
	input_bind[4].buffer = &input_numplayers;
	input_bind[4].is_unsigned = true;
	input_bind[5].buffer_type = MYSQL_TYPE_TINY;
	input_bind[5].buffer = &input_pplayers;
	input_bind[5].is_unsigned = true;
	input_bind[6].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[6].buffer = &input_tplayed;
	input_bind[6].is_unsigned = true;
	input_bind[7].buffer_type = MYSQL_TYPE_TINY;
	input_bind[7].buffer = &input_clanid_t0;
	input_bind[7].is_unsigned = true;
	input_bind[8].buffer_type = MYSQL_TYPE_TINY;
	input_bind[8].buffer = &input_clanid_t1;
	input_bind[8].is_unsigned = true;
	input_bind[9].buffer_type = MYSQL_TYPE_TINY;
	input_bind[9].buffer = &input_country_t0;
	input_bind[9].is_unsigned = true;
	input_bind[10].buffer_type = MYSQL_TYPE_TINY;
	input_bind[10].buffer = &input_country_t1;
	input_bind[10].is_unsigned = true;
	input_bind[11].buffer_type = MYSQL_TYPE_TINY;
	input_bind[11].buffer = &input_victory_t0;
	input_bind[11].is_unsigned = true;
	input_bind[12].buffer_type = MYSQL_TYPE_TINY;
	input_bind[12].buffer = &input_victory_t1;
	input_bind[12].is_unsigned = true;
	
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
	
	// Update GameStat id
	int id = mysql_stmt_insert_id(statement);
	game_stat.SetId(id);
	
	for(Battlefield::GameStatPlayer gsplayer : game_stat.GetPlayers())
	{
		this->_insertGameStatPlayer(game_stat, gsplayer);
	}
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	
	return true;
}

// Game Stat Player
bool Database::queryGameStatPlayers(Battlefield::GameStat& game_stat)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "SELECT ";
	query += "	`id`, `auth`, `pid`, `score`, `rank`, `pph`, `kills`, `suicides`, `time`, ";
	query += "	`lavd`, `mavd`, `havd`, `hed`, `pld`, `bod`, ";
	query += "	`k1`, `s1`, `k2`, `s2`, `k3`, `s3`, `k4`, `s4`, `k5`, `s5`, ";
	query += "	`tk`, `medals`, `ttb`, `mv`, `ngp` ";
	query += "FROM ";
	query += "	`GameStatPlayers` ";
	query += "WHERE ";
	query += "	`gamestatid` = ?";
	
	int      input_gamestatid = game_stat.GetId();
	
	int      output_id;
	char     output_auth[33];
	int      output_pid;
	int32_t  output_score;
	uint32_t output_rank;
	uint32_t output_pph;
	uint32_t output_kills;
	uint32_t output_suicides;
	uint32_t output_time;
	uint32_t output_lavd;
	uint32_t output_mavd;
	uint32_t output_havd;
	uint32_t output_hed;
	uint32_t output_pld;
	uint32_t output_bod;
	uint32_t output_k1;
	uint32_t output_s1;
	uint32_t output_k2;
	uint32_t output_s2;
	uint32_t output_k3;
	uint32_t output_s3;
	uint32_t output_k4;
	uint32_t output_s4;
	uint32_t output_k5;
	uint32_t output_s5;
	uint32_t output_tk;
	uint32_t output_medals;
	uint32_t output_ttb;
	uint32_t output_mv;
	uint32_t output_ngp;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_gamestatid;
	input_bind[0].is_unsigned = false;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(30, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_id;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_STRING;
	output_bind[1].buffer = output_auth;
	output_bind[1].buffer_length = 33;
	output_bind[2].buffer_type = MYSQL_TYPE_LONG;
	output_bind[2].buffer = &output_pid;
	output_bind[2].is_unsigned = false;
	output_bind[3].buffer_type = MYSQL_TYPE_LONG;
	output_bind[3].buffer = &output_score;
	output_bind[3].is_unsigned = false;
	output_bind[4].buffer_type = MYSQL_TYPE_LONG;
	output_bind[4].buffer = &output_rank;
	output_bind[4].is_unsigned = true;
	output_bind[5].buffer_type = MYSQL_TYPE_LONG;
	output_bind[5].buffer = &output_pph;
	output_bind[5].is_unsigned = true;
	output_bind[6].buffer_type = MYSQL_TYPE_LONG;
	output_bind[6].buffer = &output_kills;
	output_bind[6].is_unsigned = true;
	output_bind[7].buffer_type = MYSQL_TYPE_LONG;
	output_bind[7].buffer = &output_suicides;
	output_bind[7].is_unsigned = true;
	output_bind[8].buffer_type = MYSQL_TYPE_LONG;
	output_bind[8].buffer = &output_time;
	output_bind[8].is_unsigned = true;
	output_bind[9].buffer_type = MYSQL_TYPE_LONG;
	output_bind[9].buffer = &output_lavd;
	output_bind[9].is_unsigned = true;
	output_bind[10].buffer_type = MYSQL_TYPE_LONG;
	output_bind[10].buffer = &output_mavd;
	output_bind[10].is_unsigned = true;
	output_bind[11].buffer_type = MYSQL_TYPE_LONG;
	output_bind[11].buffer = &output_havd;
	output_bind[11].is_unsigned = true;
	output_bind[12].buffer_type = MYSQL_TYPE_LONG;
	output_bind[12].buffer = &output_hed;
	output_bind[12].is_unsigned = true;
	output_bind[13].buffer_type = MYSQL_TYPE_LONG;
	output_bind[13].buffer = &output_pld;
	output_bind[13].is_unsigned = true;
	output_bind[14].buffer_type = MYSQL_TYPE_LONG;
	output_bind[14].buffer = &output_bod;
	output_bind[14].is_unsigned = true;
	output_bind[15].buffer_type = MYSQL_TYPE_LONG;
	output_bind[15].buffer = &output_k1;
	output_bind[15].is_unsigned = true;
	output_bind[16].buffer_type = MYSQL_TYPE_LONG;
	output_bind[16].buffer = &output_s1;
	output_bind[16].is_unsigned = true;
	output_bind[17].buffer_type = MYSQL_TYPE_LONG;
	output_bind[17].buffer = &output_k2;
	output_bind[17].is_unsigned = true;
	output_bind[18].buffer_type = MYSQL_TYPE_LONG;
	output_bind[18].buffer = &output_s2;
	output_bind[18].is_unsigned = true;
	output_bind[19].buffer_type = MYSQL_TYPE_LONG;
	output_bind[19].buffer = &output_k3;
	output_bind[19].is_unsigned = true;
	output_bind[20].buffer_type = MYSQL_TYPE_LONG;
	output_bind[20].buffer = &output_s3;
	output_bind[20].is_unsigned = true;
	output_bind[21].buffer_type = MYSQL_TYPE_LONG;
	output_bind[21].buffer = &output_k4;
	output_bind[21].is_unsigned = true;
	output_bind[22].buffer_type = MYSQL_TYPE_LONG;
	output_bind[22].buffer = &output_s4;
	output_bind[22].is_unsigned = true;
	output_bind[23].buffer_type = MYSQL_TYPE_LONG;
	output_bind[23].buffer = &output_k5;
	output_bind[23].is_unsigned = true;
	output_bind[24].buffer_type = MYSQL_TYPE_LONG;
	output_bind[24].buffer = &output_s5;
	output_bind[24].is_unsigned = true;
	output_bind[25].buffer_type = MYSQL_TYPE_LONG;
	output_bind[25].buffer = &output_tk;
	output_bind[25].is_unsigned = true;
	output_bind[26].buffer_type = MYSQL_TYPE_LONG;
	output_bind[26].buffer = &output_medals;
	output_bind[26].is_unsigned = true;
	output_bind[27].buffer_type = MYSQL_TYPE_LONG;
	output_bind[27].buffer = &output_ttb;
	output_bind[27].is_unsigned = true;
	output_bind[28].buffer_type = MYSQL_TYPE_LONG;
	output_bind[28].buffer = &output_mv;
	output_bind[28].is_unsigned = true;
	output_bind[29].buffer_type = MYSQL_TYPE_LONG;
	output_bind[29].buffer = &output_ngp;
	output_bind[29].is_unsigned = true;
	
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
		
		Battlefield::GameStatPlayer gsplayer;
		
		gsplayer.SetId(output_id);
		gsplayer.SetAuth(output_auth);
		gsplayer.SetProfileId(output_pid);
		gsplayer.SetScore(output_score);
		gsplayer.SetRank(output_rank);
		gsplayer.SetPPH(output_pph);
		gsplayer.SetKills(output_kills);
		gsplayer.SetSuicides(output_suicides);
		gsplayer.SetTime(output_time);
		gsplayer.SetLAVsDestroyed(output_lavd);
		gsplayer.SetMAVsDestroyed(output_mavd);
		gsplayer.SetHAVsDestroyed(output_havd);
		gsplayer.SetHelicoptersDestroyed(output_hed);
		gsplayer.SetPlanesDestroyed(output_pld);
		gsplayer.SetBoatsDestroyed(output_bod);
		gsplayer.SetKillsAssualtKit(output_k1);
		gsplayer.SetDeathsAssualtKit(output_s1);
		gsplayer.SetKillsSniperKit(output_k2);
		gsplayer.SetDeathsSniperKit(output_s2);
		gsplayer.SetKillsSpecialOpKit(output_k3);
		gsplayer.SetDeathsSpecialOpKit(output_s3);
		gsplayer.SetKillsCombatEngineerKit(output_k4);
		gsplayer.SetDeathsCombatEngineerKit(output_s4);
		gsplayer.SetKillsSupportKit(output_k5);
		gsplayer.SetDeathsSupportKit(output_s5);
		gsplayer.SetTeamKills(output_tk);
		gsplayer.SetMedals(output_medals);
		gsplayer.SetTotalTopPlayer(output_ttb);
		gsplayer.SetTotalVictories(output_mv);
		gsplayer.SetTotalGameSessions(output_ngp);
		
		game_stat.AddPlayer(gsplayer);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
	
	return true;
}

bool Database::_insertGameStatPlayer(const Battlefield::GameStat& game_stat, Battlefield::GameStatPlayer& gsplayer)
{
	std::string query = "";
	query += "INSERT INTO `GameStatPlayers` ";
	query += "	(`gamestatid`, `auth`, `pid`, `score`, `rank`, `pph`, `kills`, ";
	query += "	`suicides`, `time`, `lavd`, `mavd`, `havd`, `hed`, `pld`, `bod`, ";
	query += "	`k1`, `s1`, `k2`, `s2`, `k3`, `s3`, `k4`, `s4`, `k5`, `s5`, ";
	query += "	`tk`, `medals`, `ttb`, `mv`, `ngp`) ";
	query += "VALUES ";
	query += "	(?, ?, ?, ?, ?, ?, ?, ";
	query += "	?, ?, ?, ?, ?, ?, ?, ?, ";
	query += "	?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ";
	query += "	?, ?, ?, ?, ?);";
	
	int         input_gamestatid = game_stat.GetId();
	std::string input_auth       = gsplayer.GetAuth();
    int         input_pid        = gsplayer.GetProfileId();
	int32_t     input_score      = gsplayer.GetScore();
	uint32_t    input_rank       = gsplayer.GetRank();
	uint32_t    input_pph        = gsplayer.GetPPH();
	uint32_t    input_kills      = gsplayer.GetKills();
	uint32_t    input_suicides   = gsplayer.GetSuicides();
	uint32_t    input_time       = gsplayer.GetTime();
	uint32_t    input_lavd       = gsplayer.GetLAVsDestroyed();
	uint32_t    input_mavd       = gsplayer.GetMAVsDestroyed();
	uint32_t    input_havd       = gsplayer.GetHAVsDestroyed();
	uint32_t    input_hed        = gsplayer.GetHelicoptersDestroyed();
	uint32_t    input_pld        = gsplayer.GetPlanesDestroyed();
	uint32_t    input_bod        = gsplayer.GetBoatsDestroyed();
	uint32_t    input_k1         = gsplayer.GetKillsAssualtKit();
	uint32_t    input_s1         = gsplayer.GetDeathsAssualtKit();
	uint32_t    input_k2         = gsplayer.GetKillsSniperKit();
	uint32_t    input_s2         = gsplayer.GetDeathsSniperKit();
	uint32_t    input_k3         = gsplayer.GetKillsSpecialOpKit();
	uint32_t    input_s3         = gsplayer.GetDeathsSpecialOpKit();
	uint32_t    input_k4         = gsplayer.GetKillsCombatEngineerKit();
	uint32_t    input_s4         = gsplayer.GetDeathsCombatEngineerKit();
	uint32_t    input_k5         = gsplayer.GetKillsSupportKit();
	uint32_t    input_s5         = gsplayer.GetDeathsSupportKit();
	uint32_t    input_tk         = gsplayer.GetTeamKills();
	uint32_t    input_medals     = gsplayer.GetMedals();
	uint32_t    input_ttb        = gsplayer.GetTotalTopPlayer();
	uint32_t    input_mv         = gsplayer.GetTotalVictories();
	uint32_t    input_ngp        = gsplayer.GetTotalGameSessions();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(30, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_gamestatid;
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_STRING;
	input_bind[1].buffer = const_cast<char*>(&(input_auth[0]));
	input_bind[1].buffer_length = input_auth.size();
	input_bind[2].buffer_type = MYSQL_TYPE_LONG;
	input_bind[2].buffer = &input_pid;
	input_bind[2].is_unsigned = false;
	input_bind[3].buffer_type = MYSQL_TYPE_LONG;
	input_bind[3].buffer = &input_score;
	input_bind[3].is_unsigned = false;
	input_bind[4].buffer_type = MYSQL_TYPE_LONG;
	input_bind[4].buffer = &input_rank;
	input_bind[4].is_unsigned = true;
	input_bind[5].buffer_type = MYSQL_TYPE_LONG;
	input_bind[5].buffer = &input_pph;
	input_bind[5].is_unsigned = true;
	input_bind[6].buffer_type = MYSQL_TYPE_LONG;
	input_bind[6].buffer = &input_kills;
	input_bind[6].is_unsigned = true;
	input_bind[7].buffer_type = MYSQL_TYPE_LONG;
	input_bind[7].buffer = &input_suicides;
	input_bind[7].is_unsigned = true;
	input_bind[8].buffer_type = MYSQL_TYPE_LONG;
	input_bind[8].buffer = &input_time;
	input_bind[8].is_unsigned = true;
	input_bind[9].buffer_type = MYSQL_TYPE_LONG;
	input_bind[9].buffer = &input_lavd;
	input_bind[9].is_unsigned = true;
	input_bind[10].buffer_type = MYSQL_TYPE_LONG;
	input_bind[10].buffer = &input_mavd;
	input_bind[10].is_unsigned = true;
	input_bind[11].buffer_type = MYSQL_TYPE_LONG;
	input_bind[11].buffer = &input_havd;
	input_bind[11].is_unsigned = true;
	input_bind[12].buffer_type = MYSQL_TYPE_LONG;
	input_bind[12].buffer = &input_hed;
	input_bind[12].is_unsigned = true;
	input_bind[13].buffer_type = MYSQL_TYPE_LONG;
	input_bind[13].buffer = &input_pld;
	input_bind[13].is_unsigned = true;
	input_bind[14].buffer_type = MYSQL_TYPE_LONG;
	input_bind[14].buffer = &input_bod;
	input_bind[14].is_unsigned = true;
	input_bind[15].buffer_type = MYSQL_TYPE_LONG;
	input_bind[15].buffer = &input_k1;
	input_bind[15].is_unsigned = true;
	input_bind[16].buffer_type = MYSQL_TYPE_LONG;
	input_bind[16].buffer = &input_s1;
	input_bind[16].is_unsigned = true;
	input_bind[17].buffer_type = MYSQL_TYPE_LONG;
	input_bind[17].buffer = &input_k2;
	input_bind[17].is_unsigned = true;
	input_bind[18].buffer_type = MYSQL_TYPE_LONG;
	input_bind[18].buffer = &input_s2;
	input_bind[18].is_unsigned = true;
	input_bind[19].buffer_type = MYSQL_TYPE_LONG;
	input_bind[19].buffer = &input_k3;
	input_bind[19].is_unsigned = true;
	input_bind[20].buffer_type = MYSQL_TYPE_LONG;
	input_bind[20].buffer = &input_s3;
	input_bind[20].is_unsigned = true;
	input_bind[21].buffer_type = MYSQL_TYPE_LONG;
	input_bind[21].buffer = &input_k4;
	input_bind[21].is_unsigned = true;
	input_bind[22].buffer_type = MYSQL_TYPE_LONG;
	input_bind[22].buffer = &input_s4;
	input_bind[22].is_unsigned = true;
	input_bind[23].buffer_type = MYSQL_TYPE_LONG;
	input_bind[23].buffer = &input_k5;
	input_bind[23].is_unsigned = true;
	input_bind[24].buffer_type = MYSQL_TYPE_LONG;
	input_bind[24].buffer = &input_s5;
	input_bind[24].is_unsigned = true;
	input_bind[25].buffer_type = MYSQL_TYPE_LONG;
	input_bind[25].buffer = &input_tk;
	input_bind[25].is_unsigned = true;
	input_bind[26].buffer_type = MYSQL_TYPE_LONG;
	input_bind[26].buffer = &input_medals;
	input_bind[26].is_unsigned = true;
	input_bind[27].buffer_type = MYSQL_TYPE_LONG;
	input_bind[27].buffer = &input_ttb;
	input_bind[27].is_unsigned = true;
	input_bind[28].buffer_type = MYSQL_TYPE_LONG;
	input_bind[28].buffer = &input_mv;
	input_bind[28].is_unsigned = true;
	input_bind[29].buffer_type = MYSQL_TYPE_LONG;
	input_bind[29].buffer = &input_ngp;
	input_bind[29].is_unsigned = true;
	
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
	
	// Update GameStat id
	int id = mysql_stmt_insert_id(statement);
	gsplayer.SetId(id);
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	
	return true;
}

// Leaderboard Rank
bool Database::queryLeaderboardRank(Battlefield::RankPlayers& rank_players,
		uint32_t limit, uint32_t offset)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "SELECT ";
	query += "	`rank`, `profileid`, `uniquenick`, `score`, `ran`, `pph` ";
	query += "FROM ";
	query += "	`Leaderboard_rank` ";
	query += "ORDER BY ";
	query += "	`rank` ASC, ";
	query += "	`ran` DESC, ";
	query += "	`score` DESC, ";
	query += "	`pph` DESC ";
	query += "LIMIT ? OFFSET ?";
	
	int  output_rank;
	int  output_profileid;
	char output_uniquenick[33];
	int  output_score = -1;
	int  output_ran = -1;
	int  output_pph = -1;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(30, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &limit;
	input_bind[0].is_unsigned = true;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = &offset;
	input_bind[1].is_unsigned = true;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(6, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = const_cast<int*>(&output_rank);
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = const_cast<int*>(&output_profileid);
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_uniquenick;
	output_bind[2].buffer_length = 33;
	output_bind[3].buffer_type = MYSQL_TYPE_LONG;
	output_bind[3].buffer = const_cast<int*>(&output_score);
	output_bind[3].is_unsigned = false;
	output_bind[4].buffer_type = MYSQL_TYPE_LONG;
	output_bind[4].buffer = const_cast<int*>(&output_ran);
	output_bind[4].is_unsigned = false;
	output_bind[5].buffer_type = MYSQL_TYPE_LONG;
	output_bind[5].buffer = const_cast<int*>(&output_pph);
	output_bind[5].is_unsigned = false;

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
		
		Battlefield::Player player;
		
		player.SetProfileId(output_profileid);
		player.SetUniquenick(output_uniquenick);
		player.SetScore(output_score);
		player.SetRank(output_ran);
		player.SetPPH(output_pph);
		
		rank_players.insert(std::make_pair(output_rank, player));
	}
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
		
	return true;
}

bool Database::queryLeaderboardRankByProfileid(Battlefield::RankPlayers& rank_players, int profileid)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "SELECT `rp`.* ";
	query += "FROM ";
	query += "	`Leaderboard_rank` AS `rp` ";
	query += "JOIN ( ";
	query += "	SELECT ";
	query += "		`profileid`, ";
	query += "		`rank` AS `start_rank` ";
	query += "	FROM ";
	query += "		`Leaderboard_rank` ";
	query += "	WHERE ";
	query += "		`profileid` = ? ";
	query += ") AS `start` ON rp.rank >= CASE WHEN start.start_rank <= 4 THEN 1 ELSE start.start_rank - 4 END ";
	query += "JOIN ( ";
	query += "	SELECT ";
	query += "		`profileid`, ";
	query += "		`rank` AS `end_rank` ";
	query += "	FROM ";
	query += "		`Leaderboard_rank` ";
	query += "	WHERE ";
	query += "		`profileid` = ? ";
	query += ") AS `end` ON rp.rank <= CASE WHEN end.end_rank <= 4 THEN 10 ELSE end.end_rank + 5 END ";
	query += "ORDER BY ";
	query += "	`rank` ASC, ";
	query += "	`ran` DESC, ";
	query += "	`score` DESC, ";
	query += "	`pph` DESC";
	
	int input_profileid = profileid;
	
	int  output_rank;
	int  output_profileid;
	char output_uniquenick[33];
	int  output_score = -1;
	int  output_ran = -1;
	int  output_pph = -1;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_profileid;
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = &input_profileid;
	input_bind[1].is_unsigned = false;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(6, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = const_cast<int*>(&output_rank);
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = const_cast<int*>(&output_profileid);
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_uniquenick;
	output_bind[2].buffer_length = 33;
	output_bind[3].buffer_type = MYSQL_TYPE_LONG;
	output_bind[3].buffer = const_cast<int*>(&output_score);
	output_bind[3].is_unsigned = false;
	output_bind[4].buffer_type = MYSQL_TYPE_LONG;
	output_bind[4].buffer = const_cast<int*>(&output_ran);
	output_bind[4].is_unsigned = false;
	output_bind[5].buffer_type = MYSQL_TYPE_LONG;
	output_bind[5].buffer = const_cast<int*>(&output_pph);
	output_bind[5].is_unsigned = false;

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
		
		Battlefield::Player player;
		
		player.SetProfileId(output_profileid);
		player.SetUniquenick(output_uniquenick);
		player.SetScore(output_score);
		player.SetRank(output_ran);
		player.SetPPH(output_pph);
		
		rank_players.insert(std::make_pair(output_rank, player));
	}
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
		
	return true;
}

bool Database::queryLeaderboardRankByFriends(Battlefield::RankPlayers& rank_players, const std::vector<int>& friends)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "SELECT ";
	query += "	ROW_NUMBER() OVER (";
	query += "		ORDER BY ";
	query += "			`ran` DESC, ";
	query += "			`score` DESC, ";
	query += "			`pph` DESC";
	query += "	) AS `rank`, ";
	query += "	Players.profileid  AS `profileid`, ";
	query += "	Players.uniquenick AS `uniquenick`, ";
	query += "	PlayerStats.score AS `score`, ";
	query += "	PlayerStats.ran AS `ran`, ";
	query += "	PlayerStats.pph AS `pph` ";
	query += "FROM ";
	query += "	`Players`, ";
	query += "	`PlayerStats` ";
	query += "WHERE ";
	query += "	Players.profileid = PlayerStats.profileid ";
	query += "AND ";
	query += "	Players.profileid IN (" + Util::ToString(friends) + ") ";
	query += "ORDER BY ";
	query += "	`ran` DESC, ";
	query += "	`score` DESC, ";
	query += "	`pph` DESC ";
	query += "LIMIT 10;";
	
	int  output_rank;
	int  output_profileid;
	char output_uniquenick[33];
	int  output_score = -1;
	int  output_ran = -1;
	int  output_pph = -1;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(6, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = const_cast<int*>(&output_rank);
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = const_cast<int*>(&output_profileid);
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_uniquenick;
	output_bind[2].buffer_length = 33;
	output_bind[3].buffer_type = MYSQL_TYPE_LONG;
	output_bind[3].buffer = const_cast<int*>(&output_score);
	output_bind[3].is_unsigned = false;
	output_bind[4].buffer_type = MYSQL_TYPE_LONG;
	output_bind[4].buffer = const_cast<int*>(&output_ran);
	output_bind[4].is_unsigned = false;
	output_bind[5].buffer_type = MYSQL_TYPE_LONG;
	output_bind[5].buffer = const_cast<int*>(&output_pph);
	output_bind[5].is_unsigned = false;

	// Prepare and execute with binds
	MYSQL_STMT* statement;
	
	if(
		!this->_init(&statement) ||
		!this->_prepare(statement, query) ||
		!this->_execute(statement, output_bind)
	)
	{
		// Cleanup
		free(output_bind);
		
		return false;
	}
	
	// Fetch and process rows
	while (true)
	{
		int status = mysql_stmt_fetch(statement);
		
		if (status == 1 || status == MYSQL_NO_DATA)
			break;
		
		Battlefield::Player player;
		
		player.SetProfileId(output_profileid);
		player.SetUniquenick(output_uniquenick);
		player.SetScore(output_score);
		player.SetRank(output_ran);
		player.SetPPH(output_pph);
		
		rank_players.insert(std::make_pair(output_rank, player));
	}
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(output_bind);
		
	return true;
}

// Leaderboard Type
bool Database::queryLeaderboardType(Battlefield::RankPlayers& rank_players, const std::string& type,
		uint32_t limit, uint32_t offset)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "SELECT ";
	query += "	`rank`, `profileid`, `uniquenick`, `" + type + "` ";
	query += "FROM ";
	query += "	`Leaderboard_" + type + "` ";
	query += "ORDER BY ";
	query += "	`rank` ASC, ";
	query += "	`" + type + "` DESC ";
	query += "LIMIT ? OFFSET ?";
	
	int  output_rank;
	int  output_profileid;
	char output_uniquenick[33];
	int  output_value;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(30, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &limit;
	input_bind[0].is_unsigned = true;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = &offset;
	input_bind[1].is_unsigned = true;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(4, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = const_cast<int*>(&output_rank);
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = const_cast<int*>(&output_profileid);
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_uniquenick;
	output_bind[2].buffer_length = 33;
	output_bind[3].buffer_type = MYSQL_TYPE_LONG;
	output_bind[3].buffer = const_cast<int*>(&output_value);
	output_bind[3].is_unsigned = false;
	
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
		
		Battlefield::Player player;
		
		player.SetProfileId(output_profileid);
		player.SetUniquenick(output_uniquenick);
		
		if(type == "score")         { player.SetScore(output_value);                  }
		else if(type == "pph")      { player.SetPPH(output_value);                    }
		else if(type == "kills")    { player.SetKills(output_value);                  }
		else if(type == "vehicles") { player.SetVehiclesDestroyed(output_value);      }
		else if(type == "lavd")     { player.SetLAVsDestroyed(output_value);          }
		else if(type == "mavd")     { player.SetMAVsDestroyed(output_value);          }
		else if(type == "havd")     { player.SetHAVsDestroyed(output_value);          }
		else if(type == "hed")      { player.SetHelicoptersDestroyed(output_value);   }
		else if(type == "bod")      { player.SetBoatsDestroyed(output_value);         }
		else if(type == "k1")       { player.SetKillsAssualtKit(output_value);        }
		else if(type == "k2")       { player.SetKillsSniperKit(output_value);         }
		else if(type == "k3")       { player.SetKillsSpecialOpKit(output_value);      }
		else if(type == "k4")       { player.SetKillsCombatEngineerKit(output_value); }
		else if(type == "k5")       { player.SetKillsSupportKit(output_value);        }
		
		rank_players.insert(std::make_pair(output_rank, player));
	}
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
		
	return true;
}

bool Database::queryLeaderboardTypeByProfileid(Battlefield::RankPlayers& rank_players, const std::string& type, int profileid)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "SELECT `rp`.* ";
	query += "FROM ";
	query += "	`Leaderboard_" + type + "` AS `rp` ";
	query += "JOIN ( ";
	query += "	SELECT ";
	query += "		`profileid`, ";
	query += "		`rank` AS `start_rank` ";
	query += "	FROM ";
	query += "		`Leaderboard_" + type + "` ";
	query += "	WHERE ";
	query += "		`profileid` = ? ";
	query += ") AS `start` ON rp.rank >= CASE WHEN start.start_rank <= 4 THEN 1 ELSE start.start_rank - 4 END ";
	query += "JOIN ( ";
	query += "	SELECT ";
	query += "		`profileid`, ";
	query += "		`rank` AS `end_rank` ";
	query += "	FROM ";
	query += "		`Leaderboard_" + type + "` ";
	query += "	WHERE ";
	query += "		`profileid` = ? ";
	query += ") AS `end` ON rp.rank <= CASE WHEN end.end_rank <= 4 THEN 10 ELSE end.end_rank + 5 END ";
	query += "ORDER BY ";
	query += "	`rank` ASC, ";
	query += "	`" + type + "` DESC";
	
	int input_profileid = profileid;
	
	int  output_rank;
	int  output_profileid;
	char output_uniquenick[33];
	int  output_value;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_profileid;
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = &input_profileid;
	input_bind[1].is_unsigned = false;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(4, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = const_cast<int*>(&output_rank);
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = const_cast<int*>(&output_profileid);
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_uniquenick;
	output_bind[2].buffer_length = 33;
	output_bind[3].buffer_type = MYSQL_TYPE_LONG;
	output_bind[3].buffer = const_cast<int*>(&output_value);
	output_bind[3].is_unsigned = false;

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
		
		Battlefield::Player player;
		
		player.SetProfileId(output_profileid);
		player.SetUniquenick(output_uniquenick);
		
		if(type == "score")         { player.SetScore(output_value);                  }
		else if(type == "pph")      { player.SetPPH(output_value);                    }
		else if(type == "kills")    { player.SetKills(output_value);                  }
		else if(type == "vehicles") { player.SetVehiclesDestroyed(output_value);      }
		else if(type == "lavd")     { player.SetLAVsDestroyed(output_value);          }
		else if(type == "mavd")     { player.SetMAVsDestroyed(output_value);          }
		else if(type == "havd")     { player.SetHAVsDestroyed(output_value);          }
		else if(type == "hed")      { player.SetHelicoptersDestroyed(output_value);   }
		else if(type == "bod")      { player.SetBoatsDestroyed(output_value);         }
		else if(type == "k1")       { player.SetKillsAssualtKit(output_value);        }
		else if(type == "k2")       { player.SetKillsSniperKit(output_value);         }
		else if(type == "k3")       { player.SetKillsSpecialOpKit(output_value);      }
		else if(type == "k4")       { player.SetKillsCombatEngineerKit(output_value); }
		else if(type == "k5")       { player.SetKillsSupportKit(output_value);        }
		
		rank_players.insert(std::make_pair(output_rank, player));
	}
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
		
	return true;
}

bool Database::queryLeaderboardTypeByFriends(Battlefield::RankPlayers& rank_players, const std::string& type,
		const std::vector<int>& friends)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "SELECT ";
	query += "	ROW_NUMBER() OVER (";
	query += "		ORDER BY ";
	query += "			`" + type + "` DESC ";
	query += "	) AS `rank`, ";
	query += "	Players.profileid  AS `profileid`, ";
	query += "	Players.uniquenick AS `uniquenick`, ";
	query += "	PlayerStats." + type + " AS '" + type + "' ";
	query += "FROM ";
	query += "	`Players`, ";
	query += "	`PlayerStats` ";
	query += "WHERE ";
	query += "	Players.profileid = PlayerStats.profileid ";
	query += "AND ";
	query += "	Players.profileid IN (" + Util::ToString(friends) + ") ";
	query += "ORDER BY ";
	query += "	`rank` ASC, ";
	query += "	`" + type + "` DESC ";
	query += "LIMIT 10;";
	
	int  output_rank;
	int  output_profileid;
	char output_uniquenick[33];
	int  output_value;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(4, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = const_cast<int*>(&output_rank);
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = const_cast<int*>(&output_profileid);
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_uniquenick;
	output_bind[2].buffer_length = 33;
	output_bind[3].buffer_type = MYSQL_TYPE_LONG;
	output_bind[3].buffer = const_cast<int*>(&output_value);
	output_bind[3].is_unsigned = false;

	// Prepare and execute with binds
	MYSQL_STMT* statement;
	if(
		!this->_init(&statement) ||
		!this->_prepare(statement, query) ||
		!this->_execute(statement, output_bind)
	)
	{
		// Cleanup
		free(output_bind);
		
		return false;
	}
	
	// Fetch and process rows
	while (true)
	{
		int status = mysql_stmt_fetch(statement);
		
		if (status == 1 || status == MYSQL_NO_DATA)
			break;
		
		Battlefield::Player player;
		
		player.SetProfileId(output_profileid);
		player.SetUniquenick(output_uniquenick);
		
		if(type == "score")         { player.SetScore(output_value);                  }
		else if(type == "pph")      { player.SetPPH(output_value);                    }
		else if(type == "kills")    { player.SetKills(output_value);                  }
		else if(type == "vehicles") { player.SetVehiclesDestroyed(output_value);      }
		else if(type == "lavd")     { player.SetLAVsDestroyed(output_value);          }
		else if(type == "mavd")     { player.SetMAVsDestroyed(output_value);          }
		else if(type == "havd")     { player.SetHAVsDestroyed(output_value);          }
		else if(type == "hed")      { player.SetHelicoptersDestroyed(output_value);   }
		else if(type == "bod")      { player.SetBoatsDestroyed(output_value);         }
		else if(type == "k1")       { player.SetKillsAssualtKit(output_value);        }
		else if(type == "k2")       { player.SetKillsSniperKit(output_value);         }
		else if(type == "k3")       { player.SetKillsSpecialOpKit(output_value);      }
		else if(type == "k4")       { player.SetKillsCombatEngineerKit(output_value); }
		else if(type == "k5")       { player.SetKillsSupportKit(output_value);        }
		
		rank_players.insert(std::make_pair(output_rank, player));
	}
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(output_bind);
		
	return true;
}

// Leaderboard Ratio
bool Database::queryLeaderboardRatio(Battlefield::RankPlayers& rank_players, const std::string& k, const std::string& s,
		uint32_t limit, uint32_t offset)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)
	
	std::string query = "";
	query += "SELECT ";
	query += "	`rank`, `profileid`, `uniquenick`, `ratio`, `" + k + "`, `" + s + "` ";
	query += "FROM ";
	query += "	`Leaderboard_ratio_" + k + "_" + s + "` ";
	query += "ORDER BY ";
	query += "	`rank` ASC, ";
	query += "	`ratio` DESC ";
	query += "LIMIT ? OFFSET ?";
	
	int  output_rank;
	int  output_profileid;
	char output_uniquenick[33];
	int  output_ratio;
	int  output_k;
	int  output_s;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(30, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &limit;
	input_bind[0].is_unsigned = true;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = &offset;
	input_bind[1].is_unsigned = true;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(6, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = const_cast<int*>(&output_rank);
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = const_cast<int*>(&output_profileid);
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_uniquenick;
	output_bind[2].buffer_length = 33;
	output_bind[3].buffer_type = MYSQL_TYPE_LONG;
	output_bind[3].buffer = const_cast<int*>(&output_ratio);
	output_bind[3].is_unsigned = false;
	output_bind[4].buffer_type = MYSQL_TYPE_LONG;
	output_bind[4].buffer = const_cast<int*>(&output_k);
	output_bind[4].is_unsigned = false;
	output_bind[5].buffer_type = MYSQL_TYPE_LONG;
	output_bind[5].buffer = const_cast<int*>(&output_s);
	output_bind[5].is_unsigned = false;
	
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
		
		Battlefield::Player player;
		
		player.SetProfileId(output_profileid);
		player.SetUniquenick(output_uniquenick);
		
		if(k == "kills")   { player.SetKills(output_k); }
		else if(k == "k1") { player.SetKillsAssualtKit(output_k); }
		else if(k == "k2") { player.SetKillsSniperKit(output_k); }
		else if(k == "k3") { player.SetKillsSpecialOpKit(output_k); }
		else if(k == "k4") { player.SetKillsCombatEngineerKit(output_k); }
		else if(k == "k5") { player.SetKillsSupportKit(output_k); }
		
		if(s == "deaths")  { player.SetDeaths(output_s); }
		else if(s == "s1") { player.SetDeathsAssualtKit(output_s); }
		else if(s == "s2") { player.SetDeathsSniperKit(output_s); }
		else if(s == "s3") { player.SetDeathsSpecialOpKit(output_s); }
		else if(s == "s4") { player.SetDeathsCombatEngineerKit(output_s); }
		else if(s == "s5") { player.SetDeathsSupportKit(output_s); }
		
		rank_players.insert(std::make_pair(output_rank, player));
	}
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
		
	return true;
}

bool Database::queryLeaderboardRatioByProfileid(Battlefield::RankPlayers& rank_players, int profileid, const std::string& k,
		const std::string& s)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "SELECT `rp`.* ";
	query += "FROM ";
	query += "	`Leaderboard_ratio_" + k + "_" + s + "` AS `rp` ";
	query += "JOIN ( ";
	query += "	SELECT ";
	query += "		`profileid`, ";
	query += "		`rank` AS `start_rank` ";
	query += "	FROM ";
	query += "		`Leaderboard_ratio_" + k + "_" + s + "` ";
	query += "	WHERE ";
	query += "		`profileid` = ? ";
	query += ") AS `start` ON rp.rank >= CASE WHEN start.start_rank <= 4 THEN 1 ELSE start.start_rank - 4 END ";
	query += "JOIN ( ";
	query += "	SELECT ";
	query += "		`profileid`, ";
	query += "		`rank` AS `end_rank` ";
	query += "	FROM ";
	query += "		`Leaderboard_ratio_" + k + "_" + s + "` ";
	query += "	WHERE ";
	query += "		`profileid` = ? ";
	query += ") AS `end` ON rp.rank <= CASE WHEN end.end_rank <= 4 THEN 10 ELSE end.end_rank + 5 END ";
	query += "ORDER BY ";
	query += "	`rank` ASC, ";
	query += "	`ratio` DESC";
	
	int input_profileid = profileid;
	
	int  output_rank;
	int  output_profileid;
	char output_uniquenick[33];
	int  output_ratio;
	int  output_k;
	int  output_s;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_profileid;
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = &input_profileid;
	input_bind[1].is_unsigned = false;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(6, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = const_cast<int*>(&output_rank);
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = const_cast<int*>(&output_profileid);
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_uniquenick;
	output_bind[2].buffer_length = 33;
	output_bind[3].buffer_type = MYSQL_TYPE_LONG;
	output_bind[3].buffer = const_cast<int*>(&output_ratio);
	output_bind[3].is_unsigned = false;
	output_bind[4].buffer_type = MYSQL_TYPE_LONG;
	output_bind[4].buffer = const_cast<int*>(&output_k);
	output_bind[4].is_unsigned = false;
	output_bind[5].buffer_type = MYSQL_TYPE_LONG;
	output_bind[5].buffer = const_cast<int*>(&output_s);
	output_bind[5].is_unsigned = false;
	
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
		
		Battlefield::Player player;
		
		player.SetProfileId(output_profileid);
		player.SetUniquenick(output_uniquenick);
		
		if(k == "kills")   { player.SetKills(output_k); }
		else if(k == "k1") { player.SetKillsAssualtKit(output_k); }
		else if(k == "k2") { player.SetKillsSniperKit(output_k); }
		else if(k == "k3") { player.SetKillsSpecialOpKit(output_k); }
		else if(k == "k4") { player.SetKillsCombatEngineerKit(output_k); }
		else if(k == "k5") { player.SetKillsSupportKit(output_k); }
		
		if(s == "deaths")  { player.SetDeaths(output_s); }
		else if(s == "s1") { player.SetDeathsAssualtKit(output_s); }
		else if(s == "s2") { player.SetDeathsSniperKit(output_s); }
		else if(s == "s3") { player.SetDeathsSpecialOpKit(output_s); }
		else if(s == "s4") { player.SetDeathsCombatEngineerKit(output_s); }
		else if(s == "s5") { player.SetDeathsSupportKit(output_s); }
		
		rank_players.insert(std::make_pair(output_rank, player));
	}
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
		
	return true;
}

bool Database::queryLeaderboardRatioByFriends(Battlefield::RankPlayers& rank_players, const std::vector<int>& friends,
		const std::string& k, const std::string& s)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "SELECT ";
	query += "	ROW_NUMBER() OVER (";
	query += "		ORDER BY ";
	query += "			PlayerStats." + k + " / PlayerStats." + s + " DESC ";
	query += "	) AS `rank`, ";
	query += "	Players.profileid                             AS `profileid`, ";
	query += "	Players.uniquenick                            AS `uniquenick`, ";
	query += "	PlayerStats." + k + " / PlayerStats." + s + " AS `ratio`, ";
	query += "	PlayerStats." + k + "                         AS `" + k + "`, ";
	query += "	PlayerStats." + s + "                         AS `" + s + "` ";
	query += "FROM ";
	query += "	`Players`, ";
	query += "	`PlayerStats` ";
	query += "WHERE ";
	query += "	Players.profileid = PlayerStats.profileid  AND ";
	query += "	PlayerStats." + k + " != 0 AND ";
	query += "	PlayerStats." + s + " != 0 ";
	query += "AND ";
	query += "	Players.profileid IN (" + Util::ToString(friends) + ") ";
	query += "ORDER BY ";
	query += "	`rank` ASC, ";
	query += "	`ratio` DESC ";
	query += "LIMIT 10;";
	
	int  output_rank;
	int  output_profileid;
	char output_uniquenick[33];
	int  output_ratio;
	int  output_k;
	int  output_s;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(6, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = const_cast<int*>(&output_rank);
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = const_cast<int*>(&output_profileid);
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_uniquenick;
	output_bind[2].buffer_length = 33;
	output_bind[3].buffer_type = MYSQL_TYPE_LONG;
	output_bind[3].buffer = const_cast<int*>(&output_ratio);
	output_bind[3].is_unsigned = false;
	output_bind[4].buffer_type = MYSQL_TYPE_LONG;
	output_bind[4].buffer = const_cast<int*>(&output_k);
	output_bind[4].is_unsigned = false;
	output_bind[5].buffer_type = MYSQL_TYPE_LONG;
	output_bind[5].buffer = const_cast<int*>(&output_s);
	output_bind[5].is_unsigned = false;

	// Prepare and execute with binds
	MYSQL_STMT* statement;
	
	if(
		!this->_init(&statement) ||
		!this->_prepare(statement, query) ||
		!this->_execute(statement, output_bind)
	)
	{
		// Cleanup
		free(output_bind);
		
		return false;
	}
	
	// Fetch and process rows
	while (true)
	{
		int status = mysql_stmt_fetch(statement);
		
		if (status == 1 || status == MYSQL_NO_DATA)
			break;
		
		Battlefield::Player player;
		
		player.SetProfileId(output_profileid);
		player.SetUniquenick(output_uniquenick);
		
		if(k == "kills")   { player.SetKills(output_k); }
		else if(k == "k1") { player.SetKillsAssualtKit(output_k); }
		else if(k == "k2") { player.SetKillsSniperKit(output_k); }
		else if(k == "k3") { player.SetKillsSpecialOpKit(output_k); }
		else if(k == "k4") { player.SetKillsCombatEngineerKit(output_k); }
		else if(k == "k5") { player.SetKillsSupportKit(output_k); }
		
		if(s == "deaths")  { player.SetDeaths(output_s); }
		else if(s == "s1") { player.SetDeathsAssualtKit(output_s); }
		else if(s == "s2") { player.SetDeathsSniperKit(output_s); }
		else if(s == "s3") { player.SetDeathsSpecialOpKit(output_s); }
		else if(s == "s4") { player.SetDeathsCombatEngineerKit(output_s); }
		else if(s == "s5") { player.SetDeathsSupportKit(output_s); }
		
		rank_players.insert(std::make_pair(output_rank, player));
	}
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(output_bind);
		
	return true;
}

bool Database::createLeaderboards()
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

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

// Private functions
bool Database::_connect()
{	
	std::shared_lock<std::shared_mutex> guard(g_mutex_settings); // Read only

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

