/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2015
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file
 * @author pfnf
 * @brief Definitions, structures and functions to handle Inter-partition
 *        Communications via Queuing Ports
 */

#ifndef __PORTS_QUEUING_H__
#define __PORTS_QUEUING_H__

#include <pmk.h>
#include <ports.h>

/**
 * @defgroup pmk_queuing_ports PMK - Ports & Channels (Queuing port support)
 * @brief Definitions, structures and functions to handle Inter-partition
 *        Communications via Queuing Ports
 * @{
 */

/**
 * @brief Queuing Port/Channel control & configuration structure
 */
typedef struct {

    xky_u32_t max_message_size;
    xky_u32_t max_nb_message;
    xky_u32_t discipline;

    pmk_message_slot_t *queue;
    xky_u32_t first;
    xky_u32_t last;
    atomic_t count;
    atomic_t overflowed;
    atomic_t reading;
    atomic_t writing;

} pmk_queuing_config_t;

/**
 * @brief Initializes Queuing Channel Configuration
 * @param channel Channel control structure
 */
void pmk_queuing_channel_init(pmk_channel_t *channel);

/**
 * @brief Check the user configuration against the internal configuration
 * @param port Port to check
 * @param usr_config User supplied configuration
 * @return 0 if valid, 1 otherwise
 */
xky_u32_t pmk_queuing_port_check_configuration(
        pmk_port_t *port, xky_queuing_port_status_t *usr_config);

/**
 * @brief Handle a get status from a queuing port
 * @param context Controlling core context
 * @param port Port to get the status
 * @param status Status of the queuing port
 *
 * @note The parameter status is used to handle different types of get status
 *       operations, this pointer is in partition space and must be checked
 *       before using
 */
xky_status_code_e pmk_queuing_port_get_status(
        core_context_t *context, pmk_port_t *port,
        xky_queuing_port_status_t *status);

/**
 * @brief Handle an read operation from a queuing port
 * @param context Controlling core context
 * @param port Port being read
 * @param msg Partition message pointer
 * @param length Length of message
 * @param status Status of the queuing port
 *
 * @note the parameter status is used to handle different types of writing
 *       operations (currently not used), this pointer is in partition space
 *       and must be checked before using
 */
xky_status_code_e pmk_queuing_port_read(
        core_context_t *context,
        pmk_port_t *port, xky_message_ptr_t msg, xky_sz_t *length,
        xky_queuing_port_status_t *status);

/**
 * @brief Handle an write operation from a queuing port
 * @param context Controlling core context
 * @param port Port being written
 * @param msg Partition message pointer
 * @param length Length of message
 * @param status Status of the queuing port
 *
 * @note The parameter status is used to handle different types of writing
 *       operations (currently not used), this pointer is in partition space
 *       and must be checked before using
 */
xky_status_code_e pmk_queuing_port_write(
        core_context_t *context,
        pmk_port_t *port, xky_message_ptr_t msg, xky_sz_t length,
        xky_queuing_port_status_t *status);

/** @} */

#endif /* __PORTS_QUEUING_H__ */
