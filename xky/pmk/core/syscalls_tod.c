/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file syscalls_tod.c
 * @author pfnf
 * @brief System calls related to the handling of the time of day
 */

#include <pmk.h>
#include <tod.h>
#include <multicore.h>
#include <segregation.h>
#include <configurations.h>

xky_status_code_e pmk_syscall_get_tod(pmk_core_ctrl_t *core, xky_time_t *tod) {

    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;

    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* check if the system time is defined */
    if (pmk_tod_defined != 1) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_CONFIG;
    }

    /* copy ToD to the partition */
    if (pmk_segregation_put_user(context, pmk_tod_now, tod) != 0) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_POINTER;
    }

    /* disable preemption and return */
    cpu_disable_preemption(flags);
    return XKY_NO_ERROR;
}

xky_status_code_e pmk_syscall_set_tod(pmk_core_ctrl_t *core, xky_time_t *tod) {

    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;
    pmk_partition_t *partition = core->partition;

    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* check if the partition have permissions to set the system time */
    if ((partition->permissions & XKY_PERMISSION_SET_TOD) == 0) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_CONFIG;
    }

    /* copy ToD from the partition */
    xky_time_t local_tod;
    if (pmk_segregation_get_user(context, local_tod, tod) != 0) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_POINTER;
    }

    /* apply new ToD */
    pmk_tod_now.tv_sec  = local_tod.tv_sec;
    pmk_tod_now.tv_nsec = local_tod.tv_nsec;
    pmk_tod_defined = 1;

    /* disable preemption and return */
    cpu_disable_preemption(flags);
    return XKY_NO_ERROR;
}
