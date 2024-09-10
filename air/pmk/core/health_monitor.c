/*
 * Copyright (C) 2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 *  @file health_monitor.c
 *  @author pfnf
 *	@brief Health Monitor functionality
 *
 *	The Health Monitor in PMK follows the ARINC 653 specifications.
 *	The HM tables for the Module are stored in the PMK shared-area and
 *	each partition holds the its specific HM tables in its control structure
 *
 *	An HM event is passed to the Guest OS by injecting a trap in its execution
 */

#include <pmk.h>
#include <segregation.h>
#include <bsp.h>
#include <ipc.h>
#include <printk.h>
#include <workspace.h>
#include <multicore.h>
#include <configurations.h>
#include <health_monitor.h>

void pmk_module_shutdown(pmk_core_ctrl_t *core)
{

    /* shutdown all configured cores */
    air_u32_t i;
    for (i = 0; i < air_shared_area.configured_cores; ++i)
    {

        /* shutdown current core directly */
        if (air_shared_area.core[i].idx == core->idx)
        {

            bsp_shutdown_core(core);

            /* shutdown core via IPC message */
        }
        else
        {

            core_context_set_ipc_message(air_shared_area.core[i].context, PMK_IPC_MODULE_SHUTDOWN);
        }
    }
}

void pmk_module_restart(pmk_core_ctrl_t *core)
{

    /* restart all configured cores */
    air_u32_t i;
    for (i = 0; i < air_shared_area.configured_cores; ++i)
    {

        /* restart current core directly */
        if (air_shared_area.core[i].idx == core->idx)
        {

            bsp_restart_core(core);

            /* restart core via IPC message */
        }
        else
        {

            core_context_set_ipc_message(air_shared_area.core[i].context, PMK_IPC_MODULE_RESTART);
        }
    }
}

/**
 * @brief Health-Monitor Initialization
 */
void pmk_hm_init()
{

#ifdef PMK_DEBUG
    printk(" :: Health-Monitor initialization\n");
#endif

    /* setup the architecture dependent health-monitor */
    cpu_hm_init();

    /* setup the system state table and module health-monitor table */
    air_shared_area.hm_system_table = pmk_get_usr_hm_system_table();
    air_shared_area.hm_module_table = pmk_get_usr_hm_module_table();

    /* setup the health monitoring log */
    air_shared_area.hm_log.policy = PMK_HM_LOG_OVERWRITE; //TODO: For now leave as default overwrite policy
    air_shared_area.hm_log.n_events = 0;
    air_shared_area.hm_log.head = 0;
    air_shared_area.hm_log.tail = 0;

#ifdef PMK_DEBUG
    printk(" :: Health-Monitor log policy %d\n", air_shared_area.hm_log.policy);
    pmk_workspace_debug();
#endif
}

/**
 * @brief Health-Monitor handler for the Module level
 * @param core_ctrl current core control structure
 * @param state_id current core state id
 * @param error_id current error id
 */
void pmk_hm_isr_handler_module_level(pmk_core_ctrl_t *core, air_state_e state_id, air_error_e error_id)
{

    /* setup the undefined action */
    pmk_hm_action_id action_id = PMK_ACTION_UNDEFINED;

    printk("HM Module level error \n");

    /* lookup for the action in Module Health-Monitor table */
    pmk_hm_action_id *actions = (pmk_hm_action_id *)air_shared_area.hm_module_table[state_id];
    if (actions != NULL)
    {

        action_id = actions[error_id];
    }

    /* perform the action */
    switch (action_id)
    {

    /* reset module */
    case PMK_ACTION_RESET:
        pmk_module_restart(core);
        break;

    /* shutdown module */
    case PMK_ACTION_SHUTDOWN:
        pmk_module_shutdown(core);
        break;

    /* ignore error */
    default:
        break;
    }
}

/**
 * @brief Health-Monitor handler for the Partition level
 * @param core current core control structure
 * @param state_id current core state id
 * @param error_id current error id
 */
void pmk_hm_isr_handler_partition_level(pmk_core_ctrl_t *core, air_state_e state_id, air_error_e error_id)
{

    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;
    pmk_partition_t *partition = core->partition;
    printk("HM Partition %d state id %d, error id %d \n", partition->id, state_id, error_id);

    partition->events |= AIR_EVENT_HEALTH_MONITOR;

    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* lookup for the action_id in Partition Health-Monitor table */
    pmk_hm_action_id action_id = PMK_ACTION_UNDEFINED;
    pmk_hm_action_id *actions = (pmk_hm_action_id *)partition->hm_table[state_id];
    if (partition->hm_table[state_id] != NULL)
    {

        action_id = actions[error_id];
    }

    /* perform the action_id */
    switch (action_id)
    {

    /* set partition in idle mode */
    case PMK_ACTION_IDLE:
        printk("HM Taking Action Idle\n\n");

        /* disable preemption */
        cpu_disable_preemption(flags);

        /* halt partition */
        pmk_partition_halt(partition);
        partition->mode = AIR_MODE_IDLE;
        break;

    /* cold restart the partition */
    case PMK_ACTION_COLD_START:
        printk("HM Taking Action Cold Start\n\n");

        /* disable preemption */
        cpu_disable_preemption(flags);

        /* cold restart partition */
        pmk_partition_restart(partition);
        partition->mode = AIR_MODE_COLD_START;
        partition->start_condition = AIR_START_CONDITION_HM_PARTITION_RESTART;
        break;

    /* warm restart the partition */
    case PMK_ACTION_WARM_START:
        printk("HM Taking Action Warm Start\n\n");

        /* disable preemption */
        cpu_disable_preemption(flags);

        /* warm restart partition */
        pmk_partition_restart(partition);
        partition->mode = AIR_MODE_WARM_START;
        partition->start_condition = AIR_START_CONDITION_HM_PARTITION_RESTART;
        break;

    /* ignore event and pass it to the partition callback */
    default:
        printk("HM Taking Action Ignore\n\n");

        /* disable preemption */
        cpu_disable_preemption(flags);

        /* add HM event */
        core_context_add_hm_event(context, state_id, error_id);
        break;
    }
}

