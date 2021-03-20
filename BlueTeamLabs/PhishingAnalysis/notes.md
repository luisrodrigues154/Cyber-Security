# Phishing Analysis

A user has received a phishing email and forwarded it to the SOC. Can you investigate the email and attachment to collect useful artifacts? 

# Questions

## Who is the primary recipient of this email?

Since the challenge files is comprised of an email, we can open that in text editor <br>

In this case, we can see this question's answer in the email header

```
From: Mail Delivery System <Mailer-Daemon@se7-syd.hostedmail.net.au>
Sent: 18 March 2021 04:14
To: kinnar1975@yahoo.co.uk <kinnar1975@yahoo.co.uk>
Subject: Undeliverable: Website contact form submission
```
**Answer:** kinnar1975@yahoo.co.uk 


## What is the subject of this email?

Well, this can be answered the same way as the above one

**Answer:** Undeliverable: Website contact form submission

## What is the date and time the email was sent?

Guess what, same as previous (which relays on the previous... inception )
**Answer:** 18 March 2021 04:14

## What is the Originating IP?

In the email, we can see the servers it was relayed through

```
Content-Type: message/rfc822
Content-Disposition: attachment;
	creation-date="Thu, 18 Mar 2021 04:14:19 GMT";
	modification-date="Thu, 18 Mar 2021 04:14:19 GMT"
Content-ID: <5A9638EA6676A449B32643CFC62AAB5F@eurprd01.prod.exchangelabs.com>

Received: from c5s2-1e-syd.hosting-services.net.au ([103.9.171.10])
	by se7-syd.hostedmail.net.au with esmtps (TLSv1.2:AES128-GCM-SHA256:128)
	(Exim 4.92)
	id 1lMk2r-0007vB-6O
	for kinnar1975@yahoo.co.uk; Thu, 18 Mar 2021 15:14:06 +1100
Received: from markgard by c5s2-1e-syd.hosting-services.net.au with local (Exim 4.94)
	id 1lMk2m-002w3b-NT
	for kinnar1975@yahoo.co.uk; Thu, 18 Mar 2021 15:13:56 +1100
```
**Answer:** 103.9.171.10

## Perform reverse DNS on this IP address, what is the resolved host? (whois.domaintools.com)

For this a simple tool on the terminal is much quicker than using the recommended page

```
cmd: host 103.9.171.10

10.171.9.103.in-addr.arpa domain name pointer c5s2-1e-syd.hosting-services.net.au.
```
**Answer:** c5s2-1e-syd.hosting-services.net.au

## What email address will receive replies to this email?

In the content we can see the **References** and the **In-Reply-To:**

```
References: <E1lMk2z-00086Y-Jw@se7-syd.hostedmail.net.au>
In-Reply-To: <E1lMk2z-00086Y-Jw@se7-syd.hostedmail.net.au>
```
**Answer:** E1lMk2z-00086Y-Jw@se7-syd.hostedmail.net.au

## What is the name of the attached file? 

Opening the **.eml** file in thunderbird we can see the attachment

**Answer:** Website contact form submission.eml

## What is the URL found inside the attachment? 

Continuing using the thunderbird we can see the content of the attachment

**Answer:** https://35000usdperwwekpodf.blogspot.sg?p=9swghttps://35000usdperwwekpodf.blogspot.co.il?o=0hnd

## What service is this webpage hosted on?

Well, we can see that from the link

**Answer:** blogspot

## Using URL2PNG, what is the heading text on this page? 

Using [URL2PNG](https://www.url2png.com/) we can answer this

**Answer:** Blog has been removed