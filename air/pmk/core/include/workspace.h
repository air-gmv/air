/*
 * Copyright (C) 2013-2015  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
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
typedef struct
{

    /**< core index                                                         */
    air_u32_t idx;
    /** partition virtual core context control                              */
    core_context_t *volatile context;
    /** partition current partition                                         */
    pmk_partition_t *volatile partition;
    /** core Health-Monitor state                                           */
    air_state_e state;
    /** current core schedule                                               */
    pmk_core_schedule_t *volatile schedule;
    /** current core schedule preemption point                              */
    volatile air_u32_t preempt_point;
    /** flag of partition switching                                         */
    volatile air_u32_t partition_switch;
    /** core next context switch ticks                                      */
    volatile air_u64_t next_switch_ticks;

} pmk_core_ctrl_t;

/**
 * @brief Health-Monitor log event
 */
typedef struct
{
    air_clocktick_t absolute_date;
    air_error_e error_type;
    pmk_hm_level_id level;
    air_identifier_t partition_id;
} pmk_hm_log_event_t;

/**
 * @brief Health-Monitor log structure
 * Implemented as a circular buffer with length `HM_LOGG_MAX_EVENT_NB`.
 */
typedef struct
{
    pmk_hm_log_policy policy;
    air_u32_t head;  // Points to the next write position
    air_u32_t tail;  // Points to the oldest log
    air_u32_t n_events;  
    pmk_hm_log_event_t events[HM_LOGG_MAX_EVENT_NB];
} pmk_hm_log_t;

/**
 *  @brief Share area structure
 *
 *  This area is accessible to all system cores and provides access to each
 *  core control structure as well to the current schedule and system/module
 *  health-monitor tables
 */
typedef struct
{

    /** Numner of configured cores */
    air_u32_t configured_cores;
    /** Per core control structure */
    pmk_core_ctrl_t *core;
    /** New schedule control                                                */
    pmk_schedule_ctrl_t *schedule_ctrl;
    /** System state Health-Monitor table                                   */
    pmk_hm_level_list_t *hm_system_table;
    /** Module Health-Monitor table                                         */
    pmk_hm_action_list_t **hm_module_table;
    /** Health-Monitor log                                                  */
    pmk_hm_log_t hm_log;
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
void *pmk_workspace_alloc(air_sz_t size);

/**
 * @brief Allocates space on the PMK workspace
 * @param size Size of the required memory block
 * @param align Alignment of the memory block
 * @note This function generates an fatal error if no more memory is available
 *       on the workspace
 */
void *pmk_workspace_aligned_alloc(air_sz_t size, air_u32_t align);

#ifdef PMK_DEBUG
/**
 * @brief Gets the space current used by the workspace
 */
void pmk_workspace_debug(void);
#endif

/**
 * @brief PMK Shared area
 */
extern pmk_sharedarea_t air_shared_area;

/** @} */

/**
 * @brief Partition scheduler
 * @param isf Interrupt Stack Frame
 * @param core Executing core control structure
 * @ingroup pmk_scheduler
 */
void pmk_partition_scheduler(void *isf, pmk_core_ctrl_t *core);

/**
 * @brief IPC trap handler
 * @param isf Executing stack frame
 * @param core Executing core control structure
 */
void pmk_ipc_handler(void *isf, pmk_core_ctrl_t *core);

#endif /* __WORKSPACE_H__ */
