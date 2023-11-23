/*
 * Copyright (C) 2008-2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/** 
 *  @file health_monitor.h
 *  @author pfnf
 *  @brief Health Monitor functionality
 *
 *  The Health Monitor in PMK follows the ARINC 653 specifications.
 *  The HM tables for the Module are stored in the PMK shared-area and
 *  each partition holds the its specific HM tables in its control structure
 *
 *  An HM event is passed to the Guest OS by injecting a trap in its execution
 */
 
#ifndef __HEALTH_MONITOR_H__
#define __HEALTH_MONITOR_H__

#ifndef ASM
#include <pmk.h>

#define PMK_HM_STATES_COUNT                                  (7)
#define PMK_HM_ERRORS_COUNT                                 (16)

/**
 * @brief PMK Error Levels
 */
typedef enum {

    PMK_ERROR_LEVEL_UNDEFINED   = 0x01, /**< Undefined/default level         */
    PMK_ERROR_LEVEL_MODULE      = 0x01, /**< Error at Module level           */
    PMK_ERROR_LEVEL_PARTITION   = 0x02, /**< Error at Partition level        */
    PMK_ERROR_LEVEL_PROCESS     = 0x03, /**< Error at Process level          */

} pmk_hm_level_id;

/**
 * @brief PMK Health-Monitor actions
 */
typedef enum {

    PMK_ACTION_UNDEFINED        = 0x00, /**< Undefined error                 */
    PMK_ACTION_IGNORE           = 0x00, /**< Ignore error                    */
    PMK_ACTION_SHUTDOWN         = 0x01, /**< Shutdown system                 */
    PMK_ACTION_RESET            = 0x02, /**< Restart system                  */
    PMK_ACTION_IDLE             = 0x03, /**< Set Partition in Idle           */
    PMK_ACTION_WARM_START       = 0x04, /**< Set Partition in Warm restart   */
    PMK_ACTION_COLD_START       = 0x05, /**< Set Partition in Cold restart   */

} pmk_hm_action_id;

/**
 * @brief List of Levels for the System State Table
 */
typedef pmk_hm_level_id pmk_hm_level_list_t[PMK_HM_ERRORS_COUNT];

/**
 * @brief System State Table
 */
typedef pmk_hm_level_list_t pmk_hm_system_state_table_t[PMK_HM_STATES_COUNT];

/**
 * @brief List of Actions for the Health-Monitor Table
 */
typedef pmk_hm_action_id pmk_hm_action_list_t[PMK_HM_ERRORS_COUNT];

/**
 * @brief Health-Monitor Table
 */
typedef pmk_hm_action_list_t pmk_hm_action_table_t[PMK_HM_STATES_COUNT];

/**
 * @brief Health-Monitor event structure
 */
typedef struct {

    air_error_e error_id;              /**< HM last error Id                 */
    air_state_e state_id;               /**< HM last state Id                 */
    air_state_e previous_state_id;      /**< State prior to the 1st HM event  */
    air_u32_t nesting;                   /**< HM error nesting                 */

} pmk_hm_event_t;

/**
 * @brief Health-Monitor Initialization
 */
void pmk_hm_init();
/**
 * @brief Health-Monitor ISR handler
 * @param error_id current error id to be handled
 */
void pmk_hm_isr_handler(air_error_e error_id);
/**
 * @brief Set core system state
 * @param state new system state
 *
 * By setting the system state, the behavior of the Health-Monitor is
 * changed according to the system configuration
 */
void pmk_hm_set_core_state(air_state_e state);
/**
 * @brief Set all cores system state
 * @param state new system state
 *
 * By setting the system state, the behavior of the Health-Monitor is
 * changed according to the system configuration
 */
void pmk_hm_set_state(air_state_e state);
/**
 * @brief Gets the Health-Monitor System State table
 * @returns Health-Monitor System State table pointer
 */
pmk_hm_level_list_t *pmk_hm_get_system_state_table(void);
/**
 * @brief Gets the module Health-Monitor Table
 * @returns Module Health-Monitor System State table pointer
 */
pmk_hm_action_list_t **pmk_hm_get_module_hm_table(void);

#else /* ASM */

#define AIR_STATE_MODULE_INIT                                   0x00
#define AIR_STATE_MODULE_EXEC                                   0x01
#define AIR_STATE_PARTITION_INIT                                0x02
#define AIR_STATE_PARTITION_EXEC                                0x03
#define AIR_STATE_PARTITION_PROC                                0x04
#define AIR_STATE_PARTITION_HM                                  0x05
#define AIR_STATE_MODULE_HM                                     0x06

#define PMK_ERROR_LEVEL_UNDEFINED                               0x01
#define PMK_ERROR_LEVEL_MODULE                                  0x01
#define PMK_ERROR_LEVEL_PARTITION                               0x02
#define PMK_ERROR_LEVEL_PROCESS                                 0x03

#define PMK_ACTION_UNDEFINED                                    0x00
#define PMK_ACTION_IGNORE                                       0x00
#define PMK_ACTION_SHUTDOWN                                     0x01
#define PMK_ACTION_RESET                                        0x02
#define PMK_ACTION_SET_IDLE                                     0x03
#define PMK_ACTION_SET_WARM                                     0x04
#define PMK_ACTION_SET_COLD                                     0x05

#endif
#endif /* __HEALTH_MONITOR_H__ */
