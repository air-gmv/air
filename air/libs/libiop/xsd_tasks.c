/*
 * Copyright (C) 2021  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/air-dev/-/blob/dev/LICENSE
 */
/**
 * @file xsd_tasks.c
 * @author TTAO
 * @brief XSD read and write tasks
 */

#include <iop_error.h>
#include <iop_mms.h>
#include <xsd_support.h>

void xsd_writer(air_uptr_t arg)
{

    iop_physical_device_t *pdev = (iop_physical_device_t *)arg;
    iop_sd_device_t *xsd_driver = (iop_sd_device_t *)pdev->driver;

    iop_chain_control error;
    iop_chain_initialize_empty(&error);

    while (!iop_chain_is_empty(&pdev->sendqueue))
    {

        iop_wrapper_t *wrapper = obtain_wrapper(&pdev->sendqueue);

        if (xsd_driver->dev.write((iop_device_driver_t *)xsd_driver, wrapper) == AIR_SUCCESSFUL)
        {
            release_wrapper(wrapper);
        }
        else
        {
            iop_chain_append(&error, &wrapper->node);
            iop_raise_error(HW_WRITE_ERROR);
        }
    }

    while (!iop_chain_is_empty(&error))
    {
        iop_wrapper_t *wrapper = obtain_wrapper(&error);
        iop_chain_append(&pdev->sendqueue, &wrapper->node);
    }
}

void xsd_reader(air_uptr_t arg)
{

    iop_physical_device_t *pdev = (iop_physical_device_t *)arg;
    iop_sd_device_t *xsd_driver = (iop_sd_device_t *)pdev->driver;

    if (xsd_driver->all_read == 0)
    {

        for (int i = 0; i < pdev->routes.length; ++i)
        {

            iop_wrapper_t *wrapper = obtain_free_wrapper();
            iop_physical_route_t *route = &((iop_physical_route_t *)pdev->routes.elements)[i];

            if (route->schedule[air_schedule.current_schedule_index] == 1)
            {

                xsd_driver->header_read = (sd_header_t *)route->header;

                if (wrapper == NULL)
                {
                    iop_raise_error(OUT_OF_MEMORY);
                    break;
                }

                if (xsd_driver->dev.read((iop_device_driver_t *)xsd_driver, wrapper) == AIR_SUCCESSFUL)
                {
                    iop_chain_append(&pdev->rcvqueue, &wrapper->node);
                    wrapper = NULL;
                }

                if (wrapper != NULL)
                {
                    release_wrapper(wrapper);
                }
            }
        }

        xsd_driver->all_read = 1;
    }
}
