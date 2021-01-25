#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
    Performs rotation for 16 bit values
*/


unsigned short ROL(unsigned short value, int n);
unsigned short ROR(unsigned short value, int n);
void pBin(unsigned short);
int main(int argc, char ** argv){
    if(argc != 3){
        perror("Usage: <prog> value n_rotation");
        exit(1);
    }
    unsigned short value = atoi(argv[1]);
    int n = atoi(argv[2]);
    printf("Value received: %d (", value);
    pBin(value); putchar(')');putchar('\n');
    printf("Rotating (L & R) %d times \n\n", n);
    
    /* ------------------------------------------------------------- */
    unsigned short ROL_value = ROL(value, n);
    printf("Results with %d rotations\nValue: \t%d\n", n, ROL_value);
    printf("Binar: \t"); pBin(ROL_value); putchar('\n');

    putchar('\n');
    unsigned short ROR_value = ROR(value, n);
    printf("Results with %d rotations\nValue: \t%d\n", n, ROR_value);
    printf("Binar: \t"); pBin(ROR_value); putchar('\n');
    putchar('\n');
    return 0;
}

void pBin(unsigned short val){
    for(int i = 15, c = 0; i>=0; i--, c++){
        if(c == 4) {
            putchar(' ');
            c = 0;
        }
        putchar((val >> i) & 0x1 ? '1' : '0');
    } 
    
}

unsigned short ROL(unsigned short value, int n){
    puts("Rotating left (L)");
    unsigned int HObitMask = (1 << 15);
    for(int i = 0; i < n; i++){
        unsigned short bit = (value & HObitMask) ? 1 : 0;
        value <<=1;
        value |= bit & 0x1;   
    }
    return value;
}

unsigned short ROR(unsigned short value, int n){
    puts("Rotating right (R)");
    unsigned int HObitMask = (1 << 15);
    for(int i = 0; i < n; i++){
        unsigned short bit = (value & 0x1) ? 1 : 0;
        // printf("Starting bin: ");
        // pBin(value);
        // printf("Bit to pop: %i\n", bit);
        value >>= 1;
        value |= ((bit << 15) & HObitMask);
        // printf("New bin: ");
        // pBin(value);
    }
    return value;
}