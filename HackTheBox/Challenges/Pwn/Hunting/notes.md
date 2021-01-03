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

Since the ghidra and binary ninja decompilation was pretty innacurate, i downloaded IDA pro and use its decompiler (which came better than the others)
```
int main(){
    void *addr; // ST2C_4
    void *buf; // ST24_4
    int v3; // [esp-10h] [ebp-24h]
    int v4; // [esp-Ch] [ebp-20h]
    int v5; // [esp-8h] [ebp-1Ch]
    int v6; // [esp-4h] [ebp-18h]
    char *dest; // [esp+4h] [ebp-10h]

    addr = (void *)sub_12E8();
    signal(14, (__sighandler_t)&exit);
    alarm(3u);
    dest = (char *)mmap(addr, 0x1000u, 3, 49, -1, 0);
    if ( dest == (char *)-1 )
        sub_1118(-1, v3, v4, v5);
    strcpy(dest, aHtbXxxxxxxxxxx);
    memset(aHtbXxxxxxxxxxx, 0, 0x25u);
    sub_1259();
    buf = malloc(0x3Cu);
    read(0, buf, 0x3Cu);
    ((void (__stdcall *)(int, void *, _DWORD))buf)(v6, buf, 0);
    return 0;
}
```

Although the decompilation is better, i dont have a clue on how to exploit this program... Maybe further down the road <br>

 