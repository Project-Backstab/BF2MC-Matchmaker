"""
    
        Reference:
            See functions:
                .text:005AC230      Handles the request and push the request data to function .text:005ABDA0. The returned server status is then be send.  
"""

import socket

SERVER_IP = "86.87.139.235"
SERVER_PORT = 3658

QR_MAGIC = b'\xfe\xfd'
QR_REQUEST_HEARTBEAT = b'\x09'
QR_REQUEST_KEEPALIVE = b'\x0A'

def send(session_token, request_data):
    # Create a UDP socket
    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    try:
        request = QR_MAGIC + QR_REQUEST_HEARTBEAT + session_token + request_data + b'\x00\x00'
        
        # Send data to the server
        udp_socket.sendto(request, (SERVER_IP, SERVER_PORT))

        # Receive response from the server (assuming the response is not more than 1024 bytes)
        response, server_address = udp_socket.recvfrom(1024)

        # Print the response from the server
        print("Response from server:", response[5:-1].decode('ascii'))

    except Exception as e:
        print("Error:", e)

    finally:
        # Close the UDP socket
        udp_socket.close()


def main():
    session_token = b'\xfe\xb9\xc2\x8a'
    
    ## Raw example
    """
    request_data = b''
    send(session_token, request_data)
    """

if __name__ == "__main__":
    main()