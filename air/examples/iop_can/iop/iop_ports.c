/*
 * ============================================================================
 * This file was generated by the GMV's Configurator Tool and is part of an I/O
 * partition configuration. 
 * File Purpose:	IO Partition user configuration. Contains the routing 
 * 					configuration options of the IO Partition.
 * ============================================================================
 */

#include <iop.h>

/**
 * @brief Logical devices
 */
extern iop_logical_device_t logical_device_0;

/**
 * @brief Queuing ports configurations
 */
//static air_queuing_port_configuration_t queuing_ports[] = {
//    {
//        .max_message_size   = 1024,
//        .max_nb_message     = 32,
//        .port_direction     = AIR_SOURCE_PORT,
//        .port_discipline    = 0
//    },
//};

/**
 * @brief Sampling ports configurations
 */
static air_sampling_port_configuration_t sampling_ports[] = {
    {
        .max_message_size   = 8,
        .refresh_period     = 1000,
        .port_direction     = AIR_DESTINATION_PORT,
    },
	{
		.max_message_size   = 8,
		.refresh_period     = 1000,
		.port_direction     = AIR_SOURCE_PORT,
	}
};

/**
 * @brief IOP remote ports
 */
iop_port_t remote_ports[2] = {
    {
        .type               = AIR_SAMPLING_PORT,
        .name               = "iop_sender",
        .configuration      = &sampling_ports[0],
        .device             = &logical_device_0
    },
    {
        .type               = AIR_SAMPLING_PORT,
        .name               = "iop_receiver",
        .configuration      = &sampling_ports[1],
        .device             = NULL
    },
};

