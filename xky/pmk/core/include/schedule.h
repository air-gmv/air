/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2008-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file 
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE. 
 * ==========================================================================*/
/**
 * @file schedule.h
 * @author pfnf
 * @brief Partition core scheduling
 *
 * This file defines all data structures and functions necessary to store
 * the (various) partition schedules, and to determine which partition and in
 * which core(s) it be executed
 */

#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

#include <pmk.h>
#include <barrier.h>
#include <partition.h>

/**
 * @defgroup pmk_scheduler PMK - Scheduler
 * @brief Definitions, structures and functions to control the PMK scheduler
 * @{
 */

/**
 * @brief Schedule change actions
 */ 
typedef enum {

	PMK_SCHED_CHANGE_ACTION_IGNORE		= 0,
	PMK_SCHED_CHANGE_ACTION_IDLE		= 1,
	PMK_SCHED_CHANGE_ACTION_WARM_START	= 2,
	PMK_SCHED_CHANGE_ACTION_COLD_START	= 3,

} pmk_sched_change_action_e;

/**
 * @brief Core schedule window of execution
 */
typedef struct {

    xky_u32_t        duration;              /**< Length of the window         */
    xky_u32_t        period_start;          /**< Period start                 */
    pmk_partition_t *partition;             /**< Window partition             */
    xky_u32_t        virtual_core;          /**< Partition virtual-core       */

} pmk_window_t;

/**
 * @brief Core schedule window chain of execution
 */
typedef struct {

    xky_u32_t        count;                 /**< Number of windows            */
    pmk_window_t    *windows;               /**< Array of windows             */

} pmk_core_schedule_t;

/**
 * @brief Partition schedule configuration
 */
typedef struct {

    pmk_partition_t *partition;              /**< Partition                   */
    xky_u32_t set_schedule;                  /**< Change schedule permission  */
    pmk_sched_change_action_e action;        /**< Schedule change action      */
    xky_u64_t period;                        /**< Partition period            */
    xky_u64_t duration;                      /**< Partition period duration   */

} pmk_partition_schedule_t;

/**
 * @brief Module schedule
 */
typedef struct {

    xky_u32_t idx;                           /**< Schedule index              */
    xky_u32_t mtf;                           /**< Schedule MTF size           */
    xky_u32_t initial;                       /**< Initial schedule flag       */
    pmk_list_t partitions;                   /**< Partitions in this schedule */
    pmk_core_schedule_t *cores;              /**< Core schedules              */
    xky_identifier_t id;                     /**< Schedule id                 */
    xky_name_t name;                         /**< Schedule name               */

} pmk_schedule_t;

/**
 * @brief Scheduler control
 */
typedef struct {

    pmk_schedule_t *volatile curr_schedule;  /**< Current schedule            */
    pmk_schedule_t *next_schedule;           /**< Next schedule               */
    xky_clocktick_t mtf_ticks;               /**< Current MTF ticks           */
    xky_clocktick_t total_ticks;             /**< Total elapsed ticks         */
    xky_clocktick_t last_switch_ticks;       /**< Last MTF ticks              */
    pmk_barrier_t   mtf_barrier;             /**< MTF sync barrier            */

} pmk_schedule_ctrl_t;

/**
 * @brief Partition scheduler initialization
 */
void pmk_scheduler_init(void);

/**
 * @brief Get schedule configuration by Id
 * @param pid Schedule Id
 * @return Schedule configuration pointer if Id is valid, NULL otherwise
 */
pmk_schedule_t *pmk_get_schedule_by_id(xky_identifier_t pid);

/**
 * @brief Get schedule configuration by name
 * @param name Schedule name
 * @return Schedule configuration pointer if name is valid, NULL otherwise
 */
pmk_schedule_t *pmk_get_schedule_by_name(xky_name_ptr_t name);

/** @} */

#endif /* ___SCHEDULE_H__ */
