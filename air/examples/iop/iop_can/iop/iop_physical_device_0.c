 /*
 * ============================================================================
 * This file was generated by the GMV's Configurator Tool and is part of an I/O
 * partition configuration. 
 * File Purpose:	IO Partition user configuration. Contains the configuration 
 * 					options for GRCAN driver of CAN devices.
 * ============================================================================
 */
 
 #include <iop.h>
 #include <grcan.h>
 #include <can_support.h>

 #define CAN_TX_SIZE	16
 #define CAN_RX_SIZE	48

 /**
 * @brief IOP remote ports
 */
extern iop_port_t remote_ports[4];

/**
 *@brief CAN data buffer allocation
 */
static uint8_t can_msg_buffer[CAN_RX_SIZE*16*4+CAN_TX_SIZE*16*4+1024];

/** @brief GRCAN control structure*/
static grcan_priv grcan_driver = \
{
	/* Acceptance filter */
	.afilter		= {
		.code			= 0,
		.mask			= 0,
	},
	/* Synchronization filter */
	.sfilter		= {
		.code			= 0xffffffff,
		.mask			= 0x00000000,
	},

	.txbuf_adr = 0,
	.rxbuf_adr = 0,
	
	.txcomplete = 0,
	.rxcomplete = 0,
	
	.txblock = 0,
	.rxblock = 0,
	
	.txbuf_size = CAN_TX_SIZE*16*4,
	.rxbuf_size = CAN_RX_SIZE*16*4,
	
	.config = {
		.selection = {
			.selection 	= 1,
			.enable0 	= 0,
			.enable1 	= 0,
		},
	},
	
	._tx = can_msg_buffer,
	._rx = can_msg_buffer,
};

/**  @brief GRCAN control strucutre */
static iop_can_device_t device_configuration = \
{
	/* device configuration */
	.dev 		= {
		.driver			= (void *)&grcan_driver,
		.init			= iop_grcan_initialize,
		.open			= iop_grcan_open,
		.read			= iop_grcan_read,
		.write			= iop_grcan_write,
		.close			= iop_grcan_close,
	},
	.can_core 	= 1,
	.baud_rate 	= 250,
};

/**
 * @brief Device Scheduling
 */
static air_u32_t reads_per_period[] = \
    { 1 };

/**
 * @brief Routes Headers
 */
static iop_header_t route_header[1] = \
{
	{
		.can_header = {
			.extended = 1,
			.sshot	  = 1,
			.rtr 	  = 0,
			.id		  = 35
		}
	}
};

/**
 * @brief Routes Scheduling
 */
static air_u32_t route_schedule_0[1] = \
    { 1 };

/**
 * @brief Routes Configuration
 */
static iop_physical_route_t physical_routes[1] =\
{
    {
        .schedule   = route_schedule_0,
        .header     = &route_header[0],
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
        .length         = 1,
        .elements       = physical_routes
     },
    .reads_per_period   = reads_per_period,

    .reader_task        = can_reader,
    .writer_task        = can_writer,
    .header_prebuild    = NULL,
    .header_compare     = can_compare_header,
    .header_copy        = can_copy_header,
};
