# Invite 

This challenge intends to hack the invitation process in order to access the platform.


## Steps taken

First thing to analyze is the html and the javascript code behind this page.
<br>
Openinng the browser console it is displayed the following message:

```
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
.           The first step of the challenge!                                      .
.                                                                                 .
.                      uuuuuuu                                                    .
.                  uu$$$$$$$$$$$uu                                                .
.               uu$$$$$$$$$$$$$$$$$uu                                             .
.              u$$$$$$$$$$$$$$$$$$$$$u                                            .
.             u$$$$$$$$$$$$$$$$$$$$$$$u                                           .
.            u$$$$$$$$$$$$$$$$$$$$$$$$$u                   K E E P  C A L M       .
.            u$$$$$$$$$$$$$$$$$$$$$$$$$u                                          .
.            u$$$$$$"   "$$$"   "$$$$$$u                        A N D             .
.            "$$$$"      u$u       $$$$"                                          .
.             $$$u       u$u       u$$$                        H A C K            .
.             $$$u      u$$$u      u$$$                                           .
.              "$$$$uu$$$   $$$uu$$$$"                         T H I S            .
.               "$$$$$$$"   "$$$$$$$"                                             .
.                 u$$$$$$$u$$$$$$$u                             B O X             .
.                  u$"$"$"$"$"$"$u                                                .
.       uuu        $$u$ $ $ $ $u$$       uuu                                      .
.      u$$$$        $$$$$u$u$u$$$       u$$$$                                     .
.       $$$$$uu      "$$$$$$$$$"     uu$$$$$$                                     .
.     u$$$$$$$$$$$uu    """""    uuuu$$$$$$$$$$                                   .
.     $$$$"""$$$$$$$$$$uuu   uu$$$$$$$$$"""$$$"                                   .
.      """      ""$$$$$$$$$$$uu ""$"""                                            .
.                uuuu ""$$$$$$$$$$uuu                                             .
.       u$$$uuu$$$$$$$$$uu ""$$$$$$$$$$$uuu$$$                                    .
.       $$$$$$$$$$""""           ""$$$$$$$$$$$"              HackTheBox           .
.        "$$$$$"                      ""$$$$""           info@hackthebox.eu       .
.          $$$"                         $$$$"                                     .
.                                                                                 .
.    This page loads an interesting javascript file. See if you can find it :)    .
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
```
It is said that a java script file is being loaded, lets try to find it. Going to the debugger section of the programmer tools (firefox), under the www.hackthebox.eu there is a js named folder.
<br>
Inside that folder exist a file called <b> inviteapi.min.js</b>

```
//This javascript code looks strange...is it obfuscated???

eval(function(p,a,c,k,e,r){e=function(c){return c.toString(a)};if(!''.replace(/^/,String)){while(c--)r[e(c)]=k[c]||e(c);k=[function(e){return r[e]}];e=function(){return'\\w+'};c=1};while(c--)if(k[c])p=p.replace(new RegExp('\\b'+e(c)+'\\b','g'),k[c]);return p}('0 3(){$.4({5:"6",7:"8",9:\'/b/c/d/e/f\',g:0(a){1.2(a)},h:0(a){1.2(a)}})}',18,18,'function|console|log|makeInviteCode|ajax|type|POST|dataType|json|url||api|invite|how|to|generate|success|error'.split('|'),0,{}))
```

As stated in the very first line, the comment, the code is obfuscated, so lets find a tool to deobfuscate this automatically.
<br>
The used webapp was: http://deobfuscatejavascript.com/ , because it detects automatically the type of obfuscation used (in this case through eval).

The clean code is:

```
function makeInviteCode() {
    $.ajax({
        type: "POST",
        dataType: "json",
        url: '/api/invite/how/to/generate',
        success: function(a) {
            console.log(a)
        },
        error: function(a) {
            console.log(a)
        }
    })
}
```

Is visible that a function named makeInviteCode() exists and needs to be called. It will print to the console either it fails or succeeds.
<br>
Using the browser terminal lets issue a call to this function.

```
makeInviteCode()

Console logs a returned JSON OBJECT
Data:"SW4gb3JkZXIgdG8gZ2VuZXJhdGUgdGhlIGludml0ZSBjb2RlLCBtYWtlIGEgUE9TVCByZXF1ZXN0IHRvIC9hcGkvaW52aXRlL2dlbmVyYXRl"
enctype: "BASE64"
```

Its clear that the data is Encrypted/encoded and as it appears the enctype determines how it is hidden, in this case base64.
<br>

After decoding:
```
In order to generate the invite code, make a POST request to /api/invite/generate
```

To do this i just pulled something faster from my sleeve, just renamed the function and changed the url, pasted it in the console and called it.


```
function makeInviteCodeReal() {
    $.ajax({
        type: 'POST',
        dataType: 'json',
        url: '/api/invite/generate',
        success: function(a) {
            console.log(a);
        },
        error: function(a) {
            console.log(a);
        }
    });
}

makeInviteCodeReal()

RESPONSE: 
Code: "QkJaQVctUkNHQVMtSVBDUlUtS1BIQUQtSU9TSk0="
Format: "encoded"

```

It seems to be encoded, and looks like base64 again, lets try to decode it

```
BBZAW-RCGAS-IPCRU-KPHAD-IOSJM
```

And voila, success. I was able to register an account !