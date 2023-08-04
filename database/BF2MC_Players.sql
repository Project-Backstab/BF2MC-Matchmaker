-- MySQL dump 10.13  Distrib 8.0.33, for Win64 (x86_64)
--
-- Host: 128.140.0.23    Database: BF2MC
-- ------------------------------------------------------
-- Server version	8.0.33-0ubuntu0.22.04.4

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
-- Table structure for table `Players`
--

DROP TABLE IF EXISTS `Players`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `Players` (
  `profileid` int NOT NULL AUTO_INCREMENT,
  `userid` int NOT NULL,
  `nick` varchar(41) DEFAULT NULL,
  `uniquenick` varchar(32) DEFAULT NULL,
  `email` varchar(50) DEFAULT NULL,
  `password` varchar(32) DEFAULT NULL,
  `last_login` datetime DEFAULT NULL,
  `last_login_ip` varchar(15) DEFAULT NULL,
  `created_at` datetime DEFAULT NULL,
  PRIMARY KEY (`profileid`),
  UNIQUE KEY `id_UNIQUE` (`profileid`)
) ENGINE=InnoDB AUTO_INCREMENT=10037099 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Players`
--

LOCK TABLES `Players` WRITE;
/*!40000 ALTER TABLE `Players` DISABLE KEYS */;
INSERT INTO `Players` VALUES (10036819,64679,'IamLupo@6507BAD7','IamLupo','help0001@gmail.com','827ccb0eea8a706c4c34a16891f84e7b','2023-08-04 19:08:26','86.87.139.235',NULL),(10037049,64679,'IamLupo3@3BEAA015','IamLupo3','help0001@gmail.com','827ccb0eea8a706c4c34a16891f84e7b','2023-07-23 22:58:05','86.87.139.235',NULL),(10037061,64682,'noahallen522@3CA95DEC','noahallen522','fortnite','900150983cd24fb0d6963f7d28e17f72',NULL,NULL,NULL),(10037062,64683,'Shika@9AE43FAE','Shika','lol@batman.com','417e4705aee1415f8583243b8c403af3',NULL,NULL,NULL),(10037063,64684,'NoBackstab@ADDEDBD6','NoBackstab','aaaaaaaaa','6b60bbc4e0db240ad3a6a82d9c2f7fc6','2023-07-03 23:26:56','73.41.248.54',NULL),(10037064,64685,'G3RM4NY_SOLDI3R@B44746DF','G3RM4NY_SOLDI3R','gabriel.trindade@.com','aa37fc28085ffab741119a63804093ae',NULL,NULL,NULL),(10037065,64686,'7DV-1NS4N3-_@1F5D3D36','7DV-1NS4N3-_','edu@.com','6481190fe9cbe79ae62666a0daf92036',NULL,NULL,NULL),(10037066,64687,'__SiNiSTe]Z__@51E49C39','__SiNiSTe]Z__','eduardomarques97@gmail.com','0689414493c2e2e82f80c9e62fbc61f3',NULL,NULL,NULL),(10037067,64688,'aaaaa@A3A74090','aaaaa','aaaaa','827ccb0eea8a706c4c34a16891f84e7b','2023-07-24 18:25:12','86.87.139.235',NULL),(10037068,64689,'bbbbb@C7D840A1','bbbbb','bbbbb','827ccb0eea8a706c4c34a16891f84e7b',NULL,NULL,NULL),(10037069,64690,'ccccc@A3A540AF','ccccc','ccccc','827ccb0eea8a706c4c34a16891f84e7b',NULL,NULL,NULL),(10037070,64691,'ddddd@156640DD','ddddd','ddddd','827ccb0eea8a706c4c34a16891f84e7b',NULL,NULL,NULL),(10037071,64692,'eXiLe@EF3F40CC','eXiLe','aaabcd','ea26b0075d29530c636d6791bb5d73f4',NULL,NULL,NULL),(10037073,64694,'eeeee@C3A340CA','eeeee','eeeee','827ccb0eea8a706c4c34a16891f84e7b','2023-06-08 05:55:42','10.10.10.110','2023-06-08 05:30:51'),(10037074,64695,'Omega@F4433F7F','Omega','bremner360@gmail.com','41e1be2beae2a7965807b33d6aacc5f5','2023-07-27 22:45:12','147.148.215.218','2023-06-08 11:14:55'),(10037078,64699,'ababb@D2DC4092','ababb','ababb','1be4fd99f3b7b2702744f3a803248f6d','2023-07-17 23:49:53','147.148.215.218','2023-06-21 21:40:21'),(10037080,64693,'Gee[Test]@7597002A','Gee[Test]','Gee','554d5f3a7d066f90384a5ff31e5659b5','2023-08-04 19:09:24','75.244.90.196','2023-06-26 04:38:58'),(10037081,64700,'^_^@E1BBEDF3','^_^','^_^','cad20cd54745dfb5f822238054777cef','2023-07-31 20:29:35','75.244.90.196','2023-06-26 16:23:44'),(10037086,64704,'Gee@7DE7EDF3','Gee','geeuhteen@gmail.com','827ccb0eea8a706c4c34a16891f84e7b','2023-07-02 03:46:14','75.192.108.79','2023-07-01 16:31:01'),(10037091,64706,'username','username@email@mail.com','email@mail.com','5f4dcc3b5aa765d61d8327deb882cf99','2023-07-02 21:52:58','86.160.99.212','2023-07-02 21:11:24'),(10037093,64707,'test-clan@F7DBA035','test-clan','aaaaaaaaaa','96e79218965eb72c92a549dd5a330112','2023-08-04 13:08:08','147.148.215.218','2023-07-03 22:57:44'),(10037094,64708,'Tester@E85FBBBE','Tester','tester','cad20cd54745dfb5f822238054777cef','2023-08-04 16:15:29','75.244.90.196','2023-07-05 02:52:33'),(10037095,64709,'test-clan2@03B76689','test-clan2','abababaaAa','e807f1fcf82d132f9bb018ca6738a19f','2023-08-04 13:18:07','147.148.215.218','2023-08-03 19:56:32'),(10037096,64710,'cenorexia@CC9E9180','cenorexia','ceno@krankikom.de','447cee932c22931a3f5f9a902acaff6c','2023-08-04 07:52:48','82.83.227.36','2023-08-04 07:52:31'),(10037097,64711,'111111@3ED88426','111111','111111','96e79218965eb72c92a549dd5a330112',NULL,NULL,'2023-08-04 16:10:19'),(10037098,64712,'JP_Test@5AD3C9DA','JP_Test','aaaaaaaaaaaaaakkkk','8ce87b8ec346ff4c80635f667d1592ae','2023-08-04 19:06:35','147.148.215.218','2023-08-04 19:06:26');
/*!40000 ALTER TABLE `Players` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2023-08-04 21:13:31
