/*
 * ============================================================================
 * This file was generated by the GMV's Configurator Tool and is part of an I/O
 * partition configuration. 
 * File Purpose:	IO Partition user configuration. Contains the configuration 
 * 					options for Ethernet's GRETH devices.
 * ============================================================================
 */
#include <iop.h>
#include <iop_greth.h>
#include <eth_support.h>

/**
 * @brief IOP remote ports
 */
extern iop_port_t remote_ports[2];

static iop_buffer_t iop_buffers[64];
static uint8_t iop_buffers_storage[64 * IOP_BUFFER_SIZE];

/**
 * @brief TX descriptor to IOP buffer mapping
 */
static iop_buffer_t *tx_iop_buffer[32];
/**
 * @brief RX descriptor to IOP buffer mapping
 */
static iop_buffer_t *rx_iop_buffer[32];

/**
 * @brief RX and TX descriptor table
 * @warning this should be 2048, but we need 3072 to ensure the 0x400 alignment
 */
static uint8_t descriptor_table[3072];

/** @brief  GRETH control structure*/
static greth_softc_t greth_driver = \
{
    .iop_buffers            = iop_buffers,
    .iop_buffers_storage    = iop_buffers_storage,

    /** @note descriptor table address are split and aligned at the runtime */
    .txdesc = descriptor_table,
    .rxdesc = descriptor_table,

    .tx_iop_buffer = tx_iop_buffer,
    .rx_iop_buffer = rx_iop_buffer
};

/** @brief GRETH driver configuration */
static iop_eth_device_t device_configuration = \
{
    /* device configuration */
    .dev        = {

        .driver         = (void *)&greth_driver,
        .init           = greth_initialize,
        .open           = greth_open,
        .read           = greth_read,
        .write          = greth_write,
        .close          = greth_close,
    },

    /* ethernet configuration */
    .ip         = { 192, 168, 0, 17 },
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
            .dst_ip      = { 192,168,0,3},
            .dst_mac     = { 0x00,0x50,0xbf,0x50,0x07,0x0d},
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
