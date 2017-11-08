/*
 * can_tasks.c
 *
 *  Created on: Sep 7, 2017
 *      Author: gmvs@gmv.com
 *
 *      Read and write functions for CAN
 */

#include <iop.h>
#include <can_support.h>
#include <iop_error.h>

void can_write(iop_physical_device_t *pdev)
{

	/* Initialize error chain (packets to be re-sent) */
	iop_chain_control error;
	iop_chain_initialize_empty(&error);

	/* Get the underlying driver */
	iop_can_device_t *can_driver = (iop_can_device_t *) pdev->driver;

	iop_debug("  :: can-write running!\n");

	while(!iop_chain_is_empty(&pdev->sendqueue)){
		iop_wrapper_t *wrapper = obtain_wrapper(&pdev->sendqueue);
		uint8_t *message = (uint8_t *) wrapper->buffer->v_addr;

		/* Write to the device */
		if(can_driver->dev.write((iop_device_driver_t *) can_driver,
				wrapper) == RTEMS_SUCCESSFUL){
			release_wrapper(wrapper);
		} else {
			iop_chain_append(&error, &wrapper->node);
			iop_raise_error(HW_WRITE_ERROR);
		}

	}
	/* Failed transmissions */
	while(!iop_chain_is_empty(&error)) {
		iop_wrapper_t *wrapper = obtain_wrapper(&error);
		iop_chain_append(&pdev->sendqueue, &wrapper->node);
	}

}


void can_reader(iop_physical_device_t *pdev)
{
	/* Initialize error control chain (packets to be resent) */
	iop_chain_control error;
	iop_chain_initialize_empty(&error);

	/* Get underlaying driver */
	iop_can_device_t *driver = (iop_can_device_t *) pdev->driver;

	iop_debug("  :: can-read running!\n");

	/* Read from the buffer */


}
