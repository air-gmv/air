/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2008-2015
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file
 * @author pfnf
 * @brief Commmon definitions, structures and functions to handle
 *        Inter-partition Communications
 */

#ifndef __PORTS_H___
#define __PORTS_H___

#include <pmk.h>
#include <partition.h>

/**
 * @defgroup pmk_ports PMK - Ports & Channels (Common)
 * @brief Commmon definitions, structures and functions to handle
 *        Inter-partition Communications
 * @{
 */

/**
 * @brief Channel Control & Configuration Structure
 */
typedef struct {

    air_identifier_t id;            /**< Channel Id                         */
    air_port_type_e type;           /**< Channel type                       */
    void *configuration;            /**< Channel configuration              */
    pmk_list_t src;                 /**< Channel source ports               */
    pmk_list_t dst;                 /**< Channel destination ports          */
    air_name_t name;                /**< Channel name                       */

} pmk_channel_t;

/**
 * @brief Port Control & Configuration Structure
 */
typedef struct {

    /**< Port Id                                                            */
    air_identifier_t id;
    /**< Port type                                                          */
    air_port_direction_e direction;
    /**< Port message validity                                              */
    air_message_validity_e last_msg_validity;
    /**< Port created flag                                                  */
    atomic_t created;
    /**< Port updated flag                                                  */
    atomic_t updated;
    /**< Port partition                                                     */
    pmk_partition_t *partition;
    /**< Port channel                                                       */
    pmk_channel_t *channel;
    /**< Port name                                                          */
    air_name_t name;

} pmk_port_t;

/**
 * @brief Message Slot structure
 */
typedef struct {

    air_message_ptr_t message;      /**< Message pointer                    */
    air_sz_t length;                  /**< Message Length                     */
    air_clocktick_t timestamp;      /**< Message time stamp                 */

} pmk_message_slot_t;

/**
 * @brief Initializes the communication channels
 */
void pmk_channels_init(void);

/**
 * @brief Get partition port by name
 * @param partition Partition containing the port
 * @param name Name of the port
 * @return port Pointer if port name exists, NULL otherwise
 */
pmk_port_t *pmk_port_get_from_partition_by_name(
        pmk_partition_t *partition, air_name_ptr_t name);

/**
 * @brief Gets a port from the partition
 * @param partition Partition containing the port
 * @param id Port identifier
 * @param type Type of port
 * @return port Pointer if the port was found and its type is correct,
 *         NULL otherwise
 */
pmk_port_t *pmk_port_get_from_partition_by_id(
        pmk_partition_t *partition, air_identifier_t id, air_port_type_e type);

/**
 * @brief Update channel ports
 * @param channel Channel to update
 * @param state State to apply to the port
 */
void pmk_channel_update_ports(pmk_channel_t *channel, air_port_updated_e state);

/**
 * @brief Partition port initialization
 * @param partition Partition structure pointer
 */
void pmk_partition_ports_init(pmk_partition_t *partition);

/** @} */

#endif /* __PORTS_H__ */
