import socket

# Define the server's address (use the IP address or hostname of the server)
server_address = ("10.10.10.113", 8080)

# Create a socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect to the server
client_socket.connect(server_address)

print(f"Connected to {server_address}")

while True:
	# Receive data from the server
	data = client_socket.recv(1024)

	if not data:
		# No more data from the server, break the loop
		break

	# Process the received data (replace this with your own logic)
	print(f"Received data: {data.decode('utf-8')}")

# Close the connection
client_socket.close()

