
#ifndef __OCCANLIB_H__
#define __OCCANLIB_H__

/* include driver definitions for CANMsg
 * struct.
 *
 */
#include <occan.h>

typedef struct {
	int fd;
	int txblk;
	int rxblk;
} occan_s;

typedef occan_s *occan_t;

/* Open device driver named 'name'. 
 *  - /dev/occan0           (ON-CHIP BUS)
 *  - /dev/occan1           (ON-CHIP BUS)
 *  ...
 *  - /dev/gr701_0/occan0   (GR-701 PCI TARGET's AMBA BUS)
 *
 * Returns a dynamically allocated (malloced) 
 * pointer to a occan_s structure used in following
 * calls to occan_lib*()
 */
occan_t occanlib_open(char *name);

/* Closes /dev/occanX and frees chan */
void occanlib_close(occan_t chan);

int occanlib_send_multiple(occan_t chan, CANMsg *msgs, int msgcnt);

int occanlib_send(occan_t chan, CANMsg *msg);

int occanlib_recv_multiple(occan_t chan, CANMsg *msgs, int msgcnt);

int occanlib_recv(occan_t chan, CANMsg *msg);

int occanlib_set_speed(occan_t chan, unsigned int speed);

int occanlib_set_speed_auto(occan_t chan);

int occanlib_set_btrs(occan_t chan, unsigned char btr0, unsigned char btr1);

int occanlib_set_buf_length(occan_t chan, unsigned short txlen, unsigned short rxlen);

int occanlib_get_stats(occan_t chan, occan_stats *stats);

int occanlib_get_status(occan_t chan, unsigned int *status);

int occanlib_set_filter(occan_t chan, struct occan_afilter *new_filter);

int occanlib_set_blocking_mode(occan_t chan, int txblocking, int rxblocking);

int occanlib_start(occan_t chan);

int occanlib_stop(occan_t chan);

void occanlib_stats_summary_print(occan_stats *stats);

void occanlib_stats_arblost_print(occan_stats *stats);

void occanlib_stats_buserr_print(occan_stats *stats);

void occanlib_stats_print(occan_stats *stats);

/* CAN HELP DEBUG FUNCTIONS */
void print_msg(int i, CANMsg *msg);

#endif
