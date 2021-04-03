#!/bin/bash

echo -e "Assemlbing..."
nasm -f elf32 code.asm

echo -e "Linking..."
ld -melf_i386 -s -o runnable code.o

echo -e "############ -> EXECUTION BELOW <- ###################\n"
#./runnable

echo -e "\n############ ->    SHELL CODE   <- ###################"
objdump -d ./code.o|grep '[0-9a-f]:'|grep -v 'file'|cut -f2 -d:|cut -f1-6 -d' '|tr -s ' '|tr '\t' ' '|sed 's/ $//g'|sed 's/ /\\x/g'|paste -d '' -s |sed 's/^/"/'|sed 's/$/"/g'