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

	int  input_profileid = player.GetProfileId();
	
	int  output_userid = 0;
	char output_nick[42];
	char output_uniquenick[33];
	char output_email[51];
	char output_md5password[33];

	std::string query = "SELECT `userid`, `nick`, `uniquenick`, `email`, `password` FROM `Players` WHERE `profileid` = ?";

	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_profileid;
	input_bind[0].is_unsigned = false;

	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(5, sizeof(MYSQL_BIND));
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
	
	std::string query = "SELECT `profileid`, `userid`, `nick`, `email`, `password` FROM `Players` WHERE `uniquenick` = ?";
	
	std::string input_uniquenick = player.GetUniquenick();
	
	int  output_profileid = 0;
	int  output_userid = 0;
	char output_nick[42];
	char output_email[51];
	char output_md5password[33];

	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_STRING;
	input_bind[0].buffer = const_cast<char*>(&(input_uniquenick[0]));
	input_bind[0].buffer_length = input_uniquenick.size();

	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(5, sizeof(MYSQL_BIND));
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

	std::string query = "SELECT `profileid`, `userid`, `nick`, `uniquenick`, `email`, `password`  FROM `Players` WHERE `email` = ?";
	
	int  output_profileid = 0;
	int  output_userid = 0;
	char output_nick[42];
	char output_uniquenick[33];
	char output_email[51];
	char output_md5password[33];

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

	std::string query;
	
	query += "SELECT `profileid`, `userid`, `nick`, `uniquenick`, `email`, `password` ";
	query += "FROM `Players` ";
	query += "WHERE `email` = ? OR `uniquenick` = ?";
	
	int  output_profileid = 0;
	int  output_userid = 0;
	char output_nick[42];
	char output_uniquenick[33];
	char output_email[51];
	char output_md5password[33];

	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_STRING;
	input_bind[0].buffer = const_cast<char*>(&(email[0]));
	input_bind[0].buffer_length = email.size();
	input_bind[1].buffer_type = MYSQL_TYPE_STRING;
	input_bind[1].buffer = const_cast<char*>(&(uniquenick[0]));
	input_bind[1].buffer_length = uniquenick.size();
	
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

	std::string query = "Select MAX(userid) + 1 as `maxuserid` FROM `Players`;";
	
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
	std::string query = "UPDATE `Players` SET ";
	query += "`last_login` = DATE_FORMAT(NOW(), '%Y-%m-%d %H:%i:%s'), ";
	query += "`last_login_ip` = ? ";
	query += "WHERE `profileid` = ?;";
	
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

