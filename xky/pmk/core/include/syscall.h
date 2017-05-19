/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file syscall.h
 * @author pfnf
 * @brief Common system call function defintions
 */

#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <pmk.h>
#include <multicore.h>

/**
 * @brief Get Partition Id
 * @param core executing core information
 * @param name name of the partition to look for (current if name is NULL)
 * @param pid pointer to store the partition id
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if not enough permissions,
 *         INVALID_PARAM   - if invalid partition name,
 *         NO_ERROR        - otherwise
 */
xky_status_code_e pmk_syscall_get_partition_id(
        pmk_core_ctrl_t *core,
        xky_name_ptr_t name,
        xky_identifier_t *pid);
/**
 * @brief Get Partition Status
 * @param core executing core information
 * @param pid Id of the partition
 * @param status pointer to store the status
 * @return INVALID_CONFIG  - if not enough permissions,
 *         INVALID_PARAM   - if invalid partition id,
 *         NO_ERROR        - no error otherwise
 */
xky_status_code_e pmk_syscall_get_partition_status(
        pmk_core_ctrl_t *core,
        xky_identifier_t pid,
        xky_partition_status_t *status);
/**
 * @brief Set partition mode
 * @param idx partition index
 * @param mode mode to be set
 * @return INVALID_CONFIG  - if no permissions
 *         INVALID_PARAM   - if invalid mode or target partition
 *         NO_ACTION       - if transition from normal to normal
 *         NO_ERROR        - no error otherwise
 */
xky_status_code_e pmk_syscall_set_partition_mode(
        pmk_core_ctrl_t *core_ctrl,
        xky_identifier_t pid,
        xky_operating_mode_e mode);
/**
 * @brief Get Schedule Id
 * @param core executing core information
 * @param sid pointer to store the schedule id
 * @param name name of the schedule to look for (current if name is NULL)
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_PARAM   - if invalid schedule name,
 *         NO_ERROR        - otherwise
 */
xky_status_code_e pmk_syscall_get_schedule_id(
        pmk_core_ctrl_t *core,
        xky_name_ptr_t name,
        xky_identifier_t *sid);
/**
 * @brief Get Schedule Status
 * @param core executing core information
 * @param status pointer to store the schedule status
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         NO_ERROR        - otherwise
 */
xky_status_code_e pmk_syscall_get_schedule_status(
        pmk_core_ctrl_t *core,
        xky_schedule_status_t *status);
/**
 * @brief Set Module schedule
 * @param core executing core information
 * @param sid next schedule id
 * @return INVALID_CONFIG  - if the partition doesn't have enough permissions,
 *         INVALID_PARAM   - if schedule id isn't valid,
 *         NO_ACTION       - if the current id is the schedule id
 *         NO_ERROR        - otherwise
 */
xky_status_code_e pmk_syscall_set_schedule(
        pmk_core_ctrl_t *core,
        xky_identifier_t sid);
/**
 * @brief Gets the system time of day
 * @param core executing core information
 * @param[out] tod time of day of system
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if the ToD haven't been set
 *         NO_ERROR        - otherwise
 */
xky_status_code_e pmk_syscall_get_tod(pmk_core_ctrl_t *core, xky_time_t *tod);
/**
 * @brief Sets the system time of day
 * @param core executing core information
 * @param tod time of day to be set
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if the partition doesn't have enough permissions,
 *         NO_ERROR        - otherwise
 */
xky_status_code_e pmk_syscall_set_tod(pmk_core_ctrl_t *core, xky_time_t *tod);
/**
 * @brief Get port Id
 * @param core current controlling core
 * @param type type of port to create
 * @param name name of the port to look for
 * @param[out] pid pointer to store the port id
 * @return INVALID_POINTER if the partition pointers aren't valid
 *         INVALID_CONFIG if port not found
 *         NO_ERROR otherwise
 */
xky_status_code_e pmk_syscall_get_port_id(
    pmk_core_ctrl_t *core,
    xky_port_type_e type,
    xky_name_ptr_t name,
    xky_identifier_t *pid);
/**
 * @brief System call create port
 * @param core current controlling core
 * @param type type of port to create
 * @param name name of the port to create
 * @param config port configuration
 * @param[out] pid port id
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if port not found or mismatch in configuration
 *         NO_ACTION       - if port already created
 *         NO_ERROR        - otherwise
 */
xky_status_code_e pmk_syscall_create_port(
        pmk_core_ctrl_t *core,
        xky_port_type_e type,
        xky_name_ptr_t name,
        void *config,
        xky_identifier_t *pid);
/**
 * @brief System call get port status
 * @param core current controlling core
 * @param type type of port to get status
 * @param pid port identifier
 * @param[in/out] status port status
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_PARAM   - if port not found
 *         NO_ERROR        - otherwise
 */
xky_status_code_e pmk_syscall_get_port_status(
        pmk_core_ctrl_t *core,
        xky_port_type_e type,
        xky_identifier_t pid,
        void *status);
/**
 * @brief System call read from port
 * @param core current controlling core
 * @param type type of port to read from
 * @param id port identifier
 * @param[out] msg message buffer
 * @param[out] length length of the message
 * @param[in/out] status port status/configuration
 */
xky_status_code_e pmk_syscall_read_port(
        pmk_core_ctrl_t *core,
        xky_port_type_e type,
        xky_identifier_t id,
        xky_message_ptr_t msg,
        xky_sz_t *length,
        void *status);
/**
 * @brief System call write to port
 * @param core current controlling core
 * @param type type of port to write
 * @param id port identifier
 * @param msg message buffer
 * @param length length of the message
 * @param[in/out] status port status/configuration
 */
xky_status_code_e pmk_syscall_write_port(
        pmk_core_ctrl_t *core,
        xky_port_type_e type,
        xky_identifier_t id,
        xky_message_ptr_t msg,
        xky_sz_t length,
        void *status);
/**
 * @brief Gets the information of a shared memory area
 * @param core executing core information
 * @param name name of the shared memory to look for
 * @param[out] sharedmemory shared memory information
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         INVALID_CONFIG  - if the shared memory doesn't not exist or if the
 *                           partition doesn't have access to it
 *         NO_ERROR        - otherwise
 */
xky_status_code_e pmk_syscall_get_sharedmemory(
        pmk_core_ctrl_t *core,
        xky_name_ptr_t name,
        xky_sharedmemory_t *sharedmemory);

/**
 * @brief Sets the system state
 * @param core executing core information
 * @param state New system state
 * @return NO_ACTION - if state change is not possible or already in the desired
 *                     state
 *         NO_ERROR  - otherwise
 */
xky_status_code_e pmk_syscall_set_system_state(
        pmk_core_ctrl_t *core, xky_state_e state);

/**
 * @brief Gets the last HM event
 * @param core executing core information
 * @param[out] event HM event
 * @return INVALID_POINTER - if the partition pointers aren't valid,
 *         NO_ERROR        - otherwise
 */
xky_status_code_e pmk_syscall_get_hm_event(
        pmk_core_ctrl_t *core,
        xky_hm_event_t *event);

/**
 * @brief Prints a partition character
 * @param core executing core information
 * @param ch buffer to be printed
 */
void pmk_syscall_putchar(pmk_core_ctrl_t *core, char ch);
/**
 * @brief Prints a partition buffer
 * @param core executing core information
 * @param buffer buffer to be printed
 * @param len size of the buffer
 * @return Number of characters printed
 */
xky_u32_t pmk_syscall_print(pmk_core_ctrl_t *core, char *buffer, xky_sz_t len);

#endif /* __SYSCALL_H__ */

