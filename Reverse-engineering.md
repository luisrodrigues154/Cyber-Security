# Reverse engineering cheat sheet

## Radare2
| cmd | action |
| :-: | :-:|
| [x]?  | help for command x | 
| a     | analyze (has more types) |
| afl   | list all functions |
| pdf @function  | disassemble function |
| e asm.syntax=X | change assembly syntax for X[att, intel] | 
| db [addr] | set breakpoint at addr | 
| dc | run children | 
| dr | print registers |
| px @rbp-0x4 | print hexdump of rbp-0x4 |
| ds | move to next instruction |
| afv | displays local variables values (and stack position)|
| dm | show memory maps |

<br>

## Objdump 

| cmd | action |
| :-: | :-: |
| -h | print section headers |
| -x | print all headers |
| -d | disassemble |
| -t | symbols |
| -T | dynamic symbols |
| -r | reloc |
| -R | dynamic reloc |
| -p | private headers |


<br>

## UPX
Binary packer/unpacker <br>

| cmd | action | 
| :-: | :-: |
| -d | decompress (unpack) |
| -o | output file |

<br>

## DIE (detect it easy)
Used to check file composition (if it is and how it is packed ) <br>

<br>

## Python 
- Conversions 
  - Int(binary, base)  -> int("111", 2) = 7 
  - Hex(int_value) 
- Encode/decode 
  - String.encode(mode) 
  - String.decode(mode)   
- Struct 
  - Struct.unpack(format, bytes) -> convert bytes to value 
  - Struct.pack(format, value) -> convert value to bytes 
- MODES: 
  - <  - little endian 
  - \>  - bing-endian  
  - ! - network (big-endian)
- Print (payloads): sys.stdout.buffer.write()

<br>

## Python compiled binaries 

- pyi_archive_viewer.py: Can List and extract python installers
- pyinstxtractor: extracts all the files within the installer
- uncompyle6/decompyle3: decompiles .pyc to .py

### Notes
1. This is a mess because the decompilers only allow until python 3.8 (the default installation is now 3.9)
2. The .pyc's might have broken magic numbers
   - Magic number for version: ```python -c "import importlib.util; from binascii import hexlify, unhexlify; print(hexlify(importlib.util.MAGIC_NUMBER))" ```
   - Edit with hexeditor (the ouput is a direct map, not in LE)

<br>

## IDA debugging remote linux server

1. Ensure that networking is enabled on the guest system and that it can communicate via TCP/IP with the host system.
2. Copy <IDA installation directory>\dbgsrv\linux_serverx64 to your host system and run it.
3. Copy the target ELF binary to the guest system and load it into IDA (disassemble it).
4. In IDA (on the guest system), go to Debugger → Select debugger... in the menu bar and choose Remote Linux debugger.
5. In IDA (on the guest system), go to Debugger → Process options... in the menu bar and specify the hostname or IP of your host system, the debugging port used by linux_serverx64, and the debugging password (if you specified one when running linux_serverx64).
6. In IDA (on the guest system), select Debugger → Start process in the menu bar (or Attach to process... if the target is already running on the host system).

## Anti-debugging 

### Methods (ELFs)
1. Strip (dynamic linked)
2. Self modifying code
3. Static compiled + stripped
4. Packing 
5. Dynamic loading (dlopen + dlsym)
6. Syscalls 
7. Sections permissions (.text)
   
### Detection
1. .dynsym table 
2. Tools and debug untill syscall call
3. (Near) empty import table
4. If public packer, use DIE (binary entropy would discolse it)
5. Strace/ltrace
6. Import tables (+ point 5)
7. Strace/ltrace 
   
#### ptrace_traps through constructors
- The program defines a constructor that ptraces itself, if the operation fails the program exits (other program already ptracing) 

