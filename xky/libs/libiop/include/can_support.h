/**
 *  @author gmvs@gmv.com
 *  @brief CAN support structures and functions definitions
 */

#ifndef __CAN_SUPPORT_H__
#define __CAN_SUPPORT_H__

#include <iop.h>
#include <rtems.h>
#include <iop_support.h>

typedef struct {
	uint16_t identifier;
	uint8_t RTR;
	uint8_t data_len; // Data length in bytes to a maximum of 8
	char data[8];
}can_frame;

typedef struct {
	iop_device_driver_t dev;
	uint16_t self_id; // This device's ID
	uint16_t rx_count;
	uint16_t tx_count;

} iop_can_device_t;



#endif
