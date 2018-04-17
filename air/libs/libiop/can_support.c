/*
 * can_support.c
 *
 *  Created on: Sep 7, 2017
 *      Author: gmvs@gmv.com
 */

#include <can_support.h>
#include <iop.h>
#include <iop_support.h>

void can_copy_header(
		iop_physical_device_t *iop_dev,
		iop_wrapper_t *wrapper,
		iop_header_t *header){

	/* get underlying ETH device */
//	iop_can_device_t *can_dev = (iop_can_device_t *)iop_dev->driver;

	/* get underlying IOP buffer */
	iop_buffer_t *iop_buf = wrapper->buffer;

	/* set the header size and offsets */
	iop_buf->header_off = iop_buf->header_size - sizeof(can_header_t);
	iop_buf->header_size = sizeof(can_header_t);

	/* copy header from the route */
	memcpy(get_header(iop_buf), header, iop_buf->header_size);
}

/* This function compares the ID on the wrapper
 * to the ID  of a given header. */

uint16_t can_compare_header(
		iop_wrapper_t *wrapper,
		iop_header_t *header){

	iop_buffer_t *buf = wrapper->buffer;
	can_header_t *can_header1 = (can_header_t *) header;
//	can_header_t *can_header2 = (can_header_t *) buf->v_addr + buf->header_off;
	can_header_t can_header2;
	memcpy(&can_header2, get_header(buf), buf->header_size);
	iop_debug("Comparing headers - H1:%d H2:%d\n", can_header1->id , can_header2.id);
	if (can_header1->id == can_header2.id){
		return 1;
	}
	return 0;
}

void can_prebuild_header(
		can_header_t *header){
	/* If ID not defined */
	if(header->id == 0){
		header->id = 255;
		header->extended = 0;
		header->rtr = 0;
		header->sshot = 0;
	}
}


