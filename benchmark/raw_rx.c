#include "raw_rx.h"

struct rawrx_helper * rawrx_alloc(void){
	struct rawrx_helper * ptr = malloc(sizeof(struct rawrx_helper));
	
	if(ptr != NULL){
		// ETH_P_ALL captures all incoming and outcoming packets
		ptr->proto = ETH_P_IP;
		
		ptr->bulkptr = malloc(sizeof(struct bulk_trx_d));
		
		if(ptr->bulkptr == NULL){
			free(ptr);
			return NULL;
		}
		return ptr;
	}
	
	return NULL;
}

int rawrx_open(struct rawrx_helper * rxdev, char * ifname, bool raw_dev_mode){
	rxdev->raw_device_mode = true;
	if(raw_dev_mode){
		char devname[50];
		sprintf((char *) &devname, "/dev/%s", ifname);
		if((rxdev->fd = open((char *) &devname, O_RDONLY, 640)) < 0){
			printf("could not open %s\n", (char *) &devname);
			return 0;
		}
		
		return 1;
	}else{
		if ((rxdev->fd = socket(AF_PACKET, SOCK_RAW, htons(rxdev->proto))) < 0) {
			printf("could not open socket\n");
			return 0;
		}
		
		struct ifreq buffer;
		memset(&buffer, 0, sizeof(buffer));
		strncpy(buffer.ifr_name, ifname, IFNAMSIZ);
		strncpy(rxdev->ifname, ifname, IFNAMSIZ);
		
		if(ioctl(rxdev->fd, SIOCGIFINDEX, &buffer) < 0) {
			printf("could not get interface index\n");
			close(rxdev->fd);
			return 0;
		}
		
		if (ioctl(rxdev->fd, SIOCGIFHWADDR, &buffer) < 0) {
			printf("could not get interface address\n");
			close(rxdev->fd);
			return 0;
		}
		 
		struct sockaddr_ll sock_address;
		memset(&sock_address, 0, sizeof(sock_address));
		
		sock_address.sll_family = PF_PACKET;
		sock_address.sll_protocol = htons(rxdev->proto);
		sock_address.sll_ifindex = if_nametoindex(ifname);
		sock_address.sll_pkttype = PACKET_BROADCAST;
		if (bind(rxdev->fd, (struct sockaddr*) &sock_address, sizeof(sock_address)) < 0) {
			printf("bind failed\n");
			return 0;
		}
		
		return 1;
	}
	
	return 0;
}

int rawrx_rawsocket_recv(struct rawrx_helper * rxdev, char * buff, int len){
	if(rxdev->raw_device_mode){
		return 0;
	}else{
		return recvfrom(rxdev->fd, buff, len, MSG_DONTWAIT, NULL, NULL);
	}
}

int rawrx_close(struct rawrx_helper * rxdev){
	close(rxdev->fd);
	return 1;
}

int rawrx_bulk_recv(struct rawrx_helper * rxdev){
	
	rxdev->bulkptr->no_frames = 0;
	
	if(rxdev->raw_device_mode){
		memset(rxdev->bulkptr, 0, sizeof(struct bulk_trx_d));
		int bytes_read = read(rxdev->fd, rxdev->bulkptr, sizeof(struct bulk_trx_d));
		printf("RET: %i\n", bytes_read);
		if(bytes_read > 0){ 
			return rxdev->bulkptr->no_frames;
		}else if(bytes_read < 0){
			return -1;
		}
	}else{
		int recv_ret;
		do{
			char * frame_ptr = rxdev->bulkptr->frame_payload[rxdev->bulkptr->no_frames];
			recv_ret = recvfrom(rxdev->fd, frame_ptr, ETH_FRAME_LEN, MSG_DONTWAIT, NULL, NULL);
			if(recv_ret > 0){
				rxdev->bulkptr->frame_size[rxdev->bulkptr->no_frames] = recv_ret;
				rxdev->bulkptr->no_frames++;
			}
		}while(rxdev->bulkptr->no_frames < RAW_ACCESS_TRX_MAX_FRAMES && recv_ret > 0);
		
		return rxdev->bulkptr->no_frames;
	}
	
	return 0;
}

int receive_raw_ether_frame(bool direct){
	struct rawrx_helper * rxdev;	
	rxdev = rawrx_alloc();
	if(rawrx_open(rxdev, "eth1", direct)){
		int recv_pkts = 1;
		while(1){
			int to_recv = rawrx_bulk_recv(rxdev);
			
			if(to_recv < 0){
				return 0;
			}
			int i;
			for(i=0;i<to_recv;i++){
				printf("RECV: %i [%i]\n", recv_pkts, rxdev->bulkptr->frame_size[i]);
				print_hex((char *) &rxdev->bulkptr->frame_payload[i], 6);
				recv_pkts++;
			}
			printf("Received %i packets in this turn\n", to_recv);
			sleep(3);			
		}
	}
	
	return 0;
}