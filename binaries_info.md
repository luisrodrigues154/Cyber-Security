# Binaries information

## Call Conventions (arguments)
- x86 
  - All in the stack
- SystemV (x86-64)
  - rdi, rsi, rdx, rcx, r8, r9, remaining in stack (reversed)


## python/pwntools reminders
- Sending double/float return addresses: ```def get_ready(raw_data): val = p64(raw_data).hex(); return (struct.unpack("d", bytes.fromhex(val))[0]) ``` 
- Get header address: ```elf.get_section_by_name("<header>).header.sh_addr```

## Scanf particularities
- Bypass var modification: send '.', '-', '+'

## Compile c code with disabled protections (gcc)
- canaries: ``` -fno-stack-protector``` 
- NX: ```-z execstack```  
- PIE: ``` -no-pie ``` 
- ALL: ```gcc -fno-stack-protector -no-pie -g -O0 -Wl,-z,norelro,-z,execstack -o out_file code.c```

##  .CTORS (init) and .DTORS (fini)
- .init_array -> Construtors to prepare the execution (align stack, etc)
- .fini_array -> Destructors to clean the execution (handle errors etc)
    - Both can be abused with **NORELRO** by overwriting their address with a **win function**

### Extracting them
```
objdump -s -j <header> elf.bin
```

### Structure
``` 
<func_addr> <func_addr> (...) 00000000 
```

## Libraries loading sequence
1. LD_PRELOAD (and /etc/ld.so.preload)
2. LD_LIBRARY_PATH
3. DT_RUNPATH/DT_RPATH
4. system configuration
5. /lib and /usr/lib
