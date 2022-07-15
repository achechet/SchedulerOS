import socket
import binascii

UDP_IP = "192.168.1.22"
UDP_PORT = 5270
 
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))
     
while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    print (binascii.hexlify(bytearray(data)))