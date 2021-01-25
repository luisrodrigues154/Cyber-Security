/*
	Expected input: <prog> day month year
	Packed data structur: ddddd-mmmm-yyyyyyy
*/

#include <stdio.h>
#include <stdlib.h>

#define YEAR_SIZE 7
#define MONTH_SIZE 4
#define DAY_SIZE 5
#define MAX_BITS 15
unsigned int toDate(unsigned int, unsigned int, unsigned int);
unsigned int packIt(unsigned int curr_mask, unsigned int val, unsigned int startMask, unsigned int endMask);
void printBin(unsigned int);
unsigned int extractMonth(unsigned int);
unsigned int extractDay(unsigned int);
unsigned int extractYear(unsigned int);

int main(int argc, char ** argv){
	unsigned int packedDate = toDate(19, 9, 1995);
	puts("\n\nExtraction ....");

	printf("The day is: \t%d\n", extractDay(packedDate));
	printf("The month is: \t%d\n", extractMonth(packedDate));
	printf("The year is: \t%d\n", extractYear(packedDate));
	return 0;
}

void printBin(unsigned int value){
	for(int i = 31; i >= 0 ; i--) putchar((value >> i) & 0x1 ? '1' : '0');
	putchar('\n');
}
unsigned int extractMonth(unsigned int p){
	unsigned int month = 0;
	p = (p >> YEAR_SIZE);
	for(int i = 0; i < MONTH_SIZE; i++) month |= (p & (1 << i));
	return month;
}
unsigned int extractDay(unsigned int p){
	return (p >> (YEAR_SIZE + MONTH_SIZE));
}
unsigned int extractYear(unsigned int p){
	unsigned int year = 0;	
	for(int i = 0; i < YEAR_SIZE; i++)	year |= (p & (1 << i)); 
	return year;
}

unsigned int packIt(unsigned int currPack, unsigned int val, unsigned int size, unsigned int shifter){
	currPack |= (val << shifter);
	return currPack;
}

unsigned int toDate(unsigned int day, unsigned int month, unsigned int year){
	unsigned int packed = 0;
	// to write 99 (year) needs 	7 bits 0-6th bits	
	// to write 12 (month) needs 	4 bits 7th-10th bits
	// to write 31 (day) needs 		5 bits 11th-15th bits
	// total --------------------  16-bits
	// format dddd mmmmm yyyyyyy

	printf("day: %d .. \t", day);
	printBin(day);
	packed = packIt(packed, day, DAY_SIZE, 11);

	printf("month: %d .. \t", month);
	printBin(month);
	packed = packIt(packed, month, MONTH_SIZE, 7);

	printf("year: %d .. \t", year);
	year %= 100;
	printBin(year);
	packed = packIt(packed, year, YEAR_SIZE, 0);
	puts("-----------------------------------");
	printf("packed: %d .. ", packed);
	printBin(packed);

	return packed;
}	


