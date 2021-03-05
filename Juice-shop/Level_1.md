# Juice-Shop - Level 1


## Challenges' solutions 
<hr>

**Name:** Bonus payload </br>
**Description:** Use the bonus payload \<iframe width="100%" height="166" scrolling="no" frameborder="no" allow="autoplay" src="https://w.soundcloud.com/player/?url=https%3A//api.soundcloud.com/tracks/771984076&color=%23ff5500&auto_play=true&hide_related=false&show_comments=true&show_user=true&show_reposts=false&show_teaser=true"></iframe> in the DOM XSS challenge. </br> 
**Category:** XSS </br>
**Solution:**
<p>
The DOM XSS was found in a lower challenge
</p>
<hr>

**Name:** Bully Chatbot </br>
**Description:** Receive a coupon code from the support chatbot. </br> 
**Category:** Miscellaneous  </br>
**Solution:**
<p>

Going to **/chatbot** and spamming some messages the bot retrieves a cupom code: o*IVjf!Cal

</p>

<hr>

**Name:** Confidential Document </br>
**Description:** Access a confidential document. </br> 
**Category:** Sensitive Data Exposure  </br>
**Solution:**
<p>

By performing spidering/crawling in the website we can see that there's exist an FTP (which is also mentioned in the robots.txt). Within the FTP there's a file called acquisitions.md which should not be accessible to the public.

</p>
<hr>

**Name:** DOM XSS </br>
**Description:** Perform a DOM XSS attack with \<iframe src="javascript:alert(`xss`)">. </br> 
**Category:** XSS</br>
**Solution:**
<p>

To perform a DOM xss we need some input field that reflects it in the document elements. Such field is the search box (on the top right). <br>
To test it, we can search for 

```
<b>apples</b>
```

And we can see in the results that the search query appears bolded out ! <br>

Using the payload in the description, we got the challenge 

</p>
<hr>

**Name:** Error Handling </br>
**Description:** Provoke an error that is neither very gracefully nor consistently handled. </br> 
**Category:**  Security Misconfiguration  </br>
**Solution:**
<p>

I solved this challenge in an unintended way (i tried to upload an image, from a local webserver, which requested a non existing file causing all kinds of crashes). This can also be solved with SQLi in the email/password at login

</p>
<hr>

**Name:** Exposed Metrics </br>
**Description:** Find the endpoint that serves usage data to be scraped by a [popular monitoring system](https://github.com/prometheus/prometheus). </br> 
**Category:** Sensitive Data Exposure  </br>
**Solution:**
<p>

Checking the prometheus documentation it seems that a common endpoint is **/metrics**. By accessing it, we have all the platform information which should not be.

</p>
<hr>

**Name:** Missing Encoding </br>
**Description:** Retrieve the photo of Bjoern's cat in "melee combat-mode". </br> 
**Category:** Improper Input Validation   </br>
**Solution:**
<p>

If we analyze the **/photo-wall** page we can see that a picture is broken! Analyzing the dom elements we can see something wrong

```
src="assets/public/images/uploads/😼-#zatschi-#whoneedsfourlegs-1572600969477.jpg"
```

The src is not url-encoded, making the browser render them as anchors and breaking the image because the path will not resolve to anything ! <br>

To solve it, just do
```
curl http://127.0.0.1:3000/assets/public/images/uploads/😼-%23zatschi-%23whoneedsfourlegs-1572600969477.jpg
```

</p>
<hr>

**Name:** Outdated Allowlist </br>
**Description:** Let us redirect you to one of our crypto currency addresses which are not promoted any longer. </br> 
**Category:**  Unvalidated Redirects   </br>
**Solution:**
<p>

If we look up for redirects in the main.js file we have an entry for a bitcoin wallet. To Redirect for it just need to prefix the address with **http://localhost:300/addr_here**

```
data: {
    data: "bitcoin:1AbKfgvw9psQ41NbLi8kufDQTezwG8DRZm",
    url: "./redirect?to=https://blockchain.info/address/1AbKfgvw9psQ41NbLi8kufDQTezwG8DRZm",
    address: "1AbKfgvw9psQ41NbLi8kufDQTezwG8DRZm",
    title: "TITLE_BITCOIN_ADDRESS"
}
```

</p>
<hr>

**Name:** Privacy Policy </br>
**Description:** Read our privacy policy. </br> 
**Category:**  Miscellaneous   </br>
**Solution:**
<p>
Just open the privacy policy from the left-side menu
</p>
<hr>

**Name:** Repetitive Registration </br>
**Description:** Follow the DRY principle while registering a user. </br> 
**Category:** Improper Input Validation   </br>
**Solution:**
<p>
When registering a user, if two equal passwords are inserted and then on of them is changed, the web app does not verify it a second time. This leads to inserting different password and the account being registered anyways (considers only the first one)
</p>
<hr>

**Name:** Score Board </br>
**Description:** Find the carefully hidden 'Score Board' page. </br> 
**Category:**  Miscellaneous   </br>
**Solution:**
<p>

By viewing the .js files content (searching for **score**), we can find the mapping to the score board page in the **main-es2018.js** at line 12158

```
path: "score-board",
component: Ko
```
</p>
<hr>

**Name:**  Zero Stars </br>
**Description:** Give a devastating zero-star feedback to the store </br> 
**Category:** Improper Input Validation   </br>
**Solution:**
<p>

IF we send a normal feedback (with any star value), copy the request, change the rating to 0 resend it, the server accepts it.

</p>
<hr>
