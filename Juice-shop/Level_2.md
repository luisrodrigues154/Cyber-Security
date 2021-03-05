# Juice-Shop - Level 2

## Chalanges' solutions

<hr>

**Name:** Admin Section </br>
**Description:** Access the administration section of the store. </br> 
**Category:**  Broken Access Control </br>
**Solution:**
<p>
Solution goes here
</p>

<hr>

**Name:**  Login MC SafeSearch </br>
**Description:** Log in with MC SafeSearch's original user credentials without applying SQL Injection or any other bypass. </br> 
**Category:**  Sensitive Data Exposure  </br>
**Solution:**
<p>

Since i have no idea of the credentials, i went ahead and searched in the products to check if Mc SafeSearch posted some review in any product <br>
```
Product: Juice Shop "Permafrost" 2020 Edition
Review: mc.safesearch@juice-sh.op 🧊 Let it go, let it go 🎶 Can't hold it back anymore 🎶 Let it go, let it go 🎶 Turn away and slam the door ❄️
```

Alright, now we need the password. Since i could not find anything related to it, i looked at the hint which said : Listen to "Protect Ya Passwordz" <br>

In the song it states that the initial password is his first dog name (Mr.Noodles) with the O's as 0's

```
mc.safesearch@juice-sh.op:Mr. N00dles
```
</p>

<hr>

**Name:** Security Policy </br>
**Description:**  Behave like any "white-hat" should before getting into the action.</br> 
**Category:**  Miscellaneous</br>
**Solution:**
<p>
This demonstrates a standard propose, did not really knew that it existed but seems pretty awesome
</p>

<hr>

**Name:**  View Basket</br>
**Description:**  View another user's shopping basket. </br> 
**Category:**  Broken Access Control</br>
**Solution:**
<p>

Tampering the request by sending a different user id does not work, the token is verified. But we can change the local cookies/session. By changing them in the browser developers tools **Session storage** section (by changing the bid, which might stand for basket id), we solve this challenge 

</p>

<hr>

**Name:**  Visual Geo Stalking</br>
**Description:**  Determine the answer to Emma's security question by looking at an upload of her to the Photo Wall and use it to reset her password via the Forgot Password mechanism. </br> 
**Category:** Sensitive Data Exposure </br>
**Solution:**
<p>

Downloading the image and uploading at TinEye or Google did not provided anything <br>
Exfiltrating the image (in hope for GPS coordinates or any information) nothing else was found <br>

By zooming at the image, we can see a window with a sign with a name **"ITsec"**, which turns out to be the secret answer to the **emma@juice-sh.op** email ! <br>

</p>

<hr>

**Name:**  Meta Geo Stalking </br>
**Description:**  Determine the answer to John's security question by looking at an upload of him to the Photo Wall and use it to reset his password via the Forgot Password mechanism. </br> 
**Category:** Sensitive Data Exposure </br>
**Solution:**
<p>

First we need to know what is the question
```
What's your favorite place to go hiking
```

Using the above techniques, we can exfiltrate some information from it, such as GPS coordinates

```
cmd: exiftool -gpslatitude -gpslongitude john_image.png
GPS Latitude                    : 36 deg 57' 31.38" N
GPS Longitude                   : 84 deg 20' 53.58" W
```

Using the coordinates itself was a pain, so i used [this tool](https://tool.geoimgr.com/) which located to **Daniel Boone National Forest** which is, in fact, the answer to the question

</p>

<hr>

**Name:** Password Strength </br>
**Description:** Log in with the administrator's user credentials without previously changing them or applying SQL Injection. </br> 
**Category:**  Broken Authentication </br>
**Solution:**
<p>

From the description and the name we can pretty much assume that it needs to be solved using bruteforce! <br>

The tricky part is finding the correct wordlist! 

```
admin@juice-sh.op:admin123
```

</p>

<hr>

**Name:** Login Admin </br>
**Description:**  Log in with the administrator's user account. </br> 
**Category:** Injection  </br>
**Solution:**
<p>

Well this was solved with the afore challenge, but since the category is different, it is not technically solved. <br>

From the previous level challenges we saw that the application is vulnerable to sql injection <br>

So if we craft the following login payload, we get authenticated as the administrator

```
admin@juice-sh.op' or 1==1--:w
```

</p>

<hr>

**Name:** Admin Section </br>
**Description:** Access the administration section of the store. </br> 
**Category:**  Broken Access Control </br>
**Solution:**
<p>

If we open the file **main-es2018.js**, and search for **admin** we can see the route to the **administration** page 

```
16243: path: 'administration',
16244: component: Za,
```

</p>

<hr>

**Name:**        Five-Star Feedback   </br>
**Description:** Get rid of all 5-star customer feedback.   </br> 
**Category:**    Broken Access Control   </br>
**Solution:**
<p>

Just access the administration page and delete an entry

</p>


<hr>

**Name:**         Deprecated Interface  </br>
**Description:**  Use a deprecated B2B interface that was not properly shut down.  </br> 
**Category:**     Security Misconfiguration  </br>
**Solution:**
<p>

This one i could not find by myself (a reminder to always think outside the box). The issue is under the complaint form which allows a file upload <br>

The thing is that it supports more types than the ones listed in the file explorer (when picking the files) and, when a file which is not listed there is used, a **410 Gone** response is received and the challenge is solved... <br>

Again, from the hints and the description this is not something that is understandable (i guess)

</p>


