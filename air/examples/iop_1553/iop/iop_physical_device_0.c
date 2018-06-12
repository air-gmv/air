/*
 * This file is manually created as the 1553 device cannot yet be generated from the configurator tool
 */
#include <iop.h>
#include <gr1553_support.h>
#include <IOPgr1553b.h>
#include <IOPgr1553bc.h>

/**
 * @brief IOP remote ports
 */
extern iop_port_t remote_ports[10];

/** @brief GR1553B0 driver configuration */
static iop_device_driver_t device_configuration = \
{
    /* device configuration */
        .driver         = NULL,
        .init           = grb_initialize,
        .open           = grb_open,
        .read           = NULL,
        .write          = NULL,
        .close          = gr1553bc_close,
};

/**
 * @brief Device Scheduling
 */
static uint32_t reads_per_period[] = \
    { 1 };

/**
 * @brief Routes Headers
 */
static iop_header_t route_header[] = \
{
    {
        // RT1
        .milstd_hdr = {
          .desc = 1,
          .address = 2,
        }
    },
    {
        // RT2
        .milstd_hdr = {
          .desc = 2,
          .address = 2,
        }
    },
    {
        // RT3
        .milstd_hdr = {
          .desc = 3,
          .address = 2,
        }
    },
    {
        // RT4
        .milstd_hdr = {
          .desc = 4,
          .address = 2,
        }
    }
};

/**
 * @brief Routes Scheduling
 */
static uint32_t route_schedule_0[1] = \
    { 1 };

/**
 * @brief Routes Configuration
 */
static iop_physical_route_t physical_routes[] =\
{
    {
        .schedule   = route_schedule_0,
        .header     = &route_header[0],
        .port       = &remote_ports[0]
    },
    {
        .schedule   = route_schedule_0,
        .header     = &route_header[1],
        .port       = &remote_ports[1]
    },
    {
        .schedule   = route_schedule_0,
        .header     = &route_header[2],
        .port       = &remote_ports[2]
    },
    {
        .schedule   = route_schedule_0,
        .header     = &route_header[3],
        .port       = &remote_ports[3]
    }
};

/**
 * @brief Physical Device configuration
 */
iop_physical_device_t physical_device_0 =\
{
    .driver             = (iop_device_driver_t *)&device_configuration,
    .routes             = {
        .length         = 4,
        .elements       = physical_routes
     },
    .reads_per_period   = reads_per_period,

    .reader_task        = grbc_read,
    .writer_task        = grbc_write,
    .header_prebuild    = NULL,
    .header_compare     = gr1553_compare_header,
    .header_copy        = gr1553_copy_header,
};