bool Database::insertPlayer(const Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)
	
	std::string query;
	query += "INSERT INTO ";
	query += "	`Players` (";
	query += "		`userid`, `nick`, `uniquenick`, `email`, `password`, `created_at`";
	query += "	)";
	query += "	VALUES ";
	query += "		(?, ?, ?, ?, ?, DATE_FORMAT(NOW(), '%Y-%m-%d %H:%i:%s'));";
	
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

	std::string query = "SELECT `profileid`, `target_profileid` FROM `PlayerFriends` WHERE `profileid` = ? OR `target_profileid` = ?";
	
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

	std::string query = "INSERT INTO `PlayerFriends` (`profileid`, `target_profileid`) VALUES (?, ?);";
	
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

	std::string query;
	
	query += "DELETE FROM ";
	query += "	`PlayerFriends` ";
	query += "WHERE ";
	query += "	(`profileid` = ? and `target_profileid` = ?) ";
	query += "OR ";
	query += "	(`profileid` = ? and `target_profileid` = ?);";
	
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

	std::string query = "SELECT ";
	query += "`score`, ";
	query += "`ran`, ";
	query += "`pph`, ";
	query += "`kills`, ";
	query += "`deaths`, ";
	query += "`suicides`, ";
	query += "`time`, ";
	query += "`vehicles`, ";
	query += "`lavd`, ";
	query += "`mavd`, ";
	query += "`havd`, ";
	query += "`hed`, ";
	query += "`bod`, ";
	query += "`k1`, ";
	query += "`s1`, ";
	query += "`k2`, ";
	query += "`s2`, ";
	query += "`k3`, ";
	query += "`s3`, ";
	query += "`k4`, ";
	query += "`s4`, ";
	query += "`k5`, ";
	query += "`s5`, ";
	query += "`medals`, ";
	query += "`ttb`, ";
	query += "`mv`, ";
	query += "`ngp` ";
	query += "FROM `PlayerStats` WHERE `profileid` = ?;";
	
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

	std::string query = "UPDATE `PlayerStats` SET ";
	query += "`score` = ?, ";
	query += "`ran` = ?, ";
	query += "`pph` = ?, ";
	query += "`kills` = ?, ";
	query += "`deaths` = ?, ";
	query += "`suicides` = ?, ";
	query += "`time` = ?, ";
	query += "`vehicles` = ?, ";
	query += "`lavd` = ?, ";
	query += "`mavd` = ?, ";
	query += "`havd` = ?, ";
	query += "`hed` = ?, ";
	query += "`bod` = ?, ";
	query += "`k1` = ?, ";
	query += "`s1` = ?, ";
	query += "`k2` = ?, ";
	query += "`s2` = ?, ";
	query += "`k3` = ?, ";
	query += "`s3` = ?, ";
	query += "`k4` = ?, ";
	query += "`s4` = ?, ";
	query += "`k5` = ?, ";
	query += "`s5` = ?, ";
	query += "`medals` = ?, ";
	query += "`ttb` = ?, ";
	query += "`mv` = ?, ";
	query += "`ngp` = ? ";
	query += "WHERE `profileid` = ?;";
	
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

	std::string query = "INSERT INTO `PlayerStats` (`profileid`) VALUES (?);";
	
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
bool Database::queryClanByNameOrTag(Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)
	
	std::string query = "SELECT `clanid`, `homepage`, `info`, `region` FROM `Clans` WHERE `name` = ? OR `tag` = ?";
	
	std::string input_name = clan.GetName();
	std::string input_tag = clan.GetTag();
	
	int  output_clanid = 0;
	char output_homepage[257];
	char output_info[1025];
	int  output_region = 0;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	input_bind[0].buffer = const_cast<char*>(&(input_name[0]));
	input_bind[0].buffer_length = input_name.size();
	input_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	input_bind[1].buffer = const_cast<char*>(&(input_tag[0]));
	input_bind[1].buffer_length = input_tag.size();
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(4, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_clanid;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[1].buffer = &output_homepage;
	output_bind[1].buffer_length = 257;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_info;
	output_bind[2].buffer_length = 1025;
	output_bind[3].buffer_type = MYSQL_TYPE_LONG;
	output_bind[3].buffer = &output_region;
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

	int status = mysql_stmt_fetch(statement);

	if (status != 1 && status != MYSQL_NO_DATA)
	{		
		clan.SetClanId(output_clanid);
		clan.SetHomepage(output_homepage);
		clan.SetInfo(output_info);
		clan.SetRegion(output_region);
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
	
	std::string query = "SELECT `clanid` FROM `ClanRanks` WHERE `profileid` = ?";
	
	int input_profileid = player.GetProfileId();
	
	int output_clanid;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_profileid;
	input_bind[0].is_unsigned = false;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_clanid;
	output_bind[0].is_unsigned = false;

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
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
	
	return true;
}

bool Database::queryClanByClanId(Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	int input_clanid = clan.GetClanId();
	
	char output_name[33];
	char output_tag[4];
	char output_homepage[257];
	char output_info[1025];
	int  output_region = 0;

	std::string query = "SELECT `name`, `tag`, `homepage`, `info`, `region` FROM `Clans` WHERE `clanid` = ?";

	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_clanid;
	input_bind[0].is_unsigned = false;	
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(5, sizeof(MYSQL_BIND));
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
	output_bind[4].buffer_type = MYSQL_TYPE_LONG;
	output_bind[4].buffer = &output_region;
	output_bind[4].is_unsigned = false;	

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
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
	
	return true;
}

bool Database::queryClanRanksByClanId(Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	int input_clanid = clan.GetClanId();
	
	int output_profileid;
	int output_rank;

	std::string query = "SELECT `profileid`, `rank` FROM `ClanRanks` WHERE `clanid` = ?";

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
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = &output_rank;
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

bool Database::insertClan(const Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query;
	
	query += "INSERT INTO `Clans` ";
	query += "	(`name`, `tag`, `homepage`, `info`, `region`, `created_at`) ";
	query += "VALUES ";
	query += "(?, ?, ?, ?, ?, DATE_FORMAT(NOW(), '%Y-%m-%d %H:%i:%s'));";
	
	std::string  input_name     = clan.GetName();
	std::string  input_tag      = clan.GetTag();
	std::string  input_homepage = clan.GetHomepage();
	std::string  input_info     = clan.GetInfo();
	int          input_region   = static_cast<int>(clan.GetRegion());
	
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
	input_bind[4].buffer_type = MYSQL_TYPE_LONG;
	input_bind[4].buffer = const_cast<int*>(&input_region);
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

bool Database::updateClan(const Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "UPDATE `Clans` SET `tag` = ?, `homepage` = ?, `info` = ?, `region` = ? WHERE `clanid` = ?;";
	
	std::string  input_tag      = clan.GetTag();
	std::string  input_homepage = clan.GetHomepage();
	std::string  input_info     = clan.GetInfo();
	int          input_region   = static_cast<int>(clan.GetRegion());
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
	input_bind[3].buffer_type = MYSQL_TYPE_LONG;
	input_bind[3].buffer = const_cast<int*>(&input_region);
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

	std::string query = "DELETE FROM `Clans` WHERE `clanid` = ?;";
	
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
bool Database::insertClanRank(const Battlefield::Clan& clan, const Battlefield::Player& player, Battlefield::Clan::Ranks rank)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "INSERT INTO `ClanRanks` (`clanid`, `profileid`, `rank`) VALUES (?, ?, ?);";
	
	int input_clanid    = clan.GetClanId();
	int input_profileid = player.GetProfileId();
	int input_rank      = static_cast<int>(rank);
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(3, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = const_cast<int*>(&input_clanid);
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = const_cast<int*>(&input_profileid);
	input_bind[1].is_unsigned = false;
	input_bind[2].buffer_type = MYSQL_TYPE_LONG;
	input_bind[2].buffer = const_cast<int*>(&input_rank);
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

bool Database::updateClanRank(const Battlefield::Clan& clan, const Battlefield::Player& player, Battlefield::Clan::Ranks rank)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)
	
	std::string query = "UPDATE `ClanRanks` SET `rank` = ? WHERE `clanid` = ? AND profileid = ?;";
	 
	int input_rank      = static_cast<int>(rank);
	int input_clanid    = clan.GetClanId();
	int input_profileid = player.GetProfileId();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(3, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = const_cast<int*>(&input_rank);
	input_bind[0].is_unsigned = false;
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

	std::string query = "DELETE FROM `ClanRanks` WHERE `clanid` = ? AND `profileid` = ?;";
	
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

	std::string query = "DELETE FROM `ClanRanks` WHERE `clanid` = ?;";
	
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
bool Database::queryGameServers(Battlefield::GameServers& game_servers)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "SELECT `ip`, `port`, `flag` FROM `GameServers`";
	
	char output_ip[16];
	int  output_port;
	int  output_flag;

	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(6, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[0].buffer = output_ip;
	output_bind[0].buffer_length = 16;
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = &output_port;
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_LONG;
	output_bind[2].buffer = &output_flag;
	output_bind[2].is_unsigned = false;

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

		game_server.SetIp(output_ip);
		game_server.SetPort(output_port);
		game_server.SetFlag(output_flag);
		
		game_servers.push_back(game_server);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(output_bind);
	
	return true;
}

bool Database::updateGameServer(const Battlefield::GameServer& game_server)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	query += "INSERT INTO";
	query += "	`GameServers` ";
	query += "		(`ip`, `port`, `flag`, `localip0`, `localport`, `natneg`, `gamename`, `hostname`, `hostport`, ";
	query += "		`gamever`, `cl`, `rv`, `map`, `mc`, `mapname`, `gc`, `gametype`, `gamevariant`, `numplayers`, ";
	query += "		`maxplayers`, `numteams`, `gamemode`, `teamplay`, `fraglimit`, `teamfraglimit`, `timelimit`, ";
	query += "		`timeelapsed`, `password`, `nr`, `xr`, `ff`, `sr`, `rc`, `ni`, `xi`, `qm`, `region`, ";
	query += "		`c0`, `c1`, `n0`, `n1`, `c0c`, `c1c`, `team0`, `team1`, `score0`, `score1`) ";
	query += "	VALUES ";
	query += "		(?, ?, ?, ?, ?, ?, ?, ?, ?, ";
	query += "		?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ";
	query += "		?, ?, ?, ?, ?, ?, ?, ";
	query += "		?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ";
	query += "		?, ?, ?, ?, ?, ?, ?, ?, ?, ?) ";
	query += "ON DUPLICATE KEY UPDATE ";
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
	query += "	`score1` = ? ";
	
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
	uint8_t     input_region        = game_server.GetRegion();
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
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(92, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_STRING;
	input_bind[0].buffer = const_cast<char*>(&(input_ip[0]));
	input_bind[0].buffer_length = input_ip.size();
	input_bind[1].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[1].buffer = &input_port;
	input_bind[1].is_unsigned = true;
	
	int items = 2;
	for(int i = 0; i < 2; i++)
	{
		input_bind[items    ].buffer_type = MYSQL_TYPE_TINY;
		input_bind[items    ].buffer = &input_flag;
		input_bind[items    ].is_unsigned = true;
		input_bind[items + 1].buffer_type = MYSQL_TYPE_STRING;
		input_bind[items + 1].buffer = const_cast<char*>(&(input_localip[0]));
		input_bind[items + 1].buffer_length = input_localip.size();
		input_bind[items + 2].buffer_type = MYSQL_TYPE_SHORT;
		input_bind[items + 2].buffer = &input_localport;
		input_bind[items + 2].is_unsigned = true;
		input_bind[items + 3].buffer_type = MYSQL_TYPE_TINY;
		input_bind[items + 3].buffer = &input_natneg;
		input_bind[items + 3].is_unsigned = true;
		input_bind[items + 4].buffer_type = MYSQL_TYPE_STRING;
		input_bind[items + 4].buffer = const_cast<char*>(&(input_gamename[0]));
		input_bind[items + 4].buffer_length = input_gamename.size();
		input_bind[items + 5].buffer_type = MYSQL_TYPE_STRING;
		input_bind[items + 5].buffer = const_cast<char*>(&(input_hostname[0]));
		input_bind[items + 5].buffer_length = input_hostname.size();
		input_bind[items + 6].buffer_type = MYSQL_TYPE_SHORT;
		input_bind[items + 6].buffer = &input_hostport;
		input_bind[items + 6].is_unsigned = true;
		input_bind[items + 7].buffer_type = MYSQL_TYPE_STRING;
		input_bind[items + 7].buffer = const_cast<char*>(&(input_gamever[0]));
		input_bind[items + 7].buffer_length = input_gamever.size();
		input_bind[items + 8].buffer_type = MYSQL_TYPE_STRING;
		input_bind[items + 8].buffer = const_cast<char*>(&(input_cl[0]));
		input_bind[items + 8].buffer_length = input_cl.size();
		input_bind[items + 9].buffer_type = MYSQL_TYPE_STRING;
		input_bind[items + 9].buffer = const_cast<char*>(&(input_rv[0]));
		input_bind[items + 9].buffer_length = input_rv.size();
		input_bind[items + 10].buffer_type = MYSQL_TYPE_STRING;
		input_bind[items + 10].buffer = const_cast<char*>(&(input_map[0]));
		input_bind[items + 10].buffer_length = input_map.size();
		input_bind[items + 11].buffer_type = MYSQL_TYPE_TINY;
		input_bind[items + 11].buffer = &input_mc;
		input_bind[items + 11].is_unsigned = true;
		input_bind[items + 12].buffer_type = MYSQL_TYPE_TINY;
		input_bind[items + 12].buffer = &input_mapname;
		input_bind[items + 12].is_unsigned = true;
		input_bind[items + 13].buffer_type = MYSQL_TYPE_TINY;
		input_bind[items + 13].buffer = &input_gc;
		input_bind[items + 13].is_unsigned = true;
		input_bind[items + 14].buffer_type = MYSQL_TYPE_STRING;
		input_bind[items + 14].buffer = const_cast<char*>(&(input_gametype[0]));
		input_bind[items + 14].buffer_length = input_gametype.size();
		input_bind[items + 15].buffer_type = MYSQL_TYPE_STRING;
		input_bind[items + 15].buffer = const_cast<char*>(&(input_gamevariant[0]));
		input_bind[items + 15].buffer_length = input_gamevariant.size();
		input_bind[items + 16].buffer_type = MYSQL_TYPE_TINY;
		input_bind[items + 16].buffer = &input_numplayers;
		input_bind[items + 16].is_unsigned = true;
		input_bind[items + 17].buffer_type = MYSQL_TYPE_TINY;
		input_bind[items + 17].buffer = &input_maxplayers;
		input_bind[items + 17].is_unsigned = true;
		input_bind[items + 18].buffer_type = MYSQL_TYPE_TINY;
		input_bind[items + 18].buffer = &input_numteams;
		input_bind[items + 18].is_unsigned = true;
		input_bind[items + 19].buffer_type = MYSQL_TYPE_STRING;
		input_bind[items + 19].buffer = const_cast<char*>(&(input_gamemode[0]));
		input_bind[items + 19].buffer_length = input_gamemode.size();
		input_bind[items + 20].buffer_type = MYSQL_TYPE_TINY;
		input_bind[items + 20].buffer = &input_teamplay;
		input_bind[items + 20].is_unsigned = true;
		input_bind[items + 21].buffer_type = MYSQL_TYPE_TINY;
		input_bind[items + 21].buffer = &input_fraglimit;
		input_bind[items + 21].is_unsigned = true;
		input_bind[items + 22].buffer_type = MYSQL_TYPE_TINY;
		input_bind[items + 22].buffer = &input_teamfraglimit;
		input_bind[items + 22].is_unsigned = true;
		input_bind[items + 23].buffer_type = MYSQL_TYPE_SHORT;
		input_bind[items + 23].buffer = &input_timelimit;
		input_bind[items + 23].is_unsigned = true;
		input_bind[items + 24].buffer_type = MYSQL_TYPE_SHORT;
		input_bind[items + 24].buffer = &input_timeelapsed;
		input_bind[items + 24].is_unsigned = true;
		input_bind[items + 25].buffer_type = MYSQL_TYPE_TINY;
		input_bind[items + 25].buffer = &input_password;
		input_bind[items + 25].is_unsigned = true;
		input_bind[items + 26].buffer_type = MYSQL_TYPE_TINY;
		input_bind[items + 26].buffer = &input_nr;
		input_bind[items + 26].is_unsigned = true;
		input_bind[items + 27].buffer_type = MYSQL_TYPE_TINY;
		input_bind[items + 27].buffer = &input_xr;
		input_bind[items + 27].is_unsigned = true;
		input_bind[items + 28].buffer_type = MYSQL_TYPE_TINY;
		input_bind[items + 28].buffer = &input_ff;
		input_bind[items + 28].is_unsigned = true;
		input_bind[items + 29].buffer_type = MYSQL_TYPE_TINY;
		input_bind[items + 29].buffer = &input_sr;
		input_bind[items + 29].is_unsigned = true;
		input_bind[items + 30].buffer_type = MYSQL_TYPE_TINY;
		input_bind[items + 30].buffer = &input_rc;
		input_bind[items + 30].is_unsigned = true;
		input_bind[items + 31].buffer_type = MYSQL_TYPE_LONGLONG;
		input_bind[items + 31].buffer = &input_ni;
		input_bind[items + 31].is_unsigned = false;
		input_bind[items + 32].buffer_type = MYSQL_TYPE_LONGLONG;
		input_bind[items + 32].buffer = &input_xi;
		input_bind[items + 32].is_unsigned = false;
		input_bind[items + 33].buffer_type = MYSQL_TYPE_TINY;
		input_bind[items + 33].buffer = &input_qm;
		input_bind[items + 33].is_unsigned = true;
		input_bind[items + 34].buffer_type = MYSQL_TYPE_TINY;
		input_bind[items + 34].buffer = &input_region;
		input_bind[items + 34].is_unsigned = true;
		input_bind[items + 35].buffer_type = MYSQL_TYPE_LONG;
		input_bind[items + 35].buffer = &input_c0;
		input_bind[items + 35].is_unsigned = false;
		input_bind[items + 36].buffer_type = MYSQL_TYPE_LONG;
		input_bind[items + 36].buffer = &input_c1;
		input_bind[items + 36].is_unsigned = false;
		input_bind[items + 37].buffer_type = MYSQL_TYPE_STRING;
		input_bind[items + 37].buffer = const_cast<char*>(&(input_n0[0]));
		input_bind[items + 37].buffer_length = input_n0.size();
		input_bind[items + 38].buffer_type = MYSQL_TYPE_STRING;
		input_bind[items + 38].buffer = const_cast<char*>(&(input_n1[0]));
		input_bind[items + 38].buffer_length = input_n1.size();
		input_bind[items + 39].buffer_type = MYSQL_TYPE_TINY;
		input_bind[items + 39].buffer = &input_c0c;
		input_bind[items + 39].is_unsigned = true;
		input_bind[items + 40].buffer_type = MYSQL_TYPE_TINY;
		input_bind[items + 40].buffer = &input_c1c;
		input_bind[items + 40].is_unsigned = true;
		input_bind[items + 41].buffer_type = MYSQL_TYPE_STRING;
		input_bind[items + 41].buffer = const_cast<char*>(&(input_team0[0]));
		input_bind[items + 41].buffer_length = input_team0.size();
		input_bind[items + 42].buffer_type = MYSQL_TYPE_STRING;
		input_bind[items + 42].buffer = const_cast<char*>(&(input_team1[0]));
		input_bind[items + 42].buffer_length = input_team1.size();
		input_bind[items + 43].buffer_type = MYSQL_TYPE_SHORT;
		input_bind[items + 43].buffer = &input_score0;
		input_bind[items + 43].is_unsigned = false;
		input_bind[items + 44].buffer_type = MYSQL_TYPE_SHORT;
		input_bind[items + 44].buffer = &input_score1;
		input_bind[items + 44].is_unsigned = false;
		items += 45;
	}
	
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

/*

*/
// Game Server Player
bool Database::_insertGameServerPlayer(const Battlefield::GameServer& game_server, const Battlefield::GameServerPlayer& gsplayer)
{
	//std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	
	query += "INSERT INTO ";
	query += "	`GameServerPlayers` ";
	query += "		(`gameserverid`, `name`, `score`, `skill`, `ping`, `team`, `deaths`, `profileid`) ";
	query += "SELECT ";
	query += "	GameServers.id as `gameserverid`, ";
	query += "	? AS `name`, ";
	query += "	? AS `score`, ";
	query += "	? AS `skill`, ";
	query += "	? AS `ping`, ";
	query += "	? AS `team`, ";
	query += "	? AS `deaths`, ";
	query += "	? AS `profileid` ";
	query += "FROM ";
	query += "	`GameServers` ";
	query += "WHERE ";
	query += "	`ip` = ? AND ";
	query += "	`port` = ?";
	
	std::string input_name      = gsplayer.GetName();
	int16_t     input_score     = gsplayer.GetScore();
	std::string input_skill     = gsplayer.GetSkill();
	uint8_t     input_ping      = gsplayer.GetPing();
	uint8_t     input_team      = gsplayer.GetTeam();
	uint16_t    input_deaths    = gsplayer.GetDeaths();
	int         input_profileid = gsplayer.GetProfileId();
	std::string input_ip        = game_server.GetIp();
	uint16_t    input_port      = game_server.GetPort();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(9, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_STRING;
	input_bind[0].buffer = const_cast<char*>(&(input_name[0]));
	input_bind[0].buffer_length = input_name.size();
	input_bind[1].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[1].buffer = &input_score;
	input_bind[1].is_unsigned = false;
	input_bind[2].buffer_type = MYSQL_TYPE_STRING;
	input_bind[2].buffer = const_cast<char*>(&(input_skill[0]));
	input_bind[2].buffer_length = input_skill.size();
	input_bind[3].buffer_type = MYSQL_TYPE_TINY;
	input_bind[3].buffer = &input_ping;
	input_bind[3].is_unsigned = true;
	input_bind[4].buffer_type = MYSQL_TYPE_TINY;
	input_bind[4].buffer = &input_team;
	input_bind[4].is_unsigned = true;
	input_bind[5].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[5].buffer = &input_deaths;
	input_bind[5].is_unsigned = true;
	input_bind[6].buffer_type = MYSQL_TYPE_LONG;
	input_bind[6].buffer = const_cast<int*>(&input_profileid);
	input_bind[6].is_unsigned = false;
	input_bind[7].buffer_type = MYSQL_TYPE_STRING;
	input_bind[7].buffer = const_cast<char*>(&(input_ip[0]));
	input_bind[7].buffer_length = input_ip.size();
	input_bind[8].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[8].buffer = &input_port;
	input_bind[8].is_unsigned = true;
	
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

bool Database::_removeGameServerPlayers(const Battlefield::GameServer& game_server)
{
	//std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query = "";
	
	query += "DELETE `GameServerPlayers` ";
	query += "FROM `GameServerPlayers` ";
	query += "INNER JOIN ";
	query += "	`GameServers` ";
	query += "ON ";
	query += "	GameServerPlayers.gameserverid = GameServers.id ";
	query += "WHERE ";
	query += "	GameServers.ip = ? AND ";
	query += "	GameServers.port = ?";
	
	std::string input_ip        = game_server.GetIp();
	uint16_t    input_port      = game_server.GetPort();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_STRING;
	input_bind[0].buffer = const_cast<char*>(&(input_ip[0]));
	input_bind[0].buffer_length = input_ip.size();
	input_bind[1].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[1].buffer = &input_port;
	input_bind[1].is_unsigned = true;
	
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

// Rank Players
bool Database::queryRankPlayersTopByRank(Battlefield::RankPlayers& rank_players)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query;
	
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
	query += "ORDER BY ";
	query += "	`rank` ASC, ";
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

bool Database::queryRankPlayersTopByType(Battlefield::RankPlayers& rank_players, const std::string& type)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query;
	
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

/*

*/
bool Database::queryRankPlayersTopByRatio(Battlefield::RankPlayers& rank_players, const std::string& k, const std::string& s)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query;
	
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
	query += "	Players.profileid = PlayerStats.profileid AND ";
	query += "	PlayerStats." + k + " != 0 AND ";
	query += "	PlayerStats." + s + " != 0 ";
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

/*
SELECT *
FROM (
    SELECT
        ROW_NUMBER() OVER (
			ORDER BY
				`ran` DESC,
                `score` DESC,
				`pph` DESC
		) AS `rank`,
        Players.profileid  AS `profileid`,
        Players.uniquenick AS `uniquenick`,
        PlayerStats.score  AS `score`,
        PlayerStats.ran    AS `ran`,
	    PlayerStats.pph    AS `pph`
    FROM Players, PlayerStats
    WHERE Players.profileid = PlayerStats.profileid
) AS `ranked_players`
WHERE `rank` >= (
    SELECT CASE WHEN `subquery`.`rank` <= 4
                THEN 1
                ELSE `subquery`.`rank` - 4
           END
    FROM (
        SELECT
            ROW_NUMBER() OVER (
				ORDER BY
					`ran` DESC,
					`score` DESC,
					`pph` DESC
			) AS `rank`,
            Players.profileid  AS `profileid`,
			Players.uniquenick AS `uniquenick`,
			PlayerStats.score  AS `score`,
			PlayerStats.ran    AS `ran`,
			PlayerStats.pph    AS `pph`
        FROM Players, PlayerStats
        WHERE Players.profileid = PlayerStats.profileid
    ) AS `subquery`
    WHERE `subquery`.`profileid` = 10036819
)
AND `rank` <= (
    SELECT `rank` + 9
    FROM (
        SELECT
            ROW_NUMBER() OVER (
				ORDER BY
					`ran` DESC,
					`score` DESC,
					`pph` DESC
			) AS `rank`,
            Players.profileid  AS `profileid`,
			Players.uniquenick AS `uniquenick`,
			PlayerStats.score  AS `score`,
			PlayerStats.ran    AS `ran`,
			PlayerStats.pph    AS `pph`
        FROM Players, PlayerStats
        WHERE Players.profileid = PlayerStats.profileid
    ) AS `subquery`
    WHERE `subquery`.`profileid` = 10036819
)
LIMIT 10;
*/
bool Database::queryRankPlayersSelfByRank(Battlefield::RankPlayers& rank_players, int profileid)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query;
	
	query += "SELECT * ";
	query += "FROM ( ";
	query += "	SELECT ";
	query += "		ROW_NUMBER() OVER ( ";
	query += "			ORDER BY ";
	query += "				`ran` DESC, ";
	query += "				`score` DESC, ";
	query += "				`pph` DESC ";
	query += "		) AS `rank`, ";
	query += "		Players.profileid  AS `profileid`, ";
	query += "		Players.uniquenick AS `uniquenick`, ";
	query += "		PlayerStats.score  AS `score`, ";
	query += "		PlayerStats.ran    AS `ran`, ";
	query += "		PlayerStats.pph    AS `pph` ";
	query += "	FROM ";
	query += "		`Players`, ";
	query += "		`PlayerStats` ";
	query += "	WHERE ";
	query += "		Players.profileid = PlayerStats.profileid ";
	query += ") AS `ranked_players` ";
	query += "WHERE ";
	query += "	`rank` >= ( ";
	query += "		SELECT CASE WHEN `subquery`.`rank` <= 4 ";
	query += "				THEN 1 ";
	query += "				ELSE `subquery`.`rank` - 4 ";
	query += "			END ";
	query += "		FROM ( ";
	query += "			SELECT ";
	query += "				ROW_NUMBER() OVER ( ";
	query += "					ORDER BY ";
	query += "						`ran` DESC, ";
	query += "						`score` DESC, ";
	query += "						`pph` DESC ";
	query += "				) AS `rank`, ";
	query += "				Players.profileid  AS `profileid`, ";
	query += "				Players.uniquenick AS `uniquenick`, ";
	query += "				PlayerStats.score  AS `score`, ";
	query += "				PlayerStats.ran    AS `ran`, ";
	query += "				PlayerStats.pph    AS `pph` ";
	query += "			FROM ";
	query += "				Players, ";
	query += "				PlayerStats ";
	query += "			WHERE ";
	query += "				Players.profileid = PlayerStats.profileid ";
	query += "		) AS `subquery` ";
	query += "		WHERE ";
	query += "			`subquery`.`profileid` = ? ";
	query += "	) ";
	query += "AND ";
	query += "	`rank` <= ( ";
	query += "		SELECT ";
	query += "			`rank` + 9 ";
	query += "		FROM ";
	query += "			(";
	query += "				SELECT ";
	query += "					ROW_NUMBER() OVER ( ";
	query += "						ORDER BY ";
	query += "							`ran` DESC, ";
	query += "							`score` DESC, ";
	query += "							`pph` DESC ";
	query += "					) AS `rank`, ";
	query += "					Players.profileid  AS `profileid`, ";
	query += "					Players.uniquenick AS `uniquenick`, ";
	query += "					PlayerStats.score  AS `score`, ";
	query += "					PlayerStats.ran    AS `ran`, ";
	query += "					PlayerStats.pph    AS `pph` ";
	query += "				FROM ";
	query += "					Players, ";
	query += "					PlayerStats ";
	query += "				WHERE ";
	query += "					Players.profileid = PlayerStats.profileid ";
	query += "			) AS `subquery` ";
	query += "		WHERE `subquery`.`profileid` = ? ";
	query += "	) ";
	query += "LIMIT 10;";
	
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

/*
SELECT *
FROM (
    SELECT
        ROW_NUMBER() OVER (
			ORDER BY `score` DESC
		) AS `rank`,
        Players.profileid                       AS `profileid`,
        Players.uniquenick                      AS `uniquenick`,
        PlayerStats.score                       AS `score`
    FROM Players, PlayerStats
    WHERE Players.profileid = PlayerStats.profileid
) AS `ranked_players`
WHERE `rank` >= (
    SELECT CASE WHEN `subquery`.`rank` <= 4
                THEN 1
                ELSE `subquery`.`rank` - 4
           END
    FROM (
        SELECT
            ROW_NUMBER() OVER (
				ORDER BY `score` DESC
			) AS `rank`,
            Players.profileid                       AS `profileid`,
            Players.uniquenick                      AS `uniquenick`,
            PlayerStats.score                       AS `score`
        FROM Players, PlayerStats
        WHERE Players.profileid = PlayerStats.profileid
    ) AS `subquery`
    WHERE `subquery`.`profileid` = 10036819
)
AND `rank` <= (
    SELECT `rank` + 9
    FROM (
        SELECT
            ROW_NUMBER() OVER (
				ORDER BY `score` DESC
			) AS `rank`,
            Players.profileid                       AS `profileid`,
            Players.uniquenick                      AS `uniquenick`,
            PlayerStats.score                       AS `score`
        FROM Players, PlayerStats
        WHERE Players.profileid = PlayerStats.profileid
    ) AS `subquery`
    WHERE `subquery`.`profileid` = 10036819
)
LIMIT 10;
*/
bool Database::queryRankPlayersSelfByType(Battlefield::RankPlayers& rank_players, const std::string& type, int profileid)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query;
	
	query += "SELECT * ";
	query += "FROM ( ";
	query += "	SELECT ";
	query += "		ROW_NUMBER() OVER (ORDER BY `" + type + "` DESC) AS `rank`, ";
	query += "		Players.profileid                                AS `profileid`, ";
	query += "		Players.uniquenick                               AS `uniquenick`, ";
	query += "		PlayerStats." + type + "                         AS `" + type + "` ";
	query += "	FROM ";
	query += "		`Players`, ";
	query += "		`PlayerStats` ";
	query += "	WHERE ";
	query += "		Players.profileid = PlayerStats.profileid ";
	query += ") AS `ranked_players` ";
	query += "WHERE ";
	query += "	`rank` >= ( ";
	query += "		SELECT CASE WHEN `subquery`.`rank` <= 4 ";
	query += "				THEN 1 ";
	query += "				ELSE `subquery`.`rank` - 4 ";
	query += "			END ";
	query += "		FROM ( ";
	query += "			SELECT ";
	query += "				ROW_NUMBER() OVER (ORDER BY `" + type + "` DESC) AS `rank`, ";
	query += "				Players.profileid                              AS `profileid`, ";
	query += "				Players.uniquenick                             AS `uniquenick`, ";
	query += "				PlayerStats." + type + "                       AS `" + type + "` ";
	query += "			FROM ";
	query += "				Players, ";
	query += "				PlayerStats ";
	query += "			WHERE ";
	query += "				Players.profileid = PlayerStats.profileid ";
	query += "		) AS `subquery` ";
	query += "		WHERE ";
	query += "			`subquery`.`profileid` = ? ";
	query += "	) ";
	query += "AND ";
	query += "	`rank` <= ( ";
	query += "		SELECT ";
	query += "			`rank` + 9 ";
	query += "		FROM ";
	query += "			(";
	query += "				SELECT ";
	query += "					ROW_NUMBER() OVER (ORDER BY `" + type + "` DESC) AS `rank`, ";
	query += "					Players.profileid                                AS `profileid`, ";
	query += "					Players.uniquenick                               AS `uniquenick`, ";
	query += "					PlayerStats." + type + "                         AS `" + type + "` ";
	query += "				FROM ";
	query += "					Players, ";
	query += "					PlayerStats ";
	query += "				WHERE ";
	query += "					Players.profileid = PlayerStats.profileid ";
	query += "			) AS `subquery` ";
	query += "		WHERE `subquery`.`profileid` = ? ";
	query += "	) ";
	query += "LIMIT 10;";
	
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

bool Database::queryRankPlayersSelfByRatio(Battlefield::RankPlayers& rank_players, int profileid, const std::string& k,
		const std::string& s)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query;
	
	query += "SELECT * ";
	query += "FROM ( ";
	query += "	SELECT ";
	query += "		ROW_NUMBER() OVER (";
	query += "			ORDER BY ";
	query += "				PlayerStats." + k + " / PlayerStats." + s + " DESC ";
	query += "		) AS `rank`, ";
	query += "		Players.profileid                             AS `profileid`, ";
	query += "		Players.uniquenick                            AS `uniquenick`, ";
	query += "		PlayerStats." + k + " / PlayerStats." + s + " AS `ratio`, ";
	query += "		PlayerStats." + k + "                         AS `" + k + "`, ";
	query += "		PlayerStats." + s + "                         AS `" + s + "` ";
	query += "	FROM ";
	query += "		`Players`, ";
	query += "		`PlayerStats` ";
	query += "	WHERE ";
	query += "		Players.profileid = PlayerStats.profileid AND ";
	query += "		PlayerStats." + k + " != 0 AND ";
	query += "		PlayerStats." + s + " != 0 ";
	query += ") AS `ranked_players` ";
	query += "WHERE ";
	query += "	`rank` >= ( ";
	query += "		SELECT CASE WHEN `subquery`.`rank` <= 4 ";
	query += "				THEN 1 ";
	query += "				ELSE `subquery`.`rank` - 4 ";
	query += "			END ";
	query += "		FROM ( ";
	query += "			SELECT ";
	query += "				ROW_NUMBER() OVER (";
	query += "					ORDER BY ";
	query += "						PlayerStats." + k + " / PlayerStats." + s + " DESC ";
	query += "				) AS `rank`, ";
	query += "				Players.profileid                             AS `profileid`, ";
	query += "				Players.uniquenick                            AS `uniquenick`, ";
	query += "				PlayerStats." + k + " / PlayerStats." + s + " AS `ratio`, ";
	query += "				PlayerStats." + k + "                         AS `" + k + "`, ";
	query += "				PlayerStats." + s + "                         AS `" + s + "` ";
	query += "			FROM ";
	query += "				Players, ";
	query += "				PlayerStats ";
	query += "			WHERE ";
	query += "				Players.profileid = PlayerStats.profileid AND ";
	query += "				PlayerStats." + k + " != 0 AND ";
	query += "				PlayerStats." + s + " != 0 ";
	query += "		) AS `subquery` ";
	query += "		WHERE ";
	query += "			`subquery`.`profileid` = ? ";
	query += "	) ";
	query += "AND ";
	query += "	`rank` <= ( ";
	query += "		SELECT ";
	query += "			`rank` + 9 ";
	query += "		FROM ";
	query += "			(";
	query += "				SELECT ";
	query += "					ROW_NUMBER() OVER (";
	query += "						ORDER BY ";
	query += "							PlayerStats." + k + " / PlayerStats." + s + " DESC ";
	query += "					) AS `rank`, ";
	query += "					Players.profileid                             AS `profileid`, ";
	query += "					Players.uniquenick                            AS `uniquenick`, ";
	query += "					PlayerStats." + k + " / PlayerStats." + s + " AS `ratio`, ";
	query += "					PlayerStats." + k + "                         AS `" + k + "`, ";
	query += "					PlayerStats." + s + "                         AS `" + s + "` ";
	query += "				FROM ";
	query += "					Players, ";
	query += "					PlayerStats ";
	query += "				WHERE ";
	query += "					Players.profileid = PlayerStats.profileid AND ";
	query += "					PlayerStats." + k + " != 0 AND ";
	query += "					PlayerStats." + s + " != 0 ";
	query += "			) AS `subquery` ";
	query += "		WHERE `subquery`.`profileid` = ? ";
	query += "	) ";
	query += "LIMIT 10;";
	
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


bool Database::queryRankPlayersTopFriendsByRank(Battlefield::RankPlayers& rank_players, const std::vector<int>& friends)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query;
	
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

bool Database::queryRankPlayersTopFriendsByType(Battlefield::RankPlayers& rank_players, const std::string& type,
		const std::vector<int>& friends)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query;
	
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

bool Database::queryRankPlayersTopFriendsByRatio(Battlefield::RankPlayers& rank_players, const std::vector<int>& friends,
		const std::string& k, const std::string& s)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)

	std::string query;
	
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

