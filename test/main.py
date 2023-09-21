import socket
import hashlib
import time

#GPCM_SERVER_HOST = '64.225.4.169' ## OpenSpy
#GPCM_SERVER_HOST = '128.140.0.23' ## BF2MC Backend
GPCM_SERVER_HOST = '127.0.0.1'
GPCM_SERVER_PORT = 29900

def md5(input_string):
	# Create an MD5 hash object
	md5_hash = hashlib.md5()

	# Update the hash object with the bytes of your input string
	md5_hash.update(input_string.encode('utf-8'))
	
	return md5_hash.hexdigest()

def GameSpyDecode(data):
	return data.split("\\")[1:-1]

def GameSpyEncode(data):
	return "\\" + "\\".join(data) + "\\"

def GameSpyLoginResponse(password, uniquenick, client_challenge, server_challenge):
	password_hash = md5(password)
	space = "                                                "
	return md5(password_hash + space + uniquenick + client_challenge + server_challenge + password_hash)

def main():
	# Create a socket object
	client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	
	## Settings
	password = "12345"
	uniquenick = "IamLupo3"
	client_challenge = "otMLwtUZyLCV85ERGzg5mVER8nknWX0B";
	
	try:
		# Connect to the server
		client_socket.connect((GPCM_SERVER_HOST, GPCM_SERVER_PORT))
		print("Connected to {}:{}".format(GPCM_SERVER_HOST, GPCM_SERVER_PORT))

		# Receive the server's request
		request = client_socket.recv(1024).decode("ascii")
		gs_request = GameSpyDecode(request)
		server_challenge = gs_request[3]
		
		print("Server says: {}".format(gs_request))
		print(" [*] Recieve server challenge: \"{}\"".format(gs_request[3]))
		
		# Send data to the server
		gs_response = [
			"login",       "",
			"challenge",   "otMLwtUZyLCV85ERGzg5mVER8nknWX0B",
			"uniquenick",  uniquenick,
			"response",    GameSpyLoginResponse(password, uniquenick, client_challenge, server_challenge),
			"firewall",    "1",
			"port",        "0",
			"productid",   "10307",
			"gamename",    "bfield1942ps2",
			"namespaceid", "13",
			"id",          "1",
			"final"
		]
		client_socket.sendall(GameSpyEncode(gs_response).encode("ascii"))
		print("Client says: {}".format(gs_response))
		
		# Receive the server's request
		request = client_socket.recv(1024).decode("ascii")
		gs_request = GameSpyDecode(request)
		print("Server says: {}".format(gs_request))
		
		"""
		gs_response = [
			"inviteto",  "",
			"sesskey",   "1",
			"products",  "10307",
			"final"
		]
		client_socket.sendall(GameSpyEncode(gs_response).encode("ascii"))
		print("Client says: {}".format(gs_response))
		
		# Receive the server's request
		request = client_socket.recv(1024).decode("ascii")
		gs_request = GameSpyDecode(request)
		print("Server says: {}".format(gs_request))
		
		gs_response = [
			"getprofile", "",
			"sesskey",    "1",
			"profileid",  "10037061",
			"id",         "1",
			"final"
		]
		client_socket.sendall(GameSpyEncode(gs_response).encode("ascii"))
		print("Client says: {}".format(gs_response))
		
		# Receive the server's request
		request = client_socket.recv(1024).decode("ascii")
		gs_request = GameSpyDecode(request)
		print("Server says: {}".format(gs_request))
		"""
		
		for i in range(1000):
			# Send data to the server
			gs_response = [
				"bm",        "1",
				"sesskey",   "1",
				"t",         "10036819",
				"msg",       "WTF???!!??!?!!?",
				"final"
			]
			client_socket.sendall(GameSpyEncode(gs_response).encode("ascii"))
			print("Client says: {}".format(gs_response))
		
			time.sleep(0.1)
		
		
	except Exception as e:
		print(f"Error: {e}")

	finally:
		# Close the client socket
		client_socket.close()

main()