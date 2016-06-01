#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "common.h"

int send_raw_ether_frame(bool direct); // defined in raw_tx

int main(int argc, char *argv[]){
	
	bool direct = false;
	
	if(argc > 1 && strcmp(argv[1], "direct") == 0){
		direct = true;
	}
	
	if(direct){
		printf("Direct mode activated\n");
	}else{
		printf("RAW Socket mode activated\n");
	}
	
	send_raw_ether_frame(direct);
	
	return 0;
}