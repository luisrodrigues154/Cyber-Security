# The Find Command

This is a walkthrough

## create 2 files 

```
touch file-1 file-2
```
## wildcards

```
find . -name "file*" #name start with file
find . -name "*1" #name ends with 1
```

## be more specific

1 - Find all files that end with .xml
```
find / -name "*.xml" -type f
```

2 - Find all files in the /home directory (recursive) whose name is "user.txt" (case insensitive)
```
find /home -type f -iname "user.txt"
```

3 - Find all directories whose name contains the word "exploits"

```
find / -type d -name "*exploits*"
```

## know exactly what you're looking for

1 - Find all files owned by the user "kittycat"
```
find / -type f -user "kittycat"
```

2 - Find all files that are exactly 150 bytes in size
```
find / -type f -size 150c
```

3 - Find all files in the /home directory (recursive) with size less than 2 KiB’s and extension ".txt"
```
find /home -type f -size -2k -name "*.txt"
```

4 - Find all files that are exactly readable and writeable by the owner, and readable by everyone else (use octal format)
```
find / -type f -perm -644
```

5 - Find all files that are only readable by anyone (use octal format)
```
find / -type f -perm /444
```

6-Find all files with write permission for the group "others", regardless of any other permissions, with extension ".sh" (use symbolic format)
```
find / -type f -name "*.sh" -perm -o=w
```

7 - Find all files in the /usr/bin directory (recursive) that are owned by root and have at least the SUID permission (use symbolic format)
```
find /usr/bin -type f -user root -perm -u=s
```

8 - Find all files that were not accessed in the last 10 days with extension ".png"
```
find / -type f -name "*.png" -atime -10
```

9 - Find all files in the /usr/bin directory (recursive) that have been modified within the last 2 hours
```
find /usr/bin -type f -mmin -120
```