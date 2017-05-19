/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file syscalls_partition.c
 * @author pfnf
 * @brief System calls related to partitions
 */

#include <pmk.h>
#include <printk.h>
#include <multicore.h>
#include <segregation.h>
#include <configurations.h>


xky_status_code_e pmk_syscall_get_partition_id(
        pmk_core_ctrl_t *core,
        xky_name_ptr_t name,
        xky_identifier_t *pid) {

    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;
    pmk_partition_t *partition = core->partition;

    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* get other partition id */
    if (NULL != name) {

        /* check partition permissions */
        if ((partition->permissions & XKY_PERMISSION_ARINC_SYSTEM) == 0 &&
            (partition->permissions & XKY_PERMISSION_SET_PARTITION_MODE) == 0) {

            /* disable preemption and return */
            cpu_disable_preemption(flags);
            return XKY_INVALID_CONFIG;
        }

        /* copy partition name to PMK space */
        xky_name_t local_name;
        if (pmk_segregation_copy_from_user(
                context, local_name, name, sizeof(xky_name_t)) != 0) {

            /* disable preemption and return */
            cpu_disable_preemption(flags);
            return XKY_INVALID_POINTER;
        }

        /* check if the name is valid */
        partition = pmk_get_partition_by_name(local_name);
    }

    /* check if partition exists */
    if (NULL == partition) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_PARAM;
    }

    /* pass id to partition */
    if (pmk_segregation_put_user(context, partition->id, pid) != 0) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_POINTER;
    }

    /* disable preemption and return */
    cpu_disable_preemption(flags);
    return XKY_NO_ERROR;
}

xky_status_code_e pmk_syscall_get_partition_status(
        pmk_core_ctrl_t *core,
        xky_identifier_t pid,
        xky_partition_status_t *status) {

    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;
    pmk_partition_t *partition = core->partition;

    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* get other partition status */
    if (pid >= 0 && partition->id != pid) {

        /* check partition permissions */
        if ((partition->permissions & XKY_PERMISSION_ARINC_SYSTEM) == 0 &&
            (partition->permissions & XKY_PERMISSION_SET_PARTITION_MODE) == 0) {

            /* disable preemption and return */
            cpu_disable_preemption(flags);
            return XKY_INVALID_CONFIG;
        }

        /* check if the name is valid */
        partition = pmk_get_partition_by_id(pid);
    }

    /* check if partition exists */
    if (NULL == partition) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_PARAM;
    }

    /* fill status structure */
    xky_partition_status_t local_status;
    local_status.index = partition->idx;
    local_status.period = partition->period;
    local_status.duration = partition->duration;
    local_status.identifier = partition->id;
    local_status.permissions = partition->permissions;
    local_status.operating_mode = partition->mode;
    local_status.start_condition = partition->start_condition;

    /* check if the partition was already executed */
    if (partition->state != PMK_PARTITION_STATE_NOT_RUN) {
        local_status.restart_count = partition->restart_count;
    } else {
        local_status.restart_count = 0;
    }

    /* copy status to partition */
    if (pmk_segregation_put_user(context, local_status, status) != 0) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return XKY_INVALID_POINTER;
    }

    /* disable preemption and return */
    cpu_disable_preemption(flags);
    return XKY_NO_ERROR;
}

xky_status_code_e pmk_syscall_set_partition_mode(
        pmk_core_ctrl_t *core_ctrl,
        xky_identifier_t pid,
        xky_operating_mode_e mode) {

    pmk_partition_t *partition = core_ctrl->partition;

    /* set other partition mode */
    if (partition->id != pid && pid != -1) {

        /* check for valid permissions */
        if ((partition->permissions & XKY_PERMISSION_SET_PARTITION_MODE) == 0 &&
            (partition->permissions & XKY_PERMISSION_ARINC_SYSTEM) == 0) {

            return XKY_INVALID_CONFIG;
        }

        /* get other partition */
        partition = pmk_get_partition_by_id(pid);
        if (NULL == partition) {

            return XKY_INVALID_PARAM;
        }

        /* check if the state is valid for other partition */
        if (XKY_MODE_NORMAL == mode) {

            return XKY_INVALID_MODE;
        }
    }

    /* check if mode is valid */
    if (mode < XKY_MODE_IDLE || mode > XKY_MODE_NORMAL) {

        return XKY_INVALID_PARAM;
    }

    /* check if action is necessary */
    if ((XKY_MODE_IDLE == mode && XKY_MODE_IDLE == partition->mode) ||
        (XKY_MODE_NORMAL == mode && XKY_MODE_NORMAL == partition->mode)) {

        return XKY_NO_ACTION;
    }

    /* check if the mode is valid */
    if (XKY_MODE_WARM_START == mode && XKY_MODE_COLD_START == partition->mode) {

       //return XKY_INVALID_MODE;
    }

    /* apply state to the partition */
    partition->mode = mode;
    switch (mode) {

        /* restart partition */
        case XKY_MODE_COLD_START:
        case XKY_MODE_WARM_START:
            pmk_partition_restart(partition);
            partition->start_condition = XKY_START_CONDITION_PARTITION_RESTART;
            break;

        /* halt partition */
        case XKY_MODE_IDLE:
            pmk_partition_halt(partition);
            break;

        /* set to normal */
        default:
            /* no action is required */
            break;
    }

    /* return no error */
    return XKY_NO_ERROR;
}

void pmk_syscall_putchar(pmk_core_ctrl_t *core, char ch) {

    /* allow partition to be preempted */
    cpu_preemption_flags_t flags;
    cpu_enable_preemption(flags);

    /* output character */
    pmk_console_outbyte(ch);

    /* disable preemption */
    cpu_disable_preemption(flags);
}

xky_u32_t pmk_syscall_print(pmk_core_ctrl_t *core, char *buffer, xky_sz_t len) {

    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;

    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* print buffer @todo needs optimization */
    char ch;
    xky_u32_t count;
    for (count = 0; count < len; ++count) {

        /* get character from user land */
        if (pmk_segregation_get_user(context, ch, buffer) != 0 || ch == '\0') {
            break;
        }

        /* print character */
        pmk_console_outbyte(ch);
        ++buffer;
    }

    /* disable preemption and return */
    cpu_disable_preemption(flags);
    return count;
}
