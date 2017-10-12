/**
 *  @author gmvs@gmv.com
 *  @brief CAN support structures and functions definitions
 */

#ifndef __CAN_SUPPORT_H__
#define __CAN_SUPPORT_H__

#include <iop.h>
#include <rtems.h>
#include <iop_support.h>
#include <occan_msg_queue.h>

//typedef struct {
//	uint16_t identifier;
//	uint8_t RTR;
//	uint8_t data_len; // Data length in bytes to a maximum of 8
//	char data[8];
//}can_frame;

typedef struct {
	/* Generic device configuration */
	iop_device_driver_t dev;

	char *dev_name;
	struct occan_afilter *filter;
//	uint16_t self_id; // This device's ID
//	uint16_t rx_count;
//	uint16_t tx_count;
//	char                   *buffer;
	uint32_t                count;
	uint32_t                flags;
	uint32_t                bytes_moved;

} iop_can_device_t;

/* Not used right now TODO take it or leave it*/
void can_copy_header();
void can_compare_header();
void can_prebuild_header();

void can_write(iop_physical_device_t *pdev);
void can_reader(iop_physical_device_t *pdev);

#endif
