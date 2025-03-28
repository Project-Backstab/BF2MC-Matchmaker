-- MySQL dump 10.13  Distrib 8.0.33, for Win64 (x86_64)
--
-- Host: 128.140.0.23    Database: BF2MC
-- ------------------------------------------------------
-- Server version	8.0.36-0ubuntu0.22.04.1

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
-- Table structure for table `GameServers`
--

DROP TABLE IF EXISTS `GameServers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `GameServers` (
  `id` int NOT NULL AUTO_INCREMENT,
  `ip` varchar(15) CHARACTER SET ascii COLLATE ascii_general_ci NOT NULL DEFAULT '',
  `port` smallint unsigned NOT NULL DEFAULT '3658',
  `flag` tinyint unsigned NOT NULL DEFAULT '59',
  `localip0` varchar(15) CHARACTER SET ascii COLLATE ascii_general_ci NOT NULL DEFAULT '',
  `localport` smallint unsigned NOT NULL DEFAULT '3658',
  `natneg` tinyint unsigned NOT NULL DEFAULT '0',
  `gamename` varchar(45) CHARACTER SET ascii COLLATE ascii_general_ci NOT NULL DEFAULT '',
  `hostname` varchar(45) CHARACTER SET ascii COLLATE ascii_general_ci NOT NULL DEFAULT '',
  `hostport` smallint unsigned NOT NULL DEFAULT '3658',
  `gamever` varchar(20) CHARACTER SET ascii COLLATE ascii_general_ci NOT NULL DEFAULT 'V1.31a',
  `cl` varchar(20) CHARACTER SET ascii COLLATE ascii_general_ci NOT NULL DEFAULT '',
  `rv` varchar(20) CHARACTER SET ascii COLLATE ascii_general_ci NOT NULL DEFAULT '',
  `map` varchar(20) CHARACTER SET ascii COLLATE ascii_general_ci NOT NULL DEFAULT '',
  `mc` tinyint unsigned NOT NULL DEFAULT '0',
  `mapname` tinyint unsigned NOT NULL DEFAULT '0',
  `gc` tinyint unsigned NOT NULL DEFAULT '0',
  `gametype` varchar(20) CHARACTER SET ascii COLLATE ascii_general_ci NOT NULL DEFAULT '',
  `gamevariant` varchar(20) CHARACTER SET ascii COLLATE ascii_general_ci NOT NULL DEFAULT '',
  `numplayers` tinyint unsigned NOT NULL DEFAULT '0',
  `maxplayers` tinyint unsigned NOT NULL DEFAULT '0',
  `numteams` tinyint unsigned NOT NULL DEFAULT '0',
  `gamemode` varchar(15) CHARACTER SET ascii COLLATE ascii_general_ci NOT NULL DEFAULT '',
  `teamplay` tinyint unsigned NOT NULL DEFAULT '0',
  `fraglimit` tinyint unsigned NOT NULL DEFAULT '0',
  `teamfraglimit` tinyint unsigned NOT NULL DEFAULT '0',
  `timelimit` smallint unsigned NOT NULL DEFAULT '0',
  `timeelapsed` smallint unsigned NOT NULL DEFAULT '0',
  `password` tinyint unsigned NOT NULL DEFAULT '0',
  `nr` tinyint unsigned NOT NULL DEFAULT '0',
  `xr` tinyint unsigned NOT NULL DEFAULT '0',
  `ff` tinyint unsigned NOT NULL DEFAULT '0',
  `sr` tinyint unsigned NOT NULL DEFAULT '0',
  `rc` tinyint unsigned NOT NULL DEFAULT '0',
  `ni` bigint NOT NULL DEFAULT '0',
  `xi` bigint NOT NULL DEFAULT '0',
  `qm` tinyint unsigned NOT NULL DEFAULT '0',
  `region` bigint unsigned NOT NULL DEFAULT '1',
  `c0` int NOT NULL DEFAULT '0',
  `c1` int NOT NULL DEFAULT '0',
  `n0` varchar(32) CHARACTER SET ascii COLLATE ascii_general_ci NOT NULL DEFAULT '',
  `n1` varchar(32) CHARACTER SET ascii COLLATE ascii_general_ci NOT NULL DEFAULT '',
  `c0c` tinyint unsigned NOT NULL DEFAULT '0',
  `c1c` tinyint unsigned NOT NULL DEFAULT '0',
  `team0` varchar(5) CHARACTER SET ascii COLLATE ascii_general_ci NOT NULL DEFAULT '',
  `team1` varchar(5) CHARACTER SET ascii COLLATE ascii_general_ci NOT NULL DEFAULT '',
  `score0` smallint NOT NULL DEFAULT '0',
  `score1` smallint NOT NULL DEFAULT '0',
  `updated_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `verified` tinyint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`),
  UNIQUE KEY `ip_port_UNIQUE` (`ip`,`port`)
) ENGINE=InnoDB AUTO_INCREMENT=634 DEFAULT CHARSET=ascii;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2024-06-08 11:56:31
