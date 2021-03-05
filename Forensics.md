# Forensics 

<br>

## Memory Dumps

### LIVE memory

| Program | OS |
|:-:| :-:|
|FTK imager | Linux/Windows(?) |
|Dumplt | Windows |
| win32dd/win64dd | windows
| LiME | Linux |

<br>

### Memory information from shutdowned systems 

<b>WINDOWS only</b>: the file stores the last previous boot image (compressed): %SystemDrive%/hiberfil.sys <br>

<br>

### Virtual machines 
| Hypervisor | file |
| :-:| :-:|
| VMware | .vmem |
| Hyper-V | .bin |
| Parallels | .mem |
| VirtualBox | .sav (partial) |

### Checking for malicious behaviors (windows, volatility)

- psxview - check for hiden processes (with False columns)
- ldrmodules - check if ANY False (indicates injection)
- apihooks - with module = \<unknown>