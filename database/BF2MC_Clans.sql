-- MySQL dump 10.13  Distrib 8.0.33, for Win64 (x86_64)
--
-- Host: 10.10.10.109    Database: BF2MC
-- ------------------------------------------------------
-- Server version	8.0.33-0ubuntu0.22.04.2

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
-- Table structure for table `Clans`
--

DROP TABLE IF EXISTS `Clans`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Clans` (
  `clanid` int NOT NULL AUTO_INCREMENT,
  `name` varchar(32) DEFAULT NULL,
  `tag` varchar(3) DEFAULT NULL,
  `homepage` varchar(256) DEFAULT NULL,
  `info` varchar(1024) DEFAULT NULL,
  `region` int DEFAULT NULL,
  `date` varchar(15) DEFAULT '',
  `rating` int DEFAULT '0',
  `wins` int DEFAULT '0',
  `losses` int DEFAULT '0',
  `draws` int DEFAULT '0',
  `membercount` int DEFAULT '0',
  `created_at` datetime DEFAULT NULL,
  PRIMARY KEY (`clanid`),
  UNIQUE KEY `clanid_UNIQUE` (`clanid`)
) ENGINE=InnoDB AUTO_INCREMENT=23 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Clans`
--

LOCK TABLES `Clans` WRITE;
/*!40000 ALTER TABLE `Clans` DISABLE KEYS */;
INSERT INTO `Clans` VALUES (19,'aaa','aaa','aaa','aaa',1,'',0,0,0,0,0,NULL),(20,'Noah%5fGaming%5fClan','NGC','We+play+games','We+made+the+clan',1,'',0,0,0,0,0,NULL),(21,'aaaaaaaaa','aap','aaaa','aaaa',1,'',0,0,0,0,0,NULL),(22,'lllll','lll','aaaa','aaaa',1,'',0,0,0,0,0,'2023-06-08 05:56:04');
/*!40000 ALTER TABLE `Clans` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2023-06-08  8:02:44
