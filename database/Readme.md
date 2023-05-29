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

![../images/mysql/connection.png](../images/mysql/connection.png | width=200)

<img src="https://github.com/Project-Backstab/BF2MC-Matchmaker/blob/main/images/mysql/schema_new.png?raw=true" alt="" data-canonical-src="https://gyazo.com/eb5c5741b6a9a16c692170a41a49c858.png" width="200" height="400" />

### Create schema

Once your logged in you can see your server stats. On the left bottom you will click tab Schema.

![../images/mysql/schema.png](../images/mysql/schema.png)

Then right click and create a new Schema.

![../images/mysql/schema_new.png](../images/mysql/schema_new.png | width=200)

