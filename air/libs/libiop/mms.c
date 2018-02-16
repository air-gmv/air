/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2008-2013 
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file 
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE. 
 * ==========================================================================*/
/**
 *  @file
 *  @author
 *  @brief
 */

#include <iop.h>
#include <iop_mms.h>
#include <iop_error.h>

rtems_interval last_task_ticks;
air_schedule_status_t air_schedule;
air_partition_status_t air_partition;

void
iop_init_mms(void)
{

    iop_debug(" :: IOP - initializing MMS support\n");

    /* get current schedule and partition status */
    air_syscall_get_schedule_status(&air_schedule);
    air_syscall_get_partition_status(-1, &air_partition);
}

void
iop_task_sleep(uint32_t first_task)
{

    rtems_status_code rc;
    rtems_interval current_task_ticks, sleep_ticks;

    /* compute ticks until next period */
    /* this  commented call is for RTEMS 4.8, it is not deprecated */
    
#define RTEMS48i
#ifdef RTEMS48i
    rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &current_task_ticks);
#else /* RTEMS5 */
    current_task_ticks = rtems_clock_get_ticks_since_boot();
#endif

    sleep_ticks = air_partition.period -
        ((current_task_ticks - last_task_ticks) % air_partition.period);
    last_task_ticks = current_task_ticks;

    /* sleep until next period */
    rtems_task_wake_after(sleep_ticks);

    /* get period 'start' ticks */
    if (1 == first_task)
    {
#ifdef RTEMS48i
        /* this  commented call is for RTEMS 4.8, it is not deprecated */
        rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &last_task_ticks);
#else /* RTEMS5 */
        last_task_ticks = rtems_clock_get_ticks_since_boot();
#endif
    }
}

void
iop_change_schedule()
{

    uint32_t current_sid = air_schedule.current_schedule_index;

    /* get current schedule status */
    air_syscall_get_schedule_status(&air_schedule);

    /* check if the schedule changed */
    if (current_sid != air_schedule.current_schedule_index)
    {

        /* get partition peridod and duration */
        air_syscall_get_partition_status(-1, &air_partition);
    }
}
