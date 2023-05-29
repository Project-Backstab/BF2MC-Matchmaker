# Database

This project requires a Mysql server.

## Install

```
sudo apt install mysql-server
```

## Config

Run mysql secury script to configure to your liking
```
sudo mysql_secure_installation
```

In my case i allow to open up remote connections to admin the server.

## Remote control

I use [Mysql Workbench](https://www.mysql.com/products/workbench/) to remote control the database server.

### Connect

Use fill in your ip, port and username to connect to the database.

<img src="https://github.com/Project-Backstab/BF2MC-Matchmaker/blob/main/images/mysql/connection.png?raw=true" width="450" />

### Create schema

Once your logged in you can see your server stats. On the left bottom you will click tab Schema.

<img src="https://github.com/Project-Backstab/BF2MC-Matchmaker/blob/main/images/mysql/schema.png?raw=true" width="200" />

Then right click and create a new Schema.

<img src="https://github.com/Project-Backstab/BF2MC-Matchmaker/blob/main/images/mysql/schema_new.png?raw=true" width="200" />