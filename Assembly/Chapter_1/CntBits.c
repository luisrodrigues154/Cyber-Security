#include <stdio.h>
#include <stdlib.h>
/*
    Counts the number of 1 set bits in a 16-bit value and test a specific bit...
    It does not not consider bigger numbers, manual conversion, etc. 
    Just a simple quick program
*/

int cntBits(unsigned int);
void pBin(unsigned int);
int isBitSet(unsigned int val, int pos);

int main(int argc, char ** argv){
    unsigned int value = atoi(argv[1]);
    unsigned int pos = atoi(argv[2]);
    printf("Value: %d .. bits: ", value);
    pBin(value);
    printf("number of 1's: %d\n", cntBits(value));
    printf("Bit on pos %d (0-15th) is %s\n", pos, isBitSet(value, pos) ? "SET" : "NOT-SET");
    return 0;
}

void pBin(unsigned int val){
    for(int i = 15; i >= 0; i--) putchar(( val >> i) & 0x1 ? '1' : '0');
    putchar('\n');
}

int isBitSet(unsigned int val, int pos){
    if(pos > 15 || pos < 0) return 0;
    
    return (val >> pos) & 0x1 ? 1 : 0;
}

int cntBits(unsigned int val){
    int n = 0;
    for(int i = 0; i<16; i++) {
       if((val >> i ) & 0x1) n++;
    }
    return n;
}