#include <stdio.h>
#include <stdlib.h>

long long * charToInt(char * str);

int main(int argc, char ** argv){
	long long * result = charToInt(argv[1]);
	
	if(result[1] != NULL)	printf("Value: %lld.%lld\n", result[0], result[1]);
	else 					printf("Value: %lld\n", result[0]);
	
	return 0;
}

long long * charToInt(char * str){
	long long * ret = (long long *) malloc(sizeof(long long) * 2);
	short goDecimal = 0;
	
	ret[0] = 0;
	ret[1] = NULL;
	for(; *str != 0; str++){
		if(*str == '.' || *str == ','){
			goDecimal |= (1 << 0x0);
			ret[1] = 0;
			continue;
		}
		long long value = 0;

		if(*str >= '0' && *str <= '9')	value = *str - '0';
		else							value = (long long) *str;
		
		ret[goDecimal] *= 10;
		ret[goDecimal] += value;
		
	}
	return ret;
}
