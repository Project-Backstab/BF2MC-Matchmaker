#include <iostream>
#include <mysql/mysql.h>

#include <settings.h>
#include <logger.h>
#include <util.h>

#include <database.h>

bool Database::queryPlayerByProfileId(Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "SELECT ";
	query += "	`userid`, `nick`, `uniquenick`, `email`, `password`, `last_login`, `last_login_ip`, `created_at`, ";
	query += "	`verified`, `restricted` ";
	query += "FROM ";
	query += "	`Players` ";
	query += "WHERE ";
	query += "	`profileid` = ?";

	int        input_profileid = player.GetProfileId();
	
	int        output_userid = 0;
	char       output_nick[VARCHAR_LEN(41)];
	char       output_uniquenick[VARCHAR_LEN(32)];
	char       output_email[VARCHAR_LEN(50)];
	char       output_md5password[VARCHAR_LEN(32)];
	MYSQL_TIME output_last_login;
	char       output_last_login_ip[VARCHAR_LEN(15)];
	MYSQL_TIME output_created_at;
	uint8_t    output_verified;
	uint8_t    output_restricted;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_profileid;
	input_bind[0].is_unsigned = false;

	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(10, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_userid;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[1].buffer = output_nick;
	output_bind[1].buffer_length = VARCHAR_LEN(41);
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = output_uniquenick;
	output_bind[2].buffer_length = VARCHAR_LEN(32);
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = output_email;
	output_bind[3].buffer_length = VARCHAR_LEN(50);
	output_bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[4].buffer = output_md5password;
	output_bind[4].buffer_length = VARCHAR_LEN(32);
	output_bind[5].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[5].buffer = &output_last_login;
	output_bind[6].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[6].buffer = output_last_login_ip;
	output_bind[6].buffer_length = VARCHAR_LEN(15);
	output_bind[7].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[7].buffer = &output_created_at;
	output_bind[8].buffer_type = MYSQL_TYPE_TINY;
	output_bind[8].buffer = &output_verified;
	output_bind[8].is_unsigned = true;
	output_bind[9].buffer_type = MYSQL_TYPE_TINY;
	output_bind[9].buffer = &output_restricted;
	output_bind[9].is_unsigned = true;
	
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
		player.SetVerified(output_verified);
		player.SetRestricted(output_restricted);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);

	return true;
}

