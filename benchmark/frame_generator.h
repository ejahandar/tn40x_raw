#include <stdlib.h>
#include <stdio.h>
#include <netinet/tcp.h>   //Provides declarations for tcp header
#include <netinet/udp.h>   //Provides declarations for udp header
#include <netinet/ip.h>	//Provides declarations for ip header
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/ether.h>

#include "common.h"


#define ASSERT(arg)		if(arg){ printf("ASSERTION : %i\n", __LINE__); exit(-1); }

#define FG_ETH_HDR_LEN		14
#define FG_IP_HDR_LEN		20
#define FG_TCP_HDR_LEN		20
#define FG_UDP_HDR_LEN		8

//#define ENABLE_HW_CHECKSUM_OFFLOAD

struct pseudo_header{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;
};

void construct_tcp_syn(char * frame, char * data, short data_len, short src_port, short dest_port);
void construct_udp(char * frame, char * data, short data_len, short src_port, short dest_port);
void construct_ipv4(char * frame, uint8_t proto, uint16_t proto_payload_length, uint32_t saddr, uint32_t daddr, uint16_t id, uint8_t ttl);
int construct_ipv4_frame(char * frame, uint8_t * src_mac, uint8_t * dest_mac);
int generate_ipv4_test_frame(char * frame, uint8_t * src_mac, uint8_t * dest_mac);
