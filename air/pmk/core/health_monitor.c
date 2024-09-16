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
#include <error.h>

/**
 * @brief Lock for accessing the Health-Monitor log
 */
atomic_t hm_log_lock;

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
    #ifdef PMK_HM_LOG_OVERWRITE
    air_shared_area.hm_log.policy = PMK_HM_LOG_NO_OVERWRITE; 
    #else
    air_shared_area.hm_log.policy = PMK_HM_LOG_OVERWRITE; 
    #endif
    
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

/**
 * @brief Retrieves the Health Monitor log. Partitions must have SUPERVISOR permissions.
 *        Returns the log data to the user space, with the most recent events first.
 * @param[in] core pointer to current core control structure
 * @param[out] log pointer to the `air_hm_log_t` structure where the HM log data 
 *                 will be copied.
 *
 * @return `AIR_NO_ERROR` if the operation is successful.
 * @return `AIR_INVALID_CONFIG` if the partition does not have supervisor permissions.
 * @return `AIR_INVALID_POINTER` if there is an issue copying the log data to user space.
 */
air_status_code_e pmk_get_hm_log(pmk_core_ctrl_t *core, air_hm_log_t *log){
    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;
    air_u32_t current = 0;

    //Local structure to hold the log
    air_hm_log_t local;

    /* Check if partition has supervisor permission*/
    if (core->partition->permissions != AIR_PERMISSION_SUPERVISOR)
    {
        return AIR_INVALID_CONFIG;
    }

    if (air_shared_area.hm_log.n_events == 0) {
        return AIR_NO_ERROR;
    }

    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* fill local structure */
    local.n_events = air_shared_area.hm_log.n_events;

    current = air_shared_area.hm_log.tail;

    for (int i = 0; i < air_shared_area.hm_log.n_events; i++)
    {
        // Copy to the local structure
        local.events[i].absolute_date = air_shared_area.hm_log.events[current].absolute_date;
        local.events[i].error_type = air_shared_area.hm_log.events[current].error_type;
        local.events[i].level = air_shared_area.hm_log.events[current].level;
        local.events[i].partition_id = air_shared_area.hm_log.events[current].partition_id;
        
        // Increment the current pointer to the next element, circularly
        current = (current + 1) % air_shared_area.hm_log.n_events;
    }

     /* copy the local struct to the user land */
    if (pmk_segregation_put_user(context, local, log) != 0)
    {
        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return AIR_INVALID_POINTER;
    }

    /* disable preemption and return */
    cpu_disable_preemption(flags);
    return AIR_NO_ERROR;
}

/**
 * @brief Pops the most recent event from the Health Monitor log.
 *
 * Retrieves the most recent event from the Health Monitor log removes it from the log, 
 * and copies it to the user-provided `log` structure.
 * Partitions must have SUPERVISOR permission.
 *
 * @param[in] core pointer to the core control structure
 * @param[out] log pointer to the `air_hm_log_event_t` structure where the most recent HM log event will be copied.
 *
 * @warning Some kind of lock may need to be implemented
 * @return `AIR_NO_ERROR` if the operation is successful or if the log is empty.
 * @return `AIR_INVALID_CONFIG` if the partition does not have supervisor permissions.
 * @return `AIR_INVALID_POINTER` if there is an issue copying the log event to user space.
 */
air_status_code_e pmk_pop_from_hm_log(pmk_core_ctrl_t *core, air_hm_log_event_t *log){

    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;
    air_hm_log_event_t local;
    air_u32_t new_head = 0;

    /* Check if partition has supervisor permission*/
    if (core->partition->permissions != AIR_PERMISSION_SUPERVISOR)
    {
        return AIR_INVALID_CONFIG;
    }
    
    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* Check if the log is empty */
    if (air_shared_area.hm_log.n_events == 0) {
        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return AIR_NO_ERROR;
    }

    //Lock access to the LOG
    while (atomic_swap(1, &hm_log_lock) == 1)
    {
        //Wait for lock to be released
    }	

    // Get the new head
    new_head = (air_shared_area.hm_log.head - 1 + HM_LOGG_MAX_EVENT_NB) % HM_LOGG_MAX_EVENT_NB;
    
    // Copy the most the most recent log event to the user land
    local.absolute_date = air_shared_area.hm_log.events[new_head].absolute_date;  
    local.error_type = air_shared_area.hm_log.events[new_head].error_type;  
    local.level = air_shared_area.hm_log.events[new_head].level;  
    local.partition_id = air_shared_area.hm_log.events[new_head].partition_id;  
    

    /* copy the local struct to the user land */
    if (pmk_segregation_put_user(context, local, log) != 0)
    {
        /* disable preemption and return */
        cpu_disable_preemption(flags);
        return AIR_INVALID_POINTER;
    }
    
    // If the copy was successfull update the head, efectively removing the last event
    air_shared_area.hm_log.n_events--;  // Decrement count
    air_shared_area.hm_log.head = new_head; // Update head

    //unlock after use
    atomic_set(0, &hm_log_lock);
   
    /* disable preemption and return */
    cpu_disable_preemption(flags);
    
    return AIR_NO_ERROR;
}

/**
 * @brief Adds a new entry to the Health Monitor log.
 * 
 * Adds a new entry to the log. Takes into account the log policy when the log is full.
 * Implemented as a circular buffer with lenght `HM_LOGG_MAX_EVENT_NB`.
 * 
 * If policy is set to `PMK_HM_LOG_OVERWRITE`, the oldest entry will be overwritten.
 * 
 * If policy is set to `PMK_HM_LOG_NO_OVERWRITE`, the function will return without adding a new entry. 
 * The new entry is lost.
 * 
 * @param[in] error_id The error identifier (`air_error_e`) to be logged.
 * @param[in] level The severity level (`pmk_hm_level_id`) of the log entry.
 * @param[in] core A pointer to the core control structure (`pmk_core_ctrl_t`) 
 *                 containing the context and partition information.
 *
 * @warning Some kind of lock may need to be implemented
 * @note If the log is full and the policy is set to `PMK_HM_LOG_NO_OVERWRITE`, the function will return without adding a new entry.
 * @note If the log is full and the policy is set to `PMK_HM_LOG_OVERWRITE`, the oldest log entry will be overwritten.
 * @note If an invalid log policy is set, the function will print an error message and shut down the module.
 */
void pmk_add_hm_log_entry(air_error_e error_id, pmk_hm_level_id level, pmk_core_ctrl_t *core){
    pmk_hm_log_event_t * log_empty_event = NULL;

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
            pmk_fatal_error(PMK_INTERNAL_ERROR_CONFIG, __func__, __FILE__, __LINE__);
            pmk_module_shutdown(core);
        }        
    } else {
        // Its not full, increment the count
        air_shared_area.hm_log.n_events++;
    }

    // Get the next writing position
    log_empty_event = &air_shared_area.hm_log.events[air_shared_area.hm_log.head];
    
    if (log_empty_event == NULL)
    {
        pmk_fatal_error(PMK_INTERNAL_ERROR_CONFIG, __func__, __FILE__, __LINE__);
        pmk_module_shutdown(core);
    }
    
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
    
    // Lock access to the log
    while (atomic_swap(1, &hm_log_lock) == 1)
    {
        //Wait for lock to be released
    }
    
    pmk_add_hm_log_entry(error_id, level, core_ctrl);
    
    //unlock after use
    atomic_set(0, &hm_log_lock);

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
