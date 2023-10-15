-- MySQL dump 10.13  Distrib 8.0.33, for Win64 (x86_64)
--
-- Host: 128.140.0.23    Database: BF2MC
-- ------------------------------------------------------
-- Server version	8.0.34-0ubuntu0.22.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Dumping routines for database 'BF2MC'
--
/*!50003 DROP PROCEDURE IF EXISTS `CreateLeaderboardClan` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `CreateLeaderboardClan`()
BEGIN
	CALL DropTableIfExists('Leaderboard_clan');
    
	CREATE TABLE Leaderboard_clan AS
	SELECT 
		ROW_NUMBER() OVER (
			ORDER BY 
				`score` DESC, 
				`wins` DESC, 
				`losses` DESC, 
				`draws` DESC
		) AS `rank`, 
		`Clans`.`clanid`  AS `clanid`, 
		`Clans`.`name` AS `name`, 
		`Clans`.`tag` AS `tag`, 
		`Clans`.`score` AS `score`, 
		`Clans`.`wins` AS `wins`, 
		`Clans`.`losses` AS `losses`, 
		`Clans`.`draws` AS `draws`
	FROM 
		`Clans`
	ORDER BY 
		`rank` ASC,
		`score` DESC, 
		`wins` DESC, 
		`losses` DESC, 
		`draws` DESC;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `CreateLeaderboardRank` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `CreateLeaderboardRank`()
BEGIN
	CALL DropTableIfExists('Leaderboard_rank');
    
	CREATE TABLE Leaderboard_rank AS
	SELECT 
		ROW_NUMBER() OVER (
			ORDER BY 
				`ran` DESC, 
				`score` DESC, 
				`pph` DESC
		) AS `rank`, 
		Players.profileid  AS `profileid`, 
		Players.uniquenick AS `uniquenick`, 
		PlayerStats.score AS `score`, 
		PlayerStats.ran AS `ran`, 
		PlayerStats.pph AS `pph` 
	FROM 
		`Players`, 
		`PlayerStats` 
	WHERE 
		Players.profileid = PlayerStats.profileid 
	ORDER BY 
		`rank` ASC, 
		`ran` DESC, 
		`score` DESC, 
		`pph` DESC;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `CreateLeaderboardRatio` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `CreateLeaderboardRatio`(
		IN input_k VARCHAR(255),
        IN input_s VARCHAR(255)
)
BEGIN
	DECLARE fullTableName VARCHAR(255);
	SET fullTableName = CONCAT('Leaderboard_ratio_', input_k, '_', input_s);
    
    CALL DropTableIfExists(fullTableName);
    
	SET @query = CONCAT('CREATE TABLE ', fullTableName, ' AS
        SELECT 
            ROW_NUMBER() OVER (
                ORDER BY 
                    PlayerStats.', input_k, ' / PlayerStats.', input_s, ' DESC
            ) AS `rank`, 
            Players.profileid  AS `profileid`, 
            Players.uniquenick AS `uniquenick`, 
            PlayerStats.', input_k, ' / PlayerStats.', input_s, ' AS `ratio`,
            PlayerStats.', input_k, ' AS `', input_k, '`, 
            PlayerStats.', input_s, ' AS `', input_s, '`
        FROM 
            `Players`, 
            `PlayerStats` 
        WHERE 
            Players.profileid = PlayerStats.profileid AND
			PlayerStats.', input_k, ' != 0 AND
            PlayerStats.', input_s, ' != 0
        ORDER BY 
            `rank` ASC, 
            `ratio` DESC');
    
    PREPARE stmt FROM @query;
    EXECUTE stmt;
    DEALLOCATE PREPARE stmt;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `CreateLeaderboards` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `CreateLeaderboards`()
BEGIN
	CALL CreateLeaderboardRank();
	CALL CreateLeaderboardType('pph');
	CALL CreateLeaderboardType('score');
	CALL CreateLeaderboardType('k1');
	CALL CreateLeaderboardType('k2');
	CALL CreateLeaderboardType('k3');
	CALL CreateLeaderboardType('k4');
	CALL CreateLeaderboardType('k5');
	CALL CreateLeaderboardType('kills');
	CALL CreateLeaderboardType('lavd');
	CALL CreateLeaderboardType('mavd');
	CALL CreateLeaderboardType('havd');
	CALL CreateLeaderboardType('hed');
	CALL CreateLeaderboardType('bod');
	CALL CreateLeaderboardType('vehicles');
    CALL CreateLeaderboardRatio('k1', 's1');
    CALL CreateLeaderboardRatio('k2', 's2');
    CALL CreateLeaderboardRatio('k3', 's3');
    CALL CreateLeaderboardRatio('k4', 's4');
    CALL CreateLeaderboardRatio('k5', 's5');
    CALL CreateLeaderboardRatio('kills', 'deaths');
    CALL CreateLeaderboardClan();
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `CreateLeaderboardType` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `CreateLeaderboardType`(
		IN input_name VARCHAR(255)
)
BEGIN
	DECLARE fullTableName VARCHAR(255);
	SET fullTableName = CONCAT('Leaderboard_', input_name);
    
    CALL DropTableIfExists(fullTableName);
    
	SET @query = CONCAT('CREATE TABLE ', fullTableName, ' AS
        SELECT 
            ROW_NUMBER() OVER (
                ORDER BY 
                    ', input_name, ' DESC
            ) AS `rank`, 
            Players.profileid  AS `profileid`, 
            Players.uniquenick AS `uniquenick`, 
            PlayerStats.', input_name, ' AS ', input_name, '
        FROM 
            `Players`, 
            `PlayerStats` 
        WHERE 
            Players.profileid = PlayerStats.profileid 
        ORDER BY 
            `rank` ASC, 
            ', input_name, ' DESC');
    
    PREPARE stmt FROM @query;
    EXECUTE stmt;
    DEALLOCATE PREPARE stmt;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `DropTableIfExists` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `DropTableIfExists`(tableName VARCHAR(255))
BEGIN
    DECLARE tableCount INT;
    
    -- Check if the table exists
    SELECT COUNT(*)
    INTO tableCount
    FROM INFORMATION_SCHEMA.TABLES
    WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = tableName;

    -- If the table exists, drop it
    IF tableCount > 0 THEN
        SET @sql = CONCAT('DROP TABLE ', tableName);
        PREPARE stmt FROM @sql;
        EXECUTE stmt;
        DEALLOCATE PREPARE stmt;
    END IF;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `insertClan` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `insertClan`(
		IN input_name VARCHAR(32),
        IN input_tag VARCHAR(3),
        IN input_homepage VARCHAR(256),
		IN input_info VARCHAR(1024),
        IN input_region TINYINT
)
BEGIN
	INSERT INTO `Clans` 
		(`name`, `tag`, `homepage`, `info`, `region`) 
	VALUES 
		(input_name, input_tag, input_homepage, input_info, input_region);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `insertPlayer` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `insertPlayer`(
		IN input_userid INT,
        IN input_nick VARCHAR(41),
        IN input_uniquenick VARCHAR(32),
        IN input_email VARCHAR(50),
        IN input_password VARCHAR(32)
)
BEGIN
	INSERT INTO `Players` 
		(`userid`, `nick`, `uniquenick`, `email`, `password`) 
	VALUES 
		(input_userid, input_nick, input_uniquenick, input_email, input_password);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `insertPlayerFriend` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `insertPlayerFriend`(
		IN input_profileid INT,
        IN input_target_profileid INT
)
BEGIN
	INSERT INTO `PlayerFriends` 
		(`profileid`, `target_profileid`) 
	VALUES 
		(input_profileid, input_target_profileid);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryClanByClanId` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryClanByClanId`(
		IN input_clanid INT
)
BEGIN
	SELECT 
		`name`, `tag`, `homepage`, `info`, `region`, `created_at`
	FROM 
		`Clans` 
	WHERE 
		`clanid` = input_clanid;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryClanByNameOrTag` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryClanByNameOrTag`(
		IN input_name VARCHAR(32),
        IN input_tag VARCHAR(3)
)
BEGIN
	SELECT 
		`clanid`, `name`, `tag`, `homepage`, `info`, `region`, `created_at` 
	FROM 
		`Clans` 
	WHERE 
		`name` = input_name
	OR 
		`tag` = input_tag;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryClanByProfileId` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryClanByProfileId`(
		IN input_profileid INT
)
BEGIN
	SELECT 
		Clans.clanid as `clanid`, `name`, `tag`, `homepage`, `info`, `region`, `created_at` 
	FROM 
		`Clans`, `ClanRanks` 
	WHERE 
		Clans.clanid = ClanRanks.clanid  
	AND 
		ClanRanks.profileid = input_profileid;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryClanRanksByClanId` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryClanRanksByClanId`(
		IN input_clanid INT
)
BEGIN
	SELECT 
		`profileid`, `rank` 
	FROM 
		`ClanRanks` 
	WHERE 
		`clanid` = input_clanid;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryGameCount` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryGameCount`(
		IN clan_games_filter TINYINT
)
BEGIN
    IF clan_games_filter = 1 THEN
        -- Count only clan games
        SELECT COUNT(*) AS game_count FROM `GameStats`
        WHERE `clanid_t0` <> 0 AND `clanid_t1` <> 0;
    ELSEIF clan_games_filter = 2 THEN
        -- Count all games (public and clan)
        SELECT COUNT(*) AS game_count FROM `GameStats`;
    ELSE
        -- Count only public games
        SELECT COUNT(*) AS game_count FROM `GameStats`
        WHERE `clanid_t0` = 0 AND `clanid_t1` = 0;
    END IF;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryGameServerByIpAndPort` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryGameServerByIpAndPort`(
		IN input_ip VARCHAR(15),
        IN input_port SMALLINT
)
BEGIN
	SELECT 
		`id`, `flag`, `localip0`, `localport`, `natneg`, `gamename`, 
		`hostname`, `hostport`, `gamever`, `cl`, `rv`, `map`, `mc`, `mapname`, `gc`, 
		`gametype`, `gamevariant`, `numplayers`, `maxplayers`, `numteams`, `gamemode`, 
		`teamplay`, `fraglimit`, `teamfraglimit`, `timelimit`, `timeelapsed`, `password`, 
		`nr`, `xr`, `ff`, `sr`, `rc`, `ni`, `xi`, `qm`, `region`, 
		`c0`, `c1`, `n0`, `n1`, `c0c`, `c1c`, 
		`team0`, `team1`, `score0`, `score1`, `updated_at`, `verified`
	FROM 
		`GameServers`
	WHERE 
		`ip` = input_ip
	AND
		`port` = input_port;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryGameServerPlayersByGameServerId` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryGameServerPlayersByGameServerId`(
		IN input_gameserverid INT
)
BEGIN
	SELECT 
		`id`, `name`, `score`, `skill`, `ping`, `team`, `deaths`, `profileid` 
	FROM 
		`GameServerPlayers` 
	WHERE 
		`gameserverid` = input_gameserverid;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryGameServers` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryGameServers`()
BEGIN
	SELECT 
		`id`, `ip`, `port`, `flag`, `localip0`, `localport`, `natneg`, `gamename`, 
		`hostname`, `hostport`, `gamever`, `cl`, `rv`, `map`, `mc`, `mapname`, `gc`, 
		`gametype`, `gamevariant`, `numplayers`, `maxplayers`, `numteams`, `gamemode`, 
		`teamplay`, `fraglimit`, `teamfraglimit`, `timelimit`, `timeelapsed`, `password`, 
		`nr`, `xr`, `ff`, `sr`, `rc`, `ni`, `xi`, `qm`, `region`, 
		`c0`, `c1`, `n0`, `n1`, `c0c`, `c1c`, 
		`team0`, `team1`, `score0`, `score1`, `updated_at`, `verified`
	FROM 
		`GameServers`;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryGameStatById` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryGameStatById`(
		IN input_id INT
)
BEGIN
	SELECT 
		`gametype`, `gamver`, `hostname`, `mapid`, `numplayers`, `pplayers`, `tplayed`, 
		`clanid_t0`, `clanid_t1`, `country_t0`, `country_t1`, `victory_t0`, `victory_t1`, 
		`created_at` 
	FROM 
		`GameStats` 
	WHERE 
		`id` = input_id;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryGameStatPlayersByGameStatId` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryGameStatPlayersByGameStatId`(
		IN input_gamestatid INT
)
BEGIN
	SELECT 
		`id`, `auth`, `pid`, `score`, `rank`, `pph`, `kills`, `suicides`, `time`, 
		`lavd`, `mavd`, `havd`, `hed`, `pld`, `bod`, 
		`k1`, `s1`, `k2`, `s2`, `k3`, `s3`, `k4`, `s4`, `k5`, `s5`, 
		`tk`, `medals`, `ttb`, `mv`, `ngp` 
	FROM 
		`GameStatPlayers` 
	WHERE 
		`gamestatid` = input_gamestatid;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryGameStatsByDate` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryGameStatsByDate`(
		IN input_date VARCHAR(10)
)
BEGIN
	SELECT 
		`id`, `gametype`, `gamver`, `hostname`, `mapid`, `numplayers`, `pplayers`, `tplayed`, 
		`clanid_t0`, `clanid_t1`, `country_t0`, `country_t1`, `victory_t0`, `victory_t1`, 
		`created_at` 
	FROM 
		`GameStats` 
	WHERE 
		DATE(`created_at`) = input_date;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryGameStatsResults` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryGameStatsResults`(
	IN input_profileid INT
)
BEGIN
	SELECT
		`GameStats`.`id` AS `gamestatid`,
        `GameStats`.`victory_t0` AS `victory_t0`,
        `GameStats`.`victory_t1` AS `victory_t1`,
        `GameStatPlayers`.`team` AS `team`,
        CASE
            WHEN (`GameStats`.`victory_t1` = 3 OR `GameStats`.`victory_t0` = 3) THEN 'draw'
            WHEN (`GameStatPlayers`.`team` = 0 AND `GameStats`.`victory_t0` > 0) THEN 'win'
            WHEN (`GameStatPlayers`.`team` = 1 AND `GameStats`.`victory_t1` > 0) THEN 'win'
            ELSE 'lose'
        END AS `state`
	FROM
		`GameStats`,
        `GameStatPlayers`
	WHERE
		`GameStatPlayers`.`gamestatid` = `GameStats`.`id`
	AND
		`GameStatPlayers`.`pid` = input_profileid
	ORDER BY
		`GameStats`.`created_at` DESC
	LIMIT 10;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryGameStatsResultsWithStreaks` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryGameStatsResultsWithStreaks`(
    IN input_profileid INT
)
BEGIN
    DECLARE current_streak INT;
    DECLARE max_streak INT;
    DECLARE last_state VARCHAR(255);
    DECLARE done INT DEFAULT 0;
    DECLARE cur CURSOR FOR SELECT state FROM temp_game_stats ORDER BY gamestatid DESC;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = 1;
    
    -- Create a temporary table to store the game stats
    CREATE TEMPORARY TABLE IF NOT EXISTS temp_game_stats (
        gamestatid INT,
        victory_t0 INT,
        victory_t1 INT,
        team INT,
        state VARCHAR(255)
    );

    -- Initialize variables
    SET current_streak = 0;
    SET max_streak = 0;
    SET last_state = NULL;
	
    -- Insert game stats for the specified profile into the temporary table
    INSERT INTO temp_game_stats (gamestatid, victory_t0, victory_t1, team, state)
    SELECT
        `GameStats`.`id`,
        `GameStats`.`victory_t0`,
        `GameStats`.`victory_t1`,
        `GameStatPlayers`.`team`,
        CASE
            WHEN (`GameStats`.`victory_t1` = 3 OR `GameStats`.`victory_t0` = 3) THEN 'draw'
            WHEN (`GameStatPlayers`.`team` = 0 AND `GameStats`.`victory_t0` > 0) THEN 'win'
            WHEN (`GameStatPlayers`.`team` = 1 AND `GameStats`.`victory_t1` > 0) THEN 'win'
            ELSE 'lose'
        END AS `state`
    FROM
        `GameStats`,
        `GameStatPlayers`
    WHERE
        `GameStatPlayers`.`gamestatid` = `GameStats`.`id`
    AND
        `GameStatPlayers`.`pid` = input_profileid
    ORDER BY
        `GameStats`.`created_at` DESC;
    
    OPEN cur;

    read_loop: LOOP
        FETCH cur INTO last_state;
        IF done THEN
            LEAVE read_loop;
        END IF;

        IF last_state = 'win' THEN
            SET current_streak = current_streak + 1;
        ELSE
			SET current_streak = 0;
		END IF;
        
		IF current_streak > max_streak THEN
			SET max_streak = current_streak;
		END IF;
    END LOOP;

    CLOSE cur;

    -- Drop the temporary table
    DROP TEMPORARY TABLE IF EXISTS temp_game_stats;
    
    SELECT max_streak AS win_streak;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryLeaderboardClan` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryLeaderboardClan`(
		IN input_limit INT,
        IN input_offset INT
)
BEGIN
	SELECT 
		`rank`, `clanid`, `name`, `tag`, `score`, `wins`, `losses`, `draws` 
	FROM 
		`Leaderboard_clan` 
	ORDER BY 
		`rank` ASC, 
		`score` DESC, 
		`wins` DESC, 
		`losses` DESC, 
		`draws` DESC  
	LIMIT input_limit OFFSET input_offset;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryLeaderboardRank` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryLeaderboardRank`(
		IN input_limit INT,
        IN input_offset INT
)
BEGIN
	SELECT 
		`rank`, `profileid`, `uniquenick`, `score`, `ran`, `pph` 
	FROM 
		`Leaderboard_rank` 
	ORDER BY 
		`rank` ASC, 
		`ran` DESC, 
		`score` DESC, 
		`pph` DESC 
	LIMIT input_limit OFFSET input_offset;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryLeaderboardRankByProfileid` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryLeaderboardRankByProfileid`(
		IN input_profileid INT
)
BEGIN
	SELECT `rp`.* 
	FROM 
		`Leaderboard_rank` AS `rp` 
	JOIN ( 
		SELECT 
			`profileid`, 
			`rank` AS `start_rank` 
		FROM 
			`Leaderboard_rank` 
		WHERE 
			`profileid` = input_profileid
	) AS `start` ON rp.rank >= CASE WHEN start.start_rank <= 4 THEN 1 ELSE start.start_rank - 4 END 
	JOIN ( 
		SELECT 
			`profileid`, 
			`rank` AS `end_rank` 
		FROM 
			`Leaderboard_rank` 
		WHERE 
			`profileid` = input_profileid
	) AS `end` ON rp.rank <= CASE WHEN end.end_rank <= 4 THEN 10 ELSE end.end_rank + 5 END
    ORDER BY 
		`rank` ASC, 
		`ran` DESC, 
		`score` DESC, 
		`pph` DESC;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryLeaderboardRatio` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryLeaderboardRatio`(
	IN input_k VARCHAR(16),
    IN input_s VARCHAR(16),
    IN input_limit INT,
	IN input_offset INT
)
BEGIN
	SET @input_k = input_k;
	SET @input_s = input_s;
	SET @input_limit = input_limit;
	SET @input_offset = input_offset;
    
	SET @query = CONCAT('
		SELECT 
			`rank`, `profileid`, `uniquenick`, `ratio`, ?, ? 
		FROM 
			`Leaderboard_ratio_', input_k, '_', input_s, '` 
		ORDER BY 
			`rank` ASC, 
			`ratio` DESC 
		LIMIT ? OFFSET ?
	');
    
    PREPARE stmt FROM @query;
    EXECUTE stmt USING @input_k, @input_s, @input_limit, @input_offset;
    DEALLOCATE PREPARE stmt;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryLeaderboardRatioByProfileid` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryLeaderboardRatioByProfileid`(
	IN input_k VARCHAR(16),
    IN input_s VARCHAR(16),
    IN input_profileid INT
)
BEGIN
	SET @input_profileid = input_profileid;
    
	SET @query = CONCAT('
		SELECT `rp`.* 
		FROM 
			`Leaderboard_ratio_', input_k, '_', input_s, '` AS `rp` 
		JOIN ( 
			SELECT 
				`profileid`, 
				`rank` AS `start_rank` 
			FROM 
				`Leaderboard_ratio_', input_k, '_', input_s, '` 
			WHERE 
				`profileid` = ?
		) AS `start` ON rp.rank >= CASE WHEN start.start_rank <= 4 THEN 1 ELSE start.start_rank - 4 END 
		JOIN ( 
			SELECT 
				`profileid`, 
				`rank` AS `end_rank` 
			FROM 
				`Leaderboard_ratio_', input_k, '_', input_s, '` 
			WHERE 
				`profileid` = ?
		) AS `end` ON rp.rank <= CASE WHEN end.end_rank <= 4 THEN 10 ELSE end.end_rank + 5 END
        ORDER BY 
			`rank` ASC, 
			`ratio` DESC
	');
    
    PREPARE stmt FROM @query;
    EXECUTE stmt USING @input_profileid, @input_profileid;
    DEALLOCATE PREPARE stmt;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryLeaderboardTopByRank` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryLeaderboardTopByRank`(
		IN input_limit INT,
        IN input_offset INT
)
BEGIN
	SELECT 
		`rank`, `profileid`, `uniquenick`, `score`, `ran`, `pph` 
	FROM 
		`Leaderboard_rank` 
	ORDER BY 
		`rank` ASC, 
		`ran` DESC, 
		`score` DESC, 
		`pph` DESC 
	LIMIT input_limit OFFSET input_offset;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryLeaderboardTopByType` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryLeaderboardTopByType`(
		IN input_name VARCHAR(16),
        IN input_limit INT,
        IN input_offset INT
)
BEGIN
	SET @query = CONCAT('
		SELECT 
			`rank`, `profileid`, `uniquenick`, `', input_name, '` 
		FROM 
			`Leaderboard_', input_name, '` 
		ORDER BY 
			`rank` ASC, 
			`', input_name, '` DESC 
		LIMIT ', input_limit, ' OFFSET ', input_offset, ''
    );
    
    PREPARE stmt FROM @query;
    EXECUTE stmt;
    DEALLOCATE PREPARE stmt;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryLeaderboardType` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryLeaderboardType`(
		IN input_type VARCHAR(16),
        IN input_limit INT,
        IN input_offset INT
)
BEGIN
	SET @input_type = input_type;
	SET @input_limit = input_limit;
	SET @input_offset = input_offset;
    
	SET @query = CONCAT('
		SELECT 
			`rank`, `profileid`, `uniquenick`, ? 
		FROM 
			`Leaderboard_', input_type, '` 
		ORDER BY 
			`rank` ASC, 
			`', input_type, '` DESC 
		LIMIT ? OFFSET ?
	');
    
    PREPARE stmt FROM @query;
    EXECUTE stmt USING @input_type, @input_limit, @input_offset;
    DEALLOCATE PREPARE stmt;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryLeaderboardTypeByProfileid` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryLeaderboardTypeByProfileid`(
		IN input_type VARCHAR(16),
        IN input_profileid INT
)
BEGIN
	SET @input_profileid = input_profileid;
    
	SET @query = CONCAT('
		SELECT `rp`.* 
		FROM 
			`Leaderboard_', input_type, '` AS `rp` 
		JOIN ( 
			SELECT 
				`profileid`, 
				`rank` AS `start_rank` 
			FROM 
				`Leaderboard_', input_type, '` 
			WHERE 
				`profileid` = ?
		) AS `start` ON rp.rank >= CASE WHEN start.start_rank <= 4 THEN 1 ELSE start.start_rank - 4 END 
		JOIN ( 
			SELECT 
				`profileid`, 
				`rank` AS `end_rank` 
			FROM 
				`Leaderboard_', input_type, '` 
			WHERE 
				`profileid` = ?
		) AS `end` ON rp.rank <= CASE WHEN end.end_rank <= 4 THEN 10 ELSE end.end_rank + 5 END
        ORDER BY 
			`rank` ASC, 
			`', input_type, '` DESC 
	');
    
    PREPARE stmt FROM @query;
    EXECUTE stmt USING @input_profileid, @input_profileid;
    DEALLOCATE PREPARE stmt;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryPlayerByProfileid` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryPlayerByProfileid`(
		IN input_profile_id INT
)
BEGIN
		SELECT 
		`userid`, `nick`, `uniquenick`, `email`, `password`, `last_login`, `last_login_ip`, `created_at`, `verified` 
	FROM 
		`Players` 
	WHERE 
		`profileid` = input_profile_id;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryPlayerByUniquenick` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryPlayerByUniquenick`(
		IN input_uniquenick VARCHAR(32)
)
BEGIN
	SELECT 
		`profileid`, `userid`, `nick`, `email`, `password`, `last_login`, `last_login_ip`, `created_at`, `verified` 
	FROM 
		`Players` 
	WHERE 
		`uniquenick` = input_uniquenick;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryPlayerCount` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryPlayerCount`(
		IN unique_user BOOLEAN
)
BEGIN
    IF unique_user = FALSE THEN
        -- Count all rows in the Players table
        SELECT COUNT(*) AS player_count FROM `Players`;
    ELSE
        -- Count rows with unique 'userid'
        SELECT COUNT(DISTINCT `last_login_ip`) AS unique_user_count FROM `Players`;
    END IF;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryPlayerFriendsByProfileId` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryPlayerFriendsByProfileId`(
		IN input_profileid INT
)
BEGIN
	SELECT 
		`profileid`, `target_profileid` 
	FROM 
		`PlayerFriends` 
	WHERE 
		`profileid` = input_profileid
	OR 
		`target_profileid` = input_profileid;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryPlayerGametypesPlayed` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryPlayerGametypesPlayed`(
		IN input_profileid INT
)
BEGIN
	SELECT
		`GameStats`.`gametype` AS `gametype_id`
	FROM
		`GameStats`,
        `GameStatPlayers`
	WHERE
		`GameStatPlayers`.`gamestatid` = `GameStats`.`id`
	AND
		`GameStatPlayers`.`pid` = input_profileid;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryPlayersByEmail` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryPlayersByEmail`(
		IN input_email VARCHAR(50)
)
BEGIN
	SELECT 
		`profileid`, `userid`, `nick`, `email`, `password`, `last_login`, `last_login_ip`, `created_at`, `verified` 
	FROM 
		`Players` 
	WHERE 
		`email` = input_email;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryPlayersByEmailAndUniquenick` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryPlayersByEmailAndUniquenick`(
		IN input_email VARCHAR(50),
        IN input_uniquenick VARCHAR(32)
)
BEGIN
	SELECT 
		`profileid`, `userid`, `nick`, `uniquenick`, `email`, `password`, `last_login`, `last_login_ip`, `created_at`, `verified` 
	FROM 
		`Players` 
	WHERE 
		`email` = input_email 
	OR 
		`uniquenick` = input_uniquenick;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryPlayerStatsByProfileId` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryPlayerStatsByProfileId`(
		IN input_profileid INT
)
BEGIN
	SELECT 
		`score`, `ran`, `pph`, `kills`, `deaths`, `suicides`, `time`, 
		`vehicles`, `lavd`, `mavd`, `havd`, `hed`, `bod`, 
		`k1`, `s1`, `k2`, `s2`, `k3`, `s3`, `k4`, `s4`, `k5`, `s5`, 
		`medals`, `ttb`, `mv`, `ngp` 
	FROM 
		`PlayerStats` 
	WHERE 
		`profileid` = input_profileid;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryPlayerTeamCountriesPlayed` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryPlayerTeamCountriesPlayed`(
    IN input_profileid INT
)
BEGIN
	SELECT
		CASE
			WHEN (`GameStatPlayers`.`team` = 1) THEN `GameStats`.`country_t1`
			ELSE `GameStats`.`country_t0`
		END AS `country_id`
        /* Country ID Values:
			1 = US
            2 = CH
            3 = AC
            4 = EU
		*/
	FROM
		`GameStats`,
		`GameStatPlayers`
	WHERE
		`GameStatPlayers`.`gamestatid` = `GameStats`.`id`
	AND
		`GameStatPlayers`.`pid` = input_profileid
	AND
		`GameStats`.`country_t0` != 0
	AND
		`GameStats`.`country_t1` != 0
	AND
		`GameStatPlayers`.`team` != -1
	ORDER BY
		`GameStats`.`created_at` DESC;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryPlayerWinLoseDraw` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryPlayerWinLoseDraw`(
		IN input_profileid INT
)
BEGIN
	SELECT
		`GameStats`.`id` AS `gamestatid`,
        `GameStats`.`victory_t0` AS `victory_t0`,
        `GameStats`.`victory_t1` AS `victory_t1`,
        `GameStatPlayers`.`pid` AS `profileid`,
        `GameStatPlayers`.`team` AS `team`,
        CASE
            WHEN (`GameStats`.`victory_t1` = 3 OR `GameStats`.`victory_t0` = 3) THEN 'draw'
            WHEN (`GameStatPlayers`.`team` = 0 AND `GameStats`.`victory_t0` > 0) THEN 'win'
            WHEN (`GameStatPlayers`.`team` = 1 AND `GameStats`.`victory_t1` > 0) THEN 'win'
            ELSE 'lose'
        END AS `state`
	FROM
		`GameStats`,
        `GameStatPlayers`
	WHERE
		`GameStatPlayers`.`gamestatid` = `GameStats`.`id`
	AND
		`GameStatPlayers`.`pid` = input_profileid;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `queryPlayerWinLoseDraw_test` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `queryPlayerWinLoseDraw_test`(
        IN input_profileid INT
)
BEGIN
    SELECT
        `GameStats`.`id` AS `gamestatid`,
        `GameStatPlayers`.`pid` AS `profileid`,
        `GameStatPlayers`.`team` AS `team`,
        CASE
            WHEN (`GameStatPlayers`.`team` = 0) THEN
              `GameStats`.`victory_t0`
            ELSE 
              `GameStats`.`victory_t1`
        END AS `victory_state`,
        CASE
            WHEN (`GameStatPlayers`.`team` = 0) THEN
              `GameStats`.`victory_t1`
            ELSE 
              `GameStats`.`victory_t0`
        END AS `other_team_victory_state`,
        `GameStats`.`created_at` AS `created_at`
    FROM
        `GameStats`,
        `GameStatPlayers`
    WHERE
        `GameStatPlayers`.`gamestatid` = `GameStats`.`id`
    AND
        `GameStatPlayers`.`pid` = input_profileid
	ORDER BY
		`GameStats`.`created_at` DESC;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `removeClan` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `removeClan`(
		IN input_clanid INT
)
BEGIN
	DELETE FROM 
		`Clans` 
	WHERE 
		`clanid` = input_clanid;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `removeClanRank` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `removeClanRank`(
		IN input_clanid INT,
        IN input_profileid INT
)
BEGIN
	DELETE FROM 
		`ClanRanks` 
	WHERE 
		`clanid` = input_clanid
	AND 
		`profileid` = input_profileid;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `removeClanRanksByClanId` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `removeClanRanksByClanId`(
		IN input_clanid INT
)
BEGIN
	DELETE FROM 
		`ClanRanks` 
	WHERE 
		`clanid` = input_clanid;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `removeGameServerPlayers` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `removeGameServerPlayers`(
		IN input_gameserverid INT
)
BEGIN
	DELETE FROM 
		`GameServerPlayers` 
	WHERE 
		`gameserverid` = input_gameserverid;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `removePlayerFriend` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `removePlayerFriend`(
		IN input_profileid INT,
        IN input_target_profileid INT
)
BEGIN
	DELETE FROM 
		`PlayerFriends` 
	WHERE 
		(`profileid` = input_profileid AND `target_profileid` = input_target_profileid) 
	OR 
		(`profileid` = input_target_profileid AND `target_profileid` = input_profileid);
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `updateClanRank` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE PROCEDURE `updateClanRank`(
		IN input_clanid INT,
        IN input_profileid INT,
        IN new_rank TINYINT
)
BEGIN
	UPDATE 
		`ClanRanks` 
	SET 
		`rank` = new_rank
	WHERE 
		`clanid` = input_clanid
	AND 
		`profileid` = input_profileid;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2023-10-15 15:31:03
