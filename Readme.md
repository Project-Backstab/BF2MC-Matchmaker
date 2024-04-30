![http://bf2mc.com](images/BF2MC0_LOGO_no_colon.png)

# Battlefield 2: Modern Combat Matchmaker server

This is a project to revive the online matchmaking services. This allows you to create a account and go online again. 
It also has the possebility to save your stats and communicate with other players.

## DNS

To let this server work there are some DNS changes required before it all works.
The following domains needs to be redirected to this server:

	*.dnas.playstation.org
	*.easports.com
	*.gamespy.com

Read [here](dns/Readme.md) more about it how to install your own DNS.


## Clone Project

```
git clone https://github.com/Project-Backstab/BF2MC-Matchmaker.git
cd BF2MC-Matchmaker

git submodule init
git submodule update

sudo apt-get install libmysqlclient-dev doxygen graphviz ccache
```

## Compile project

```
./make_release_build.sh
```
This will create the bf2mc executable and copy the data to the "BF2MC-Matchmaker-release" directory.

For development:
```
cd BF2MC-Matchmaker
mkdir build
cd build

cmake ..
cmake --build . --target doc
make -j 4
```

## Database

This project requires a mysql database to save/extract information.
Read [here](database/Readme.md) to setup. 

## Firewall

```
sudo ufw allow 80/tcp
sudo ufw allow 8080/tcp
sudo ufw allow 27900/udp
sudo ufw allow 28910/tcp
sudo ufw allow 29900/tcp
sudo ufw allow 29901/tcp
sudo ufw allow 29920/tcp
```

For MySQL:
```
sudo ufw allow from <myip> to any port 3306
```

If you want to avoid to run the application as sudo you must Preroute port 80 to 8080
```
sudo iptables -A PREROUTING -t nat -i <network interface> -p tcp --dport 80 -j REDIRECT --to-port 8080
```

## Run

For release:
```
cd BF2MC-Matchmaker-release/
./bin/bf2mc
```

For development:
```
cd BF2MC-Matchmaker/build
./bin/bf2mc
```

For development debugging:
```
cd BF2MC-Matchmaker/build
gdbserver --multi :2345 bin/bf2mc
```

## Services

If you want to run this project as a services you can read more about it over [here](service/Readme.md).