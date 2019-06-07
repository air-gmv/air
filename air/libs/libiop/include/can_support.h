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
	uint16_t rx_count;
	uint16_t tx_count;
} iop_can_device_t;

/* TODO the next three functions are stubs */
void can_copy_header(
		iop_physical_device_t *iop_dev,
		iop_wrapper_t *wrapper,
		iop_header_t *header);

uint16_t can_compare_header(
		iop_wrapper_t *wrapper,
		iop_header_t *header);

void can_prebuild_header(
		can_header_t *header);

void can_writer(iop_physical_device_t *pdev);
void can_reader(iop_physical_device_t *pdev);

#endif
