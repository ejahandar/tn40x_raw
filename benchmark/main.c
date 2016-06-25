#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "common.h"

int send_raw_ether_frame(bool direct); // defined in raw_tx.c
int receive_raw_ether_frame(bool direct); // defined in raw_rx.c

int main(int argc, char *argv[]){
	
	int mode = 0;
	if(argc > 1 && strcmp(argv[1], "rxdirect") == 0){
		mode = 1;
	}else if(argc > 1 && strcmp(argv[1], "rx") == 0){
		mode = 2;
	}else if(argc > 1 && strcmp(argv[1], "txdirect") == 0){
		mode = 3;
	}else if(argc > 1 && strcmp(argv[1], "tx") == 0){
		mode = 4;
	}
	
	switch(mode){
		case 1:
			printf("Direct RX mode activated\n");
			receive_raw_ether_frame(true);
		break;
		case 2:
			printf("RAW Socket RX mode activated\n");
			receive_raw_ether_frame(false);
		break;
		case 3:
			printf("Direct TX mode activated\n");
			send_raw_ether_frame(true);
		break;
		case 4:
			printf("RAW Socket TX mode activated\n");
			send_raw_ether_frame(false);
		break;
		default:
			printf("Valid modes are rx rx_direct tx tx_direct\n");
		break;
	}
	
	return 0;
}