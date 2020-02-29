/*
 * Copyright (C) 2008-2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
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

	PMK_SCHED_CHANGE_ACTION_IGNORE      = 0,
	PMK_SCHED_CHANGE_ACTION_IDLE        = 1,
	PMK_SCHED_CHANGE_ACTION_WARM_START  = 2,
	PMK_SCHED_CHANGE_ACTION_COLD_START  = 3,

} pmk_sched_change_action_e;

/**
 * @brief Core schedule window of execution
 */
typedef struct {

    air_identifier_t id;                    /**< Window Identifier            */
    air_u32_t        duration;              /**< Length of the window         */
    air_u32_t        period_start;          /**< Period start                 */
    pmk_partition_t *partition;             /**< Window partition             */
    air_u32_t        virtual_core;          /**< Partition virtual-core       */

} pmk_window_t;

/**
 * @brief Core schedule window chain of execution
 */
typedef struct {

    air_u32_t        count;                 /**< Number of windows            */
    pmk_window_t    *windows;               /**< Array of windows             */

} pmk_core_schedule_t;

/**
 * @brief Partition schedule configuration
 */
typedef struct {

    pmk_partition_t *partition;              /**< Partition                   */
    air_u32_t set_schedule;                  /**< Change schedule permission  */
    pmk_sched_change_action_e action;        /**< Schedule change action      */
    air_u64_t period;                        /**< Partition period            */
    air_u64_t duration;                      /**< Partition period duration   */

} pmk_partition_schedule_t;

/**
 * @brief Module schedule
 */
typedef struct {

    air_u32_t idx;                           /**< Schedule index              */
    air_u32_t mtf;                           /**< Schedule MTF size           */
    air_u32_t initial;                       /**< Initial schedule flag       */
    pmk_list_t partitions;                   /**< Partitions in this schedule */
    pmk_core_schedule_t *cores;              /**< Core schedules              */
    air_identifier_t id;                     /**< Schedule id                 */
    air_name_t name;                         /**< Schedule name               */

} pmk_schedule_t;

/**
 * @brief Scheduler control
 */
typedef struct {

    pmk_schedule_t *volatile curr_schedule;  /**< Current schedule            */
    pmk_schedule_t *next_schedule;           /**< Next schedule               */
    air_clocktick_t mtf_ticks;               /**< Current MTF ticks           */
    air_clocktick_t total_ticks;             /**< Total elapsed ticks         */
    air_clocktick_t last_switch_ticks;       /**< Last MTF ticks              */
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
pmk_schedule_t *pmk_get_schedule_by_id(air_identifier_t pid);

/**
 * @brief Get schedule configuration by name
 * @param name Schedule name
 * @return Schedule configuration pointer if name is valid, NULL otherwise
 */
pmk_schedule_t *pmk_get_schedule_by_name(air_name_ptr_t name);

/**
 * @brief Get system total elapsed ticks
 * @return Total elapsed ticks
 */
air_clocktick_t pmk_get_schedule_total_ticks(void);

/** @} */

#endif /* ___SCHEDULE_H__ */
