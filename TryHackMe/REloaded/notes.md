# REloaded

Reverse engineering challenges

## Level 0

They give an exe as the challenge's files. Checking the strings we got the flag...

**Flag:** L3v3lZ340_is_D02e

## Level 1

Well, by simple analysis from ghidra's decompilation we can see that the program needs at least a parameter (param_1 -> argc -> number of arguments). It then converts the second argv element (first argument) and converts it to integer via **atoi**
```c
if (param_1 < 2) {
    printf("N00b a day, pro for life");
}
else {
    iVar1 = atoi(*(char **)(param_2 + 4));
    FUN_00401410(iVar1);
}
```

Entering **FUN_00401410** we can see the flag 
```c
if (param_1 == 0x6ad) {
    printf("Thats ur lucky number !!!");
}
```

0x6ad (h) -> 1709 (d)

**Flag:** 1702 

## Level 2

This level is quite easy too, but requires more "work". <br>

```c
if (param_1 < 2) {              // argc needs to be at least 2
    printf("Wow Ur At L3?");
}
else {
    pcVar1 = *(char **)(param_2 + 4);
    FUN_0040147f();
    FUN_00401410(pcVar1);       // check this
}
```

Checking that function, we can see several things
```c
void __cdecl FUN_00401410(char *param_1)
{
  int iVar1;
  undefined4 local_20;              // buffer start 
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  
  local_20 = 0x315f334c;
  local_1c = 0x30325f73;
  local_18 = 0x68545f74;
  local_14 = 0x314c5f33;
  local_10 = 0x743133;
  iVar1 = strcmp((char *)&local_20,param_1);

  if (iVar1 == 0) {                 // correct password as argv1
    puts("Get Ready For L4 ;)");
    printf("%s",&local_20);         // it prints the string from the buffer which is the flag
  }
  else {
    printf("In order to advance you have to break your mindset");
  }
  return;
}
```

To solve this, we have 2 approaches:
- patch the instruction for the branch (comparison made with TEST eax,eax) 
  - Current OPCODE: JNZ
  - Patch to OPCODE: JZ (basically just invert the opcode)
- decode the string
  - Caveat: little endian.

The string, converted to char (using ghidra functionality), ends like so
```r
00401416 c7 45 e4        MOV        dword ptr [EBP + local_20],"1_3L"   # -> L3_1
0040141d c7 45 e8        MOV        dword ptr [EBP + local_1c],"02_s"   # -> s_20
00401424 c7 45 ec        MOV        dword ptr [EBP + local_18],"hT_t"   # -> t_Th  
0040142b c7 45 f0        MOV        dword ptr [EBP + local_14],"1L_3"   # -> 3_L1 
00401432 c7 45 f4        MOV        dword ptr [EBP + local_10],"\0t13"  # -> 31t

Joined: L3_1s_20t_Th3_L131t
```

**Flag:** L3_1s_20t_Th3_L131t

**Which instruction did you modified?** JNZ

## Level 3

First thing i noted was, this program has a buffer overflow might be exploitable but that aint the goal of the challenge though 

The code snippets i'll paste here originate either from ghidra and/or IDA so they might not be compliant with each other meaning that functions and variables might have different name. 
```c
int main(void)
{
  char local_24 [32];
  FUN_0040c050();
  printf("Enter the flag ::");
  scanf("%s",local_24);         // buffer overflow
  decrypt(local_24);            // i renamed this
  return 0;
}
```

If we enter in the decrypt function, we can see interesting stuff
```c

void __cdecl decrypt(char *param_1)
{
  char *pcVar1;
  int iVar2;
  char local_40 [24];
  undefined4 local_28 [6];
  uint local_10;
  
  FUN_00401410(local_28);
  for (local_10 = 0; local_10 < 24; local_10 = local_10 + 1) {
    pcVar1 = (char *)FUN_00495480(local_28,local_10);
    local_40[local_10] = *pcVar1;
  }
  iVar2 = strcmp(param_1,local_40);
  if (iVar2 == 0) {
    printf("Rooted !!!");
  }
  else {
    printf("-_-");
  }
  FUN_00495290(local_28);
  return;
}
```

Well, in this case we want the key for the decryption but if we needed the password then we could just breakpoint at the strcmp and check it (pwd: SOJdsa*K3\a)


We could check the key by iteration at the loop but what about that function? Lets see

```c
// Ghidra
void * __cdecl FUN_00401410(void *param_1)

{
  undefined4 local_1f;
  undefined4 local_1b;
  undefined2 local_17;
  undefined4 local_14;
  uint local_10;
  
  local_1f = 0x634d4854;
  local_1b = 0x4c2d6674;
  local_17 = 0x34;
  local_14 = 7;
  for (local_10 = 0; local_10 < 10; local_10 = local_10 + 1) {
    *(byte *)((int)&local_1f + local_10) = *(byte *)((int)&local_1f + local_10) ^ 7;
  }
  FUN_00462c80();
  FUN_00494c60(param_1,(char *)&local_1f);
  FUN_00462cb0();
  return param_1;
}
```

I dont feel like reversing this (it is simple but aint no one got time for that), lets check IDA decompilation

```c
int __cdecl sub_401410(int a1)
{
  char v2[4]; // [esp+1Dh] [ebp-1Bh]
  char v3[5]; // [esp+27h] [ebp-11h]
  unsigned int i; // [esp+2Ch] [ebp-Ch]

  strcpy(v2, "THMctf-L4");
  *(_DWORD *)&v3[1] = 7;
  for ( i = 0; i <= 9; ++i )
    v2[i] ^= v3[1];
  nullsub_2(v3);
  sub_494C60(v2, (int)v3);
  nullsub_3(v3);
  return a1;

```

Alright, easy enough :)

**Flag:** THMctf-L4

## Level 4

Well this level is kinda stupid since the solution is in the main function because the person who made this was thinking that it was too much effort to insert the flag via other means...

Since he didn't bother i wont bother either to demangle and explain this easy stupid ass "encryption" loop lul

```c
// argument char *Str = Alan Turing Was a Geniuse
int __cdecl sub_401453(char *Str)
{
  char v1; // dl
  signed int v3; // [esp+18h] [ebp-10h]
  signed int i; // [esp+1Ch] [ebp-Ch]

  v3 = strlen(Str);
  for ( i = 0; i < v3; ++i )
  {
    if ( (unsigned __int8)sub_401410(i) )
      v1 = i ^ Str[i];
    else
      v1 = Str[i] ^ 0x37;
    Str[i] = v1;
  }
  return printf("%s", Str);
}
```

This stupid ass challenge is assuming what? that we cannot decompile it? well, if that was the goal it would make more sense ... 

The needed decryptor just needs to reverse all the XOR's made and apply the same conditions. That first if statement within the above for loop just makes further verifications

```c
signed int __cdecl sub_401410(signed int a1)
{
  signed int i; // [esp+Ch] [ebp-4h]

  if ( a1 <= 3 )
    return 1;
  for ( i = 2; i < a1; ++i )
  {
    if ( !(a1 % i) )
      return 0;
  }
  return 1;
}
```

**Flag:** Alan Turing Was a Geniuse

