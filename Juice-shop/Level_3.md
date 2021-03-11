# Juice-Shop - Level 3

## Chalanges' solutions

<hr>

**Name:**         Admin Registration  </br>
**Description:**  Register as a user with administrator privileges.  </br> 
**Category:**     Improper Input Validation   </br>
**Solution:**
<p>

When we login we can see that in the token is set the account role

```
{"typ":"JWT","alg":"RS256"}{"status":"success","data":{"id":21,"username":"","email":"email@email.com","password":"4297f44b13955235245b2497399d7a93","role":"customer" <- HERE
```

If we, when creating an account we add the json key **role** to the object, we can create an admin account
```
"email":"newadmin@admins.com","password":"123123","passwordRepeat":"123123","role" : "admin", <- HERE
```

</p>
<hr>

**Name:**       Bjoern's Favorite Pet    </br>
**Description:**Reset the password of Bjoern's OWASP account via the Forgot Password mechanism with the original answer to his security question.  </br> 
**Category:**   Broken Authentication     </br>
**Solution:**
<p>

Accessing the **/administration** page we get the Bjoern's email.. well we got 2
```
bjoern.kimminich@gmail.com  
bjoern@owasp.org            <- This works
```
Checking the **Photo wall** we can see an hastag in the cat picture (which we worked befor) that might be the cat name, but it did not work
``` 
zatschi
```

The next thing is to search the web for cat related content posted by him <br>

So i came accross a video where he is demonstrating the juice-shop and he states that he has a family cat named **Zaya**, which is the answer to the secret question <br>
</p>

<hr>

**Name:**        CAPTCHA Bypass    </br>
**Description:** Submit 10 or more customer feedbacks within 10 seconds.    </br> 
**Category:**    Broken Anti Automation     </br>
**Solution:**
<p>

To solve this i made a customer feedback, intercepted in burp and used the repeater to send 10 requests sequentially <br>

I could've used the intruder tab but with the comunity edition it does not send the 10 requests under 10 seconds <br> 

</p>

<hr>

**Name:**        CSRF    </br>
**Description:** Change the name of a user by performing Cross-Site Request Forgery from another origin.    </br> 
**Category:**    Broken Access Control     </br>
**Solution:**
<p>

First we need to see how a user name change is requested to the webserver (below i'll paste only the relevant things for the action)
```
POST /profile HTTP/1.1
Host: localhost:3000

username=waza+here
```

now we just need to use some HTML editor (can be an online one) and create a form to send this request
```
<form
    action="http://localhost:3000/profile"
    method="POST"
>
    <input type="hidden" name="username" value="ah+shit"/>
    <input type="submit" value="Submit">
</form>
``` 
In this case we need a button to trigger since we will not send to anyone and thus it does not need to trigger automatically <br>

If we are using an online editor, we click it and the request will not work (because we are using a docker and so the request to localhost is forbidden due to security protections), so firefox offers the option to open in a new tab which will work and solve the challenge (since the origin is not the same and the request is processed )

</p>



