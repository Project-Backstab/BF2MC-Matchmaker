rem "pass" is the default password that must be used by the clients to login

@echo off
start gs_login_server -p pass 29900
start gs_login_server -p pass 29901
start gs_login_server -p pass 29920
