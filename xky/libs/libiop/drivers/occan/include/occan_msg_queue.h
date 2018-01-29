#ifndef __OCCAN_MSG_QUEUE_H__
#define __OCCAN_MSG_QUEUE_H__

#include <air.h>
#include <iop.h>

/* CAN MESSAGE */
typedef struct {
	char extended; /* 1= Extended Frame (29-bit id), 0= STD Frame (11-bit id) */
	char rtr; /* RTR - Remote Transmission Request */
	char sshot; /* single shot */
	unsigned char len;
	unsigned char data[8];
	unsigned int id;
} CANMsg;

typedef struct {
	int cnt;
	int ovcnt;
	int next;
	int last;
	int max;
	CANMsg *fifo;
} occan_fifo;

uint8_t occan_fifo_clear(occan_fifo *queue);
uint8_t occan_fifo_put(occan_fifo *queue, CANMsg *new, int force);
CANMsg  *occan_fifo_get(occan_fifo *queue);
uint8_t occan_fifo_full(occan_fifo *queue);
uint8_t occan_fifo_empty(occan_fifo *queue);

#endif /* __OCCAN_MSG_QUEUE_H__ */
