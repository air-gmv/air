 
#ifndef _IOP_H_
#define _IOP_H_

#include <stdint.h>
#include <rtems.h>
#include "IOPgreth.h"
#include "IOPuip.h"
#include "IOPethernet.h"
#include "IOPinterface.h"

#define DEVICE_COUNT 1

typedef struct greth_args_s{
	char *hdr;			/**< Pointer to header */
	int hdr_len;		/**< Header length */
	char *data;			/**< pointer to data */
	int data_len;		/**< data length */
	int bytes_moved;	/**< bytes written or read by the driver*/
} greth_args_t;



typedef struct {
    unsigned int command; 		/**< ioctl command */
    void *buffer;		  		/**< data needed for the ioctl command*/
    unsigned int ioctl_return; 	/**< ioctl return code */
} libio_ioctl_args_t;

typedef greth_args_t libio_rw_args_t;

#endif
