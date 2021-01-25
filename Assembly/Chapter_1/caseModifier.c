/*
	Author: Luís Miguel Rodrigues
	Date: 23/01/2021
	Program: set the string characters' bits to modify their case
*/

#include <stdio.h>
#include <stdlib.h>

#define CASE_MASK 0x5

char toLower(char);
char toUpper(char);
void toB(char);
void errorArgs();

int main(int argc, char ** argv){
	short isU = 0;
	if(argc != 3) errorArgs();
	
	if(argv[1][1] != 'U' && argv[1][1] != 'u')
		if(argv[1][1] != 'l' && argv[1][1] != 'L') 
			errorArgs();
		else
			isU = 0;
	else
		isU |= (1 << 0x0);

	printf("Decider bits: "); toB(isU);
	putchar('\n');	

	char * str = argv[2];
	printf("Input before: %s\n", str);
	
	if(isU){
		printf("Use toUpper\n");
		for(; *str != 0; str++)	*str = toUpper(*str);
	}else{
		printf("Use toLower\n");
		for(; *str != 0; str++) *str = toLower(*str);
	}
	printf("Input after: %s\n", argv[2]);

	return 0;
}

void errorArgs(){
	perror("Usage: <prog> -U/-L <str>");
	exit(1);
}
void toB(char c){
	for(int i = 0; i <= 7; i++) putchar( (c << i) & 0x80 ? '1' : '0' );
}

char toUpper(char c){
	return c &= ~(1<< CASE_MASK);
}

char toLower(char c){
	return c |= (1 << CASE_MASK);
}
