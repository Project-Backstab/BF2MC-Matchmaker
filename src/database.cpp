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

// Player
bool Database::queryPlayerByProfileid(Battlefield::Player& player, const std::string profileid)
{
	std::lock_guard<std::mutex> guard(this->_mutex);

	unsigned long output_profileid = 0;
	unsigned long output_userid = 0;
	char          output_nick[46];
	char          output_uniquenick[46];
	char          output_email[46];
	char          output_md5password[46];

	std::string query = "SELECT * FROM `Players` WHERE `profileid` = ?";

	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	input_bind[0].buffer = const_cast<char*>(&(profileid[0]));
	input_bind[0].buffer_length = profileid.size();

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
	output_bind[5].buffer = output_md5password;
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

	int status = mysql_stmt_fetch(statement);

	if (status != 1 && status != MYSQL_NO_DATA)
	{		
		player.SetProfileId(output_profileid);
		player.SetUserId(output_userid);
		player.SetNick(output_nick);
		player.SetUniquenick(output_uniquenick);
		player.SetEmail(output_email);
		player.SetMD5Password(output_md5password);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);

	return true;
}

bool Database::queryPlayerByUniquenick(Battlefield::Player& player, const std::string &uniquenick)
{
	std::lock_guard<std::mutex> guard(this->_mutex);

	unsigned long output_profileid = 0;
	unsigned long output_userid = 0;
	char          output_nick[46];
	char          output_uniquenick[46];
	char          output_email[46];
	char          output_md5password[46];

	std::string query = "SELECT * FROM `Players` WHERE `uniquenick` = ?";

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
	output_bind[5].buffer = output_md5password;
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

	int status = mysql_stmt_fetch(statement);

	if (status != 1 && status != MYSQL_NO_DATA)
	{		
		player.SetProfileId(output_profileid);
		player.SetUserId(output_userid);
		player.SetNick(output_nick);
		player.SetUniquenick(output_uniquenick);
		player.SetEmail(output_email);
		player.SetMD5Password(output_md5password);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);

	return true;
}

bool Database::queryPlayersByEmail(Battlefield::Players& players, const std::string &email)
{
	std::lock_guard<std::mutex> guard(this->_mutex);

	unsigned long output_profileid = 0;
	unsigned long output_userid = 0;
	char          output_nick[46];
	char          output_uniquenick[46];
	char          output_email[46];
	char          output_md5password[46];

	std::string query = "SELECT * FROM `Players` WHERE `email` = ?";

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
	output_bind[5].buffer = output_md5password;
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

	return true;
}

bool Database::queryPlayerNewUserID(Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex);

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

	return true;
}

bool Database::insertPlayer(const Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex);
	
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
	input_bind[1].length = nullptr;
	input_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	input_bind[2].buffer = const_cast<char*>(&(input_uniquenick[0]));
	input_bind[2].buffer_length = input_uniquenick.size();
	input_bind[2].length = nullptr;
	input_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	input_bind[3].buffer = const_cast<char*>(&(input_email[0]));
	input_bind[3].buffer_length = input_email.size();
	input_bind[3].length = nullptr;
	input_bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	input_bind[4].buffer = const_cast<char*>(&(input_md5password[0]));
	input_bind[4].buffer_length = input_md5password.size();
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

// Player Friends
bool Database::queryPlayerFriends(Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex);

	int input_profileid = player.GetProfileId();
	unsigned long output_profileid = 0;
	unsigned long output_target_profileid = 0;

	std::string query = "SELECT `profileid`, `target_profileid` FROM `PlayerFriends` WHERE `profileid` = ? OR `target_profileid` = ?";

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

	return true;
}

