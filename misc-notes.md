# Notes regarding tools/methods/etc


## Nmap
- syntax: namp [options] ip
### options
- -sV	Attempts to determine the version of the services running
- -sC	Scan with the default nmap scripts
- -p \<x> or -p-	Port scan for port <x> or scan all ports
- -p-x Scan ports from 1 to x
- -Pn	Disable host discovery and just scan for open ports
- -A	Enables OS and version detection, executes in-build scripts for further enumeration 
- -v	Verbose mode
- -sU	UDP port scan
- -sS	TCP SYN port scan
- -oA   output to file


## gobuster
- -e	Print the full URLs in your console
- -u	The target URL
- -w	Path to your wordlist
- -U and -P	Username and Password for Basic Auth
- -p \<x>	Proxy to use for requests
- -c \<http cookies>	Specify a cookie for simulating your auth
- -o Output to file
## VI/VIM
- Vim -O file1 file2 (open two files vertically) <br> 
- CTRL+W+ARROWS -> switch between panes <br>
- O (insert new line and insert) <br>
- Shift+O (insert new line above and insert) 
- Settings  
  - ~/.vimrc 
  - set autoindent 
  - set tabstop=4 
  - colorscheme default 
  - Syntax on 
  - Set number 
 
## CHMOD 
 
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
 
## Objdump 
- Print headers: -x  
- Disassemble: -d 
- Syms: -t 
- Dynamic syms: -T 
- Reloc: -r 
- Dynamic reloc: -R 
 
## Strace  
- Trace syscalls 
 
## Ltrace  
- Trace library functions  
 
## Nc 
- Nc -lvp port (listen on port) 
- Nc -lvp port -e /program (listen on port and lauch program at connection) 
- Nc addr port  (connect to port) 
 
## Symlinks 
- Ln -s file sym (create soft symbolic link) 
- Ln -sf file sym (replaces if exist ) 
 
## Find files 
- find . -perm /4000 (find files at the current directory with specific permissions) 
 
## SSH 
- Ssh user@ip 
- Ssh -i rsa_key user@ip 
- Scp user@ip:/file/to/get /path/to/store 

