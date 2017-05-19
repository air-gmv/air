/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file 
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE. 
 * ==========================================================================*/
/**
 * @file
 * @author cdcs
 * @brief Contains the main control structures for the PMK and the internal
 *        memory allocation primitives
 */

#ifndef __WORKSPACE_H__
#define __WORKSPACE_H__

#include <stdint.h>
#include <stddef.h>
#include <schedule.h>
#include <partition.h>
#include <health_monitor.h>

/**
 * @defgroup pmk_workspace PMK - Workspace
 * @brief Main control structures for the PMK and the internal memory allocation
 *        primitives
 * @{
 */

/**
 * @brief Core control structure
 *
 * This core control structure contains all the relevant executing core data
 * such as the owner partition and the core schedule for the current module
 * schedule
 */
typedef struct {

    /**< core index                                                         */
    xky_u32_t idx;
    /** partition virtual core context control                              */
    core_context_t *volatile context;
    /** partition current partition                                         */
    pmk_partition_t *volatile partition;
    /** core Health-Monitor state                                           */
    xky_state_e state;
    /** current core schedule                                               */
    pmk_core_schedule_t *volatile schedule;
    /** current core schedule preemption point                              */
    volatile xky_u32_t preempt_point;
    /** flag of partition switching                                         */
    volatile xky_u32_t partition_switch;
    /** core next context switch ticks                                      */
    volatile xky_u64_t next_switch_ticks;

} pmk_core_ctrl_t;

/**
 *  @brief Share area structure
 *
 *  This area is accessible to all system cores and provides access to each
 *  core control structure as well to the current schedule and system/module
 *  health-monitor tables
 */
typedef struct {

    /** Numner of configured cores */
    xky_u32_t configured_cores;
    /** Per core control structure */
    pmk_core_ctrl_t *core;
    /** New schedule control                                                */
    pmk_schedule_ctrl_t *schedule_ctrl;
    /** System state Health-Monitor table                                   */
    pmk_hm_level_list_t *hm_system_table;
    /** Module Health-Monitor table                                         */
    pmk_hm_action_list_t **hm_module_table;

} pmk_sharedarea_t;

/**
 * @brief Initializes the PMK workspace
 */
void pmk_workspace_init(void);

/**
 * @brief Allocates space on the PMK workspace
 * @param size Size of the required memory block
 * @note This function generates an fatal error if no more memory is available
 *       on the workspace
 */
void *pmk_workspace_alloc(xky_sz_t size);

/**
 * @brief Allocates space on the PMK workspace
 * @param size Size of the required memory block
 * @param align Alignment of the memory block
 * @note This function generates an fatal error if no more memory is available
 *       on the workspace
 */
void *pmk_workspace_aligned_alloc(xky_sz_t size, xky_u32_t align);

#ifdef PMK_DEBUG
/**
 * @brief Gets the space current used by the workspace
 */
void pmk_workspace_debug(void);
#endif

/**
 * @brief PMK Shared area
 */
extern pmk_sharedarea_t xky_shared_area;

/** @} */


/**
 * @brief Partition scheduler
 * @param isf Interrupt Stack Frame
 * @param core Executing core control structure
 * @ingroup pmk_scheduler
 */
void pmk_partition_scheduler(void *isf, pmk_core_ctrl_t *core);

#endif /* __WORKSPACE_H__ */
