#include <iostream>
#include <mysql/mysql.h>

#include <settings.h>
#include <globals.h>

#include <database.h>

Database::Database()
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
		std::cerr << "[Error] Failed to connect to the database: " << mysql_error(this->_connection) << std::endl;
		std::cerr << "[Error] Database::Database() at mysql_real_connect" << std::endl;
		
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	std::string query = "SELECT * FROM `Players` WHERE `email` = ?";
	
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

bool Database::insertPlayer(const Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock (read/write)
	
	std::string query = "INSERT INTO `Players` (`userid`, `nick`, `uniquenick`, `email`, `password`) VALUES (?, ?, ?, ?, ?);";
	
	int          input_profileid   = player.GetUserId();
	std::string  input_nick        = player.GetNick();
	std::string  input_uniquenick  = player.GetUniquenick();
	std::string  input_email       = player.GetEmail();
	std::string  input_md5password = player.GetPassword();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(5, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = const_cast<int*>(&input_profileid);
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	std::string query = "DELETE FROM `PlayerFriends` WHERE (`profileid` = ? and `target_profileid` = ?) OR (`profileid` = ? and `target_profileid` = ?);";
	
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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
	query += "`suicides`, ";
	query += "`time`, ";
	query += "`lavd`, ";
	query += "`mavd`, ";
	query += "`havd`, ";
	query += "`hed`, ";
	query += "`bod`, ";
	query += "`k1`, ";
	query += "`k2`, ";
	query += "`k3`, ";
	query += "`k4`, ";
	query += "`k5`, ";
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
	int output_suicides;
	int output_time;
	int output_lavd;
	int output_mavd;
	int output_havd;
	int output_hed;
	int output_bod;
	int output_k1;
	int output_k2;
	int output_k3;
	int output_k4;
	int output_k5;
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
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(20, sizeof(MYSQL_BIND));
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
	output_bind[4].buffer = const_cast<int*>(&output_suicides);
	output_bind[4].is_unsigned = false;
	output_bind[5].buffer_type = MYSQL_TYPE_LONG;
	output_bind[5].buffer = const_cast<int*>(&output_time);
	output_bind[5].is_unsigned = false;
	output_bind[6].buffer_type = MYSQL_TYPE_LONG;
	output_bind[6].buffer = const_cast<int*>(&output_lavd);
	output_bind[6].is_unsigned = false;
	output_bind[7].buffer_type = MYSQL_TYPE_LONG;
	output_bind[7].buffer = const_cast<int*>(&output_mavd);
	output_bind[7].is_unsigned = false;
	output_bind[8].buffer_type = MYSQL_TYPE_LONG;
	output_bind[8].buffer = const_cast<int*>(&output_havd);
	output_bind[8].is_unsigned = false;
	output_bind[9].buffer_type = MYSQL_TYPE_LONG;
	output_bind[9].buffer = const_cast<int*>(&output_hed);
	output_bind[9].is_unsigned = false;
	output_bind[10].buffer_type = MYSQL_TYPE_LONG;
	output_bind[10].buffer = const_cast<int*>(&output_bod);
	output_bind[10].is_unsigned = false;
	output_bind[11].buffer_type = MYSQL_TYPE_LONG;
	output_bind[11].buffer = const_cast<int*>(&output_k1);
	output_bind[11].is_unsigned = false;
	output_bind[12].buffer_type = MYSQL_TYPE_LONG;
	output_bind[12].buffer = const_cast<int*>(&output_k2);
	output_bind[12].is_unsigned = false;
	output_bind[13].buffer_type = MYSQL_TYPE_LONG;
	output_bind[13].buffer = const_cast<int*>(&output_k3);
	output_bind[13].is_unsigned = false;
	output_bind[14].buffer_type = MYSQL_TYPE_LONG;
	output_bind[14].buffer = const_cast<int*>(&output_k4);
	output_bind[14].is_unsigned = false;
	output_bind[15].buffer_type = MYSQL_TYPE_LONG;
	output_bind[15].buffer = const_cast<int*>(&output_k5);
	output_bind[15].is_unsigned = false;
	output_bind[16].buffer_type = MYSQL_TYPE_LONG;
	output_bind[16].buffer = const_cast<int*>(&output_medals);
	output_bind[16].is_unsigned = false;
	output_bind[17].buffer_type = MYSQL_TYPE_LONG;
	output_bind[17].buffer = const_cast<int*>(&output_ttb);
	output_bind[17].is_unsigned = false;
	output_bind[18].buffer_type = MYSQL_TYPE_LONG;
	output_bind[18].buffer = const_cast<int*>(&output_mv);
	output_bind[18].is_unsigned = false;
	output_bind[19].buffer_type = MYSQL_TYPE_LONG;
	output_bind[19].buffer = const_cast<int*>(&output_ngp);
	output_bind[19].is_unsigned = false;
	

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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
		player.SetSuicides(output_suicides);
		player.SetTime(output_time);
		player.SetLAVsDestroyed(output_lavd);
		player.SetMAVsDestroyed(output_mavd);
		player.SetHAVsDestroyed(output_havd);
		player.SetHelicoptersDestroyed(output_hed);
		player.SetBoatsDestroyed(output_bod);
		player.SetKillsAssualtKit(output_k1);
		player.SetKillsSniperKit(output_k2);
		player.SetKillsSpecialOpKit(output_k3);
		player.SetKillsCombatEngineerKit(output_k4);
		player.SetKillsSupportKit(output_k5);
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
	query += "`suicides` = ?, ";
	query += "`time` = ?, ";
	query += "`lavd` = ?, ";
	query += "`mavd` = ?, ";
	query += "`havd` = ?, ";
	query += "`hed` = ?, ";
	query += "`bod` = ?, ";
	query += "`k1` = ?, ";
	query += "`k2` = ?, ";
	query += "`k3` = ?, ";
	query += "`k4` = ?, ";
	query += "`k5` = ?, ";
	query += "`medals` = ?, ";
	query += "`ttb` = ?, ";
	query += "`mv` = ?, ";
	query += "`ngp` = ? ";
	query += "WHERE `profileid` = ?;";
	
	int input_score     = player.GetScore();
	int input_ran       = player.GetRank();
	int input_pph       = player.GetPPH();
	int input_kills     = player.GetKills();
	int input_suicides  = player.GetSuicides();
	int input_time      = player.GetTime();
	int input_lavd      = player.GetLAVsDestroyed();
	int input_mavd      = player.GetMAVsDestroyed();
	int input_havd      = player.GetHAVsDestroyed();
	int input_hed       = player.GetHelicoptersDestroyed();
	int input_bod       = player.GetBoatsDestroyed();
	int input_k1        = player.GetKillsAssualtKit();
	int input_k2        = player.GetKillsSniperKit();
	int input_k3        = player.GetKillsSpecialOpKit();
	int input_k4        = player.GetKillsCombatEngineerKit();
	int input_k5        = player.GetKillsSupportKit();
	int input_medals    = player.GetMedals();
	int input_ttb       = player.GetTotalTopPlayer();
	int input_mv        = player.GetTotalVictories();
	int input_ngp       = player.GetTotalGameSessions();
	int input_profileid = player.GetProfileId();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(21, sizeof(MYSQL_BIND));
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
	input_bind[4].buffer = const_cast<int*>(&input_suicides);
	input_bind[4].is_unsigned = false;
	input_bind[5].buffer_type = MYSQL_TYPE_LONG;
	input_bind[5].buffer = const_cast<int*>(&input_time);
	input_bind[5].is_unsigned = false;
	input_bind[6].buffer_type = MYSQL_TYPE_LONG;
	input_bind[6].buffer = const_cast<int*>(&input_lavd);
	input_bind[6].is_unsigned = false;
	input_bind[7].buffer_type = MYSQL_TYPE_LONG;
	input_bind[7].buffer = const_cast<int*>(&input_mavd);
	input_bind[7].is_unsigned = false;
	input_bind[8].buffer_type = MYSQL_TYPE_LONG;
	input_bind[8].buffer = const_cast<int*>(&input_havd);
	input_bind[8].is_unsigned = false;
	input_bind[9].buffer_type = MYSQL_TYPE_LONG;
	input_bind[9].buffer = const_cast<int*>(&input_hed);
	input_bind[9].is_unsigned = false;
	input_bind[10].buffer_type = MYSQL_TYPE_LONG;
	input_bind[10].buffer = const_cast<int*>(&input_bod);
	input_bind[10].is_unsigned = false;
	input_bind[11].buffer_type = MYSQL_TYPE_LONG;
	input_bind[11].buffer = const_cast<int*>(&input_k1);
	input_bind[11].is_unsigned = false;
	input_bind[12].buffer_type = MYSQL_TYPE_LONG;
	input_bind[12].buffer = const_cast<int*>(&input_k2);
	input_bind[12].is_unsigned = false;
	input_bind[13].buffer_type = MYSQL_TYPE_LONG;
	input_bind[13].buffer = const_cast<int*>(&input_k3);
	input_bind[13].is_unsigned = false;
	input_bind[14].buffer_type = MYSQL_TYPE_LONG;
	input_bind[14].buffer = const_cast<int*>(&input_k4);
	input_bind[14].is_unsigned = false;
	input_bind[15].buffer_type = MYSQL_TYPE_LONG;
	input_bind[15].buffer = const_cast<int*>(&input_k5);
	input_bind[15].is_unsigned = false;
	input_bind[16].buffer_type = MYSQL_TYPE_LONG;
	input_bind[16].buffer = const_cast<int*>(&input_medals);
	input_bind[16].is_unsigned = false;
	input_bind[17].buffer_type = MYSQL_TYPE_LONG;
	input_bind[17].buffer = const_cast<int*>(&input_ttb);
	input_bind[17].is_unsigned = false;
	input_bind[18].buffer_type = MYSQL_TYPE_LONG;
	input_bind[18].buffer = const_cast<int*>(&input_mv);
	input_bind[18].is_unsigned = false;
	input_bind[19].buffer_type = MYSQL_TYPE_LONG;
	input_bind[19].buffer = const_cast<int*>(&input_ngp);
	input_bind[19].is_unsigned = false;
	input_bind[20].buffer_type = MYSQL_TYPE_LONG;
	input_bind[20].buffer = const_cast<int*>(&input_profileid);
	input_bind[20].is_unsigned = false;
	

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	std::string query = "INSERT INTO `Clans` (`name`, `tag`, `homepage`, `info`, `region`) VALUES (?, ?, ?, ?, ?);";
	
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	std::string query = "SELECT `ip`, `port`, `flag` FROM `GameServer`";
	
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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
		
		if(type == "score")      { player.SetScore(output_value);                  }
		else if(type == "pph")   { player.SetPPH(output_value);                    }
		else if(type == "kills") { player.SetKills(output_value);                  }
		else if(type == "k1")    { player.SetKillsAssualtKit(output_value);        }
		else if(type == "k2")    { player.SetKillsSniperKit(output_value);         }
		else if(type == "k3")    { player.SetKillsSpecialOpKit(output_value);      }
		else if(type == "k4")    { player.SetKillsCombatEngineerKit(output_value); }
		else if(type == "k5")    { player.SetKillsSupportKit(output_value);        }
		else if(type == "lavd")  { player.SetLAVsDestroyed(output_value);          }
		else if(type == "mavd")  { player.SetMAVsDestroyed(output_value);          }
		else if(type == "havd")  { player.SetHAVsDestroyed(output_value);          }
		else if(type == "hed")   { player.SetHelicoptersDestroyed(output_value);   }
		else if(type == "bod")   { player.SetBoatsDestroyed(output_value);         }
		
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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

	// Prepare the statement
	MYSQL_STMT* statement = mysql_stmt_init(this->_connection);

	// Prepare and execute with binds
	if(
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
		
		if(type == "score")      { player.SetScore(output_value);                  }
		else if(type == "pph")   { player.SetPPH(output_value);                    }
		else if(type == "kills") { player.SetKills(output_value);                  }
		else if(type == "k1")    { player.SetKillsAssualtKit(output_value);        }
		else if(type == "k2")    { player.SetKillsSniperKit(output_value);         }
		else if(type == "k3")    { player.SetKillsSpecialOpKit(output_value);      }
		else if(type == "k4")    { player.SetKillsCombatEngineerKit(output_value); }
		else if(type == "k5")    { player.SetKillsSupportKit(output_value);        }
		else if(type == "lavd")  { player.SetLAVsDestroyed(output_value);          }
		else if(type == "mavd")  { player.SetMAVsDestroyed(output_value);          }
		else if(type == "havd")  { player.SetHAVsDestroyed(output_value);          }
		else if(type == "hed")   { player.SetHelicoptersDestroyed(output_value);   }
		else if(type == "bod")   { player.SetBoatsDestroyed(output_value);         }
		
		rank_players.insert(std::make_pair(output_rank, player));
	}
	
	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
		
	return true;
}

// Private functions
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

// Events
void Database::OnDatabaseStart()
{
	std::lock_guard<std::mutex> guard(g_mutex_io); // io lock (read/write)

	std::cout << "Database started" << std::endl;
}

