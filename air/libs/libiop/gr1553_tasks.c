/*
 * Copyright (C) 2011  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/** 
 *  @file IOPgr1553_tasks.c
 *
 *  @author Cl�udio Silva
 *
 *  @brief Tasks that manage a gr1553b bus controller
 */


#include <gr1553_support.h>
#include <bsp.h>
#include <iop_error.h>
#include <iop_mms.h>

/**
 *  @brief Task reading new 1553 data
 *
 */
void gr1553_read_task(air_uptr_t arg)
{
	iop_physical_device_t *pdev = (iop_physical_device_t *) arg;

	/* return code of several operations */
	air_status_code_e status = AIR_SUCCESSFUL;

	/* Get underlying driver */
	iop_1553_device_t *driver = (iop_1553_device_t *) pdev->driver;

	/* Is data valid or should be skipped */
	int skip = 0;

	/* generic iterator */
	int i = 0;

	/* pointer to the reply wrapper being currently handled */
	iop_wrapper_t *reply_wrapper;

	/* maximum number of packets that can be read on a single period*/
	int max = pdev->reads_per_period[air_schedule.current_schedule_index];

	for(i=0; i<max; i++){

		if(!skip){
			/* get an empty reply wrapper */
			reply_wrapper = obtain_free_wrapper();
			reply_wrapper->timer = 0;
		}

		skip = 1;

		/* if previous operation succeeded */
		if(reply_wrapper != NULL){

			/* copy pointer to data field */
			reply_wrapper->buffer->header_off = 0;
			reply_wrapper->buffer->header_size = sizeof(milstd_header_t);
			reply_wrapper->buffer->payload_off = sizeof(milstd_header_t);

			status = driver->dev.read((iop_device_driver_t *) driver, reply_wrapper);

			/* check if the read operation was successful */
			if(status == AIR_SUCCESSFUL){

				skip = 0;

			} else if (status == AIR_TIMED_OUT){
				/* the list has ended */
			} else {
				/* this branch is never used */
				/** @todo this error needs to be better evaluated */
				iop_raise_error(HW_READ_ERROR);
			}


			if(!skip){

				/* append prefetched reply to physical device's queue */
				iop_chain_append(&pdev->rcvqueue, &reply_wrapper->node);
				reply_wrapper = NULL;
			}

			/* release Wrapper if not used */
			if (reply_wrapper != NULL)
			{
				release_wrapper(reply_wrapper);
			}

		} else{
			iop_raise_error(OUT_OF_MEMORY);
		}
	}
}
/**
 *  @brief Task writing new 1553 data
 *
 */
void gr1553_write_task(air_uptr_t arg)
{
	iop_physical_device_t *pdev = (iop_physical_device_t *) arg;

	/* return code of several operations */
	air_status_code_e status = AIR_NO_ACTION;

	/* Get underlying driver */
	iop_1553_device_t *driver = (iop_1553_device_t *) pdev->driver;

	/* Go through all available */
	while(!iop_chain_is_empty(&pdev->sendqueue)){

		/* pointer to the request wrapper being currently handled */
		iop_wrapper_t *req_wrapper = obtain_wrapper(&pdev->sendqueue);

		/* Write to the device */
		status = driver->dev.write((iop_device_driver_t *) driver, req_wrapper);

		if (status == AIR_INVALID_SIZE)
			iop_debug("gr1553_write Invalid Size\n");
		else
			if(status == AIR_NOT_AVAILABLE)
				iop_debug("gr1553_write No Avail Slot\n");

		/* release Wrapper*/
		release_wrapper(req_wrapper);
	}
}