bool Database::insertPlayerFriend(int profileid, int target_profileid)
{
	std::lock_guard<std::mutex> guard(this->_mutex);

	std::string query = "INSERT INTO `PlayerFriends` (`profileid`, `target_profileid`) VALUES (?, ?);";

	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = const_cast<int*>(&(profileid));
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = const_cast<int*>(&(target_profileid));
	input_bind[1].is_unsigned = false;

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

bool Database::removePlayerFriend(int profileid, int target_profileid)
{
	std::lock_guard<std::mutex> guard(this->_mutex);

	std::string query = "DELETE FROM `PlayerFriends` WHERE (`profileid` = ? and `target_profileid` = ?) OR (`profileid` = ? and `target_profileid` = ?);";

	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(4, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = const_cast<int*>(&(profileid));
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = const_cast<int*>(&(target_profileid));
	input_bind[1].is_unsigned = false;
	input_bind[2].buffer_type = MYSQL_TYPE_LONG;
	input_bind[2].buffer = const_cast<int*>(&(target_profileid));
	input_bind[2].is_unsigned = false;
	input_bind[3].buffer_type = MYSQL_TYPE_LONG;
	input_bind[3].buffer = const_cast<int*>(&(profileid));
	input_bind[3].is_unsigned = false;

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

// Player Stats
bool Database::queryPlayerStats(Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex);

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
	
	int output_score     = player.GetScore();
	int output_ran       = player.GetRank();
	int output_pph       = player.GetPPH();
	int output_kills     = player.GetKills();
	int output_suicides  = player.GetSuicides();
	int output_time      = player.GetTime();
	int output_lavd      = player.GetLAVsDestroyed();
	int output_mavd      = player.GetMAVsDestroyed();
	int output_havd      = player.GetHAVsDestroyed();
	int output_hed       = player.GetHelicoptersDestroyed();
	int output_bod       = player.GetBoatsDestroyed();
	int output_k1        = player.GetKillsAssualtKit();
	int output_k2        = player.GetKillsSniperKit();
	int output_k3        = player.GetKillsSpecialOpKit();
	int output_k4        = player.GetKillsCombatEngineerKit();
	int output_k5        = player.GetKillsSupportKit();
	int output_medals    = player.GetMedals();
	int output_ttb       = player.GetTotalTopPlayer();
	int output_mv        = player.GetTotalVictories();
	int output_ngp       = player.GetTotalGameSessions();
	
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

	return true;
}

bool Database::updatePlayerStats(const Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex);

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
		return false;
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);

	return true;
}

bool Database::insertPlayerStats(const Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex);

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
		return false;
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);

	return true;
}

// Clan
bool Database::queryClanByNameOrTag(Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex);
	
	std::string input_name = clan.GetName();
	std::string input_tag = clan.GetTag();
	
	unsigned long output_clanid = 0;
	char          output_name[46];
	char          output_tag[46];
	char          output_homepage[46];
	char          output_info[46];
	unsigned long output_region = 0;

	std::string query = "SELECT `clanid`, `name`, `tag`, `homepage`, `info`, `region` FROM `Clans` WHERE `name` = ? OR `tag` = ?";

	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	input_bind[0].buffer = const_cast<char*>(&(input_name[0]));
	input_bind[0].buffer_length = input_name.size();
	input_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	input_bind[1].buffer = const_cast<char*>(&(input_tag[0]));
	input_bind[1].buffer_length = input_tag.size();
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(6, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_clanid;
	output_bind[0].is_unsigned = false;	
	output_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[1].buffer = &output_name;
	output_bind[1].buffer_length = 46;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_tag;
	output_bind[2].buffer_length = 46;
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = &output_homepage;
	output_bind[3].buffer_length = 46;
	output_bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[4].buffer = &output_info;
	output_bind[4].buffer_length = 46;
	output_bind[5].buffer_type = MYSQL_TYPE_LONG;
	output_bind[5].buffer = &output_region;
	output_bind[5].is_unsigned = false;	

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

	int status = mysql_stmt_fetch(statement);

	if (status != 1 && status != MYSQL_NO_DATA)
	{		
		clan.SetClanId(output_clanid);
		clan.SetName(output_name);
		clan.SetTag(output_tag);
		clan.SetHomepage(output_homepage);
		clan.SetInfo(output_info);
		clan.SetRegion(
			static_cast<Battlefield::Clan::Regions>(output_region)
		);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);

	return true;
}

