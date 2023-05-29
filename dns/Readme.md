## Install

On ubuntu 22.04:
```
sudo apt-get update 
sudo apt-get install bind9
sudo apt-get install dnsutils
```

## Configuration

First modify the main configuration file for bind9.
```
sudo nano /etc/bind/named.conf.local
```
Copy over the content from [named.conf.local](named.conf.local)

```
sudo nano /etc/bind/db.dnas.playstation.org.conf
sudo nano /etc/bind/db.easports.com.conf
sudo nano /etc/bind/db.gamespy.com.conf
```
Copy over the content from [db.dnas.playstation.org.conf](db.dnas.playstation.org.conf), [db.easports.com.conf](db.easports.com.conf) and [db.gamespy.com.conf](db.gamespy.com.conf)

## Restart

```
sudo systemctl restart bind9
```

