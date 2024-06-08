#include <iostream>
#include <mysql/mysql.h>

#include <settings.h>
#include <logger.h>
#include <util.h>

#include <database.h>

bool Database::queryGameServerByIp(Battlefield::GameServer& game_server)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

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
	query += "ORDER BY ";
	query += "	`verified` DESC";
	
	std::string input_ip   = game_server.GetIp();
	uint16_t    input_port = game_server.GetPort();
	
	int        output_id;
	uint8_t    output_flag;
	char       output_localip0[VARCHAR_LEN(15)];
	uint16_t   output_localport;
	uint8_t    output_natneg;
	char       output_gamename[VARCHAR_LEN(45)];
	char       output_hostname[VARCHAR_LEN(45)];
	uint16_t   output_hostport;
	char       output_gamever[VARCHAR_LEN(20)];
	char       output_cl[VARCHAR_LEN(20)];
	char       output_rv[VARCHAR_LEN(20)];
	char       output_map[VARCHAR_LEN(20)];
	uint8_t    output_mc;
	uint8_t    output_mapname;
	uint8_t    output_gc;
	char       output_gametype[VARCHAR_LEN(20)];
	char       output_gamevariant[VARCHAR_LEN(20)];
	uint8_t    output_numplayers;
	uint8_t    output_maxplayers;
	uint8_t    output_numteams;
	char       output_gamemode[VARCHAR_LEN(15)];
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
	int        output_c0;
	int        output_c1;
	char       output_n0[VARCHAR_LEN(32)];
	char       output_n1[VARCHAR_LEN(32)];
	uint8_t    output_c0c;
	uint8_t    output_c1c;
	char       output_team0[VARCHAR_LEN(5)];
	char       output_team1[VARCHAR_LEN(5)];
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
	output_bind[2].buffer_length = VARCHAR_LEN(15);
	output_bind[3].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[3].buffer = &output_localport;
	output_bind[3].is_unsigned = true;
	output_bind[4].buffer_type = MYSQL_TYPE_TINY;
	output_bind[4].buffer = &output_natneg;
	output_bind[4].is_unsigned = true;
	output_bind[5].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[5].buffer = output_gamename;
	output_bind[5].buffer_length = VARCHAR_LEN(45);
	output_bind[6].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[6].buffer = output_hostname;
	output_bind[6].buffer_length = VARCHAR_LEN(45);
	output_bind[7].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[7].buffer = &output_hostport;
	output_bind[7].is_unsigned = true;
	output_bind[8].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[8].buffer = output_gamever;
	output_bind[8].buffer_length = VARCHAR_LEN(20);
	output_bind[9].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[9].buffer = output_cl;
	output_bind[9].buffer_length = VARCHAR_LEN(20);
	output_bind[10].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[10].buffer = output_rv;
	output_bind[10].buffer_length = VARCHAR_LEN(20);
	output_bind[11].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[11].buffer = output_map;
	output_bind[11].buffer_length = VARCHAR_LEN(20);
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
	output_bind[15].buffer_length = VARCHAR_LEN(20);
	output_bind[16].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[16].buffer = output_gamevariant;
	output_bind[16].buffer_length = VARCHAR_LEN(20);
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
	output_bind[20].buffer_length = VARCHAR_LEN(15);
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
	output_bind[36].buffer_type = MYSQL_TYPE_LONG;
	output_bind[36].buffer = &output_c0;
	output_bind[36].is_unsigned = false;
	output_bind[37].buffer_type = MYSQL_TYPE_LONG;
	output_bind[37].buffer = &output_c1;
	output_bind[37].is_unsigned = false;
	output_bind[38].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[38].buffer = output_n0;
	output_bind[38].buffer_length = VARCHAR_LEN(32);
	output_bind[39].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[39].buffer = output_n1;
	output_bind[39].buffer_length = VARCHAR_LEN(32);
	output_bind[40].buffer_type = MYSQL_TYPE_TINY;
	output_bind[40].buffer = &output_c0c;
	output_bind[40].is_unsigned = true;
	output_bind[41].buffer_type = MYSQL_TYPE_TINY;
	output_bind[41].buffer = &output_c1c;
	output_bind[41].is_unsigned = true;
	output_bind[42].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[42].buffer = output_team0;
	output_bind[42].buffer_length = VARCHAR_LEN(5);
	output_bind[43].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[43].buffer = output_team1;
	output_bind[43].buffer_length = VARCHAR_LEN(5);
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
		game_server.SetVerified(output_verified);

		break;
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
	
	return true;
}

