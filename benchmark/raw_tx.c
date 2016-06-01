#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/tcp.h>   //Provides declarations for tcp header
#include <netinet/ip.h>    //Provides declarations for ip header
#include <sys/socket.h>    //for socket ofcourse
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#include "common.h"
#include "frame_generator.h"

#define MY_DEST_MAC0	0xb0
#define MY_DEST_MAC1	0xc5
#define MY_DEST_MAC2	0x54
#define MY_DEST_MAC3	0xff
#define MY_DEST_MAC4	0xf3
#define MY_DEST_MAC5	0xb1

#define RAW_ACCESS_TRX_MAX_FRAMES	100
#define RAW_ACCESS_MAX_FRAME_SIZE	1514

struct bulk_trx_d{
	int no_frames;
	short frame_size[RAW_ACCESS_TRX_MAX_FRAMES];
	char frame_payload[RAW_ACCESS_TRX_MAX_FRAMES][RAW_ACCESS_MAX_FRAME_SIZE];
};


int send_raw_ether_frame(bool direct){
	int sockfd;
	struct ifreq if_idx;
	struct ifreq if_mac;
	int frame_len = 0;
	struct sockaddr_ll socket_address;
	char ifName[IFNAMSIZ];
	char frame[1514];
	
	strcpy(ifName, "eth1");

	/* Open RAW socket to send on */
	if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
	    perror("socket");
	}

	/* Get the index of the interface to send on */
	memset(&if_idx, 0, sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
		perror("SIOCGIFINDEX");
		
	/* Get the MAC address of the interface to send on */
	memset(&if_mac, 0, sizeof(struct ifreq));
	strncpy(if_mac.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
		perror("SIOCGIFHWADDR");
	
	memset(frame, 0, sizeof(frame));
	
	uint8_t dest_mac[6] = {
		MY_DEST_MAC0,
		MY_DEST_MAC1,
		MY_DEST_MAC2,
		MY_DEST_MAC3,
		MY_DEST_MAC4,
		MY_DEST_MAC5
	};
	
	uint8_t src_mac[6] = {
		((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0],
		((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1],
		((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2],
		((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3],
		((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4],
		((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5]
	};
	
	/* Index of the network device */
	socket_address.sll_ifindex = if_idx.ifr_ifindex;
	
	/* Address length*/
	socket_address.sll_halen = ETH_ALEN;
	
	/* Destination MAC */
	socket_address.sll_addr[0] = MY_DEST_MAC0;
	socket_address.sll_addr[1] = MY_DEST_MAC1;
	socket_address.sll_addr[2] = MY_DEST_MAC2;
	socket_address.sll_addr[3] = MY_DEST_MAC3;
	socket_address.sll_addr[4] = MY_DEST_MAC4;
	socket_address.sll_addr[5] = MY_DEST_MAC5;

	frame_len = generate_ipv4_test_frame((char *) &frame, (uint8_t *) &src_mac, (uint8_t *) &dest_mac);
	
	if(!direct){
		do{
			sendto(sockfd, (char *) &frame, frame_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll));
		}while(1);
	}else{
		int no_frames = 100;
		struct bulk_trx_d trx_desc;
		
		int fd = open("/dev/eth1", O_WRONLY, 640);
		int i;
		
		trx_desc.no_frames = no_frames;
		
		for(i=0;i<no_frames;i++){
			trx_desc.frame_size[i] = frame_len;
			memcpy((char *) &trx_desc.frame_payload[i], (char *) &frame, frame_len);
		}
		do{
			write(fd, &trx_desc, sizeof(trx_desc));
			usleep(2);
			 
		}while(1);
	}
	
	return 0;
}