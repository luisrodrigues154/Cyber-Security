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
| -x | print headers |
| -d | disassemble |
| -t | symbols |
| -T | dynamic symbols |
| -r | reloc |
| -R | dynamic reloc |
| -p | private headers |

- Get all opcodes (raw): objdump -d ./your_program|grep '[0-9a-f]:'|grep -v 'file'|cut -f2 -d:|cut -f1-6 -d' '|tr -s ' '|tr '\t' ' '|sed 's/ $//g'|paste -d '' -s |sed 's/^/"/'|sed 's/$/"/g'
- Get all opcodes (shellcode format): objdump -d ./your_program|grep '[0-9a-f]:'|grep -v 'file'|cut -f2 -d:|cut -f1-6 -d' '|tr -s ' '|tr '\t' ' '|sed 's/ $//g'|sed 's/ /\\x/g'|paste -d '' -s |sed 's/^/"/'|sed 's/$/"/g'
 
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