#include "common.h"

void print_hex(char * data, int len){
	int i = 0;
	do{
		printf("0x%x	", (unsigned char) data[i]);
		i++;
		
		if(i%8 == 0 && i != 0){
			printf("	");
		}
		if(i%16 == 0 && i != 0){
			printf("\n");
		}
	}while(i != len);
	printf("\n");
}