/*
 * Copyright (C) 2011  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */

#include <iop.h>
#include <iop_support.h>
#include <dispatcher.h>
#include <iop_functions.h>
/**
 *  \return RTEMS_SUCCESSFUL if the operation was completed correctly
 *  or the status_code returned by rtems_task_create or rtems_task_start
 *
 *  \brief Initializes IOP tasks
 *
 */

void iop_main_loop(void)
{

    iop_debug(" :: creating & launching worker tasks\n");

    int i;

#ifdef COVERAGE_ENABLED
    rtems_interval time1, time2;

    rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &time1);
    iop_debug("  IOP :: air-dev-cov times is %d %d\n", time2, time1);
#endif

    iop_physical_device_t *devs[usr_configuration.physical_devices.length];
    for (i = 0; i < usr_configuration.physical_devices.length; i++)
    {
        devs[i] = get_physical_device(i);
    }

    for (;;)
    {
        pre_dispatcher();
        pre_router();

        /* run all the device drivers writer and reader functions */

        for (i = 0; i < usr_configuration.physical_devices.length; i++)
        {

            devs[i]->writer_task((air_uptr_t)devs[i]);
            devs[i]->reader_task((air_uptr_t)devs[i]);
        }

        pos_dispatcher();
        pos_router();

#ifdef COVERAGE_ENABLED
        iop_debug("  IOP :: air-dev-cov checking time\n");
        if (rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &time2) == AIR_SUCCESSFUL)
        {
            iop_debug("  IOP :: air-dev-cov times is %d %d\n", time2, time1);
            if ((time2 - time1) > 6000)
            {
                iop_debug("  IOP :: air-dev-cov timeout on libiop\n");
                break;
            }
        }
#endif
    }

    return;
}
