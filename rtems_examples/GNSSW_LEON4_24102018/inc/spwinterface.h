/* 
 * File:   spwinterface.h
 * Author: SJWI
 *
 * Created on September 3, 2018, 10:36 AM
 */

#ifndef SPWINTERFACE_H
#define SPWINTERFACE_H

#include <rtems.h>
#include "grspw_pkt_lib.h"

/* SpaceWire parameters */
#define SPW_PROT_ID 155
#define PKT_SIZE 100005

typedef struct {
	/* GRSPW Device layout - must be the same as 'struct grspw_dev' */
	void *dh;
	void *dma[4];
	int index;
	struct grspw_hw_sup hwsup;

	/* Test structures */
	struct grspw_config cfg;
	int run;

	/* RX and TX lists with packet buffers */
	struct grspw_list rx_list, tx_list, tx_buf_list;
	int rx_list_cnt, tx_list_cnt, tx_buf_list_cnt;
}grspw_device;

#define DEV(device) ((struct grspw_dev *)(device))
#define DEVS_MAX 1

//struct grspw_device devs[DEVS_MAX];

extern int router_setup_custom(void);
extern int router_print_port_status(void);

/* SpaceWire Routing table entry */
typedef struct{
	unsigned char dstadr[16];	/* 0 terminates array */
}route_entry;

/* SpaceWire packet payload (data) content layout */
typedef struct{
	unsigned char addr;
	unsigned char protid;
	unsigned char port_src; /* port index of source */
	unsigned char resv2; /* Zero for now */
	unsigned int data[(PKT_SIZE-4)/4];
}pkt_hdr;

typedef struct{
	struct grspw_pkt p;
	unsigned long long data[PKT_SIZE/8+1]; /* 32 bytes of data - 4byte data-header (8 extra bytes to avoid truncated bad packets)*/
	unsigned long long hdr[2]; /* up to 16byte header (path address) */
}spwpkt;

/* All packet buffers used by application */
spwpkt pkts[DEVS_MAX][16+120];


#endif /* SPWINTERFACE_H */

