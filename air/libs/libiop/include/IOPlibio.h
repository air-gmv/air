/** 
 * 	@file IOPlibio.h
 * 
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT 
 *  
 *  @author Cláudio Silva
 *  
 *  The license and distribution terms for this file may be 
 *  found in the file LICENSE at: 
 *  http://www.rtems.com/license/LICENSE.
 * 
 * 	@brief Driver Interface Definition File
 * 
 * 	This file defines the structures used as a driver interface
 */ 

#ifndef IOP_LIBIO_H
#define IOP_LIBIO_H

/**
 * @brief structure used as an read/write interface with the drivers
 */
struct greth_args{
	char *hdr;			/**< Pointer to header */
	int hdr_len;		/**< Header length */
	char *data;			/**< pointer to data */
	int data_len;		/**< data length */
	int bytes_moved;	/**< bytes written or read by the driver*/
};

typedef struct greth_args libio_rw_args_t;

/**
 * @brief structure used as an ioctl interface with the drivers
 */
typedef struct {
    unsigned int command; 		/**< ioctl command */
    void *buffer;		  		/**< data needed for the ioctl command*/
    unsigned int ioctl_return; 	/**< ioctl return code */
} libio_ioctl_args_t;


#endif
