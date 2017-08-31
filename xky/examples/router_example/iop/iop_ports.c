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
static xky_queuing_port_configuration_t queuing_ports[] = {
    {
        .max_message_size   = 1520,
        .max_nb_message     = 32,
        .port_direction     = XKY_SOURCE_PORT,
        .port_discipline    = 0
    },
};

/**
 * @brief Sampling ports configurations
 */
static xky_sampling_port_configuration_t sampling_ports[] = {
    {
        .max_message_size   = 1520,
        .refresh_period     = 200,
        .port_direction     = XKY_DESTINATION_PORT,
    },
};

/**
 * @brief IOP remote ports
 */
iop_port_t remote_ports[2] = {
    {
        .type               = XKY_SAMPLING_PORT,
        .name               = "dsampling",
        .configuration      = &sampling_ports[0],
        .device             = &logical_device_0
    },
    {
        .type               = XKY_QUEUING_PORT,
        .name               = "squeuing",
        .configuration      = &queuing_ports[0],
        .device             = NULL
    },
};

