# Cyborg

## Level 0

Just login via ssh
```
cyborg1:cyborg1
```

## Level 1

The password for cyborg2 is the state that the user Chris Rogers is from as stated within Active Directory. <br>

**NOTE:**
- The password will be lowercase no matter how it appears on the screen.
- “State” refers to the location within the country and NOT the “state” of the account (enabled/ disabled).

```ps1
# lets check how to get the ADusers
PS (..)\desktop > get-command -name "*ad*user*"
Cmdlet   Get-ADUser    1.0.0.0    ActiveDirectory   

# lets get the the user using a simple query through the filter option
PS (..)\desktop > get-aduser -filter 'Name -like "*chris*"' -properties state
# -- snip --
State: kansas
# -- snip --
``` 

Creds: ```Cyborg2:kansas```

## Level 2

The password for cyborg3 is the host A record IP address for CYBORG718W100N PLUS the name of the file on the desktop. <br>

**NOTE:**
- If the IP is “10.10.1.5” and the file on the desktop is called “_address”, then the password is “10.10.1.5_address”.
- The password will be lowercase no matter how it appears on the screen.

```ps1
PS (..)\desktop > ls
_ipv4

# i had to use google to find this out (the level description aint that good tho)
PS (..)\desktop > Resolve-name CYBORG718W100N 
CYBORG718W100N.underthewire.tech    A      3600  Answer     172.31.45.167
```

Creds: ```Cyborg3:172.31.45.167_ipv4```


## Level 3

The password for cyborg4 is the number of users in the Cyborg group within Active Directory PLUS the name of the file on the desktop. <br>

**NOTE:**
- If the number of users is “20” and the file on the desktop is called “_users”, then the password is “20_users”.
- The password will be lowercase no matter how it appears on the screen.

```ps1
# check the file name first
PS (..)\desktop > ls
_objects

PS (..)\desktop > (get-adgroupmember -identity Cyborg).count
88
```

Creds: ```Cyborg4:88_objects ```  

## Level 4

The password for cyborg5 is the PowerShell module name with a version number of 8.9.8.9 PLUS the name of the file on the desktop. <br>

**NOTE:**
- If the module name is “bob” and the file on the desktop is called “_settings”, then the password is “bob_settings”.
- The password will be lowercase no matter how it appears on the screen.

```ps1
PS (..)\desktop > ls
_eggs

# tried the conventional way (installed modules)
PS (..)\desktop > get-installedmodule
# also tried get-module without success

# next lets list the available modules (either installed or to install), produced a big list, lets filter it
PS (..)\desktop > get-module -listavailable | where-object {$_.name -eq "8.9.8.9"}
Manifest   8.9.8.9    bacon   Get-bacon 
```

Creds: ```Cyborg5:bacon_eggs ```

## Level 5

The password for cyborg6 is the last name of the user who has logon hours set on their account PLUS the name of the file on the desktop. <br>

**NOTE:**
- If the last name is “fields” and the file on the desktop is called “_address”, then the password is “fields_address”.
- The password will be lowercase no matter how it appears on the screen

```ps1
PS (..)\desktop > _timer

# First i got a list of the fields (with a pipe to get-member) and then filtered. Initially i did not used select-object but it was just to much information and ended up using it to trim the output text
PS (..)\desktop > get-aduser -filter 'name -like "*"' -properties * | select-object -property name,logonhours | where-object {$_logonhours -gt 0}

Administrator {255,255,255,255,...}
Rowray, Benny {0,0,0,0,...}
```

Creds: ```Cyborg6:rowray_timer ```

## Level 6

The password for cyborg7 is the decoded text of the string within the file on the desktop. <br>

**NOTE:**
- The password is the last word of the string. For example, if it is “I like PowerShell”, the password would be “powershell”.
- The password will be lowercase no matter how it appears on the screen.
- There are no spaces in the answer.

```ps1
PS (..) desktop > ls
cypher.text

# from the chall description i assume base64 but lets check
PS (..) dekstop > Get-content .\cypher.txt
YwB5AGIAZQByAGcAZQBkAGQAbwBuAA== 

# Confirmed. Well, there's no cmdlet for this, needed google.
PS (..) desktop > [System.text.encoding]::UTF8.Getstring([System.convert]::FromBase64String($(get-content .\cypher.txt)))
c y b e r g e d d o n 
```

Creds: ```Cyborg7:cybergeddon```

## Level 7

The password for cyborg8 is the executable name of a program that will start automatically when cyborg7 logs in. <br>

**NOTE:** The password will be lowercase no matter how it appears on the screen.

```ps1
# well i know we could get this through registry but needed google and found out we can get it via CIM (common information model) 
PS (..)\desktop > get-ciminstance win32_startupcommand

C:\program files\SkyNet\skynet.exe underthewire\cyborg7 SKYNET
```

Creds: ```Cyborg8:skynet```

## Level 8

The password for cyborg9 is the Internet zone that the picture on the desktop was downloaded from. <br>

**NOTE:** The password will be lowercase no matter how it appears on the screen. <br>

