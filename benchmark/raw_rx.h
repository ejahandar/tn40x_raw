#ifndef __RAWRX_H
#define __RAWRX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>    
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/if.h>

#include "common.h"
#include "bulk_trxd.h"

struct rawrx_helper{
	int fd;
	char ifname[IFNAMSIZ];
	bool raw_device_mode;
	int proto;
	struct bulk_trx_d * bulkptr;
};

struct rawrx_helper * rawrx_alloc(void);
int rawrx_open(struct rawrx_helper * rxdev, char * ifname, bool raw_dev_mode);
int rawrx_rawsocket_recv(struct rawrx_helper * rxdev, char * buff, int len);
int rawrx_bulk_recv(struct rawrx_helper * rxdev);
int rawrx_close(struct rawrx_helper * rxdev);

#endif