bool Database::queryGameServerByIpAndPort(Battlefield::GameServer& game_server)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

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
	char       output_localip0[VARCHAR_LEN(15)];
	uint16_t   output_localport;
	uint8_t    output_natneg;
	char       output_gamename[VARCHAR_LEN(45)];
	char       output_hostname[VARCHAR_LEN(45)];
	uint16_t   output_hostport;
	char       output_gamever[VARCHAR_LEN(20)];
	char       output_cl[VARCHAR_LEN(20)];
	char       output_rv[VARCHAR_LEN(20)];
	char       output_map[VARCHAR_LEN(20)];
	uint8_t    output_mc;
	uint8_t    output_mapname;
	uint8_t    output_gc;
	char       output_gametype[VARCHAR_LEN(20)];
	char       output_gamevariant[VARCHAR_LEN(20)];
	uint8_t    output_numplayers;
	uint8_t    output_maxplayers;
	uint8_t    output_numteams;
	char       output_gamemode[VARCHAR_LEN(15)];
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
	int        output_c0;
	int        output_c1;
	char       output_n0[VARCHAR_LEN(32)];
	char       output_n1[VARCHAR_LEN(32)];
	uint8_t    output_c0c;
	uint8_t    output_c1c;
	char       output_team0[VARCHAR_LEN(5)];
	char       output_team1[VARCHAR_LEN(5)];
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
	output_bind[2].buffer_length = VARCHAR_LEN(15);
	output_bind[3].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[3].buffer = &output_localport;
	output_bind[3].is_unsigned = true;
	output_bind[4].buffer_type = MYSQL_TYPE_TINY;
	output_bind[4].buffer = &output_natneg;
	output_bind[4].is_unsigned = true;
	output_bind[5].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[5].buffer = output_gamename;
	output_bind[5].buffer_length = VARCHAR_LEN(45);
	output_bind[6].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[6].buffer = output_hostname;
	output_bind[6].buffer_length = VARCHAR_LEN(45);
	output_bind[7].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[7].buffer = &output_hostport;
	output_bind[7].is_unsigned = true;
	output_bind[8].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[8].buffer = output_gamever;
	output_bind[8].buffer_length = VARCHAR_LEN(20);
	output_bind[9].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[9].buffer = output_cl;
	output_bind[9].buffer_length = VARCHAR_LEN(20);
	output_bind[10].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[10].buffer = output_rv;
	output_bind[10].buffer_length = VARCHAR_LEN(20);
	output_bind[11].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[11].buffer = output_map;
	output_bind[11].buffer_length = VARCHAR_LEN(20);
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
	output_bind[15].buffer_length = VARCHAR_LEN(20);
	output_bind[16].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[16].buffer = output_gamevariant;
	output_bind[16].buffer_length = VARCHAR_LEN(20);
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
	output_bind[20].buffer_length = VARCHAR_LEN(15);
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
	output_bind[36].buffer_type = MYSQL_TYPE_LONG;
	output_bind[36].buffer = &output_c0;
	output_bind[36].is_unsigned = false;
	output_bind[37].buffer_type = MYSQL_TYPE_LONG;
	output_bind[37].buffer = &output_c1;
	output_bind[37].is_unsigned = false;
	output_bind[38].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[38].buffer = output_n0;
	output_bind[38].buffer_length = VARCHAR_LEN(32);
	output_bind[39].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[39].buffer = output_n1;
	output_bind[39].buffer_length = VARCHAR_LEN(32);
	output_bind[40].buffer_type = MYSQL_TYPE_TINY;
	output_bind[40].buffer = &output_c0c;
	output_bind[40].is_unsigned = true;
	output_bind[41].buffer_type = MYSQL_TYPE_TINY;
	output_bind[41].buffer = &output_c1c;
	output_bind[41].is_unsigned = true;
	output_bind[42].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[42].buffer = output_team0;
	output_bind[42].buffer_length = VARCHAR_LEN(5);
	output_bind[43].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[43].buffer = output_team1;
	output_bind[43].buffer_length = VARCHAR_LEN(5);
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
		game_server.SetVerified(output_verified);
	}

	// Cleanup
	mysql_stmt_free_result(statement);
	mysql_stmt_close(statement);
	free(input_bind);
	free(output_bind);
	
	return true;
}

