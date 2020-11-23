# OpenKeyS

export IP=10.10.10.199

# Solution

Starting with the traditional... 

```
cmd: nmap -sC -sV -oN nmap/initial $IP

22/tcp open  ssh     OpenSSH 8.1 (protocol 2.0)
| ssh-hostkey: 
|   3072 5e:ff:81:e9:1f:9b:f8:9a:25:df:5d:82:1a:dd:7a:81 (RSA)
|   256 64:7a:5a:52:85:c5:6d:d5:4a:6b:a7:1a:9a:8a:b9:bb (ECDSA)
|_  256 12:35:4b:6e:23:09:dc:ea:00:8c:72:20:c7:50:32:f3 (ED25519)
80/tcp open  http    OpenBSD httpd
|_http-title: Site doesn't have a title (text/html).
```

Lets do an all port on the side ....  UPDATE: nothing more was found<br>

Visiting the page we got a login page, from the source seems that SQLi is not possible (regexes the input for letters and numbers) <br>

Gobuster time
```
cmd: gobuster dir -u http://$IP/ -w /usr/share/wordlists/SecLists/Discovery/Web-Content/big.txt -o gobuster/big.log -x php,html,txt -t 40

/css (Status: 301)
/fonts (Status: 301)
/images (Status: 301)
/includes (Status: 301)
/index.html (Status: 200)
/index.php (Status: 200)
/js (Status: 301)
/vendor (Status: 301)
```

Checking the <b> /includes </b> we can see 2 files (auth.php is empty, auth.php.swp has content)... In the .swp we can see:
```
magic num?: b0VIM
user: jennifer
host name: openkeys.htb (add to /etc/hosts to be safe)
path1: /var/www/htdocs/includes/auth.php
path2: /auth_helpers/check_auth <- OpenBSD compiled binary (not stripped) to check the creds!!
```

Checking the file
```
cmd: file check_auth

----------------------------
cmd: checksec check_auth

Partial RELRO   
No canary found   
NX enabled    
PIE enabled     
No RPATH   
No RUNPATH   
43) Symbols

----------------------------
cmd: strings check_auth

Nothing interesting
```

Since the binary is for OpenBSD, i cant run it in kali so i dowloaded an ISO for OpenBSD to experiment with the binary <br>
The binary just checks with a local file so i cannot verify nor bruteforce this way <br>

Analyzing the magic number we can see that it is a VIM swap file... After downloading it

```
cmd: vim -r auth.swp

<?php

function authenticate($username, $password)
{
    $cmd = escapeshellcmd("../auth_helpers/check_auth " . $username . " " . $password);
    system($cmd, $retcode);
    return $retcode;
}

function is_active_session()
{
    // Session timeout in seconds
    $session_timeout = 300;

    // Start the session
    session_start();

    // Is the user logged in? 
    if(isset($_SESSION["logged_in"]))
    {
        // Has the session expired?
        $time = $_SERVER['REQUEST_TIME'];
        if (isset($_SESSION['last_activity']) && 
            ($time - $_SESSION['last_activity']) > $session_timeout)
        {
            close_session();
            return False;
        }
        else
        {
            // Session is active, update last activity time and return True
            $_SESSION['last_activity'] = $time;
            return True;
        }
    }
    else
    {
        return False;
    }
		}

function init_session()
{
    $_SESSION["logged_in"] = True;
    $_SESSION["login_time"] = $_SERVER['REQUEST_TIME'];
    $_SESSION["last_activity"] = $_SERVER['REQUEST_TIME'];
    $_SESSION["remote_addr"] = $_SERVER['REMOTE_ADDR'];
    $_SESSION["user_agent"] = $_SERVER['HTTP_USER_AGENT'];
    $_SESSION["username"] = $_REQUEST['username'];
}

function close_session()
{
    session_unset();
    session_destroy();
    session_start();
}


?>
```

After some research i found one CVE that works for this: <b>CVE-2019-19521</b> which allows an attacker to specify <b>-schallenge </b> and bypass the authentication! After bypass the webserver redirects to <b>/sshkey.php</b><br>

Now we need to trick the webserver ! <br>

From the swap file we can see that all the session attributes are set before it redirects (including the username), so when it receives the 302 from the login page, before sending the request to the browser to follow the redirection we can change the username field and get its ssh key (using the found in the swp page, the jennifer username)!! <br>

Success !!!
```
cmd: whoami && id && cat user.txt

jennifer
uid=1001(jennifer) gid=1001(jennifer) groups=1001(jennifer), 0(wheel) <------ interesting
36ab21239a15c537bde90626891d2b10
```

After poking around with linpeas and linenum i went to search for some CVE for this OpenBSD (when i looked for the bypass i saw alot of them) <br>

Found a repo with alot of exploits and found that it was possible to execute commands as root using the SMTP mailer <br>

Src: https://github.com/bcoles/local-exploits/blob/master/CVE-2020-7247/root66 <br>

And voila! 

```
whoami && id && cat root.txt
root
uid=0(root) gid=0(wheel) groups=0(wheel)
f3a553b1697050ae885e7c02dbfc6efa
```
