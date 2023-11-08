"""
    QR status:
        Request:
            0020                                 fe fd 00 fe b9 c2             ......
            0030   8a 14 01 06 03 04 10 0f 05 08 0a 0c 1c 18 33 34   ..............34
            0040   35 36 37 38 39 3a 00 00                           56789:..

            Explain values:
                fe fd                    = QR magic code
                00                       = request action
                fe b9 c2 8a              = session token
                14                       = total request's
                01 06 03 04 10 0f 05 08  = Request id's
                0a 0c 1c 18 33 34 35 36
                37 38 39 3a

        Response:
            0020                                 00 fe b9 c2 8a 5b             .....[
            0030   43 51 5d 42 46 32 4d 43 2d 49 61 6d 4c 75 70 6f   CQ]BF2MC-IamLupo
            0040   00 63 6f 6e 71 75 65 73 74 00 56 31 2e 33 31 61   .conquest.V1.31a
            0050   00 33 36 35 38 00 31 32 30 30 00 30 00 32 00 30   .3658.1200.0.2.0
            0060   00 32 34 00 30 00 00 00 2d 31 00 2d 31 00 00 00   .24.0...-1.-1...
            0070   2d 31 31 36 33 30 30 35 39 33 39 00 2d 31 31 36   -1163005939.-116
            0080   33 30 30 35 39 33 39 00 31 00 32 30 00            3005939.1.20.

            Explaine values:
                00                   = reponse action
                fe b9 c2 8a          = session token
                Rest is the requested id's results.

    Reference:
        See functions:
            .text:005AC230      Handles the request and push the request data to function .text:005ABDA0. The returned server status is then be send.
            .text:005ABDA0      Does do some checking on the requested id's and pushes it to function .text:005ABD60.
            .text:005ABD60      
"""

import socket
from enum import Enum

SERVER_IP = "86.87.139.235"
SERVER_PORT = 3658

QR_MAGIC = b'\xfe\xfd'
QR_REQUEST_STATUS = b'\x00'

# Define an enumeration class
class RequestActions(Enum):
    hostname = 1
    gamever = 3
    localport = 4
    mapname = 5
    gametype = 6
    gamevariant = 7
    maxplayers = 10
    gamemode = 11
    timelimit = 16
    xr = 58
    rv = 61
    ni = 65
    xi = 66
    qm = 67
    cl = 68
    region = 69
    map = 70

def send2(session_token, request_actions):
    request_data_len = int.to_bytes(len(request_actions), length=1, byteorder='big')
    request_data = bytes(action.value for action in request_actions)

    send(session_token, request_data_len, request_data)

def send(session_token, request_data_len, request_data):
    # Create a UDP socket
    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    try:
        request = QR_MAGIC + QR_REQUEST_STATUS + session_token + request_data_len + request_data + b'\x00\x00'
        
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
    request_data_len = b'\x14'
    request_data = b'\x01\x06\x03\x04\x10\x0f\x05\x08\x0a\x0c\x1c\x18\x33\x34\x35\x36\x37\x38\x39\x3a'
    send(session_token, request_data_len, request_data)
    """

    ## Raw Request serer name
    """
    request_data_len = b'\x01'
    request_data = b'\x3a'
    send(session_token, request_data_len, request_data)
    """

    ## Example: multiple request
    request_actions = [
        RequestActions.hostname,
        RequestActions.mapname,
        RequestActions.gametype,
        RequestActions.maxplayers,
    ]
    send2(session_token, request_actions)
    
    ## Example: Bruteforce all request actions
    """
    for i in range(71):
        request_data_len = b'\01'
        request_data = int.to_bytes(i, length=1, byteorder='big')
        send(session_token, request_data_len, request_data)
    """

if __name__ == "__main__":
    main()