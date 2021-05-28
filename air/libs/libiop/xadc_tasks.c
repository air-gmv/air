/*
 * Copyright (C) 2021  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/air-dev/-/blob/dev/LICENSE
 */
/**
 * @file xadc_tasks.c
 * @author TTAO
 * @brief XADC read and write tasks
 */


#include <iop_mms.h>
#include <iop_error.h>
#include <xadc_support.h>




void xadc_writer(air_uptr_t arg) {


}




void xadc_reader(air_uptr_t arg) {

	iop_physical_device_t *pdev = (iop_physical_device_t *) arg;
	iop_adc_device_t *xadc_driver = (iop_adc_device_t *) pdev->driver;


    iop_chain_control error;
    iop_chain_initialize_empty(&error);


    air_u32_t reads = pdev->reads_per_period[air_schedule.current_schedule_index];


    for (int i=0; i<reads; ++i){

        iop_wrapper_t *wrapper = obtain_free_wrapper();


        if (wrapper == NULL) {
            iop_raise_error(OUT_OF_MEMORY);
            break;
        }


        if (xadc_driver->dev.read((iop_device_driver_t *)xadc_driver, wrapper) == AIR_SUCCESSFUL) {
            iop_chain_append(&pdev->rcvqueue, &wrapper->node);
            wrapper = NULL;
        }


        if (wrapper != NULL)
            release_wrapper(wrapper);
    }
}
