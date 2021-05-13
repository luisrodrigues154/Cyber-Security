# Century

## Level 0

Just login via ssh 
```
century1:century1
```

## Level 1

The password for Century2 is the build version of the instance of PowerShell installed on this system. <br>

We can get it with the powershell env variable **$PSVersionTable**
```ps1
Name                           Value                                                                                                   
----                           -----                                                                                                    
PSVersion                      5.1.14393.3866                                                                                           
PSEdition                      Desktop                                                                                                  
PSCompatibleVersions           {1.0, 2.0, 3.0, 4.0...}                                                                                  
BuildVersion                   10.0.14393.3866                                                                                          
CLRVersion                     4.0.30319.42000                                                                                          
WSManStackVersion              3.0                                                                                                      
PSRemotingProtocolVersion      2.3                                                                                                      
SerializationVersion           1.1.0.1 
```

### Answer: 10.0.14393.3866 

## Level 2

The password for Century3 is the name of the built-in cmdlet that performs the wget like function within PowerShell PLUS the name of the file on the desktop. LOWERCASE<br>

```ps1
# Get the file name first because it's quicker
PS C:\users\century2\desktop> ls 

Mode                LastWriteTime         Length Name                                                                                   
----                -------------         ------ ----                                                                                   
-a----        8/30/2018   3:29 AM            693 443   

# with a bit of googling
PS > get-command -type cmdlet -name "*request*"

Cmdlet Invoke-WebRequest  3.1.0.0    Microsoft.PowerShell.Utility
``` 

Creds: ```Century3:invoke-webrequest443```

## Level 3

The password for Century4 is the number of files on the desktop. <br>

```ps1
PS C:\users\century3\desktop> (get-childitem).count                                                                                     
123  
```

Creds: ```Century4:123```

## Level 4 

The password for Century5 is the name of the file within a directory on the desktop that has spaces in its name. <br>

**NOTE:** The password will be lowercase no matter how it appears on the screen.

```ps1
PS C:\users\century4\desktop> ls '.\Can You Open Me'                                                                                    
Mode                LastWriteTime         Length Name                                                                                   
----                -------------         ----------                                                                                    
-a----        8/30/2018   3:29 AM             24 61580
```

Creds: ```Century5:61580```

## Level 5

The password for Century6 is the short name of the domain in which this system resides in PLUS the name of the file on the desktop. <br>

**NOTE:** 
- If the short name of the domain is “blob” and the file on the desktop is named “1234”, the password would be “blob1234”.
- The password will be lowercase no matter how it appears on the screen.

```ps1
# already at desktop
PS (...)\desktop > ls
3347

PS C:\users\century5\desktop> (Get-ADDomain).name                                                                                       
underthewire
```

Creds: ```Century6:underthewire3347```

## Level 6

The password for Century7 is the number of folders on the desktop.

```ps1
PS (..)\desktop > (get-childitem -attributes Directory).count                                                                
197
```

Creds: ```Century7:197``` 

## Level 7

The password for Century8 is in a readme file somewhere within the contacts, desktop, documents, downloads, favorites, music, or videos folder in the user’s profile. <br>

**NOTE:** The password will be lowercase no matter how it appears on the screen.
```ps1
PS (..)\desktop > cd ..
PS C:\users\century7 > Get-childitem -recurse -file "*readme*"

Directory: C:\users\century7\Downloads                                                                                             
Mode                LastWriteTime         LengthName                                                                                   
----                -------------         ----------                                                                                    
-a----        8/30/2018   3:29 AM              7 Readme.txt

PS (..) > get-content -path "C:\users\century7\Downloads\Readme.txt"                                                         
7points
``` 

Creds: ```Century8:7points```

## Level 8

The password for Century9 is the number of unique entries within the file on the desktop

```ps1
PS C:\users\century8\desktop> (get-content .\unique.txt | get-unique).count
696
```

Creds: ```Century9:696```

## Level 9

The password for Century10 is the 161st word within the file on the desktop. <br>

**NOTE:** The password will be lowercase no matter how it appears on the screen.

```ps1 
# Powershell arrays are 0 indexed
PS (...)\desktop > (get-content .\Word_file.txt -delimiter " ")[160]
pierid
```

Creds: ```Century10:pierid```

## Level 10

The password for Century11 is the 10th and 8th word of the Windows Update service description combined PLUS the name of the file on the desktop. <br>

**NOTE:**
- The password will be lowercase no matter how it appears on the screen.
- If the 10th and 8th word of the service description is “apple” and “juice” and the name of the file on the desktop is “88”, the password would be “applejuice88”.

```ps1

PS (..)\desktop > ls
110 

# first let get the services and see how it is displayed
PS (..)\desktop >  Get-service
Status   Name DisplayName                                                                                                  
------   ----             -----------
Running  ADWS               Active Directory Web Services

# lets find the windows update service 
PS (..)\desktop > get-service -displayname "*update*"

Status   Name               DisplayName                                                                                                 
------   ----               -----------                                                                                                 
Stopped  tzautoupdate       Auto Time Zone Updater                                                                                      
Running  UsoSvc             Update Orchestrator Service for Win...                                                                      
Stopped  wuauserv           Windows Update

```

Since the get-service does not have a member (via **get-member**) for the description, i needed to google <br>

```ps1
# it's trimmed but is enough
PS (..)\desktop > Get-WmiObject win32_service | Where-object {$_.Name -eq "wuauserv"} | select-object -property description

description
-----------
Enables the detection, download, and installation of updates for Windows and other programs. If this service is disabled, users of th..
```

Creds: ```Century11:windowsupdates110```

## Level 11

The password for Century12 is the name of the hidden file within the contacts, desktop, documents, downloads, favorites, music, or videos folder in the user’s profile. <br>

**NOTE:**
- Exclude “desktop.ini”.
- The password will be lowercase no matter how it appears on the screen.

```ps1
PS (..)\desktop > cd ..
PS (..)\century11 > get-childitem -recurse -attributes hidden
    Directory: C:\users\century11\Downloads                                                                                             
Mode                LastWriteTime         Length Name                                                                                   
----                -------------         ------ ----                                                                                   --rh--        8/30/2018   3:34 AM             30 secret_sauce
```

Creds: ```Century12:secret_sauce```

## Level 12

The password for Century13 is the description of the computer designated as a Domain Controller within this domain PLUS the name of the file on the desktop. <br>

**NOTE:**
- The password will be lowercase no matter how it appears on the screen.
- If the description “today_is” and the file on the desktop is named “_cool”, the password would be “today_is_cool”.

```ps1
PS (..)\desktop > ls
_things 

# we first need to know who is the DC
PS (..)\desktop > get-addomaincontroller
Name: UTW

# now we get the needed info about that computer
PS (..)\desktop > get-adcomputer "UTW" -properties description
Description: i_authenticate
```

Creds: ```Century13:i_authenticate_things```

## Level 13

The password for Century14 is the number of words within the file on the desktop.

```ps1

PS (..)\desktop > get-content .\countmywords | measure-object -word
755
``` 

Creds: ```Century14:755``` 

## Level 14

The password for Century15 is the number of times the word “polo” appears within the file on the desktop. <br>

**NOTE:**
- You should count the instances of the whole word only..

Well, for this i needed google
```ps1

PS (..)\desktop > (get-content .\countpolos | select-string -pattern "polo" -allmatches).matches.count
158
``` 

Creds: ```Century15:158```

## Level 15

END
