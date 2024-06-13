#include <iostream>
#include <mysql/mysql.h>

#include <settings.h>
#include <logger.h>
#include <util.h>

#include <database.h>

bool Database::queryGameStatsByDate(Battlefield::GameStats& game_stats, const std::string& date)
{
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "SELECT ";
	query += "	`id`, `gametype`, `gamver`, `hostname`, `mapid`, `numplayers`, `pplayers`, `tplayed`, ";
	query += "	`clanid_t0`, `clanid_t1`, `country_t0`, `country_t1`, `victory_t0`, `victory_t1`, ";
	query += "	`score0`, `score1`, `created_at` ";
	query += "FROM ";
	query += "	`GameStats` ";
	query += "WHERE ";
	query += "	DATE(`created_at`) = ?";
	
	int        output_id;
	uint8_t    output_gametype;
	char       output_gamever[VARCHAR_LEN(20)];
	char       output_hostname[VARCHAR_LEN(45)];
	uint8_t    output_mapid;
	uint8_t    output_numplayers;
	uint8_t    output_pplayers;
	uint16_t   output_tplayed;
	int        output_clanid_t0;
	int        output_clanid_t1;
	uint8_t    output_country_t0;
	uint8_t    output_country_t1;
	uint8_t    output_victory_t0;
	uint8_t    output_victory_t1;
	int16_t    output_score0;
	int16_t    output_score1;
	MYSQL_TIME output_created_at;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_STRING;
	input_bind[0].buffer = const_cast<char*>(&(date[0]));
	input_bind[0].buffer_length = date.size();
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(17, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_LONG;
	output_bind[0].buffer = &output_id;
	output_bind[0].is_unsigned = false;
	output_bind[1].buffer_type = MYSQL_TYPE_TINY;
	output_bind[1].buffer = &output_gametype;
	output_bind[1].is_unsigned = true;
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = output_gamever;
	output_bind[2].buffer_length = VARCHAR_LEN(20);
	output_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[3].buffer = output_hostname;
	output_bind[3].buffer_length = VARCHAR_LEN(45);
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
	output_bind[8].buffer_type = MYSQL_TYPE_LONG;
	output_bind[8].buffer = &output_clanid_t0;
	output_bind[8].is_unsigned = true;
	output_bind[9].buffer_type = MYSQL_TYPE_LONG;
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
	output_bind[14].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[14].buffer = &output_score0;
	output_bind[14].is_unsigned = false;
	output_bind[15].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[15].buffer = &output_score1;
	output_bind[15].is_unsigned = false;
	output_bind[16].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[16].buffer = &output_created_at;
	
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
		game_stat.SetTeam1Score(output_score0);
		game_stat.SetTeam2Score(output_score1);
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
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "SELECT ";
	query += "	`gametype`, `gamver`, `hostname`, `mapid`, `numplayers`, `pplayers`, `tplayed`, ";
	query += "	`clanid_t0`, `clanid_t1`, `country_t0`, `country_t1`, `victory_t0`, `victory_t1`, ";
	query += "	`score0`, `score1`, `created_at` ";
	query += "FROM ";
	query += "	`GameStats` ";
	query += "WHERE ";
	query += "	`id` = ?";
	
	int        input_id = game_stat.GetId();
	
	uint8_t    output_gametype;
	char       output_gamever[VARCHAR_LEN(20)];
	char       output_hostname[VARCHAR_LEN(45)];
	uint8_t    output_mapid;
	uint8_t    output_numplayers;
	uint8_t    output_pplayers;
	uint16_t   output_tplayed;
	int        output_clanid_t0;
	int        output_clanid_t1;
	uint8_t    output_country_t0;
	uint8_t    output_country_t1;
	uint8_t    output_victory_t0;
	uint8_t    output_victory_t1;
	int16_t    output_score0;
	int16_t    output_score1;
	MYSQL_TIME output_created_at;
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(1, sizeof(MYSQL_BIND));
	input_bind[0].buffer_type = MYSQL_TYPE_LONG;
	input_bind[0].buffer = &input_id;
	input_bind[0].is_unsigned = false;
	
	// Allocate output binds
	MYSQL_BIND* output_bind = (MYSQL_BIND *)calloc(16, sizeof(MYSQL_BIND));
	output_bind[0].buffer_type = MYSQL_TYPE_TINY;
	output_bind[0].buffer = &output_gametype;
	output_bind[0].is_unsigned = true;
	output_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[1].buffer = output_gamever;
	output_bind[1].buffer_length = VARCHAR_LEN(20);
	output_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	output_bind[2].buffer = output_hostname;
	output_bind[2].buffer_length = VARCHAR_LEN(45);
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
	output_bind[7].buffer_type = MYSQL_TYPE_LONG;
	output_bind[7].buffer = &output_clanid_t0;
	output_bind[7].is_unsigned = true;
	output_bind[8].buffer_type = MYSQL_TYPE_LONG;
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
	output_bind[13].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[13].buffer = &output_score0;
	output_bind[13].is_unsigned = false;
	output_bind[14].buffer_type = MYSQL_TYPE_SHORT;
	output_bind[14].buffer = &output_score1;
	output_bind[14].is_unsigned = false;
	output_bind[15].buffer_type = MYSQL_TYPE_DATETIME;
	output_bind[15].buffer = &output_created_at;
	
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
		game_stat.SetTeam1Score(output_score0);
		game_stat.SetTeam2Score(output_score1);
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
	std::lock_guard<std::mutex> guard(this->_mutex); // database lock

	std::string query = "";
	query += "INSERT INTO `GameStats` ";
	query += "	(`gametype`, `gamver`, `hostname`, `mapid`, `numplayers`, `pplayers`, `tplayed`, ";
	query += "	`clanid_t0`, `clanid_t1`, `country_t0`, `country_t1`, `victory_t0`, `victory_t1`, ";
	query += "	`score0`, `score1`) ";
	query += "VALUES ";
	query += "	(?, ?, ?, ?, ?, ?, ?, ";
	query += "	?, ?, ?, ?, ?, ?, ";
	query += "	?, ?)";
	
	uint8_t     input_gametype   = game_stat.GetGameType();
	std::string input_gamver     = game_stat.GetGameVersion();
	std::string input_hostname   = game_stat.GetHostName();
	uint8_t     input_mapid      = game_stat.GetMapId();
	uint8_t     input_numplayers = game_stat.GetNumPlayers();
	uint8_t     input_pplayers   = game_stat.GetPPlayers();
	uint16_t    input_tplayed    = game_stat.GetTimePlayed();
	int         input_clanid_t0  = game_stat.GetTeam1ClanId();
	int         input_clanid_t1  = game_stat.GetTeam2ClanId();
	uint8_t     input_country_t0 = game_stat.GetTeam1Country();
	uint8_t     input_country_t1 = game_stat.GetTeam2Country();
	uint8_t     input_victory_t0 = game_stat.GetTeam1Victory();
	uint8_t     input_victory_t1 = game_stat.GetTeam2Victory();
	int16_t     input_score0     = game_stat.GetTeam1Score();
	int16_t     input_score1     = game_stat.GetTeam2Score();
	
	// Allocate input binds
	MYSQL_BIND* input_bind = (MYSQL_BIND *)calloc(15, sizeof(MYSQL_BIND));
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
	input_bind[7].buffer_type = MYSQL_TYPE_LONG;
	input_bind[7].buffer = &input_clanid_t0;
	input_bind[7].is_unsigned = true;
	input_bind[8].buffer_type = MYSQL_TYPE_LONG;
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
	input_bind[13].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[13].buffer = &input_score0;
	input_bind[13].is_unsigned = false;
	input_bind[14].buffer_type = MYSQL_TYPE_SHORT;
	input_bind[14].buffer = &input_score1;
	input_bind[14].is_unsigned = false;

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
