from scapy.all import IP, UDP, Raw, send
import socket

p = IP(dst="192.168.0.123")/UDP(dport=1337)/Raw(load="hello from UDP\r\n")
r = send(p)