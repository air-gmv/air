/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file 
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE. 
 * ==========================================================================*/
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
#include <bsp.h>
#include <ipc.h>
#include <printk.h>
#include <workspace.h>
#include <multicore.h>
#include <configurations.h>
#include <health_monitor.h>

void pmk_module_shutdown(pmk_core_ctrl_t *core) {

    /* shutdown all configured cores */
    xky_u32_t i;
    for (i = 0; i < xky_shared_area.configured_cores; ++i) {

        /* shutdown current core directly */
        if (xky_shared_area.core[i].idx == core->idx) {

            bsp_shutdown_core(core);

        /* shutdown core via IPC message */
        } else {

            core_context_set_ipc_message(
                    xky_shared_area.core[i].context,
                    PMK_IPC_MODULE_SHUTDOWN);
        }
    }
}

void pmk_module_restart(pmk_core_ctrl_t *core) {

    /* restart all configured cores */
    xky_u32_t i;
    for (i = 0; i < xky_shared_area.configured_cores; ++i) {

        /* restart current core directly */
        if (xky_shared_area.core[i].idx == core->idx) {

            bsp_shutdown_core(core);

        /* restart core via IPC message */
        } else {

            core_context_set_ipc_message(
                    xky_shared_area.core[i].context,
                    PMK_IPC_MODULE_RESTART);
        }
    }
}

/**
 * @brief Health-Monitor Initialization
 */
void pmk_hm_init() {

#ifdef PMK_DEBUG
    printk(" :: Health-Monitor initialization\n");
#endif

    /* setup the architecture dependent health-monitor */
    cpu_hm_init();

    /* setup the system state table and module health-monitor table */
    xky_shared_area.hm_system_table = pmk_get_usr_hm_system_table();
    xky_shared_area.hm_module_table = pmk_get_usr_hm_module_table();

#ifdef PMK_DEBUG
    pmk_workspace_debug();
#endif
}

/**
 * @brief Health-Monitor handler for the Module level
 * @param core_ctrl current core control structure
 * @param state_id current core state id
 * @param error_id current error id
 */
void pmk_hm_isr_handler_module_level(pmk_core_ctrl_t *core,
                                     xky_state_e state_id,
                                     xky_error_e error_id) {

    /* setup the undefined action */
    pmk_hm_action_id action_id = PMK_ACTION_UNDEFINED;

    /* lookup for the action in Module Health-Monitor table */
    pmk_hm_action_id *actions =
            (pmk_hm_action_id *)xky_shared_area.hm_module_table[state_id];
    if (actions != NULL) {

        action_id = actions[error_id];
    }

    /* perform the action */
    switch (action_id) {

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
 * @param core_ctrl current core control structure
 * @param partition partition configuration structure pointer
 * @param state_id current core state id
 * @param error_id current error id
 */
void pmk_hm_isr_handler_partition_level(
        pmk_core_ctrl_t *core,
        xky_state_e state_id,
        xky_error_e error_id) {

    cpu_preemption_flags_t flags;
    core_context_t *context = core->context;
    pmk_partition_t *partition = core->partition;

    /* allow partition to be preempted */
    cpu_enable_preemption(flags);

    /* lookup for the action_id in Partition Health-Monitor table */
    pmk_hm_action_id action_id = PMK_ACTION_UNDEFINED;
    pmk_hm_action_id *actions =
            (pmk_hm_action_id *)partition->hm_table[state_id];
    if (partition->hm_table[state_id] != NULL) {

        action_id = actions[error_id];
    }

    /* perform the action_id */
    switch (action_id) {

        /* set partition in idle mode */
        case PMK_ACTION_IDLE:

            /* disable preemption */
            cpu_disable_preemption(flags);

            /* halt partition */
            pmk_partition_halt(partition);
            partition->mode = XKY_MODE_IDLE;
            break;

        /* cold restart the partition */
        case PMK_ACTION_COLD_START:

            /* disable preemption */
            cpu_disable_preemption(flags);

            /* cold restart partition */
            pmk_partition_restart(partition);
            partition->mode = XKY_MODE_COLD_START;
            partition->start_condition = XKY_START_CONDITION_HM_PARTITION_RESTART;
            break;

        /* warm restart the partition */
        case PMK_ACTION_WARM_START:

            /* disable preemption */
            cpu_disable_preemption(flags);

            /* warm restart partition */
            pmk_partition_restart(partition);
            partition->mode = XKY_MODE_WARM_START;
            partition->start_condition = XKY_START_CONDITION_HM_PARTITION_RESTART;
            break;

        /* ignore event and pass it to the partition callback */
        default:

            /* disable preemption */
            cpu_disable_preemption(flags);

            /* add HM event */
            core_context_add_hm_event(context, state_id, error_id);
            break;
    }
}

/**
 * @brief Health-Monitor ISR handler
 * @param error_id current error id to be handled
 */
void pmk_hm_isr_handler(xky_error_e error_id) {

    xky_u32_t core_id = bsp_get_core_id();
    pmk_core_ctrl_t *core_ctrl = &xky_shared_area.core[core_id];

    /* get current state and handling level */
    xky_state_e state = core_context_get_system_state(core_ctrl->context);
    pmk_hm_level_id level = xky_shared_area.hm_system_table[state][error_id];

    /* perform the HM action according to the handling level */
    switch (level) {

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
void pmk_hm_set_core_state(xky_state_e state) {

    xky_u32_t core_id = bsp_get_core_id();

    /* apply the system state */
    core_context_set_system_state(
            xky_shared_area.core[core_id].context,
            state);
}

/**
 * @brief Set all cores system state
 * @param state new system state
 *
 * By setting the system state, the behavior of the Health-Monitor is
 * changed according to the system configuration
 */
void pmk_hm_set_state(xky_state_e state) {

    xky_u32_t i;

    /* apply the system state to all cores */
    for (i = 0; i < xky_shared_area.configured_cores; ++i) {

        core_context_set_system_state(
                xky_shared_area.core[i].context,
                state);
    }
}
