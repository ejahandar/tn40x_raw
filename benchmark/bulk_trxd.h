#ifndef __BULK_TRXD_H
#define __BULK_TRXD_H

#define RAW_ACCESS_TRX_MAX_FRAMES	100
#define RAW_ACCESS_MAX_FRAME_SIZE	1514

struct bulk_trx_d{
	int no_frames;
	short frame_size[RAW_ACCESS_TRX_MAX_FRAMES];
	char frame_payload[RAW_ACCESS_TRX_MAX_FRAMES][RAW_ACCESS_MAX_FRAME_SIZE];
};

#endif