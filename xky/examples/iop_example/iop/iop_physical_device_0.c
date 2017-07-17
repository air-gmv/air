/*
 * ============================================================================
 * This file was generated by the GMV's Configurator Tool and is part of an I/O
 * partition configuration. 
 * File Purpose:	IO Partition user configuration. Contains the configuration 
 * 					options for Ethernet's GRETH devices.
 * ============================================================================
 */
#include <iop.h>
#include <iop_cpsw.h>
#include <eth_support.h>

/**
 * @brief IOP remote ports
 */
extern iop_port_t remote_ports[2];

static iop_buffer_t iop_buffers[64];
static uint8_t iop_buffers_storage[64 * IOP_BUFFER_SIZE];

/**
 * @brief CPSW control structure
 */
static cpsw_device_t cpsw_driver = \
{
    .iop_buffers            = iop_buffers,
    .iop_buffers_storage    = iop_buffers_storage,
};

/**
 * @brief CPSW driver configuration
 */
static iop_eth_device_t device_configuration = \
{
    /* device configuration */
    .dev        = {

        .driver         = (void *)&cpsw_driver,
        .init           = cpsw_initialize,
        .open           = cpsw_open,
        .read           = cpsw_read,
        .write          = cpsw_write,
        .close          = cpsw_close,
    },

    /* ethernet configuration */
    .ip         = { 192, 168, 0, 70 },
    .mac        = { 0xd0, 0x39, 0x72, 0x74, 0xd8, 0x1f },
    .rx_count   = 32,
    .tx_count   = 32
};

/**
 * @brief Device Scheduling
 */
static uint32_t reads_per_period[] = \
    { 5 };

/**
 * @brief Routes Headers
 */
static iop_header_t route_header[1] = \
{
        {
        .eth_header = {
            .dst_ip      = { 192,168,0,2},
            .dst_mac     = { 0x68,0x05,0xca,0x1f,0x39,0xaf},
            .dst_port    = HTONS(14000),
            .src_port    = HTONS(14000)
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
static iop_physical_route_t physical_routes[1] =\
{
    {
        .schedule   = route_schedule_0,
        .header     = &route_header[0],
        .port       = &remote_ports[1]
    }
};

/**
 * @brief Pysical Device configuration
 */
iop_physical_device_t physical_device_0 =\
{
    .driver             = (iop_device_driver_t *)&device_configuration,
    .routes             = {
        .length         = 1,
        .elements       = physical_routes
     },
    .reads_per_period   = reads_per_period,

    .reader_task        = eth_reader,
    .writer_task        = eth_writer,
    .header_prebuild    = eth_prebuild_header,
    .header_compare     = eth_compare_header,
    .header_copy        = eth_copy_header,
};
