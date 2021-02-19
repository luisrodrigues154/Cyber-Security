# Linux cheat sheet

## Kernel exploits
- linux-exploit-suggester-2 - identifies applicable exploits to the kernel

## SSH 
- Ssh user@ip 
- Ssh -i rsa_key user@ip 
- Scp user@ip:/file/to/get /path/to/store
- scp file_to_send user@ip:/remote/folder/ 
- Keys security: 600

## Zgrep + find
- Since zgrep does not support recursive operations, leverage the find command
  - zgrep "expr" $(find . -type f)

## Find files 

| option | action |
| :-: | :-: |
| -group \<group_name> | find files for group with groupname |
| -name \<name> | file with name <br>(case sensitive, regexable) |
| -iname \<name> | file with name <br>(case insensitive, regexable) |
| 2>/dev/null | redirect stderr |
| -f \<type> | specify file type <br>([d]irectory, [f]ile, etc.)|
| -user \<username> | sepecify owner|
| -perm \<permissions> (mode: 600 OR u=r)| find with specific permissions <br>(\- at least ; / any u/g/o that match) |
| -size [-;+;empty]n[c,k,M]  | find file with size n <br>(c=bytes, k=kbytes, M=mbytes) |
|  -[a,m,c]time/min[+/-]quantity| time related searches (a=accessed, m=modified, c=created)<br><b>example: -amin +30 </b> : file accessed more than 30 minutes ago|
 
## CHMOD

| value | action |
|:-:| :-:|
| 1 | --x |
| 2 | w-- |
| 3 | w-x |
| 4 | -r- |
| 5 | -rx |
| 6 | wr- |
| 7 | wrx |
| chmod 755 <file> | -rwxr-xr-x|

## VIM
| cmd | action |
| :-: | :-: |
| -O file1 file2 | split vertically for two files |
| CTRL+W+(arrows) | switch between panes |
| O | add line below and enter insert mode |
| SHIFT+O | add line above and enter insert mode |

## Misc

| cmd | action | 
| :-: | :-: |
| mkpasswd | Generate password hash <br> Example: mkpasswd -m sha-512 \<password>|
| openssl passwd \<password> | Generate password hash |

## Symlinks 
- Ln -s file sym (create soft symbolic link) 
- Ln -sf file sym (replaces if exist ) 

## Nmap
syntax: namp [options] ip
| options | action | 
| :-: | :-: |
| -sV	| Attempts to determine the version of the services running|
| -sC	| Scan with the default nmap scripts|
| -p \<x> or -p- |	Port scan for port <x> or scan all ports|
| -p-x | Scan ports from 1 to x|
| -Pn	| Disable host discovery and just scan for open ports|
| -A	| Enables OS and version detection, executes in-build scripts for further enumeration |
|  -v  |	Verbose mode|
|  -sU |	UDP port scan|
|  -sS |	TCP SYN port scan|
|  -oN |   output to normal file|
| --script=\<list,of,scripts/categories> | Executes a NSE script <br> (ex: --script=vuln)| 

## gobuster
| option | action 
| :-: | :-:|
| -e |	Print the full URLs in your console | 
| -u |	The target URL | 
| -w |	Path to your wordlist |
| -U and -P |	Username and Password for Basic Auth | 
| -p \<x>	| Proxy to use for requests | 
| -c \<http cookies> |	Specify a cookie for simulating your auth |
| -o | Output to file |


# Misc things
## Env vars
- LD_PRELOAD - loads a shared object before any others when a program is run. 
- LD_LIBRARY_PATH - provides a list of directories where shared libraries are searched for first

## Bash 
- Version < 4.2-048 - possible to create functions with names like paths
- Version < 4.4 - possible to abuse debug flag PS4