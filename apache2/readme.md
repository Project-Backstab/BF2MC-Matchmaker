## Install

On ubuntu 22.04:
```
sudo apt-get update 
sudo apt-get install apache2
sudo apt-get install certbot
```

Copy the confige files over:
```
sudo cp apache2/sites-available/bf2mc.com.conf /etc/apache2/sites-available/
sudo cp apache2/sites-available/backend.bf2mc.com.conf /etc/apache2/sites-available/
sudo cp apache2/sites-available/bfmc.gamespy.com.conf /etc/apache2/sites-available/
sudo cp apache2/sites-available/easports.com.conf /etc/apache2/sites-available/
```

Enable all configs:
```
cd /etc/apache2/sites-available

sudo a2ensite bf2mc.com.conf
sudo a2ensite backend.bf2mc.com.conf
sudo a2ensite bfmc.gamespy.com.conf
sudo a2ensite easports.com.conf
```

Enable all modules in apache:
```
sudo a2enmod ssl
sudo a2enmod proxy
sudo a2enmod proxy_http
sudo a2enmod rewrite
```

Get HTTPS certificates
```
sudo systemctl stop apache2
sudo certbot certonly --standalone -d bf2mc.com
sudo certbot certonly --standalone -d backend.bf2mc.com
sudo systemctl start apache2
```
