/*
 * ============================================================================
 * This file was generated by the GMV's Configurator Tool and is part of an I/O
 * partition configuration. 
 * File Purpose:	IO Partition user configuration. Contains the configuration 
 * 					options for Ethernet's GRSPW devices.
 * ============================================================================
 */
#include <iop.h>
#include <IOPgrspw.h>
#include <spw_support.h>

/**
 * @brief IOP remote ports
 */
extern iop_port_t remote_ports[2];

/**
 * @brief IOP buffers
 */
static iop_buffer_t iop_buffers[96];
static uint8_t iop_buffers_storage[96 * IOP_BUFFER_SIZE];

/**
 * @brief RX and TX descriptor table
 * @warning this should be 2048, but we need 3072 to ensure the 0x400 alignment
 */
static uint8_t descriptor_table[3072];

/** @brief  GRSPW control structure*/
static SPW_DEV spw_driver = \
{
    .iop_buffers            = iop_buffers,
    .iop_buffers_storage    = iop_buffers_storage,

    /** @note descriptor table address are split and aligned at the runtime */
    .bdtable = descriptor_table
};

/** @brief GRSPW driver configuration */
static iop_spw_device_t device_configuration = \
{
    /* device configuration */
    .dev        = {

        .driver         = (void *)&spw_driver,
        .init           = spw_initialize,
        .open           = spw_open,
        .read           = spw_read,
        .write          = spw_write,
        .close          = spw_close,
    },

    /* spacewire configuration */
	.nodeaddr		= { 10 },
	.nodemask		= 1,
	.destkey		= 0,
	.clkdiv		= 1,
	
	.rxmaxlen		= 64,
	.promiscuous	= 0,
	.rmapen		= 0,
	.rmapbufdis	= 32,
	.rm_prot_id	= 0,
	
/* The buffer size is in the var IOP_BUFFER_SIZE declared in iop.h */
	.txdbufsize	= 1520,
	.txhbufsize	= 1520,
	.rxbufsize		= 1520,
	.txdesccnt		= 32,
	.rxdesccnt		= 32,
	
	.retry			= 0,
	.wait_ticks	= 0,
	
	.init_timeout	= -1
};

/**
 * @brief Device Scheduling
 */
static uint32_t reads_per_period[] = \
    { 1 };

/**
 * @brief Routes Headers
 */
static iop_header_t route_header[1] = \
{
    {
        .spw_header = {
            .hdr         = { 10 }
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
iop_physical_device_t physical_device_2 =\
{
    .driver             = (iop_device_driver_t *)&device_configuration,
    .routes             = {
        .length         = 1,
        .elements       = physical_routes
     },
    .reads_per_period   = reads_per_period,

    .reader_task        = spw_reader,
    .writer_task        = spw_writer,
    .header_prebuild    = spw_prebuild_header,
    .header_compare     = spw_compare_header,
    .header_copy        = spw_copy_header,
};