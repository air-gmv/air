/*
 * Copyright (C) 2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
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
#include <schedule.h>


air_status_code_e pmk_syscall_get_partition_id(
        pmk_core_ctrl_t *core,
        air_name_ptr_t name,
        air_identifier_t *pid) {

    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;
    pmk_partition_t *partition = core->partition;

    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* get other partition id */
    if (NULL != name) {

        /* check partition permissions */
        if ((partition->permissions & AIR_PERMISSION_SET_PARTITION_MODE) == 0) {

            /* disable preemption and return */
            cpu_disable_preemption(flags);
            return AIR_INVALID_CONFIG;
        }

        /* copy partition name to PMK space */
        air_name_t local_name;
        if (pmk_segregation_copy_from_user(
                context, local_name, name, sizeof(air_name_t)) != 0) {

            /* disable preemption and return */
            cpu_disable_preemption(flags);
            return AIR_INVALID_POINTER;
        }

        /* check if the name is valid */
        partition = pmk_get_partition_by_name(local_name);
    }

    /* check if partition exists */
    if (NULL == partition) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return AIR_INVALID_PARAM;
    }

    /* pass id to partition */
    if (pmk_segregation_put_user(context, partition->id, pid) != 0) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return AIR_INVALID_POINTER;
    }

    /* disable preemption and return */
    cpu_disable_preemption(flags);
    return AIR_NO_ERROR;
}

air_status_code_e pmk_syscall_get_partition_status(
        pmk_core_ctrl_t *core,
        air_identifier_t pid,
        air_partition_status_t *status) {

    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;
    pmk_partition_t *partition = core->partition;

    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* get other partition status */
    if (pid >= 0 && partition->id != pid) {

        /* check partition permissions */
        if ((partition->permissions & AIR_PERMISSION_SET_PARTITION_MODE) == 0) {

            /* disable preemption and return */
            cpu_disable_preemption(flags);
            return AIR_INVALID_CONFIG;
        }

        /* check if the name is valid */
        partition = pmk_get_partition_by_id(pid);
    }

    /* check if partition exists */
    if (NULL == partition) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return AIR_INVALID_PARAM;
    }

    /* fill status structure */
    air_partition_status_t local_status;
    local_status.index = partition->idx;
    local_status.window_id = partition->window_id;
    local_status.period = partition->period;
    local_status.duration = partition->duration;
    local_status.identifier = partition->id;
    local_status.permissions = partition->permissions;
    local_status.operating_mode = partition->mode;
    local_status.start_condition = partition->start_condition;
    local_status.state = partition->state;
    local_status.restart_count = partition->restart_count;

    /* copy status to partition */
    if (pmk_segregation_put_user(context, local_status, status) != 0) {

        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return AIR_INVALID_POINTER;
    }

    /* disable preemption and return */
    cpu_disable_preemption(flags);
    return AIR_NO_ERROR;
}

air_status_code_e pmk_syscall_set_partition_mode(
        pmk_core_ctrl_t *core_ctrl,
        air_identifier_t pid,
        air_operating_mode_e mode) {

    pmk_partition_t *partition = core_ctrl->partition;

    /* set other partition mode */
    if (partition->id != pid && pid != -1) {

        /* check for valid permissions */
        if ((partition->permissions & AIR_PERMISSION_SET_PARTITION_MODE) == 0){

            return AIR_INVALID_CONFIG;
        }

        /* get other partition */
        partition = pmk_get_partition_by_id(pid);
        if (NULL == partition) {

            return AIR_INVALID_PARAM;
        }

        /* check if the state is valid for other partition */
        if (AIR_MODE_NORMAL == mode) {

            return AIR_INVALID_MODE;
        }
    }

    /* check if mode is valid */
    if (mode < AIR_MODE_IDLE || mode > AIR_MODE_NORMAL) {

        return AIR_INVALID_PARAM;
    }

    /* check if action is necessary */
    if ((AIR_MODE_IDLE == mode && AIR_MODE_IDLE == partition->mode) ||
        (AIR_MODE_NORMAL == mode && AIR_MODE_NORMAL == partition->mode)) {

        return AIR_NO_ACTION;
    }

    /* check if the mode is valid */
    if (AIR_MODE_WARM_START == mode && AIR_MODE_COLD_START == partition->mode) {

       return AIR_INVALID_MODE;
    }

    /* Avoid any change while partition is initializing*/
    if(AIR_PARTITION_STATE_INIT == partition->state){

       return AIR_NOT_AVAILABLE;
    }

    /* apply state to the partition */
    partition->mode = mode;
    switch (mode) {

        /* restart partition */
        case AIR_MODE_COLD_START:
        case AIR_MODE_WARM_START:
            pmk_partition_restart(partition);
            partition->start_condition = AIR_START_CONDITION_PARTITION_RESTART;
            break;

        /* halt partition */
        case AIR_MODE_IDLE:
            pmk_partition_halt(partition);
            break;

        /* set to normal */
        default:
            /* no action is required */
            break;
    }

    /* return no error */
    return AIR_NO_ERROR;
}

void pmk_syscall_end_window(pmk_core_ctrl_t *core) {

    pmk_partition_t *partition = core->partition;

    /* change partition state */
    partition->state = AIR_PARTITION_STATE_READY;

    /* store the current core partition context */
    core_context_save(core);

    /* update partition last ticks */
    partition->last_clock_tick = pmk_get_schedule_total_ticks();

    /* flag as not executing a Window */
    partition->window_id = -1;

    /* unmap the current core */
    partition->core_mapping[core_context_id(core->context)] = PMK_MAX_CORES;

    /* flag the switch */
    core->partition_switch = 1;

    /* unassign Partition to core */
    core->partition = NULL;

    /* ACTION: IDLE the core for the remainder of the window */
    core->context = &pmk_core_idle_context[core->idx];
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

air_u32_t pmk_syscall_print(pmk_core_ctrl_t *core, char *buffer, air_sz_t len) {

    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;

    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* print buffer @todo needs optimization */
    char ch;
    air_u32_t count;
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
