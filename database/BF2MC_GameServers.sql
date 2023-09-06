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
-- Table structure for table `GameServers`
--

DROP TABLE IF EXISTS `GameServers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `GameServers` (
  `id` int NOT NULL AUTO_INCREMENT,
  `ip` varchar(15) DEFAULT NULL,
  `port` smallint unsigned DEFAULT '3658',
  `flag` tinyint unsigned DEFAULT '59',
  `localip0` varchar(15) DEFAULT NULL,
  `localport` smallint unsigned DEFAULT NULL,
  `natneg` tinyint unsigned DEFAULT NULL,
  `gamename` varchar(45) DEFAULT NULL,
  `hostname` varchar(45) DEFAULT NULL,
  `hostport` smallint unsigned DEFAULT NULL,
  `gamever` varchar(20) DEFAULT NULL,
  `cl` varchar(20) DEFAULT NULL,
  `rv` varchar(20) DEFAULT NULL,
  `map` varchar(20) DEFAULT NULL,
  `mc` tinyint unsigned DEFAULT NULL,
  `mapname` tinyint unsigned DEFAULT NULL,
  `gc` tinyint unsigned DEFAULT NULL,
  `gametype` varchar(20) DEFAULT NULL,
  `gamevariant` varchar(20) DEFAULT NULL,
  `numplayers` tinyint unsigned DEFAULT NULL,
  `maxplayers` tinyint unsigned DEFAULT NULL,
  `numteams` tinyint unsigned DEFAULT NULL,
  `gamemode` varchar(15) DEFAULT NULL,
  `teamplay` tinyint unsigned DEFAULT NULL,
  `fraglimit` tinyint unsigned DEFAULT NULL,
  `teamfraglimit` tinyint unsigned DEFAULT NULL,
  `timelimit` smallint unsigned DEFAULT NULL,
  `timeelapsed` smallint unsigned DEFAULT NULL,
  `password` tinyint unsigned DEFAULT NULL,
  `nr` tinyint unsigned DEFAULT NULL,
  `xr` tinyint unsigned DEFAULT NULL,
  `ff` tinyint unsigned DEFAULT NULL,
  `sr` tinyint unsigned DEFAULT NULL,
  `rc` tinyint unsigned DEFAULT NULL,
  `ni` bigint DEFAULT NULL,
  `xi` bigint DEFAULT NULL,
  `qm` tinyint unsigned DEFAULT NULL,
  `region` tinyint unsigned DEFAULT NULL,
  `c0` int DEFAULT NULL,
  `c1` int DEFAULT NULL,
  `n0` varchar(20) DEFAULT NULL,
  `n1` varchar(20) DEFAULT NULL,
  `c0c` tinyint unsigned DEFAULT NULL,
  `c1c` tinyint unsigned DEFAULT NULL,
  `team0` varchar(5) DEFAULT NULL,
  `team1` varchar(5) DEFAULT NULL,
  `score0` smallint DEFAULT NULL,
  `score1` smallint DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`),
  UNIQUE KEY `ip_port_UNIQUE` (`ip`,`port`)
) ENGINE=InnoDB AUTO_INCREMENT=3339 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `GameServers`
--

LOCK TABLES `GameServers` WRITE;
/*!40000 ALTER TABLE `GameServers` DISABLE KEYS */;
INSERT INTO `GameServers` VALUES (1,'168.119.189.149',3658,59,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(2,'78.47.184.23',3658,59,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),(32,'168.119.189.149',3659,59,'168.119.189.149',3659,1,'bfield1942ps2','test-server',3659,'V1.31a','100.216335.alpha','retail','thenest',1,12,0,'conquest','Border',0,6,2,'openplaying',0,0,0,1200,0,0,1,20,1,1,0,-2147483648,2147483647,0,0,-1,-1,'','',0,0,'AC','US',450,450),(2865,'86.87.139.235',3658,59,'10.10.10.110',3658,1,'bfield1942ps2','[CQ]BF2MC-IamLupo',3658,'V1.31a','100.216335.alpha','retail','bridgetoofar',1,2,0,'conquest','Border',1,24,2,'openplaying',0,0,0,1200,60,0,1,20,1,1,1,-2147483648,2147483647,0,0,-1,-1,'','',0,0,'CH','US',450,450);
/*!40000 ALTER TABLE `GameServers` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2023-09-06 22:50:03
