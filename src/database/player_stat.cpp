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
	int input_s1        = player.GetSpawnsAssualtKit();
	int input_k2        = player.GetKillsSniperKit();
	int input_s2        = player.GetSpawnsSniperKit();
	int input_k3        = player.GetKillsSpecialOpKit();
	int input_s3        = player.GetSpawnsSpecialOpKit();
	int input_k4        = player.GetKillsCombatEngineerKit();
	int input_s4        = player.GetSpawnsCombatEngineerKit();
	int input_k5        = player.GetKillsSupportKit();
	int input_s5        = player.GetSpawnsSupportKit();
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
