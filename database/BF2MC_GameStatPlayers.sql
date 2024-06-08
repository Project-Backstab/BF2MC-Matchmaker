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
-- Table structure for table `GameStatPlayers`
--

DROP TABLE IF EXISTS `GameStatPlayers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `GameStatPlayers` (
  `id` int NOT NULL AUTO_INCREMENT,
  `gamestatid` int NOT NULL DEFAULT '-1',
  `auth` varchar(32) CHARACTER SET ascii COLLATE ascii_general_ci NOT NULL DEFAULT '',
  `pid` int NOT NULL DEFAULT '-1',
  `machine_id` varchar(16) CHARACTER SET ascii COLLATE ascii_general_ci NOT NULL DEFAULT '',
  `team` int NOT NULL DEFAULT '-1',
  `score` int NOT NULL DEFAULT '0',
  `rank` int unsigned NOT NULL DEFAULT '1',
  `pph` int unsigned NOT NULL DEFAULT '0',
  `kills` int unsigned NOT NULL DEFAULT '0',
  `deaths` int unsigned NOT NULL DEFAULT '0',
  `suicides` int unsigned NOT NULL DEFAULT '0',
  `time` int unsigned NOT NULL DEFAULT '0',
  `lavd` int unsigned NOT NULL DEFAULT '0',
  `mavd` int unsigned NOT NULL DEFAULT '0',
  `havd` int unsigned NOT NULL DEFAULT '0',
  `hed` int unsigned NOT NULL DEFAULT '0',
  `pld` int unsigned NOT NULL DEFAULT '0',
  `bod` int unsigned NOT NULL DEFAULT '0',
  `k1` int unsigned NOT NULL DEFAULT '0',
  `s1` int unsigned NOT NULL DEFAULT '0',
  `k2` int unsigned NOT NULL DEFAULT '0',
  `s2` int unsigned NOT NULL DEFAULT '0',
  `k3` int unsigned NOT NULL DEFAULT '0',
  `s3` int unsigned NOT NULL DEFAULT '0',
  `k4` int unsigned NOT NULL DEFAULT '0',
  `s4` int unsigned NOT NULL DEFAULT '0',
  `k5` int unsigned NOT NULL DEFAULT '0',
  `s5` int unsigned NOT NULL DEFAULT '0',
  `tk` int unsigned NOT NULL DEFAULT '0',
  `medals` int unsigned NOT NULL DEFAULT '0',
  `ttb` int unsigned NOT NULL DEFAULT '0',
  `mv` int unsigned NOT NULL DEFAULT '0',
  `ngp` int unsigned NOT NULL DEFAULT '0',
  `disable` tinyint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `idGameStatPlayers_UNIQUE` (`id`),
  KEY `gamestatid_idx` (`gamestatid`) /*!80000 INVISIBLE */,
  CONSTRAINT `FK_GameStatPlayer_gamestatid` FOREIGN KEY (`gamestatid`) REFERENCES `GameStats` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=70777 DEFAULT CHARSET=ascii;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2024-06-08 11:56:35
