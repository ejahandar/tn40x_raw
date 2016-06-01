#include "frame_generator.h"

unsigned short csum(unsigned short *ptr,int nbytes);

void construct_udp(char * frame, char * data, short data_len, short src_port, short dest_port){
	ASSERT(data_len > 1472);
	ASSERT(data_len < 18);
	
	struct udphdr *udph = (struct udphdr *) (frame + FG_IP_HDR_LEN + FG_ETH_HDR_LEN);
	char * data_ptr = (char *) udph + FG_UDP_HDR_LEN;
	
	//TCP Header
    udph->source = htons (src_port);
    udph->dest = htons (dest_port);
	udph->len = htons(data_len + FG_UDP_HDR_LEN);
	udph->check = 0;
	
	memcpy(data_ptr, data, data_len);
}

void construct_tcp_syn(char * frame, char * data, short data_len, short src_port, short dest_port){
	
	ASSERT(data_len > 1460);
	ASSERT(data_len < 6);
	
	struct tcphdr *tcph = (struct tcphdr *) (frame + FG_IP_HDR_LEN + FG_ETH_HDR_LEN);
	char * data_ptr = (char *) tcph + FG_TCP_HDR_LEN;
	
	//TCP Header
    tcph->source = htons (src_port);
    tcph->dest = htons (dest_port);
    tcph->seq = 0;
    tcph->ack_seq = 0;
	tcph->res1 = 0;
    tcph->doff = 5;
    tcph->fin=0;
    tcph->syn=1;
    tcph->rst=0;
    tcph->psh=0;
    tcph->ack=0;
    tcph->urg=0;
    tcph->window = htons (5840); /* maximum allowed window size */
    tcph->check = 0;
    tcph->urg_ptr = 0;
	
	memcpy(data_ptr, data, data_len);
}

void construct_ipv4(char * frame, uint8_t proto, uint16_t proto_payload_length, uint32_t saddr, uint32_t daddr, uint16_t id, uint8_t ttl){
	
	struct iphdr *iph = (struct iphdr *) (frame + FG_ETH_HDR_LEN);
	
	//IP Header
	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0;
	iph->id = htonl (id); //Id of this packet
	iph->frag_off = 0;
	iph->ttl = ttl;
	iph->protocol = proto;
	iph->check = 0;
	iph->saddr = saddr;
	iph->daddr = daddr;
	
	//IP Header (used to set tcp length)
	if(proto == IPPROTO_TCP){
		iph->tot_len = htons(FG_TCP_HDR_LEN + FG_IP_HDR_LEN + proto_payload_length);
	}else if(proto == IPPROTO_UDP){
		iph->tot_len = htons(FG_IP_HDR_LEN + FG_UDP_HDR_LEN + proto_payload_length);
	}
}

int construct_ipv4_frame(char * frame, uint8_t * src_mac, uint8_t * dest_mac){
	struct ether_header *eh = (struct ether_header *) frame;
	
	uint8_t i;
	for(i=0;i<ETH_ALEN;i++){
		eh->ether_shost[i] = src_mac[i];
		eh->ether_dhost[i] = dest_mac[i];
	}
	
	eh->ether_type = htons(ETH_P_IP);
	
	struct iphdr *iph = (struct iphdr *)( frame + FG_ETH_HDR_LEN);
	
	iph->check = 0;
	iph->check = csum ((unsigned short *) (frame + FG_ETH_HDR_LEN), FG_IP_HDR_LEN);
	
	if(iph->protocol == IPPROTO_TCP){
		#ifndef ENABLE_HW_CHECKSUM_OFFLOAD
		struct tcphdr *tcph = (struct tcphdr *) (frame + FG_IP_HDR_LEN + FG_ETH_HDR_LEN);
		struct pseudo_header psh;
		int tcp_len = ntohs(iph->tot_len) - FG_IP_HDR_LEN;
		psh.source_address = iph->saddr;
		psh.dest_address = iph->daddr;
		psh.placeholder = 0;
		psh.protocol = IPPROTO_TCP;
		psh.tcp_length = htons(tcp_len);
		 
		int psize = sizeof(struct pseudo_header) + tcp_len;
		char pseudogram[1514];
		 
		memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
		memcpy(pseudogram + sizeof(struct pseudo_header) , tcph , ntohs(iph->tot_len) - FG_IP_HDR_LEN);
		
		
		short check = csum( (unsigned short*) pseudogram , psize);
		tcph->check = check;
		#endif
	}
	
	return FG_ETH_HDR_LEN + ntohs(iph->tot_len);
}

int generate_ipv4_test_frame(char * frame, uint8_t * src_mac, uint8_t * dest_mac){
	
	int data_len = 512;
	short src_port = 100;
	short dest_port = 200;
	
	uint32_t src_ip = inet_addr("10.42.0.1");
	uint32_t dest_ip = inet_addr("10.42.0.3");
	
	unsigned char data[1500];
	
	memset(frame, 0,1514);
	memset(data, 0, sizeof(data));
	
	int i;
	for(i=0;i<data_len;i++){
		data[i] = 0xFF;
	}
	for(i=8;i>0;i--){
		data[data_len-i] = '0' + i;
	}
	
	//construct_tcp_syn(frame, (char *)  &data, data_len, src_port, dest_port);
	// construct_ipv4(frame, IPPROTO_TCP, data_len, src_ip, dest_ip, 1, 255);
	
	construct_udp(frame, (char *)  &data, data_len, src_port, dest_port);
	construct_ipv4(frame, IPPROTO_UDP, data_len, src_ip, dest_ip, 1, 255);
	
	int frame_len = construct_ipv4_frame(frame, src_mac, dest_mac);
	
	return frame_len;
}

unsigned short csum(unsigned short *ptr,int nbytes){
    register long sum;
    unsigned short oddbyte;
    register short answer;
 
    sum=0;
    while(nbytes>1) {
	    sum+=*ptr++;
	    nbytes-=2;
	}
    if(nbytes==1) {
	    oddbyte=0;
		*((u_char*)&oddbyte)=*(u_char*)ptr;
	    sum+=oddbyte;
	}
 
    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;
	 
    return(answer);
}
