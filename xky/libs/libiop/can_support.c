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

void can_compare_header(
		iop_wrapper_t *wrapper,
		iop_header_t *header){

}

void can_prebuild_header(
		can_header_t *header){

}
