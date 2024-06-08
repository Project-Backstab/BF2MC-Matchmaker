#include <iostream>
#include <mysql/mysql.h>

#include <settings.h>
#include <logger.h>
#include <util.h>

#include <database.h>

bool Database::queryGameStatPlayers(Battlefield::GameStat& game_stat)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "SELECT ";
	query += "	`id`, `auth`, `pid`, `machine_id`, `team`, `score`, `rank`, `pph`, `kills`, ";
	query += "	`deaths`, `suicides`, `time`, `lavd`, `mavd`, `havd`, `hed`, `pld`, `bod`, ";
	query += "	`k1`, `s1`, `k2`, `s2`, `k3`, `s3`, `k4`, `s4`, `k5`, `s5`, ";
	query += "	`tk`, `medals`, `ttb`, `mv`, `ngp` ";
	query += "FROM ";
	query += "	`GameStatPlayers` ";
	query += "WHERE ";
	query += "	`gamestatid` = ?";
	
	int      input_gamestatid = game_stat.GetId();
	
	int      output_id;
	char     output_auth[VARCHAR_LEN(32)];
	int      output_pid;
	char     output_machine_id[VARCHAR_LEN(16)];
	int      output_team;
	int32_t  output_score;
	uint32_t output_rank;
	uint32_t output_pph;
	uint32_t output_kills;
	uint32_t output_deaths;
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
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(33, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_id;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_STRING;
	output_bind[1].buffer = output_auth;
	output_bind[1].buffer_length = VARCHAR_LEN(32);
	output_bind[2].buffer_type = MYSQL_TYPE_LONG;
	output_bind[2].buffer = &output_pid;
	output_bind[2].is_unsigned = false;
	output_bind[3].buffer_type = MYSQL_TYPE_STRING;
	output_bind[3].buffer = output_machine_id;
	output_bind[3].buffer_length = VARCHAR_LEN(16);
	output_bind[4].buffer_type = MYSQL_TYPE_LONG;
	output_bind[4].buffer = &output_team;
	output_bind[4].is_unsigned = false;
	output_bind[5].buffer_type = MYSQL_TYPE_LONG;
	output_bind[5].buffer = &output_score;
	output_bind[5].is_unsigned = false;
	output_bind[6].buffer_type = MYSQL_TYPE_LONG;
	output_bind[6].buffer = &output_rank;
	output_bind[6].is_unsigned = true;
	output_bind[7].buffer_type = MYSQL_TYPE_LONG;
	output_bind[7].buffer = &output_pph;
	output_bind[7].is_unsigned = true;
	output_bind[8].buffer_type = MYSQL_TYPE_LONG;
	output_bind[8].buffer = &output_kills;
	output_bind[8].is_unsigned = true;
	output_bind[9].buffer_type = MYSQL_TYPE_LONG;
	output_bind[9].buffer = &output_deaths;
	output_bind[9].is_unsigned = true;
	output_bind[10].buffer_type = MYSQL_TYPE_LONG;
	output_bind[10].buffer = &output_suicides;
	output_bind[10].is_unsigned = true;
	output_bind[11].buffer_type = MYSQL_TYPE_LONG;
	output_bind[11].buffer = &output_time;
	output_bind[11].is_unsigned = true;
	output_bind[12].buffer_type = MYSQL_TYPE_LONG;
	output_bind[12].buffer = &output_lavd;
	output_bind[12].is_unsigned = true;
	output_bind[13].buffer_type = MYSQL_TYPE_LONG;
	output_bind[13].buffer = &output_mavd;
	output_bind[13].is_unsigned = true;
	output_bind[14].buffer_type = MYSQL_TYPE_LONG;
	output_bind[14].buffer = &output_havd;
	output_bind[14].is_unsigned = true;
	output_bind[15].buffer_type = MYSQL_TYPE_LONG;
	output_bind[15].buffer = &output_hed;
	output_bind[15].is_unsigned = true;
	output_bind[16].buffer_type = MYSQL_TYPE_LONG;
	output_bind[16].buffer = &output_pld;
	output_bind[16].is_unsigned = true;
	output_bind[17].buffer_type = MYSQL_TYPE_LONG;
	output_bind[17].buffer = &output_bod;
	output_bind[17].is_unsigned = true;
	output_bind[18].buffer_type = MYSQL_TYPE_LONG;
	output_bind[18].buffer = &output_k1;
	output_bind[18].is_unsigned = true;
	output_bind[19].buffer_type = MYSQL_TYPE_LONG;
	output_bind[19].buffer = &output_s1;
	output_bind[19].is_unsigned = true;
	output_bind[20].buffer_type = MYSQL_TYPE_LONG;
	output_bind[20].buffer = &output_k2;
	output_bind[20].is_unsigned = true;
	output_bind[21].buffer_type = MYSQL_TYPE_LONG;
	output_bind[21].buffer = &output_s2;
	output_bind[21].is_unsigned = true;
	output_bind[22].buffer_type = MYSQL_TYPE_LONG;
	output_bind[22].buffer = &output_k3;
	output_bind[22].is_unsigned = true;
	output_bind[23].buffer_type = MYSQL_TYPE_LONG;
	output_bind[23].buffer = &output_s3;
	output_bind[23].is_unsigned = true;
	output_bind[24].buffer_type = MYSQL_TYPE_LONG;
	output_bind[24].buffer = &output_k4;
	output_bind[24].is_unsigned = true;
	output_bind[25].buffer_type = MYSQL_TYPE_LONG;
	output_bind[25].buffer = &output_s4;
	output_bind[25].is_unsigned = true;
	output_bind[26].buffer_type = MYSQL_TYPE_LONG;
	output_bind[26].buffer = &output_k5;
	output_bind[26].is_unsigned = true;
	output_bind[27].buffer_type = MYSQL_TYPE_LONG;
	output_bind[27].buffer = &output_s5;
	output_bind[27].is_unsigned = true;
	output_bind[28].buffer_type = MYSQL_TYPE_LONG;
	output_bind[28].buffer = &output_tk;
	output_bind[28].is_unsigned = true;
	output_bind[29].buffer_type = MYSQL_TYPE_LONG;
	output_bind[29].buffer = &output_medals;
	output_bind[29].is_unsigned = true;
	output_bind[30].buffer_type = MYSQL_TYPE_LONG;
	output_bind[30].buffer = &output_ttb;
	output_bind[30].is_unsigned = true;
	output_bind[31].buffer_type = MYSQL_TYPE_LONG;
	output_bind[31].buffer = &output_mv;
	output_bind[31].is_unsigned = true;
	output_bind[32].buffer_type = MYSQL_TYPE_LONG;
	output_bind[32].buffer = &output_ngp;
	output_bind[32].is_unsigned = true;
	
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
		gsplayer.SetMachineId(output_machine_id);
		gsplayer.SetTeam(output_team);
		gsplayer.SetScore(output_score);
		gsplayer.SetRank(output_rank);
		gsplayer.SetPPH(output_pph);
		gsplayer.SetKills(output_kills);
		gsplayer.SetDeaths(output_deaths);
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

bool Database::queryGameStatPlayersByProfileId(const Battlefield::Player& player, Battlefield::GameStatPlayers& gsplayers)
{
	std::string query = "";
	query += "SELECT ";
	query += "	`id`, `auth`, `machine_id`, `team`, `score`, `rank`, `pph`, `kills`, ";
	query += "	`deaths`, `suicides`, `time`, `lavd`, `mavd`, `havd`, `hed`, `pld`, `bod`, ";
	query += "	`k1`, `s1`, `k2`, `s2`, `k3`, `s3`, `k4`, `s4`, `k5`, `s5`, ";
	query += "	`tk`, `medals`, `ttb`, `mv`, `ngp`, `disable` ";
	query += "FROM ";
	query += "	`GameStatPlayers` ";
	query += "WHERE ";
	query += "	`pid` = ?";

	int input_profileid = player.GetProfileId();

	int      output_id;
	char     output_auth[VARCHAR_LEN(32)];
	char     output_machine_id[VARCHAR_LEN(16)];
	int      output_team;
	int32_t  output_score;
	uint32_t output_rank;
	uint32_t output_pph;
	uint32_t output_kills;
	uint32_t output_deaths;
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
	uint8_t  output_disable;

	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_profileid;
	input_bind[0].is_unsigned = false;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(33, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_id;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_STRING;
	output_bind[1].buffer = output_auth;
	output_bind[1].buffer_length = VARCHAR_LEN(32);
	output_bind[2].buffer_type = MYSQL_TYPE_STRING;
	output_bind[2].buffer = output_machine_id;
	output_bind[2].buffer_length = VARCHAR_LEN(16);
	output_bind[3].buffer_type = MYSQL_TYPE_LONG;
	output_bind[3].buffer = &output_team;
	output_bind[3].is_unsigned = false;
	output_bind[4].buffer_type = MYSQL_TYPE_LONG;
	output_bind[4].buffer = &output_score;
	output_bind[4].is_unsigned = false;
	output_bind[5].buffer_type = MYSQL_TYPE_LONG;
	output_bind[5].buffer = &output_rank;
	output_bind[5].is_unsigned = true;
	output_bind[6].buffer_type = MYSQL_TYPE_LONG;
	output_bind[6].buffer = &output_pph;
	output_bind[6].is_unsigned = true;
	output_bind[7].buffer_type = MYSQL_TYPE_LONG;
	output_bind[7].buffer = &output_kills;
	output_bind[7].is_unsigned = true;
	output_bind[8].buffer_type = MYSQL_TYPE_LONG;
	output_bind[8].buffer = &output_deaths;
	output_bind[8].is_unsigned = true;
	output_bind[9].buffer_type = MYSQL_TYPE_LONG;
	output_bind[9].buffer = &output_suicides;
	output_bind[9].is_unsigned = true;
	output_bind[10].buffer_type = MYSQL_TYPE_LONG;
	output_bind[10].buffer = &output_time;
	output_bind[10].is_unsigned = true;
	output_bind[11].buffer_type = MYSQL_TYPE_LONG;
	output_bind[11].buffer = &output_lavd;
	output_bind[11].is_unsigned = true;
	output_bind[12].buffer_type = MYSQL_TYPE_LONG;
	output_bind[12].buffer = &output_mavd;
	output_bind[12].is_unsigned = true;
	output_bind[13].buffer_type = MYSQL_TYPE_LONG;
	output_bind[13].buffer = &output_havd;
	output_bind[13].is_unsigned = true;
	output_bind[14].buffer_type = MYSQL_TYPE_LONG;
	output_bind[14].buffer = &output_hed;
	output_bind[14].is_unsigned = true;
	output_bind[15].buffer_type = MYSQL_TYPE_LONG;
	output_bind[15].buffer = &output_pld;
	output_bind[15].is_unsigned = true;
	output_bind[16].buffer_type = MYSQL_TYPE_LONG;
	output_bind[16].buffer = &output_bod;
	output_bind[16].is_unsigned = true;
	output_bind[17].buffer_type = MYSQL_TYPE_LONG;
	output_bind[17].buffer = &output_k1;
	output_bind[17].is_unsigned = true;
	output_bind[18].buffer_type = MYSQL_TYPE_LONG;
	output_bind[18].buffer = &output_s1;
	output_bind[18].is_unsigned = true;
	output_bind[19].buffer_type = MYSQL_TYPE_LONG;
	output_bind[19].buffer = &output_k2;
	output_bind[19].is_unsigned = true;
	output_bind[20].buffer_type = MYSQL_TYPE_LONG;
	output_bind[20].buffer = &output_s2;
	output_bind[20].is_unsigned = true;
	output_bind[21].buffer_type = MYSQL_TYPE_LONG;
	output_bind[21].buffer = &output_k3;
	output_bind[21].is_unsigned = true;
	output_bind[22].buffer_type = MYSQL_TYPE_LONG;
	output_bind[22].buffer = &output_s3;
	output_bind[22].is_unsigned = true;
	output_bind[23].buffer_type = MYSQL_TYPE_LONG;
	output_bind[23].buffer = &output_k4;
	output_bind[23].is_unsigned = true;
	output_bind[24].buffer_type = MYSQL_TYPE_LONG;
	output_bind[24].buffer = &output_s4;
	output_bind[24].is_unsigned = true;
	output_bind[25].buffer_type = MYSQL_TYPE_LONG;
	output_bind[25].buffer = &output_k5;
	output_bind[25].is_unsigned = true;
	output_bind[26].buffer_type = MYSQL_TYPE_LONG;
	output_bind[26].buffer = &output_s5;
	output_bind[26].is_unsigned = true;
	output_bind[27].buffer_type = MYSQL_TYPE_LONG;
	output_bind[27].buffer = &output_tk;
	output_bind[27].is_unsigned = true;
	output_bind[28].buffer_type = MYSQL_TYPE_LONG;
	output_bind[28].buffer = &output_medals;
	output_bind[28].is_unsigned = true;
	output_bind[29].buffer_type = MYSQL_TYPE_LONG;
	output_bind[29].buffer = &output_ttb;
	output_bind[29].is_unsigned = true;
	output_bind[30].buffer_type = MYSQL_TYPE_LONG;
	output_bind[30].buffer = &output_mv;
	output_bind[30].is_unsigned = true;
	output_bind[31].buffer_type = MYSQL_TYPE_LONG;
	output_bind[31].buffer = &output_ngp;
	output_bind[31].is_unsigned = true;
	output_bind[32].buffer_type = MYSQL_TYPE_TINY;
	output_bind[32].buffer = &output_disable;
	output_bind[32].is_unsigned = true;

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
		gsplayer.SetProfileId(input_profileid);
		gsplayer.SetMachineId(output_machine_id);
		gsplayer.SetTeam(output_team);
		gsplayer.SetScore(output_score);
		gsplayer.SetRank(output_rank);
		gsplayer.SetPPH(output_pph);
		gsplayer.SetKills(output_kills);
		gsplayer.SetDeaths(output_deaths);
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
		gsplayer.SetDisable(output_disable);

		gsplayers.push_back(gsplayer);
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
	query += "	(`gamestatid`, `auth`, `pid`, `machine_id`, `team`, `score`, `rank`, `pph`, ";
	query += "	`kills`, `deaths`, `suicides`, `time`, `lavd`, `mavd`, `havd`, `hed`, `pld`, `bod`, ";
	query += "	`k1`, `s1`, `k2`, `s2`, `k3`, `s3`, `k4`, `s4`, `k5`, `s5`, ";
	query += "	`tk`, `medals`, `ttb`, `mv`, `ngp`) ";
	query += "VALUES ";
	query += "	(?, ?, ?, ?, ?, ?, ?, ?, ";
	query += "	?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ";
	query += "	?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ";
	query += "	?, ?, ?, ?, ?);";
	
	int         input_gamestatid = game_stat.GetId();
	std::string input_auth       = gsplayer.GetAuth();
    int         input_pid        = gsplayer.GetProfileId();
	std::string input_machine_id = gsplayer.GetMachineId();
    int         input_team       = gsplayer.GetTeam();
	int32_t     input_score      = gsplayer.GetScore();
	uint32_t    input_rank       = gsplayer.GetRank();
	uint32_t    input_pph        = gsplayer.GetPPH();
	uint32_t    input_kills      = gsplayer.GetKills();
	uint32_t    input_deaths     = gsplayer.GetDeaths();
	uint32_t    input_suicides   = gsplayer.GetSuicides();
	uint32_t    input_time       = gsplayer.GetTime();
	uint32_t    input_lavd       = gsplayer.GetLAVsDestroyed();
	uint32_t    input_mavd       = gsplayer.GetMAVsDestroyed();
	uint32_t    input_havd       = gsplayer.GetHAVsDestroyed();
	uint32_t    input_hed        = gsplayer.GetHelicoptersDestroyed();
	uint32_t    input_pld        = gsplayer.GetPlanesDestroyed();
	uint32_t    input_bod        = gsplayer.GetBoatsDestroyed();
	uint32_t    input_k1         = gsplayer.GetKillsAssualtKit();
	uint32_t    input_s1         = gsplayer.GetSpawnsAssualtKit();
	uint32_t    input_k2         = gsplayer.GetKillsSniperKit();
	uint32_t    input_s2         = gsplayer.GetSpawnsSniperKit();
	uint32_t    input_k3         = gsplayer.GetKillsSpecialOpKit();
	uint32_t    input_s3         = gsplayer.GetSpawnsSpecialOpKit();
	uint32_t    input_k4         = gsplayer.GetKillsCombatEngineerKit();
	uint32_t    input_s4         = gsplayer.GetSpawnsCombatEngineerKit();
	uint32_t    input_k5         = gsplayer.GetKillsSupportKit();
	uint32_t    input_s5         = gsplayer.GetSpawnsSupportKit();
	uint32_t    input_tk         = gsplayer.GetTeamKills();
	uint32_t    input_medals     = gsplayer.GetMedals();
	uint32_t    input_ttb        = gsplayer.GetTotalTopPlayer();
	uint32_t    input_mv         = gsplayer.GetTotalVictories();
	uint32_t    input_ngp        = gsplayer.GetTotalGameSessions();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(33, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_gamestatid;
	input_bind[0].is_unsigned = false;
	input_bind[1].buffer_type = MYSQL_TYPE_STRING;
	input_bind[1].buffer = const_cast<char*>(&(input_auth[0]));
	input_bind[1].buffer_length = input_auth.size();
	input_bind[2].buffer_type = MYSQL_TYPE_LONG;
	input_bind[2].buffer = &input_pid;
	input_bind[2].is_unsigned = false;
	input_bind[3].buffer_type = MYSQL_TYPE_STRING;
	input_bind[3].buffer = const_cast<char*>(&(input_machine_id[0]));
	input_bind[3].buffer_length = input_machine_id.size();
	input_bind[4].buffer_type = MYSQL_TYPE_LONG;
	input_bind[4].buffer = &input_team;
	input_bind[4].is_unsigned = false;
	input_bind[5].buffer_type = MYSQL_TYPE_LONG;
	input_bind[5].buffer = &input_score;
	input_bind[5].is_unsigned = false;
	input_bind[6].buffer_type = MYSQL_TYPE_LONG;
	input_bind[6].buffer = &input_rank;
	input_bind[6].is_unsigned = true;
	input_bind[7].buffer_type = MYSQL_TYPE_LONG;
	input_bind[7].buffer = &input_pph;
	input_bind[7].is_unsigned = true;
	input_bind[8].buffer_type = MYSQL_TYPE_LONG;
	input_bind[8].buffer = &input_kills;
	input_bind[8].is_unsigned = true;
	input_bind[9].buffer_type = MYSQL_TYPE_LONG;
	input_bind[9].buffer = &input_deaths;
	input_bind[9].is_unsigned = true;
	input_bind[10].buffer_type = MYSQL_TYPE_LONG;
	input_bind[10].buffer = &input_suicides;
	input_bind[10].is_unsigned = true;
	input_bind[11].buffer_type = MYSQL_TYPE_LONG;
	input_bind[11].buffer = &input_time;
	input_bind[11].is_unsigned = true;
	input_bind[12].buffer_type = MYSQL_TYPE_LONG;
	input_bind[12].buffer = &input_lavd;
	input_bind[12].is_unsigned = true;
	input_bind[13].buffer_type = MYSQL_TYPE_LONG;
	input_bind[13].buffer = &input_mavd;
	input_bind[13].is_unsigned = true;
	input_bind[14].buffer_type = MYSQL_TYPE_LONG;
	input_bind[14].buffer = &input_havd;
	input_bind[14].is_unsigned = true;
	input_bind[15].buffer_type = MYSQL_TYPE_LONG;
	input_bind[15].buffer = &input_hed;
	input_bind[15].is_unsigned = true;
	input_bind[16].buffer_type = MYSQL_TYPE_LONG;
	input_bind[16].buffer = &input_pld;
	input_bind[16].is_unsigned = true;
	input_bind[17].buffer_type = MYSQL_TYPE_LONG;
	input_bind[17].buffer = &input_bod;
	input_bind[17].is_unsigned = true;
	input_bind[18].buffer_type = MYSQL_TYPE_LONG;
	input_bind[18].buffer = &input_k1;
	input_bind[18].is_unsigned = true;
	input_bind[19].buffer_type = MYSQL_TYPE_LONG;
	input_bind[19].buffer = &input_s1;
	input_bind[19].is_unsigned = true;
	input_bind[20].buffer_type = MYSQL_TYPE_LONG;
	input_bind[20].buffer = &input_k2;
	input_bind[20].is_unsigned = true;
	input_bind[21].buffer_type = MYSQL_TYPE_LONG;
	input_bind[21].buffer = &input_s2;
	input_bind[21].is_unsigned = true;
	input_bind[22].buffer_type = MYSQL_TYPE_LONG;
	input_bind[22].buffer = &input_k3;
	input_bind[22].is_unsigned = true;
	input_bind[23].buffer_type = MYSQL_TYPE_LONG;
	input_bind[23].buffer = &input_s3;
	input_bind[23].is_unsigned = true;
	input_bind[24].buffer_type = MYSQL_TYPE_LONG;
	input_bind[24].buffer = &input_k4;
	input_bind[24].is_unsigned = true;
	input_bind[25].buffer_type = MYSQL_TYPE_LONG;
	input_bind[25].buffer = &input_s4;
	input_bind[25].is_unsigned = true;
	input_bind[26].buffer_type = MYSQL_TYPE_LONG;
	input_bind[26].buffer = &input_k5;
	input_bind[26].is_unsigned = true;
	input_bind[27].buffer_type = MYSQL_TYPE_LONG;
	input_bind[27].buffer = &input_s5;
	input_bind[27].is_unsigned = true;
	input_bind[28].buffer_type = MYSQL_TYPE_LONG;
	input_bind[28].buffer = &input_tk;
	input_bind[28].is_unsigned = true;
	input_bind[29].buffer_type = MYSQL_TYPE_LONG;
	input_bind[29].buffer = &input_medals;
	input_bind[29].is_unsigned = true;
	input_bind[30].buffer_type = MYSQL_TYPE_LONG;
	input_bind[30].buffer = &input_ttb;
	input_bind[30].is_unsigned = true;
	input_bind[31].buffer_type = MYSQL_TYPE_LONG;
	input_bind[31].buffer = &input_mv;
	input_bind[31].is_unsigned = true;
	input_bind[32].buffer_type = MYSQL_TYPE_LONG;
	input_bind[32].buffer = &input_ngp;
	input_bind[32].is_unsigned = true;
	
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
