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