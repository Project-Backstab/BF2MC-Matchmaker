![http://bf2mc.com](images/b2mconlinelogo.png)

# Battlefield 2: Modern Combat Matchmaker server

This is a project to revive the online matchmaking services. This allows you to create a account and go online again. 
It also has the possebility to save your stats and communicate with other players.

## DNS

To let this server work there are some DNS changes required before it all works.
The following domains needs to be redirected to this server:

	www.easports.com
	bfmc.gamespy.com
	gpsp.gamespy.com
	gpcm.gamespy.com
	
Read [here](dns/Readme.md) more about about it.


## Clone Project

```
git clone https://github.com/Project-Backstab/BF2MC-Matchmaker.git
cd BF2MC-Matchmaker

git submodule init
git submodule update

sudo apt-get install libmysqlclient-dev
```

## Compile project

```
mkdir build
cd build
cmake ..
make -j 4

cmake --install . --prefix ../../BF2MC-Matchmaker-release
```

This will create the bf2mc executable.

## Database

This project requires a mysql database to save/extract information.
Read [here](database/Readme.md) to setup. 

## Run

```
cd BF2MC-Matchmaker-release/bin
sudo ./bf2mc
```

This will run 4 services on TCP port: 80, 28910, 29900 and 29901.

If you want to avoid to run the application as sudo you must Preroute port 80 to 8080
```
sudo iptables -A PREROUTING -t nat -i <network interface> -p tcp --dport 80 -j REDIRECT --to-port 8080
```
Also in data/settings.json, change "webserver" -> "port" to 8080.