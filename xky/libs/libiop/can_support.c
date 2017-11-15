/*
 * can_support.c
 *
 *  Created on: Sep 7, 2017
 *      Author: gmvs@gmv.com
 */

#include <can_support.h>
#include <iop.h>

/*
 * Stub functions
 */

void can_copy_header(
		iop_physical_device_t *iop_dev,
		iop_wrapper_t *wrapper,
		iop_header_t *header) {

}

uint16_t can_compare_header(
		iop_wrapper_t *wrapper,
		iop_header_t *header){

	can_header_t *can_header = (can_header_t *) header;
	int id = *(int *) (wrapper->buffer->v_addr +
			wrapper->buffer->header_off +
			3*sizeof(int));
	if (id == can_header->id){
		return 1;
	}
	return 0;
}

void can_prebuild_header(
		can_header_t *header){

}
