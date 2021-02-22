# Windows Miscellaneous Information

## Windows versioning
| OS name | Version |
|:-:|:-:|
| Windows NT 4 	                        |  4.0 | 
| Windows 2000 	                        |  5.0 |
| Windows XP 	                          |  5.1 |
| Windows Server 2003, 2003 R2 	        |  5.2 |
| Windows Vista, Server 2008 	          |  6.0 |
| Windows 7, Server 2008 R2 	          |  6.1 |
| Windows 8, Server 2012 	              |  6.2 |
| Windows 8.1, Server 2012 R2 	        |  6.3 |
| Windows 10, Server 2016, Server 2019 	| 10.0 |

<br>

## OS Structure

| Directory | Function |
| :-:| :-:|
| Perflogs | Can hold Windows performance logs but is empty by default. |
|Program Files | 	On 32-bit systems, all 16-bit and 32-bit programs are installed here. On 64-bit systems, only 64-bit programs are installed here. |
| Program Files (x86) | 	32-bit and 16-bit programs are installed here on 64-bit editions of Windows. |
| ProgramData | 	This is a hidden folder that contains data that is essential for certain installed programs to run. This data is accessible by the program no matter what user is running it. | 
| Users 	| This folder contains user profiles for each user that logs onto the system and contains the two folders Public and Default. | 
| Default |	This is the default user profile template for all created users. Whenever a new user is added to the system, their profile is based on the Default profile. |
| Public 	| This folder is intended for computer users to share files and is accessible to all users by default. This folder is shared over the network by default but requires a valid network account to access. |
| AppData 	|Per user application data and settings are stored in a hidden user subfolder (i.e., cliff.moore\AppData). Each of these folders contains three subfolders. The Roaming folder contains machine-independent data that should follow the user's profile, such as custom dictionaries. The Local folder is specific to the computer itself and is never synchronized across the network. LocalLow is similar to the Local folder, but it has a lower data integrity level. Therefore it can be used, for example, by a web browser set to protected or safe mode.|
|Windows 	| The majority of the files required for the Windows operating system are contained here. | 
| System, System32, SysWOW64 |  	Contains all DLLs required for the core features of Windows and the Windows API. The operating system searches these folders any time a program asks to load a DLL without specifying an absolute path. |
| WinSxS |  	The Windows Component Store contains a copy of all Windows components, updates, and service packs. |

<br>

## NTFS permissions
|Permission Type | 	Description |
| :-:| :-:|
|Full Control 	      |  Allows reading, writing, changing, deleting of files/folders. | 
|Modify 	            |  Allows reading, writing, and deleting of files/folders. | 
|List Folder Contents |	Allows for viewing and listing folders and subfolders as well as executing files. Folders only inherit this permission. |
|Read and Execute 	  |  Allows for viewing and listing files and subfolders as well as executing files. Files and folders inherit this permission. |
|Write 	              |  Allows for adding files to folders and subfolders and writing to a file. | 
|Read 	              |  Allows for viewing and listing of folders and subfolders and viewing a file's contents. |
|Traverse Folder 	    |     This allows or denies the ability to move through folders to reach other files or folders. For example, a user may not have permission to list the directory contents or view files in the documents or web apps directory in this example c:\users\bsmith\documents\webapps\backups\backup_02042020.zip but with Traverse Folder permissions applied, they can access the backup archive.|

<br>

## Inheritance settings

| Abreviattion | Meaning |
| :-:| :-:| 
| (CI)  |  container inherit | 
| (OI)  |  object inherit| 
| (IO)  |  inherit only| 
| (NP)  |  do not propagate inherit | 
| (I)   | permission inherited from parent container |

<br>

## Services that require reboot 
| Service 	| Description| 
| :-:| :-:|
| smss.exe 	| Session Manager SubSystem. Responsible for handling sessions on the system.|
| csrss.exe  |	Client Server Runtime Process. The user-mode portion of the Windows subsystem.|
| wininit.exe  |	Starts the Wininit file .ini file that lists all of the changes to be made to Windows when the computer is restarted after installing a program.|
|logonui.exe 	| Used for facilitating user login into a PC|
|lsass.exe 	|The Local Security Authentication Server verifies the validity of user logons to a PC or server. It generates the process responsible for authenticating users for the Winlogon service.|
|services.exe |	Manages the operation of starting and stopping services.|
|winlogon.exe |	Responsible for handling the secure attention sequence, loading a user profile on logon, and locking the computer when a screensaver is running. |
|System  |	A background system process that runs the Windows kernel. |
|svchost.exe with RPCSS |	Manages system services that run from dynamic-link libraries (files with the extension .dll) such as "Automatic Updates," "Windows Firewall," and "Plug and Play." Uses the Remote Procedure Call (RPC) Service (RPCSS).|
|svchost.exe with Dcom/PnP  |	Manages system services that run from dynamic-link libraries (files with the extension .dll) such as "Automatic Updates," "Windows Firewall," and "Plug and Play." Uses the Distributed Component Object Model (DCOM) and Plug and Play (PnP) services. |

## SID (Security Identified)

It is what distinguishes users regarding their privileges

**Structure:** (SID)-(revision level)-(identifier-authority)-(subauthority1)-(subauthority2)-(etc)

### Breakdown
Example SID: S-1-5-21-2614195641-1726409526-3792725429-1002

| "Number" | Meaning | Description | 
| :-:| :-:| :-:|
| S | SID | The string is an SID | 
| 1 | Revision Level | until now, never changed (always 1) |
| 5 | Identified-Authority | Computer/network who created the SID<br>(48-bit string) | 
| 21 | Subauthority 1 | User to SID creator relation | 
| 2614195641-1726409526-37927254 | Subauthority 2 | Computer/domain that created the number | 
| 1002 | Subauthority 3 | What type of user <br> (normal, guest, administrator, port of other group) |  

## Registry


| Value |  Type | 
| :-: |:-:| 
| REG_BINARY             	| Binary data in any form. |
| REG_DWORD 	            | A 32-bit number. |
| REG_DWORD_LITTLE_ENDIAN 	| A 32-bit number in little-endian format. Windows is designed to run on little-endian computer architectures. <br> Therefore, this value is defined as REG_DWORD in the Windows header files. |
| REG_DWORD_BIG_ENDIAN 	    | A 32-bit number in big-endian format. Some UNIX systems support big-endian architectures.| 
| REG_EXPAND_SZ 	        |     A null-terminated string that contains unexpanded references to environment variables (for example, "%PATH%"). <br> It will be a Unicode or ANSI string depending on whether you use the Unicode or ANSI functions. <br>To expand the environment variable references, use the ExpandEnvironmentStrings function. |
| REG_LINK 	                | A null-terminated Unicode string containing the target path of a symbolic link created by calling the RegCreateKeyEx function with REG_OPTION_CREATE_LINK.| 
| REG_MULTI_SZ 	            | A sequence of null-terminated strings, terminated by an empty string (\0). |
| REG_NONE 	                | No defined value type.
| REG_QWORD 	            | A 64-bit number.
| REG_QWORD_LITTLE_ENDIAN 	| A 64-bit number in little-endian format. Windows is designed to run on little-endian computer architectures.<br> Therefore, this value is defined as REG_QWORD in the Windows header files. |
| REG_SZ 	                | A null-terminated string. This will be either a Unicode or an ANSI string, depending on whether you use the Unicode or ANSI functions. |