bool Database::queryGameServers(Battlefield::GameServers& game_servers)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

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
	char       output_ip[VARCHAR_LEN(15)];
	uint16_t   output_port;
	uint8_t    output_flag;
	char       output_localip0[VARCHAR_LEN(15)];
	uint16_t   output_localport;
	uint8_t    output_natneg;
	char       output_gamename[VARCHAR_LEN(45)];
	char       output_hostname[VARCHAR_LEN(45)];
	uint16_t   output_hostport;
	char       output_gamever[VARCHAR_LEN(20)];
	char       output_cl[VARCHAR_LEN(20)];
	char       output_rv[VARCHAR_LEN(20)];
	char       output_map[VARCHAR_LEN(20)];
	uint8_t    output_mc;
	uint8_t    output_mapname;
	uint8_t    output_gc;
	char       output_gametype[VARCHAR_LEN(20)];
	char       output_gamevariant[VARCHAR_LEN(20)];
	uint8_t    output_numplayers;
	uint8_t    output_maxplayers;
	uint8_t    output_numteams;
	char       output_gamemode[VARCHAR_LEN(15)];
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
	int        output_c0;
	int        output_c1;
	char       output_n0[VARCHAR_LEN(32)];
	char       output_n1[VARCHAR_LEN(32)];
	uint8_t    output_c0c;
	uint8_t    output_c1c;
	char       output_team0[VARCHAR_LEN(5)];
	char       output_team1[VARCHAR_LEN(5)];
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
	output_bind[1].buffer_length = VARCHAR_LEN(15);
	output_bind[2].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[2].buffer = &output_port;
	output_bind[2].is_unsigned = true;
	output_bind[3].buffer_type = MYSQL_TYPE_TINY;
	output_bind[3].buffer = &output_flag;
	output_bind[3].is_unsigned = true;
	output_bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[4].buffer = output_localip0;
	output_bind[4].buffer_length = VARCHAR_LEN(15);
	output_bind[5].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[5].buffer = &output_localport;
	output_bind[5].is_unsigned = true;
	output_bind[6].buffer_type = MYSQL_TYPE_TINY;
	output_bind[6].buffer = &output_natneg;
	output_bind[6].is_unsigned = true;
	output_bind[7].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[7].buffer = output_gamename;
	output_bind[7].buffer_length = VARCHAR_LEN(45);
	output_bind[8].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[8].buffer = output_hostname;
	output_bind[8].buffer_length = VARCHAR_LEN(45);
	output_bind[9].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[9].buffer = &output_hostport;
	output_bind[9].is_unsigned = true;
	output_bind[10].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[10].buffer = output_gamever;
	output_bind[10].buffer_length = VARCHAR_LEN(20);
	output_bind[11].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[11].buffer = output_cl;
	output_bind[11].buffer_length = VARCHAR_LEN(20);
	output_bind[12].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[12].buffer = output_rv;
	output_bind[12].buffer_length = VARCHAR_LEN(20);
	output_bind[13].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[13].buffer = output_map;
	output_bind[13].buffer_length = VARCHAR_LEN(20);
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
	output_bind[17].buffer_length = VARCHAR_LEN(20);
	output_bind[18].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[18].buffer = output_gamevariant;
	output_bind[18].buffer_length = VARCHAR_LEN(20);
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
	output_bind[22].buffer_length = VARCHAR_LEN(15);
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
	output_bind[38].buffer_type = MYSQL_TYPE_LONG;
	output_bind[38].buffer = &output_c0;
	output_bind[38].is_unsigned = false;
	output_bind[39].buffer_type = MYSQL_TYPE_LONG;
	output_bind[39].buffer = &output_c1;
	output_bind[39].is_unsigned = false;
	output_bind[40].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[40].buffer = output_n0;
	output_bind[40].buffer_length = VARCHAR_LEN(32);
	output_bind[41].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[41].buffer = output_n1;
	output_bind[41].buffer_length = VARCHAR_LEN(32);
	output_bind[42].buffer_type = MYSQL_TYPE_TINY;
	output_bind[42].buffer = &output_c0c;
	output_bind[42].is_unsigned = true;
	output_bind[43].buffer_type = MYSQL_TYPE_TINY;
	output_bind[43].buffer = &output_c1c;
	output_bind[43].is_unsigned = true;
	output_bind[44].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[44].buffer = output_team0;
	output_bind[44].buffer_length = VARCHAR_LEN(5);
	output_bind[45].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[45].buffer = output_team1;
	output_bind[45].buffer_length = VARCHAR_LEN(5);
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
		game_server.SetVerified(output_verified);
		
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
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

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
	int         input_c0            = game_server.GetClan1Id();
	int         input_c1            = game_server.GetClan2Id();
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
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

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
	int         input_c0            = game_server.GetClan1Id();
	int         input_c1            = game_server.GetClan2Id();
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