Found [this](http://powershellcookbook.com/recipe/XilI/interact-with-alternate-data-streams) resource to be extremelly helpfull and explanatory

```ps1
# Well it is obvious that we need metadata but didn't knew that windows had "alternate data streams" which is used by programs to save additional information
PS (..)\desktop > get-item .\1_qs5nwlcl7f_-SwNlQvOrAw.png -stream *

# -- snip --
Stream        : Zone.Identifier 

# to get it we just specify the zone name in the stream argument when we get the content. This way we dont access the main stream and the "hidden" Zone.identifier stream
PS (..)\desktop > get-content .\1_qs5nw1c17f_-SwN1QcOrAw.png -Stream Zone.identifier
[ZoneTransfer]
ZoneId=4
```

Creds: ```Cyborg8:4```

## Level 9

The password for cyborg10 is the first name of the user with the phone number of 876-5309 listed in Active Directory PLUS the name of the file on the desktop. <br>

**NOTE:**
- If the first name “chris” and the file on the desktop is called “23”, then the password is “chris23”.
- The password will be lowercase no matter how it appears on the screen.

```ps1
PS (..)\desktop > ls
99

#first lets get the users stored parameters
PS (..)\desktop > get-aduser -filter * -properties * | get-member
# -- snip --
HomePhone
MobilePhone
OfficePhone
# -- snip -- 

# Now lets query active directory
PS (..)\desktop > get-aduser -filter 'Homephone -eq "876-5309" -or MoilePhone -eq "876-5309" -or OfficePhone -eq "876-5309"' -properties name,homephone,mobilephone,officephone

Name : Garick, Onita 
```

Creds: ```Cyborg10:onita99```

## Level 10

The password for cyborg11 is the description of the Applocker Executable deny policy for ill_be_back.exe PLUS the name of the file on the desktop. <br>

**NOTE:**
- If the description is “green$” and the file on the desktop is called “28”, then the password is “green$28”.
- The password will be lowercase no matter how it appears on the screen.

```ps1
PS (..)\desktop > ls
99

# first teste for local
PS (..)\desktop > get-applockerpolicy -local 
1 {}              {}  

# then used get-help and there was a flag -effective (which substitutes -local), which merges all policies
# it didn't output everything (too long), so i used .tostring() and it outputed everything (kinda harder to read, but it is what we got)
PS (..)\desktop > (get-applockerpolicy -effetive).tostring()
# -- snip --
<FilePathRule Id="cf7f9744-e5de-4189-8499-236666a32796" Name="C:\Users\cyborg10\Documents\ill_be_back.exe" Description="terminated!"
# -- snip --
```

Creds: ```Cyborg11:terminated!99```

## Level 11

The password for cyborg12 is located in the IIS log. The password is not Mozilla or Opera. <br>

**NOTE:** The password will be lowercase no matter how it appears on the screen.

```ps1
# found on google that logs are at: %SystemDrive%\inetpub\logs\LogFiles where %SystemDrive$ -> C:
# the file is HUGE, need to parse it 
PS (..)\w3svc1 > get-content .\u_ex160413.log | where-object {!$_.contains("Mozilla") -and !$_.contains("Opera")}
172.31.45.65 HTTP/1.1 LordHelmet/5.0+(CombTheDesert)+Password+is:spaceballs  
```

Creds: ```Cyborg12:spaceballs```

## Level 12

The password for cyborg13 is the first four characters of the base64 encoded full path to the file that started the i_heart_robots service PLUS the name of the file on the desktop. <br>

**NOTE:**
- An example of a full path would be ‘c:\some_folder\test.exe’.
- Be sure to use ‘UTF8’ in your encoding.
- If the encoded base64 is “rwmed2fdreewrt34t” and the file on the desktop is called “_address”, then the password is “rwme_address”.
- The password will be lowercase no matter how it appears on the screen.

Found that powershell cmdlet's just show a portion of the information. To get the full information we get a WmiObject from win32_service

```ps1
PS (..)\desktop > ls
_heart

PS (..)\desktop > get-wmiobject win32_service | select * | where-object {$_.displayname -eq 'i_heart_robots'}
# -- snip --
PathName  : c:\windows\system32\cmd.exe 
# -- snip --

# convert to b64
PS (..)\desktop > [Convert]::ToBase64String([System.text.encoding]::UTF8.getBytes('c:\windows\system32\cmd.exe')) 
Yzpcd2luZG93c1xzeXN0ZW0zMlxjbWQuZXhl
```

Creds: ```Cyborg13:yzpc_heart```

## Level 13

The password cyborg14 is the number of days the refresh interval is set to for DNS aging for the underthewire.tech zone PLUS the name of the file on the desktop. <br>

**NOTE:**
- If the days are set to “08:00:00:00” and the file on the desktop is called “_tuesday”, then the password is “8_tuesday”.
- The password will be lowercase no matter how it appears on the screen.

```ps1 
PS (..)\desktop > ls
_days

PS (..)\desktop > get-dnsserver
ZoneName             : underthewire.tech
AgingEnabled         : True
AvailForScavengeTime : 9/21/2018 10:00:00 AM
RefreshInterval      : 22.00:00:00
NoRefreshInterval    : 7.00:00:00
ScavengeServers      :
```

Creds: ```Cyborg14:22_days```

## Level 14

The password for cyborg15 is the caption for the DCOM application setting for application ID {59B8AFA0-229E-46D9-B980-DDA2C817EC7E} PLUS the name of the file on the desktop. <br>

**NOTE:**
- If the caption is “dcom” and the file on the desktop is called “_address”, then the password is “dcom_address”.
- The password will be lowercase no matter how it appears on screen.

DCOM is basically a distributed object that allows program execution over the network. The windows registry contains the DCOM config data for 3 identifiers:
- CLSID -> class identifier
- PROGID -> programmatic identifier (option)
- APPID -> Application identifier. Identifies all classes that are part of the same executable

```ps1
PS (..)\desktop > ls
_objects

# again, powershell does not have a direct cmdlet for it. google is my friend tho
PS (..)\desktop > get-wmiobject win32_dcomapplication | where-object {$_.appid -eq "{59B8AFA0-229E-46D9-B980-DDA2C817EC7E}"}
# -- snip --
{59B8AFA0-229E-46d9-B980-DDA2C817EC7E}             propshts
```

Creds: ```Cyborg15:propshts_objects```

## Level 15

END
