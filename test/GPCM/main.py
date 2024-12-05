import socket
import hashlib
import time
import socket
import threading

GPCM_SERVER_HOST = '157.230.90.177' ## OpenSpy
#GPCM_SERVER_HOST = '128.140.0.23' ## BF2MC Backend
#GPCM_SERVER_HOST = '127.0.0.1'
GPCM_SERVER_PORT = 29900

LOGIN_UNIQUENICK = "IamLupo"
LOGIN_PASSWORD = "12345"

def md5(input_string):
	# Create an MD5 hash object
	md5_hash = hashlib.md5()

	# Update the hash object with the bytes of your input string
	md5_hash.update(input_string.encode('utf-8'))
	
	return md5_hash.hexdigest()

def GameSpyDecodeActions(actions):
	gs_actions = []
	current_list = []

	for action in actions:
		if(len(current_list) == 0 and action == ''):
			continue
		else:
			current_list.append(action)
		
		if action == 'final':
			if current_list:
				gs_actions.append(current_list)
			current_list = []	
	
	# Append the last sublist if it's not empty
	if current_list:
		gs_actions.append(current_list)

	return gs_actions

def GameSpyDecode(data):
	return data.split("\\")[1:-1]

def GameSpyEncode(data):
	return "\\" + "\\".join(data) + "\\"

def GameSpyLoginResponse(password, uniquenick, client_challenge, server_challenge):
	password_hash = md5(password)
	space = "                                                "
	return md5(password_hash + space + uniquenick + client_challenge + server_challenge + password_hash)

def GameSpyLoginProof(password, uniquenick, client_challenge, server_challenge):
	password_hash = md5(password)
	space = "                                                "
	return md5(password_hash + space + uniquenick + client_challenge + server_challenge + password_hash)

## Request
def requestLc(client_socket, gs_request):
	## On recieving challenge
	if(gs_request[1] == "1"):
		server_challenge = gs_request[3]
		client_challenge = "otMLwtUZyLCV85ERGzg5mVER8nknWX0B";
		
		print(" [*] Recieve server challenge: \"{}\"".format(server_challenge))
		
		# Send data to the server
		gs_response = [
			"login",       "",
			"challenge",   client_challenge,
			"uniquenick",  LOGIN_UNIQUENICK,
			"response",    GameSpyLoginResponse(LOGIN_PASSWORD, LOGIN_UNIQUENICK, client_challenge, server_challenge),
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
	## On successful login
	elif gs_request[1] == "2":
		gs_actions = [
			## Unknown
			"inviteto",  "",
			"sesskey",   "1",
			"products",  "10307",
			"final", "",
			
			## Get own profile information
			"getprofile",  "",
			"sesskey",   "1",
			"profileid",  gs_request[7],
			"id", "2",
			"final", "",
			
			## Update status
			"status",     "1",
			"sesskey",    "1",
			"statstring", "Resting",
			"locstring",  "bfield1942ps2:/",
			"final",
		]
		
		client_socket.sendall(GameSpyEncode(gs_actions).encode("ascii"))
		
		for gs_response in GameSpyDecodeActions(gs_actions):
			print("Client says: {}".format(gs_response))

def requestBdy(client_socket, gs_request):
	pass

def requestPi(client_socket, gs_request):
	pass

def requestKa(client_socket, gs_request):
	pass

def requestBm(client_socket, gs_request):
	pass

def requestUnknown(client_socket, gs_request):
    print("An unknown command has found: \"{}\"".format(gs_request[0]))

requestList = {
	"lc": requestLc,
	"bdy": requestBdy,
	"pi": requestPi,
	"ka": requestKa,
	"bm": requestBm
}

def listen(client_socket):
	while True:
		request = client_socket.recv(1024).decode("ascii")
		
		if not request:
			break
		
		gs_actions = GameSpyDecode(request)
		gs_requests = GameSpyDecodeActions(gs_actions)
		
		for gs_request in gs_requests:
			print("Server says: {}".format(gs_request))
			
			if(len(gs_request) > 0):
				action = gs_request[0]
			
				func = requestList.get(action, requestUnknown)
				
				if(func):
					func(client_socket, gs_request)

def main():
	# Create a socket object
	client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	
	try:
		# Connect to the server
		client_socket.connect((GPCM_SERVER_HOST, GPCM_SERVER_PORT))
		print("Connected to {}:{}".format(GPCM_SERVER_HOST, GPCM_SERVER_PORT))
		
		# Start a thread for listening
		receive_thread = threading.Thread(target=listen, args=(client_socket,))
		receive_thread.start()
		
		while True:
			pass
		
			"""
			time.sleep(5)
			
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
			"""
		
	except Exception as e:
		print(f"Error: {e}")

	finally:
		# Close the client socket
		client_socket.close()

main()