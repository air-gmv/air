/*
 * Copyright (C) 2017-2020  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/AIR/raw/master/air/LICENSE
 */
/**
 * \file can_tasks.c
 *
 *  Created on: Sep 7, 2017
 *      Author: gmvs@gmv.com
 *
 *      Read and write high level functions for CAN
 *
 *  Modified on: Jan 5, 2020
 *      Author: lumm <lmurta@gmv.com>
 *
 *      Revised to generalize for ARM/Xilinx can drivers
 */

#include <can_support.h>
#include <iop_error.h>
#include <iop_mms.h>
#include <iop_support.h>


void can_writer(air_uptr_t arg) {

    iop_physical_device_t *pdev = (iop_physical_device_t *) arg;

    air_u32_t status = AIR_SUCCESSFUL;

    /* Initialize error chain (packets to be re-sent) */
    iop_chain_control error;
    iop_chain_initialize_empty(&error);

    /* Get the underlying driver */
    iop_can_device_t *can_driver = (iop_can_device_t *) pdev->driver;

    iop_debug("  :: can-write running!\n");

    while(!iop_chain_is_empty(&pdev->sendqueue)) {

        iop_debug("  :: can-write chain not empty!\n");

        iop_wrapper_t *wrapper = obtain_wrapper(&pdev->sendqueue);

        /* Write to the device */
        status = can_driver->dev.write((iop_device_driver_t *)can_driver, wrapper);

        if (status == AIR_SUCCESSFUL) {

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

void can_reader(air_uptr_t arg) {

    iop_physical_device_t *pdev = (iop_physical_device_t *) arg;

    air_i32_t status = AIR_SUCCESSFUL;

    /* Initialize error control chain (packets to be resent) */
    iop_chain_control error;
    iop_chain_initialize_empty(&error);

    /* Get underlying driver */
    iop_can_device_t *driver = (iop_can_device_t *) pdev->driver;

    iop_debug("  :: can-read running!\n");

    air_u32_t i;
    air_u32_t reads = pdev->reads_per_period[air_schedule.current_schedule_index];

    for(i = 0; i < reads; i++){

        iop_wrapper_t *wrapper = obtain_free_wrapper();

        if(wrapper == NULL){

            iop_raise_error(OUT_OF_MEMORY);
            break;
        }

        status = driver->dev.read((iop_device_driver_t *) driver, wrapper);
        if(status == AIR_SUCCESSFUL){

            /* We received something lets make it
             * available to higher level */
            iop_debug("  :: can-read message received\n");
            iop_chain_append(&pdev->rcvqueue, &wrapper->node);
            wrapper = NULL;
        }

        if(wrapper != NULL){

            release_wrapper(wrapper);
        }
    }
}
