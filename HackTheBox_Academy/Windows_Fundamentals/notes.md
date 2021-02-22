# Windows Fundamentals

Need to connect to the machine via RDP (get a fully remote window)

```
cmd: xfreerdp /v:10.129.175.29 /u:htb-student /p:Academy_WinFun!
```

## What is the Build Number of the target workstation?

To achieve this we can use the Get-WmiObject command
```
cmd: Get-WmiObject -class win32_operatingsystem

SystemDirectory : C:\WINDOWS\system32
Organization    :
BuildNumber     : 18362
RegisteredUser  : mrb3n
SerialNumber    : 00329-10284-98099-AA406
Version         : 10.0.18362
``` 

**Answer:** 18362 

## Which Windows NT version is installed on the workstation? (i.e. Windows X - case sensitive)

From the previous command result
```
Version: 10.0.18362 -> Windows 10
```

**Answer:** Windows 10

## Find the non-standard directory in the C drive. Submit the contents of the flag file saved in this directory.

I'll use powershell (in cmd ls is dir)
``` 
cmd: cd \
cmd: ls

Mode                LastWriteTime         Length Name
----                -------------         ------ ----
d-----         9/7/2020   1:41 PM                Academy <-----
                    (.....)

cmd: cd Academy
cmd: ls

Mode                LastWriteTime         Length Name
----                -------------         ------ ----
-a----         9/7/2020  12:17 PM             32 flag.txt

cmd:cat flag.txt
```

**Answer:** c8fe8d977d3a0c655ed7cf81e4d13c75

## What system user has full control over the c:\users directory?

```
cmd: icacls "C:\users"

C:\users WS01\bob.smith:(F)   <- USER
         NT AUTHORITY\SYSTEM:(OI)(CI)(F)
         BUILTIN\Administrators:(OI)(CI)(F)
         BUILTIN\Users:(RX)
         BUILTIN\Users:(OI)(CI)(IO)(GR,GE)
         Everyone:(RX)
         Everyone:(OI)(CI)(IO)(GR,GE)

Successfully processed 1 files; Failed processing 0 files
```

**Answer:** bob.smith

## Identify one of the non-standard update services running on the host. Submit the full name of the service executable (not the DisplayName) as your answer

First lets list all the services that contain **update** in the name

```
cmd: Get-Service -Name "*update*"


Status   Name               DisplayName
------   ----               -----------
Running  FoxitReaderUpda... Foxit Reader Update Service <- This
Stopped  tzautoupdate       Auto Time Zone Updater
```

Since it is truncated, lets get the complete name (might be different from the DisplayName)
```
cmd: Get-Service -Name "*update*" | ft -auto

Status  Name                     DisplayName
------  ----                     -----------
Running FoxitReaderUpdateService Foxit Reader Update Service
Stopped tzautoupdate             Auto Time Zone Updater
```

The answer should be **FoxitReaderUpdateService** but for some reason it needs **.exe** at the end.. 

**Answer:** FoxitReaderUpdateService.exe

## What is the alias set for the ipconfig.exe command?

Retrieve all the aliases for a definition  
``` 
cmd: Get-Alias -definition ipconfig

CommandType     Name                                               Version    Source
-----------     ----                                               -------    ------
Alias           ifconfig -> ipconfig.exe
```
**Answer:** ifconfig

## Find the Execution Policy set for the LocalMachine scope.

```
cmd: Get-ExecutionPolicy LocalMachine
Unrestricted
```
**Answer:**Unrestricted

##  Use WMI to find the serial number of the system.

```
cmd (pshell module): Get-WmiObject -Class Win32_OperatingSystem | Select SerialNumber

SerialNumber
------------
00329-10284-98099-AA406

cmd: (pshell wmic): wmic os list brief

BuildNumber  Organization  RegisteredUser  SerialNumber             SystemDirectory      Version
18362                      mrb3n           00329-10284-98099-AA406  C:\WINDOWS\system32  10.0.18362
```
**Answer:** 00329-10284-98099-AA406

##  Find the SID of the bob.smith user.

```
cmd: wmic useraccount get sid,name | findstr "bob.smith"

bob.smith           S-1-5-21-2614195641-1726409526-3792725429-1003
```

**Answer:** S-1-5-21-2614195641-1726409526-3792725429-1003

##  What non-standard application is running under the current user ? (The answer is case sensitive).

I just went straight to the registry but retrieving the processes and just looking was also a possibility 
```
cmd: reg query HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run

HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run
    OneDrive    REG_SZ    "C:\Users\htb-student\AppData\Local\Microsoft\OneDrive\OneDrive.exe" /background
    NordVPN    REG_SZ    C:\Program Files\NordVPN\NordVPN.exe <---- HERE
```

**Answer:** NordVPN