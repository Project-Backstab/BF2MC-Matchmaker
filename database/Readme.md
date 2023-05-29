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

<img src="https://github.com/Project-Backstab/BF2MC-Matchmaker/blob/main/images/mysql/connection.png?raw=true" width="800" />

### Create schema

Once your logged in you can see your server stats. On the left bottom you will click tab Schema. Then right click and create a new Schema.

<img src="https://github.com/Project-Backstab/BF2MC-Matchmaker/blob/main/images/mysql/schema.png?raw=true" width="200" /> - <img src="https://github.com/Project-Backstab/BF2MC-Matchmaker/blob/main/images/mysql/schema_new.png?raw=true" width="200" />

Call it "BF2MC" and click Apply.

Now we go back to administration tab and click "Data import".

<img src="https://github.com/Project-Backstab/BF2MC-Matchmaker/blob/main/images/mysql/Administration-Data_Import.png?raw=true" width="200" />

- Step 1: Select the project database folder.
- Step 2: Click "Load folder Contents"
- Step 3: Select BF2MC database
- Step 4: Make sure all the tables are selected.
- Step 5: Click "Start Import"

<img src="https://github.com/Project-Backstab/BF2MC-Matchmaker/blob/main/images/mysql/Data_Import_steps.png?raw=true" width="800" />

