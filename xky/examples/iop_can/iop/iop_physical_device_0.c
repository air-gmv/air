/*
 * ============================================================================
 * File Purpose:	IO Partition user configuration. Contains the configuration 
 * 					options for CAN devices.
 * ============================================================================
 */
#include <iop.h>
#include <occan.h>
#include <can_support.h>

/**
 * @brief IOP remote ports
 */
extern iop_port_t remote_ports[2];

/**
 * @brief IOP buffers
 */
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
 * @brief Allocation of for the internal message queue
 * to be used by the occan driver
 */

static CANMsg rx_msg_fifo[32];

static CANMsg tx_msg_fifo[32];

/**
 * @brief RX and TX descriptor table
 * @warning this should be 2048, but we need 3072 to ensure the 0x400 alignment
 */
//static uint8_t descriptor_table[3072];

/** @brief  GRETH control structure*/
static occan_priv occan_driver = \
{
	.speed = 250000, // TODO maybe use a enum in the future
	.filter = {
		.single_mode = 0,
		.code = {0, 0, 0, 0},
		.mask = {0, 0, 0, 0},
	},

	.iop_buffers            = iop_buffers,
    .iop_buffers_storage    = iop_buffers_storage,

	.rx_fifo = {
		.cnt = 0,
		.ovcnt = 0,
		.next = 0,
		.last = 0,
		.max = 32,
		.fifo = rx_msg_fifo,
	},
	.tx_fifo = {
		.cnt = 0,
		.ovcnt = 0,
		.next = 0,
		.last = 0,
		.max = 32,
		.fifo = tx_msg_fifo,
	},

    /** @note descriptor table address are split and aligned at the runtime */
//    .txdesc = descriptor_table,
//    .rxdesc = descriptor_table,

    .tx_iop_buffer = tx_iop_buffer,
    .rx_iop_buffer = rx_iop_buffer
};

/** @brief GRETH driver configuration */
static iop_can_device_t device_configuration = \
{
    /* device configuration */
    .dev        = {

        .driver         = (void *)&occan_driver,
        .init           = occan_initialize,
        .open           = occan_open,
        .read           = occan_read,
        .write          = occan_write,
        .close          = occan_close,
    },
    .dev_name ="/dev/occan0",
	.count = 0,
	.flags = 0,
	.bytes_moved = 0,
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
		.can_header = {
			.extended = 0,
			.rtr = 0,
			.sshot = 0,
			.id = 2,
		},
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
        .port       = &remote_ports[2]
    },
};

/**
 * @brief Pysical Device configuration
 */
iop_physical_device_t physical_device_0 =\
{
    .driver             = (iop_device_driver_t *)&device_configuration,
    .routes             = {
        .length         = 2,
        .elements       = physical_routes
     },
    .reads_per_period   = reads_per_period,

    .reader_task        = can_reader,
    .writer_task        = can_write,
    .header_prebuild    = can_prebuild_header,
    .header_compare     = can_compare_header,
    .header_copy        = can_copy_header,
};
