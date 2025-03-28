-- MySQL dump 10.13  Distrib 8.0.33, for Win64 (x86_64)
--
-- Host: 128.140.0.23    Database: BF2MC
-- ------------------------------------------------------
-- Server version	8.0.37-0ubuntu0.22.04.3

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
-- Table structure for table `GameStats`
--

DROP TABLE IF EXISTS `GameStats`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `GameStats` (
  `id` int NOT NULL AUTO_INCREMENT,
  `gametype` tinyint unsigned NOT NULL DEFAULT '0',
  `gamver` varchar(20) CHARACTER SET ascii COLLATE ascii_general_ci NOT NULL DEFAULT '',
  `hostname` varchar(45) CHARACTER SET ascii COLLATE ascii_general_ci NOT NULL DEFAULT '',
  `mapid` tinyint unsigned NOT NULL DEFAULT '0',
  `numplayers` tinyint unsigned NOT NULL DEFAULT '0',
  `pplayers` tinyint unsigned NOT NULL DEFAULT '0',
  `tplayed` smallint unsigned NOT NULL DEFAULT '0',
  `clanid_t0` int unsigned NOT NULL DEFAULT '0',
  `clanid_t1` int unsigned NOT NULL DEFAULT '0',
  `country_t0` tinyint unsigned NOT NULL DEFAULT '0',
  `country_t1` tinyint unsigned NOT NULL DEFAULT '0',
  `victory_t0` tinyint unsigned NOT NULL DEFAULT '0',
  `victory_t1` tinyint unsigned NOT NULL DEFAULT '0',
  `score0` smallint NOT NULL DEFAULT '0',
  `score1` smallint NOT NULL DEFAULT '0',
  `created_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `disable` tinyint unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=34275 DEFAULT CHARSET=ascii;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2024-06-30  7:11:28