bool Database::queryPlayerAndPlayerStatsByProfileId(Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "SELECT ";
	query += "	`userid`, `nick`, `uniquenick`, `email`, `password`, `last_login`, `last_login_ip`, `created_at`, ";
	query += "	`verified`, `restricted`, ";
	query += "	`score`, `ran`, `pph`, `kills`, `deaths`, `suicides`, `time`, ";
	query += "	`vehicles`, `lavd`, `mavd`, `havd`, `hed`, `bod`, ";
	query += "	`k1`, `s1`, `k2`, `s2`, `k3`, `s3`, `k4`, `s4`, `k5`, `s5`, ";
	query += "	`medals`, `ttb`, `mv`, `ngp`, `cflags`, `nflags`, `sflags` ";
	query += "FROM ";
	query += "	`Players`, `PlayerStats` ";
	query += "WHERE ";
	query += "	`Players`.`profileid` = ? ";
	query += "AND ";
	query += "	`Players`.`profileid` = `PlayerStats`.`profileid`";

	int        input_profileid = player.GetProfileId();
	
	int        output_userid;
	char       output_nick[VARCHAR_LEN(41)];
	char       output_uniquenick[VARCHAR_LEN(32)];
	char       output_email[VARCHAR_LEN(50)];
	char       output_md5password[VARCHAR_LEN(32)];
	MYSQL_TIME output_last_login;
	char       output_last_login_ip[VARCHAR_LEN(15)];
	MYSQL_TIME output_created_at;
	uint8_t    output_verified;
	uint8_t    output_restricted;
	int        output_score;
	uint32_t   output_ran;
	uint32_t   output_pph;
	uint32_t   output_kills;
	uint32_t   output_deaths;
	uint32_t   output_suicides;
	uint32_t   output_time;
	uint32_t   output_vehicles;
	uint32_t   output_lavd;
	uint32_t   output_mavd;
	uint32_t   output_havd;
	uint32_t   output_hed;
	uint32_t   output_bod;
	uint32_t   output_k1;
	uint32_t   output_s1;
	uint32_t   output_k2;
	uint32_t   output_s2;
	uint32_t   output_k3;
	uint32_t   output_s3;
	uint32_t   output_k4;
	uint32_t   output_s4;
	uint32_t   output_k5;
	uint32_t   output_s5;
	uint32_t   output_medals;
	uint32_t   output_ttb;
	uint32_t   output_mv;
	uint32_t   output_ngp;
	uint32_t   output_cflags;
	uint32_t   output_nflags;
	uint32_t   output_sflags;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_profileid;
	input_bind[0].is_unsigned = false;

	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(40, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_userid;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[1].buffer = output_nick;
	output_bind[1].buffer_length = VARCHAR_LEN(41);
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = output_uniquenick;
	output_bind[2].buffer_length = VARCHAR_LEN(32);
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = output_email;
	output_bind[3].buffer_length = VARCHAR_LEN(50);
	output_bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[4].buffer = output_md5password;
	output_bind[4].buffer_length = VARCHAR_LEN(32);
	output_bind[5].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[5].buffer = &output_last_login;
	output_bind[6].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[6].buffer = output_last_login_ip;
	output_bind[6].buffer_length = VARCHAR_LEN(15);
	output_bind[7].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[7].buffer = &output_created_at;
	output_bind[8].buffer_type = MYSQL_TYPE_TINY;
	output_bind[8].buffer = &output_verified;
	output_bind[8].is_unsigned = true;
	output_bind[9].buffer_type = MYSQL_TYPE_TINY;
	output_bind[9].buffer = &output_restricted;
	output_bind[9].is_unsigned = true;
	output_bind[10].buffer_type = MYSQL_TYPE_LONG;
	output_bind[10].buffer = &output_score;
	output_bind[10].is_unsigned = false;
	output_bind[11].buffer_type = MYSQL_TYPE_LONG;
	output_bind[11].buffer = &output_ran;
	output_bind[11].is_unsigned = true;
	output_bind[12].buffer_type = MYSQL_TYPE_LONG;
	output_bind[12].buffer = &output_pph;
	output_bind[12].is_unsigned = true;
	output_bind[13].buffer_type = MYSQL_TYPE_LONG;
	output_bind[13].buffer = &output_kills;
	output_bind[13].is_unsigned = true;
	output_bind[14].buffer_type = MYSQL_TYPE_LONG;
	output_bind[14].buffer = &output_deaths;
	output_bind[14].is_unsigned = true;
	output_bind[15].buffer_type = MYSQL_TYPE_LONG;
	output_bind[15].buffer = &output_suicides;
	output_bind[15].is_unsigned = true;
	output_bind[16].buffer_type = MYSQL_TYPE_LONG;
	output_bind[16].buffer = &output_time;
	output_bind[16].is_unsigned = true;
	output_bind[17].buffer_type = MYSQL_TYPE_LONG;
	output_bind[17].buffer = &output_vehicles;
	output_bind[17].is_unsigned = true;
	output_bind[18].buffer_type = MYSQL_TYPE_LONG;
	output_bind[18].buffer = &output_lavd;
	output_bind[18].is_unsigned = true;
	output_bind[19].buffer_type = MYSQL_TYPE_LONG;
	output_bind[19].buffer = &output_mavd;
	output_bind[19].is_unsigned = true;
	output_bind[20].buffer_type = MYSQL_TYPE_LONG;
	output_bind[20].buffer = &output_havd;
	output_bind[20].is_unsigned = true;
	output_bind[21].buffer_type = MYSQL_TYPE_LONG;
	output_bind[21].buffer = &output_hed;
	output_bind[21].is_unsigned = true;
	output_bind[22].buffer_type = MYSQL_TYPE_LONG;
	output_bind[22].buffer = &output_bod;
	output_bind[22].is_unsigned = true;
	output_bind[23].buffer_type = MYSQL_TYPE_LONG;
	output_bind[23].buffer = &output_k1;
	output_bind[23].is_unsigned = true;
	output_bind[24].buffer_type = MYSQL_TYPE_LONG;
	output_bind[24].buffer = &output_s1;
	output_bind[24].is_unsigned = true;
	output_bind[25].buffer_type = MYSQL_TYPE_LONG;
	output_bind[25].buffer = &output_k2;
	output_bind[25].is_unsigned = true;
	output_bind[26].buffer_type = MYSQL_TYPE_LONG;
	output_bind[26].buffer = &output_s2;
	output_bind[26].is_unsigned = true;
	output_bind[27].buffer_type = MYSQL_TYPE_LONG;
	output_bind[27].buffer = &output_k3;
	output_bind[27].is_unsigned = true;
	output_bind[28].buffer_type = MYSQL_TYPE_LONG;
	output_bind[28].buffer = &output_s3;
	output_bind[28].is_unsigned = true;
	output_bind[29].buffer_type = MYSQL_TYPE_LONG;
	output_bind[29].buffer = &output_k4;
	output_bind[29].is_unsigned = true;
	output_bind[30].buffer_type = MYSQL_TYPE_LONG;
	output_bind[30].buffer = &output_s4;
	output_bind[30].is_unsigned = true;
	output_bind[31].buffer_type = MYSQL_TYPE_LONG;
	output_bind[31].buffer = &output_k5;
	output_bind[31].is_unsigned = true;
	output_bind[32].buffer_type = MYSQL_TYPE_LONG;
	output_bind[32].buffer = &output_s5;
	output_bind[32].is_unsigned = true;
	output_bind[33].buffer_type = MYSQL_TYPE_LONG;
	output_bind[33].buffer = &output_medals;
	output_bind[33].is_unsigned = true;
	output_bind[34].buffer_type = MYSQL_TYPE_LONG;
	output_bind[34].buffer = &output_ttb;
	output_bind[34].is_unsigned = true;
	output_bind[35].buffer_type = MYSQL_TYPE_LONG;
	output_bind[35].buffer = &output_mv;
	output_bind[35].is_unsigned = true;
	output_bind[36].buffer_type = MYSQL_TYPE_LONG;
	output_bind[36].buffer = &output_ngp;
	output_bind[36].is_unsigned = true;
	output_bind[37].buffer_type = MYSQL_TYPE_LONG;
	output_bind[37].buffer = &output_cflags;
	output_bind[37].is_unsigned = true;
	output_bind[38].buffer_type = MYSQL_TYPE_LONG;
	output_bind[38].buffer = &output_nflags;
	output_bind[38].is_unsigned = true;
	output_bind[39].buffer_type = MYSQL_TYPE_LONG;
	output_bind[39].buffer = &output_sflags;
	output_bind[39].is_unsigned = true;
	
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
		player.SetVerified(output_verified);
		player.SetRestricted(output_restricted);
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
		player.SetCapturedFlags(output_cflags);
		player.SetNeutralizedFlags(output_nflags);
		player.SetSavedFlags(output_sflags);
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
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock
	
	std::string query = "";
	query += "SELECT ";
	query += "	`profileid`, `userid`, `nick`, `email`, `password`, `last_login`, `last_login_ip`, `created_at`, `verified`, `restricted` ";
	query += "FROM ";
	query += "	`Players` ";
	query += "WHERE ";
	query += "	`uniquenick` = ?";
	
	std::string input_uniquenick = player.GetUniquenick();
	
	int        output_profileid = 0;
	int        output_userid = 0;
	char       output_nick[VARCHAR_LEN(41)];
	char       output_email[VARCHAR_LEN(50)];
	char       output_md5password[VARCHAR_LEN(32)];
	MYSQL_TIME output_last_login;
	char       output_last_login_ip[VARCHAR_LEN(15)];
	MYSQL_TIME output_created_at;
	uint8_t    output_verified;
	uint8_t    output_restricted;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_STRING;
	input_bind[0].buffer = const_cast<char*>(&(input_uniquenick[0]));
	input_bind[0].buffer_length = input_uniquenick.size();

	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(10, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_profileid;
	output_bind[0].is_unsigned = false;	
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = &output_userid;
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = output_nick;
	output_bind[2].buffer_length = VARCHAR_LEN(41);
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = output_email;
	output_bind[3].buffer_length = VARCHAR_LEN(50);
	output_bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[4].buffer = output_md5password;
	output_bind[4].buffer_length = VARCHAR_LEN(32);
	output_bind[5].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[5].buffer = &output_last_login;
	output_bind[6].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[6].buffer = output_last_login_ip;
	output_bind[6].buffer_length = VARCHAR_LEN(15);
	output_bind[7].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[7].buffer = &output_created_at;
	output_bind[8].buffer_type = MYSQL_TYPE_TINY;
	output_bind[8].buffer = &output_verified;
	output_bind[8].is_unsigned = true;
	output_bind[9].buffer_type = MYSQL_TYPE_TINY;
	output_bind[9].buffer = &output_restricted;
	output_bind[9].is_unsigned = true;
	
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
		player.SetVerified(output_verified);
		player.SetRestricted(output_restricted);
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
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "SELECT ";
	query += "	`profileid`, `userid`, `nick`, `uniquenick`, `email`, `password`, `last_login`, `last_login_ip`, `created_at`, `verified`, `restricted` ";
	query += "FROM ";
	query += "	`Players` ";
	query += "WHERE ";
	query += "	`email` = ?";
	
	int        output_profileid = 0;
	int        output_userid = 0;
	char       output_nick[VARCHAR_LEN(41)];
	char       output_uniquenick[VARCHAR_LEN(32)];
	char       output_email[VARCHAR_LEN(50)];
	char       output_md5password[VARCHAR_LEN(32)];
	MYSQL_TIME output_last_login;
	char       output_last_login_ip[VARCHAR_LEN(15)];
	MYSQL_TIME output_created_at;
	uint8_t    output_verified;
	uint8_t    output_restricted;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_STRING;
	input_bind[0].buffer = const_cast<char*>(&(email[0]));
	input_bind[0].buffer_length = email.size();

	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(11, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_profileid;
	output_bind[0].is_unsigned = false;	
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = &output_userid;
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = output_nick;
	output_bind[2].buffer_length = VARCHAR_LEN(41);
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = output_uniquenick;
	output_bind[3].buffer_length = VARCHAR_LEN(32);
	output_bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[4].buffer = output_email;
	output_bind[4].buffer_length = VARCHAR_LEN(50);
	output_bind[5].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[5].buffer = output_md5password;
	output_bind[5].buffer_length = VARCHAR_LEN(32);
	output_bind[6].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[6].buffer = &output_last_login;
	output_bind[7].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[7].buffer = output_last_login_ip;
	output_bind[7].buffer_length = VARCHAR_LEN(15);
	output_bind[8].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[8].buffer = &output_created_at;
	output_bind[9].buffer_type = MYSQL_TYPE_TINY;
	output_bind[9].buffer = &output_verified;
	output_bind[9].is_unsigned = true;
	output_bind[10].buffer_type = MYSQL_TYPE_TINY;
	output_bind[10].buffer = &output_restricted;
	output_bind[10].is_unsigned = true;
	
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
		player.SetVerified(output_verified);
		player.SetRestricted(output_restricted);
		
		players.push_back(player);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
	
	return true;
}

bool Database::queryPlayersByEmailOrUniquenick(Battlefield::Players& players, const std::string& email, const std::string& uniquenick)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "SELECT ";
	query += "	`profileid`, `userid`, `nick`, `uniquenick`, `email`, `password`, `last_login`, `last_login_ip`, `created_at`, `verified`, `restricted` ";
	query += "FROM ";
	query += "	`Players` ";
	query += "WHERE ";
	query += "	`email` = ? ";
	query += "OR ";
	query += "	`uniquenick` = ?";
	
	int        output_profileid = 0;
	int        output_userid = 0;
	char       output_nick[VARCHAR_LEN(41)];
	char       output_uniquenick[VARCHAR_LEN(32)];
	char       output_email[VARCHAR_LEN(50)];
	char       output_md5password[VARCHAR_LEN(32)];
	MYSQL_TIME output_last_login;
	char       output_last_login_ip[VARCHAR_LEN(15)];
	MYSQL_TIME output_created_at;
	uint8_t    output_verified;
	uint8_t    output_restricted;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(2, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_STRING;
	input_bind[0].buffer = const_cast<char*>(&(email[0]));
	input_bind[0].buffer_length = email.size();
	input_bind[1].buffer_type = MYSQL_TYPE_STRING;
	input_bind[1].buffer = const_cast<char*>(&(uniquenick[0]));
	input_bind[1].buffer_length = uniquenick.size();
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(11, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_profileid;
	output_bind[0].is_unsigned = false;	
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = &output_userid;
	output_bind[1].is_unsigned = false;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = output_nick;
	output_bind[2].buffer_length = VARCHAR_LEN(41);
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = output_uniquenick;
	output_bind[3].buffer_length = VARCHAR_LEN(32);
	output_bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[4].buffer = output_email;
	output_bind[4].buffer_length = VARCHAR_LEN(50);
	output_bind[5].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[5].buffer = output_md5password;
	output_bind[5].buffer_length = VARCHAR_LEN(32);
	output_bind[6].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[6].buffer = &output_last_login;
	output_bind[7].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[7].buffer = output_last_login_ip;
	output_bind[7].buffer_length = VARCHAR_LEN(15);
	output_bind[8].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[8].buffer = &output_created_at;
	output_bind[9].buffer_type = MYSQL_TYPE_TINY;
	output_bind[9].buffer = &output_verified;
	output_bind[9].is_unsigned = true;
	output_bind[10].buffer_type = MYSQL_TYPE_TINY;
	output_bind[10].buffer = &output_restricted;
	output_bind[10].is_unsigned = true;

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
		player.SetVerified(output_verified);
		player.SetRestricted(output_restricted);
		
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
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

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
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock
	
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
	input_bind[1].buffer = &input_profileid;
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
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock
	
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
	input_bind[0].buffer = &input_userid;
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
