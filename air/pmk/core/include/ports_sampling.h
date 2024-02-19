/*
 * Copyright (C) 2015  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief Definitions, structures and functions to handle Inter-partition
 *        Communications via Sampling Ports
 */

#ifndef __PORTS_SAMPLING_H__
#define __PORTS_SAMPLING_H__

#include <pmk.h>
#include <ports.h>

/**
 * @defgroup pmk_sampling_ports PMK - Ports & Channels (Sampling port support)
 * @brief Definitions, structures and functions to handle Inter-partition
 *        Communications via Sampling Ports
 * @{
 */

/**
 * @brief Sampling channel configuration
 */
typedef struct
{

    air_u32_t max_message_size; /**< maximum message size               */
    air_u32_t refresh_period;   /**< message refresh period             */

    atomic_t writing;           /**< write access flag                  */
    atomic_t current_slot;      /**< current slot                       */
    pmk_message_slot_t *buffer; /**< sampling buffer                    */

} pmk_sampling_config_t;

/**
 * @brief Initializes Sampling Channel Configuration
 * @param channel Channel control structure
 */
void pmk_sampling_channel_init(pmk_channel_t *channel);

/**
 * @brief Check the user configuration against the internal configuration
 * @param port Port to check
 * @param usr_config User supplied configuration
 * @return 0 if valid, 1 otherwise
 */
air_u32_t pmk_sampling_port_check_configuration(pmk_port_t *port, air_sampling_port_status_t *usr_config);

/**
 * @brief Handle a get status from a sampling port
 * @param context Controlling core context
 * @param port Port to get the status
 * @param status Status of the sampling port
 *
 * @note The parameter status is used to handle different types of get status
 *       operations, this pointer is in partition space and must be checked
 *       before using
 */
air_status_code_e pmk_sampling_port_get_status(core_context_t *context, pmk_port_t *port,
                                               air_sampling_port_status_t *status);

/**
 * @brief Handle an read operation from a sampling port
 * @param context Controlling core context
 * @param port Port being read
 * @param msg Partition message pointer
 * @param length Length of message
 * @param status Status of the sampling port
 *
 * @note The parameter status is used to handle different types of writing
 *       operations, this pointer is in partition space  and must be checked
 *       before using
 */
air_status_code_e pmk_sampling_port_read(core_context_t *context, pmk_port_t *port, air_message_ptr_t msg,
                                         air_sz_t *length, air_sampling_port_status_t *status);

/**
 * @brief Handle an write operation from a sampling port
 * @param context Controlling core context
 * @param port Port being written
 * @param msg Partition message pointer
 * @param length Length of message
 * @param status Status of the sampling port
 *
 * @note The parameter status is used to handle different types of writing
 *       operations (currently not used), this pointer is in partition space
 *       and must be checked before using
 */
air_status_code_e pmk_sampling_port_write(core_context_t *context, pmk_port_t *port, air_message_ptr_t msg,
                                          air_sz_t length, air_sampling_port_status_t *status);

/** @} */

#endif /* __PORTS_SAMPLING_H__ */
