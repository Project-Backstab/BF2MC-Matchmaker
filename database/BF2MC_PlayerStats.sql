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
-- Table structure for table `PlayerStats`
--

DROP TABLE IF EXISTS `PlayerStats`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `PlayerStats` (
  `profileid` int NOT NULL,
  `score` int NOT NULL DEFAULT '0',
  `ran` int unsigned NOT NULL DEFAULT '1',
  `pph` int unsigned NOT NULL DEFAULT '0',
  `kills` int unsigned NOT NULL DEFAULT '0',
  `deaths` int unsigned NOT NULL DEFAULT '0',
  `suicides` int unsigned NOT NULL DEFAULT '0',
  `time` int unsigned NOT NULL DEFAULT '0',
  `vehicles` int unsigned NOT NULL DEFAULT '0',
  `lavd` int unsigned NOT NULL DEFAULT '0',
  `mavd` int unsigned NOT NULL DEFAULT '0',
  `havd` int unsigned NOT NULL DEFAULT '0',
  `hed` int unsigned NOT NULL DEFAULT '0',
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
  `medals` int unsigned NOT NULL DEFAULT '0',
  `ttb` int unsigned NOT NULL DEFAULT '0',
  `mv` int unsigned NOT NULL DEFAULT '0',
  `ngp` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`profileid`),
  UNIQUE KEY `profileid_UNIQUE` (`profileid`),
  CONSTRAINT `FK_PlayerStats_profileid` FOREIGN KEY (`profileid`) REFERENCES `Players` (`profileid`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2023-10-15 15:30:57
