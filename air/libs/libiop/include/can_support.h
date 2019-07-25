/**
 *  @author gmvs@gmv.com
 *  @brief CAN support structures and functions definitions
 *  to integrate the occan driver into AIR
 */

#ifndef __CAN_SUPPORT_H__
#define __CAN_SUPPORT_H__

#include <iop.h>
#include <iop_support.h>

typedef struct {
	/* Generic device configuration */
	iop_device_driver_t dev;

	int can_core; // Identify which CAN core corresponds to (either CAN0 or CAN1)
	int baud_rate;

} iop_can_device_t;

void can_copy_header(
		iop_physical_device_t *iop_dev,
		iop_wrapper_t *wrapper,
		iop_header_t *header);

uint32_t can_compare_header(
		iop_wrapper_t *wrapper,
		iop_header_t *header);

void can_prebuild_header(
		can_header_t *header);

void can_writer(air_uptr_t arg);
void can_reader(air_uptr_t arg);

#endif
