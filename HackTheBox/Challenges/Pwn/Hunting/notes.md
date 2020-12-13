# Hunting

# Solution

Check the binary security

```
cmd: checksec --file=hunting

Arch:     i386-32-little
RELRO:    Partial RELRO
Stack:    No canary found
NX:       NX disabled
PIE:      PIE enabled
RWX:      Has RWX segments
```

Ok... not much security ... <br>

Running the program and providing any input gives a seg fault

```
27692 segmentation fault (core dumped)  ./hunting
```

Lets load it in ghidra <br>

From the code it is visible that the program registers an handler for the SIGALARM and launch an alarm for 3 seconds
```
signal(0xe,exit); <- call exit() when handling SIGALARM
alarm(3);
```

Since the ghidra code was hard to understand i used binary ninja to verify if it generates better code.. it did and after some renaming it is easier to analyze
```
int32_t set_security() (s fcn.00001259)
{
    int16_t var_14 = 0xe
    if (prctl(0x26, 1, 0, 0, 0) s< 0)
    {
        perror("prctl(PR_SET_NO_NEW_PRIVS)")
        exit(status: 2)
        noreturn
    }
    int32_t eax_1 = prctl(0x16, 2, &var_14)
    if (eax_1 s>= 0)
        return eax_1
    perror("prctl(PR_SET_SECCOMP)")
    exit(status: 2)
    noreturn
}

int32_t get_address() (s fcn.00001170)
{
    int32_t file = open("/dev/urandom", 0)
    int32_t rand_bytes
    read(file, &rand_bytes, 8)
    close(file)
    srand(rand_bytes)
    int32_t new_addr = 0
    while ((new_addr < 0x60000000) || (0xf7000000 < new_addr))
        new_addr = rand() << 0x10
    return new_addr
  
}

int32_t main(int32_t argc, char** argv, char** envp)
{
    int32_t addr = get_address()
    signal(0xe, exit)
    alarm(3)
    int32_t new_mapping = mmap(addr, 0x1000, 3, 0x31, 0xffffffff, 0)
    if (new_mapping != 0xffffffff){
        strcpy(new_mapping, flag)
        memset(flag, 0, 0x25)
        set_security()
        int32_t user_input = malloc(0x3c)
        read(0, user_input, 0x3c)
        user_input()
        return 0
    }
    exit(status: 0xffffffff)
    noreturn
}
```

The main function calls 
