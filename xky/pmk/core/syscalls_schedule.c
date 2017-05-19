/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file syscalls_schedule.c
 * @author pfnf
 * @brief System calls related to schedules
 */

#include <pmk.h>
#include <workspace.h>
#include <segregation.h>
#include <configurations.h>

/**
 * @brief Get Schedule Id
 * @param core executing core information
 * @param name name of the schedule to look for (current if name is NULL)
 * @param sid pointer to store the schedule id
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_PARAM   - if invalid schedule name,
 *         NO_ERROR        - otherwise
 */
xky_status_code_e pmk_syscall_get_schedule_id(
        pmk_core_ctrl_t *core,
        xky_name_ptr_t name,
        xky_identifier_t *sid) {

    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;
    pmk_schedule_t *schedule = NULL;

    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* get other schedule id */
    if (name != NULL) {

        /* copy schedule name to PMK space */
        xky_name_t local_name;
        if (pmk_segregation_copy_from_user(
                context, local_name, name, sizeof(xky_name_t)) != 0) {

            /* disable preemption and return */
            cpu_disable_preemption(flags);
            return XKY_INVALID_POINTER;
        }

        /* check if the name is valid */
        schedule = pmk_get_schedule_by_name(local_name);

    /* get current schedule id */
    } else {

        schedule = xky_shared_area.schedule_ctrl->curr_schedule;
    }

    /* check if schedule exists */
    if (NULL == schedule) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_CONFIG;
    }

    /* copy id to partition */
    if (pmk_segregation_put_user(context, schedule->id, sid) != 0) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_POINTER;
    }

    /* disable preemption and return */
    cpu_disable_preemption(flags);
    return XKY_NO_ERROR;
}

/**
 * @brief Get Schedule Status
 * @param core executing core information
 * @param status pointer to store the schedule status
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         NO_ERROR        - otherwise
 */
xky_status_code_e pmk_syscall_get_schedule_status(
        pmk_core_ctrl_t *core,
        xky_schedule_status_t *status) {

    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;

    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* get nano-seconds per tick */
    xky_u32_t nspt = pmk_get_usr_ns_per_tick();

    /* fill local structure */
    xky_schedule_status_t local;
    local.time_of_last_schedule_change = xky_shared_area.schedule_ctrl->last_switch_ticks * nspt;
    local.current_schedule_id = xky_shared_area.schedule_ctrl->curr_schedule->id;
    local.next_schedule_id = xky_shared_area.schedule_ctrl->next_schedule->id;
    local.current_schedule_index = xky_shared_area.schedule_ctrl->curr_schedule->idx;
    local.next_schedule_index = xky_shared_area.schedule_ctrl->next_schedule->idx;

    /* copy to user land */
    if (pmk_segregation_put_user(context, local, status) != 0) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_POINTER;
    }

    /* disable preemption and return */
    cpu_disable_preemption(flags);
    return XKY_NO_ERROR;
}

/**
 * @brief Set Module schedule
 * @param core executing core information
 * @param sid next schedule id
 * @return INVALID_CONFIG  - if the partition doesn't have enough permissions,
 *         INVALID_PARAM   - if schedule id isn't valid,
 *         NO_ACTION       - if the current id is the schedule id
 *         NO_ERROR        - otherwise
 */
xky_status_code_e pmk_syscall_set_schedule(
        pmk_core_ctrl_t *core,
        xky_identifier_t sid) {

    pmk_partition_t *partition = core->partition;

    /* check if the partition have the current permissions */
    if ((partition->permissions & XKY_PERMISSION_SET_SCHEDULE) == 0) {

        return XKY_INVALID_CONFIG;
    }

    /* get the schedule by ARINC ID */
    pmk_schedule_t *schedule = pmk_get_schedule_by_id(sid);

    /* check if the schedule is valid */
    if (NULL == schedule) {

        return XKY_INVALID_PARAM;
    }

    /* check if the module is already in the current schedule */
    if (schedule == xky_shared_area.schedule_ctrl->curr_schedule) {

        return XKY_NO_ACTION;
    }

    /* apply new schedule */
    xky_shared_area.schedule_ctrl->next_schedule = schedule;
    return XKY_NO_ERROR;
}