bool Database::queryClanByPlayer(Battlefield::Clan& clan, const Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex);
	
	int input_profileid = player.GetProfileId();
	
	int output_clanid = 0;

	std::string query = "SELECT `clanid` FROM `ClanRanks` WHERE `profileid` = ?";

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

	return true;
}

bool Database::queryClanByClanId(Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex);

	int input_clanid = clan.GetClanId();
	
	char output_name[46];
	char output_tag[46];
	char output_homepage[46];
	char output_info[46];
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
	output_bind[0].buffer_length = 46;
	output_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[1].buffer = &output_tag;
	output_bind[1].buffer_length = 46;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = &output_homepage;
	output_bind[2].buffer_length = 46;
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = &output_info;
	output_bind[3].buffer_length = 46;
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
		return false;
	}

	int status = mysql_stmt_fetch(statement);

	if (status != 1 && status != MYSQL_NO_DATA)
	{
		clan.SetName(output_name);
		clan.SetTag(output_tag);
		clan.SetHomepage(output_homepage);
		clan.SetInfo(output_info);
		clan.SetRegion(
			static_cast<Battlefield::Clan::Regions>(output_region)
		);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);

	return true;
}

bool Database::queryClanRanksByClanId(Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex);

	int input_clanid = clan.GetClanId();
	
	int output_profileid = 0;
	int output_rank = 0;

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
		return false;
	}

	while(true)
	{
		int status = mysql_stmt_fetch(statement);

		if (status == 1 || status == MYSQL_NO_DATA)
		{
			break;
		}
		
		clan.AddRank(output_profileid, static_cast<Battlefield::Clan::Ranks>(output_rank));
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);

	return true;
}

bool Database::insertClan(const Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex);

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
		return false;
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);

	return true;
}

bool Database::updateClan(const Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex);

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
		return false;
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);

	return true;
}

bool Database::removeClan(const Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex);

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
		return false;
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);

	return true;
}

// Clan Rank
bool Database::insertClanRank(const Battlefield::Clan& clan, const Battlefield::Player& player, int rank)
{
	std::lock_guard<std::mutex> guard(this->_mutex);

	std::string query = "INSERT INTO `ClanRanks` (`clanid`, `profileid`, `rank`) VALUES (?, ?, ?);";
	
	int input_clanid     = clan.GetClanId();
	int input_profileid  = player.GetProfileId();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(3, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = const_cast<int*>(&input_clanid);
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = const_cast<int*>(&input_profileid);
	input_bind[1].is_unsigned = false;
	input_bind[2].buffer_type = MYSQL_TYPE_LONG;
	input_bind[2].buffer = const_cast<int*>(&rank);
	input_bind[2].is_unsigned = false;

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

bool Database::updateClanRank(const Battlefield::Clan& clan, const Battlefield::Player& player, int rank)
{
	std::lock_guard<std::mutex> guard(this->_mutex);
	
	std::string query = "UPDATE `ClanRanks` SET `rank` = ? WHERE `clanid` = ? AND profileid = ?;";
	 
	int input_clanid     = clan.GetClanId();
	int input_profileid  = player.GetProfileId();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(3, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = const_cast<int*>(&rank);
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
		return false;
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);

	return true;
}


bool Database::removeClanRank(const Battlefield::Clan& clan, const Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex);

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
		return false;
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);

	return true;
}

bool Database::removeClanRanksByClanId(const Battlefield::Clan& clan)
{
	std::lock_guard<std::mutex> guard(this->_mutex);

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
		return false;
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);

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
	std::lock_guard<std::mutex> guard(g_mutex_io);

	std::cout << "Database started" << std::endl;
}