air_status_code_e pmk_print_hm_log(pmk_core_ctrl_t *core, air_hm_log_t *log){
    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;

    if (air_shared_area.hm_log.n_events == 0) {
        printk("HM Log is empty.\n");
        return;
    }

    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* fill local structure */
    air_hm_log_t local;
    local.n_events = air_shared_area.hm_log.n_events;

    printk("------------------------------\n");
    air_u32_t current = air_shared_area.hm_log.tail;
    for (int i = 0; i < air_shared_area.hm_log.n_events; i++)
    {
        printk("HM Event %d:\n", i);
        printk("  Absolute Date: Tick %d\n", (air_u32_t) air_shared_area.hm_log.events[current].absolute_date);
        printk("  Error Type: %d\n", (air_u32_t) air_shared_area.hm_log.events[current].error_type);
        printk("  Level: %d\n", (air_u32_t) air_shared_area.hm_log.events[current].level);
        printk("  Partition ID: %d\n", (air_u32_t) air_shared_area.hm_log.events[current].partition_id);
        printk("\n");

        // Increment the current pointer to the next element, circularly
        current = (current + 1) % air_shared_area.hm_log.n_events;

        // // Copy to the partition
        
        // local.events[i].absolute_date = air_shared_area.hm_log.events[i].absolute_date;
        // local.events[i].error_type = air_shared_area.hm_log.events[i].error_type;
        // local.events[i].level = air_shared_area.hm_log.events[i].level;
        // local.events[i].partition_id = air_shared_area.hm_log.events[i].partition_id;
        
        // /* copy to user land */
        // if (pmk_segregation_put_user(context, local, log) != 0)
        // {

        //     /* disable preemption and return */
        //     cpu_disable_preemption(flags);
        //     return AIR_INVALID_POINTER;
        // }
    }
    printk("------------------------------\n");

    /* disable preemption and return */
    cpu_disable_preemption(flags);
    return AIR_NO_ERROR;
}

void pmk_add_hm_log_entry(air_error_e error_id, pmk_hm_level_id level, pmk_core_ctrl_t *core){

    // Check if it is full
    if (air_shared_area.hm_log.n_events == HM_LOGG_MAX_EVENT_NB) {
        if (air_shared_area.hm_log.policy == PMK_HM_LOG_OVERWRITE)
        {
            // Overwrite policy, increment the tail
            // Means we are overwriting the oldest event
            air_shared_area.hm_log.tail = (air_shared_area.hm_log.tail + 1) % HM_LOGG_MAX_EVENT_NB;
        } else if (air_shared_area.hm_log.policy == PMK_HM_LOG_NO_OVERWRITE)
        {
            // No overwrite policy, return
            return;
        } else {
            printk("Invalid HM Log policy\n");
            pmk_module_shutdown(core);
        }        
    } else {
        // Its not full, increment the count
        air_shared_area.hm_log.n_events++;
    }

    // Get the next writing position
    pmk_hm_log_event_t * log_empty_event = &air_shared_area.hm_log.events[air_shared_area.hm_log.head];
    
    // Add the new event
    log_empty_event->absolute_date = air_shared_area.schedule_ctrl->total_ticks;
    log_empty_event->error_type = error_id;
    log_empty_event->level = level;
    log_empty_event->partition_id = core->partition->id;

    // Increment the head, circularly
    air_shared_area.hm_log.head = (air_shared_area.hm_log.head + 1) % HM_LOGG_MAX_EVENT_NB;
}

/**
 * @brief Health-Monitor ISR handler
 * @param error_id current error id to be handled
 */
void pmk_hm_isr_handler(air_error_e error_id)
{

    air_u32_t core_id = bsp_get_core_id();
    pmk_core_ctrl_t *core_ctrl = &air_shared_area.core[core_id];

    /* get current state and handling level */
    air_state_e state = core_context_get_system_state(core_ctrl->context);
    pmk_hm_level_id level = air_shared_area.hm_system_table[state][error_id];

    /* Add this hm event to the log */
    pmk_add_hm_log_entry(error_id, level, core_ctrl);

    /* perform the HM action according to the handling level */
    switch (level)
    {

    /* error at module level */
    case PMK_ERROR_LEVEL_MODULE:
        pmk_hm_isr_handler_module_level(core_ctrl, state, error_id);
        break;

    /* error at partition level */
    case PMK_ERROR_LEVEL_PARTITION:
    case PMK_ERROR_LEVEL_PROCESS:
    default:
        pmk_hm_isr_handler_partition_level(core_ctrl, state, error_id);
        break;
    }
}

/**
 * @brief Set core system state
 * @param state new system state
 *
 * By setting the system state, the behavior of the Health-Monitor is
 * changed according to the system configuration
 */
void pmk_hm_set_core_state(air_state_e state)
{

    air_u32_t core_id = bsp_get_core_id();

    /* apply the system state */
    core_context_set_system_state(air_shared_area.core[core_id].context, state);
}

/**
 * @brief Set all cores system state
 * @param state new system state
 *
 * By setting the system state, the behavior of the Health-Monitor is
 * changed according to the system configuration
 */
void pmk_hm_set_state(air_state_e state)
{

    air_u32_t i;

    /* apply the system state to all cores */
    for (i = 0; i < air_shared_area.configured_cores; ++i)
    {

        core_context_set_system_state(air_shared_area.core[i].context, state);
    }
}
