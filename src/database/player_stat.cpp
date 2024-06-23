#include <iostream>
#include <mysql/mysql.h>

#include <settings.h>
#include <logger.h>
#include <util.h>

#include <database.h>

bool Database::queryPlayerStatsByProfileId(Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "SELECT ";
	query += "	`score`, `ran`, `pph`, `kills`, `deaths`, `suicides`, `time`, ";
	query += "	`vehicles`, `lavd`, `mavd`, `havd`, `hed`, `bod`, ";
	query += "	`k1`, `s1`, `k2`, `s2`, `k3`, `s3`, `k4`, `s4`, `k5`, `s5`, ";
	query += "	`medals`, `ttb`, `mv`, `ngp`, `cflags`, `nflags`, `sflags` ";
	query += "FROM ";
	query += "	`PlayerStats` ";
	query += "WHERE ";
	query += "	`profileid` = ?";
	
	int input_profileid = player.GetProfileId();
	
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
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(30, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_score;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_LONG;
	output_bind[1].buffer = &output_ran;
	output_bind[1].is_unsigned = true;
	output_bind[2].buffer_type = MYSQL_TYPE_LONG;
	output_bind[2].buffer = &output_pph;
	output_bind[2].is_unsigned = true;
	output_bind[3].buffer_type = MYSQL_TYPE_LONG;
	output_bind[3].buffer = &output_kills;
	output_bind[3].is_unsigned = true;
	output_bind[4].buffer_type = MYSQL_TYPE_LONG;
	output_bind[4].buffer = &output_deaths;
	output_bind[4].is_unsigned = true;
	output_bind[5].buffer_type = MYSQL_TYPE_LONG;
	output_bind[5].buffer = &output_suicides;
	output_bind[5].is_unsigned = true;
	output_bind[6].buffer_type = MYSQL_TYPE_LONG;
	output_bind[6].buffer = &output_time;
	output_bind[6].is_unsigned = true;
	output_bind[7].buffer_type = MYSQL_TYPE_LONG;
	output_bind[7].buffer = &output_vehicles;
	output_bind[7].is_unsigned = true;
	output_bind[8].buffer_type = MYSQL_TYPE_LONG;
	output_bind[8].buffer = &output_lavd;
	output_bind[8].is_unsigned = true;
	output_bind[9].buffer_type = MYSQL_TYPE_LONG;
	output_bind[9].buffer = &output_mavd;
	output_bind[9].is_unsigned = true;
	output_bind[10].buffer_type = MYSQL_TYPE_LONG;
	output_bind[10].buffer = &output_havd;
	output_bind[10].is_unsigned = true;
	output_bind[11].buffer_type = MYSQL_TYPE_LONG;
	output_bind[11].buffer = &output_hed;
	output_bind[11].is_unsigned = true;
	output_bind[12].buffer_type = MYSQL_TYPE_LONG;
	output_bind[12].buffer = &output_bod;
	output_bind[12].is_unsigned = true;
	output_bind[13].buffer_type = MYSQL_TYPE_LONG;
	output_bind[13].buffer = &output_k1;
	output_bind[13].is_unsigned = true;
	output_bind[14].buffer_type = MYSQL_TYPE_LONG;
	output_bind[14].buffer = &output_s1;
	output_bind[14].is_unsigned = true;
	output_bind[15].buffer_type = MYSQL_TYPE_LONG;
	output_bind[15].buffer = &output_k2;
	output_bind[15].is_unsigned = true;
	output_bind[16].buffer_type = MYSQL_TYPE_LONG;
	output_bind[16].buffer = &output_s2;
	output_bind[16].is_unsigned = true;
	output_bind[17].buffer_type = MYSQL_TYPE_LONG;
	output_bind[17].buffer = &output_k3;
	output_bind[17].is_unsigned = true;
	output_bind[18].buffer_type = MYSQL_TYPE_LONG;
	output_bind[18].buffer = &output_s3;
	output_bind[18].is_unsigned = true;
	output_bind[19].buffer_type = MYSQL_TYPE_LONG;
	output_bind[19].buffer = &output_k4;
	output_bind[19].is_unsigned = true;
	output_bind[20].buffer_type = MYSQL_TYPE_LONG;
	output_bind[20].buffer = &output_s4;
	output_bind[20].is_unsigned = true;
	output_bind[21].buffer_type = MYSQL_TYPE_LONG;
	output_bind[21].buffer = &output_k5;
	output_bind[21].is_unsigned = true;
	output_bind[22].buffer_type = MYSQL_TYPE_LONG;
	output_bind[22].buffer = &output_s5;
	output_bind[22].is_unsigned = true;
	output_bind[23].buffer_type = MYSQL_TYPE_LONG;
	output_bind[23].buffer = &output_medals;
	output_bind[23].is_unsigned = true;
	output_bind[24].buffer_type = MYSQL_TYPE_LONG;
	output_bind[24].buffer = &output_ttb;
	output_bind[24].is_unsigned = true;
	output_bind[25].buffer_type = MYSQL_TYPE_LONG;
	output_bind[25].buffer = &output_mv;
	output_bind[25].is_unsigned = true;
	output_bind[26].buffer_type = MYSQL_TYPE_LONG;
	output_bind[26].buffer = &output_ngp;
	output_bind[26].is_unsigned = true;
	output_bind[27].buffer_type = MYSQL_TYPE_LONG;
	output_bind[27].buffer = &output_cflags;
	output_bind[27].is_unsigned = true;
	output_bind[28].buffer_type = MYSQL_TYPE_LONG;
	output_bind[28].buffer = &output_nflags;
	output_bind[28].is_unsigned = true;
	output_bind[29].buffer_type = MYSQL_TYPE_LONG;
	output_bind[29].buffer = &output_sflags;
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

bool Database::updatePlayerStats(const Battlefield::Player& player)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

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
	query += "	`ngp` = ?, ";
	query += "	`cflags` = ?, ";
	query += "	`nflags` = ?, ";
	query += "	`sflags` = ? ";
	query += "WHERE ";
	query += "	`profileid` = ?";
	
	int      input_profileid = player.GetProfileId();
	int      input_score     = player.GetScore();
	uint32_t input_ran       = player.GetRank();
	uint32_t input_pph       = player.GetPPH();
	uint32_t input_kills     = player.GetKills();
	uint32_t input_deaths    = player.GetDeaths();
	uint32_t input_suicides  = player.GetSuicides();
	uint32_t input_time      = player.GetTime();
	uint32_t input_vehicles  = player.GetVehiclesDestroyed();
	uint32_t input_lavd      = player.GetLAVsDestroyed();
	uint32_t input_mavd      = player.GetMAVsDestroyed();
	uint32_t input_havd      = player.GetHAVsDestroyed();
	uint32_t input_hed       = player.GetHelicoptersDestroyed();
	uint32_t input_bod       = player.GetBoatsDestroyed();
	uint32_t input_k1        = player.GetKillsAssualtKit();
	uint32_t input_s1        = player.GetSpawnsAssualtKit();
	uint32_t input_k2        = player.GetKillsSniperKit();
	uint32_t input_s2        = player.GetSpawnsSniperKit();
	uint32_t input_k3        = player.GetKillsSpecialOpKit();
	uint32_t input_s3        = player.GetSpawnsSpecialOpKit();
	uint32_t input_k4        = player.GetKillsCombatEngineerKit();
	uint32_t input_s4        = player.GetSpawnsCombatEngineerKit();
	uint32_t input_k5        = player.GetKillsSupportKit();
	uint32_t input_s5        = player.GetSpawnsSupportKit();
	uint32_t input_medals    = player.GetMedals();
	uint32_t input_ttb       = player.GetTotalTopPlayer();
	uint32_t input_mv        = player.GetTotalVictories();
	uint32_t input_ngp       = player.GetTotalGameSessions();
	uint32_t input_cflags    = player.GetCapturedFlags();
	uint32_t input_nflags    = player.GetNeutralizedFlags();
	uint32_t input_sflags    = player.GetSavedFlags();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(31, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_score;
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_LONG;
	input_bind[1].buffer = &input_ran;
	input_bind[1].is_unsigned = true;
	input_bind[2].buffer_type = MYSQL_TYPE_LONG;
	input_bind[2].buffer = &input_pph;
	input_bind[2].is_unsigned = true;
	input_bind[3].buffer_type = MYSQL_TYPE_LONG;
	input_bind[3].buffer = &input_kills;
	input_bind[3].is_unsigned = true;
	input_bind[4].buffer_type = MYSQL_TYPE_LONG;
	input_bind[4].buffer = &input_deaths;
	input_bind[4].is_unsigned = true;
	input_bind[5].buffer_type = MYSQL_TYPE_LONG;
	input_bind[5].buffer = &input_suicides;
	input_bind[5].is_unsigned = true;
	input_bind[6].buffer_type = MYSQL_TYPE_LONG;
	input_bind[6].buffer = &input_time;
	input_bind[6].is_unsigned = true;
	input_bind[7].buffer_type = MYSQL_TYPE_LONG;
	input_bind[7].buffer = &input_vehicles;
	input_bind[7].is_unsigned = true;
	input_bind[8].buffer_type = MYSQL_TYPE_LONG;
	input_bind[8].buffer = &input_lavd;
	input_bind[8].is_unsigned = true;
	input_bind[9].buffer_type = MYSQL_TYPE_LONG;
	input_bind[9].buffer = &input_mavd;
	input_bind[9].is_unsigned = true;
	input_bind[10].buffer_type = MYSQL_TYPE_LONG;
	input_bind[10].buffer = &input_havd;
	input_bind[10].is_unsigned = true;
	input_bind[11].buffer_type = MYSQL_TYPE_LONG;
	input_bind[11].buffer = &input_hed;
	input_bind[11].is_unsigned = true;
	input_bind[12].buffer_type = MYSQL_TYPE_LONG;
	input_bind[12].buffer = &input_bod;
	input_bind[12].is_unsigned = true;
	input_bind[13].buffer_type = MYSQL_TYPE_LONG;
	input_bind[13].buffer = &input_k1;
	input_bind[13].is_unsigned = true;
	input_bind[14].buffer_type = MYSQL_TYPE_LONG;
	input_bind[14].buffer = &input_s1;
	input_bind[14].is_unsigned = true;
	input_bind[15].buffer_type = MYSQL_TYPE_LONG;
	input_bind[15].buffer = &input_k2;
	input_bind[15].is_unsigned = true;
	input_bind[16].buffer_type = MYSQL_TYPE_LONG;
	input_bind[16].buffer = &input_s2;
	input_bind[16].is_unsigned = true;
	input_bind[17].buffer_type = MYSQL_TYPE_LONG;
	input_bind[17].buffer = &input_k3;
	input_bind[17].is_unsigned = true;
	input_bind[18].buffer_type = MYSQL_TYPE_LONG;
	input_bind[18].buffer = &input_s3;
	input_bind[18].is_unsigned = true;
	input_bind[19].buffer_type = MYSQL_TYPE_LONG;
	input_bind[19].buffer = &input_k4;
	input_bind[19].is_unsigned = true;
	input_bind[20].buffer_type = MYSQL_TYPE_LONG;
	input_bind[20].buffer = &input_s4;
	input_bind[20].is_unsigned = true;
	input_bind[21].buffer_type = MYSQL_TYPE_LONG;
	input_bind[21].buffer = &input_k5;
	input_bind[21].is_unsigned = true;
	input_bind[22].buffer_type = MYSQL_TYPE_LONG;
	input_bind[22].buffer = &input_s5;
	input_bind[22].is_unsigned = true;
	input_bind[23].buffer_type = MYSQL_TYPE_LONG;
	input_bind[23].buffer = &input_medals;
	input_bind[23].is_unsigned = true;
	input_bind[24].buffer_type = MYSQL_TYPE_LONG;
	input_bind[24].buffer = &input_ttb;
	input_bind[24].is_unsigned = true;
	input_bind[25].buffer_type = MYSQL_TYPE_LONG;
	input_bind[25].buffer = &input_mv;
	input_bind[25].is_unsigned = true;
	input_bind[26].buffer_type = MYSQL_TYPE_LONG;
	input_bind[26].buffer = &input_ngp;
	input_bind[26].is_unsigned = true;
	input_bind[27].buffer_type = MYSQL_TYPE_LONG;
	input_bind[27].buffer = &input_cflags;
	input_bind[27].is_unsigned = true;
	input_bind[28].buffer_type = MYSQL_TYPE_LONG;
	input_bind[28].buffer = &input_nflags;
	input_bind[28].is_unsigned = true;
	input_bind[29].buffer_type = MYSQL_TYPE_LONG;
	input_bind[29].buffer = &input_sflags;
	input_bind[29].is_unsigned = true;
	input_bind[30].buffer_type = MYSQL_TYPE_LONG;
	input_bind[30].buffer = &input_profileid;
	input_bind[30].is_unsigned = true;
	
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
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "INSERT INTO `PlayerStats` ";
	query += "	(`profileid`) ";
	query += "VALUES ";
	query += "	(?)";
	
	int input_profileid = player.GetProfileId();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_profileid;
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
