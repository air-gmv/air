/*
 * ============================================================================
 * File Purpose:	IO Partition user configuration. Contains the configuration 
 * 					options for CAN devices.
 * ============================================================================
 */
#include <iop.h>
#include <grcan.h>
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
//static iop_buffer_t *tx_iop_buffer[32];
/**
 * @brief RX descriptor to IOP buffer mapping
 */
//static iop_buffer_t *rx_iop_buffer[32];

/**
 * @brief Allocation of for the internal message queue
 * to be used by the grcan driver
 */

static grcan_msg rx_msg_fifo[32];

static grcan_msg tx_msg_fifo[32];

/**
 * @brief RX and TX descriptor table
 * @warning this should be 2048, but we need 3072 to ensure the 0x400 alignment
 */
//static uint8_t descriptor_table[3072];

/** @brief  GRCAN control structure*/
static grcan_priv grcan_driver = \
{
	/*
	 * Clock frequency (This should be acquired internally
	 * through the boards registers
	 * */
	.corefreq_hz = 250000000,
//	.speed = 250000, // TODO maybe use a enum in the future
//	.filter = {
//		.single_mode = 0,
//		.code = {0, 0, 0, 0},
//		.mask = {0, 0, 0, 0},
//	},

	/* Default to accept all messages */
	.afilter ={
			.mask = 0x00000000,
			.code = 0x00000000
	},

	/* Default to disable sync messages (only trigger when
	 * id is set to all ones) */
	.sfilter = { // Accept everything
			.mask = 0xffffffff,
			.code = 0x00000000
	},

	.iop_buffers            = iop_buffers,
    .iop_buffers_storage    = iop_buffers_storage,


//	.rx_fifo = {
//		.cnt = 0,
//		.ovcnt = 0,
//		.next = 0,
//		.last = 0,
//		.max = 32,
//		.fifo = rx_msg_fifo,
//	},
//	.tx_fifo = {
//		.cnt = 0,
//		.ovcnt = 0,
//		.next = 0,
//		.last = 0,
//		.max = 32,
//		.fifo = tx_msg_fifo,
//	},

    /** @note descriptor table address are split and aligned at the runtime */
//    .txdesc = descriptor_table,
//    .rxdesc = descriptor_table,
	.txbuf_adr = 0x0,
	.rxbuf_adr = 0x0,

	.txcomplete = 0,
	.rxcomplete = 0,

	.txblock = 0,
	.rxblock = 0,

    .tx = tx_msg_fifo,
    .rx = rx_msg_fifo,
};

/** @brief GRETH driver configuration */
static iop_can_device_t device_configuration = \
{
    /* device configuration */
    .dev        = {

        .driver         = (void *)&grcan_driver,
        .init           = grcan_initialize,
        .open           = grcan_open,
        .read           = grcan_read,
        .write          = grcan_write,
        .close          = grcan_close,
    },
    .can_core = 0,
	.baud_rate = 250, // 250k
//    .dev_name ="/dev/grcan0",
//	.count = 0,
//	.flags = 0,
//	.bytes_moved = 0,
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
		/* Send message to
		 * node with ID 2 */
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
    .writer_task        = can_writer,
    .header_prebuild    = can_prebuild_header,
    .header_compare     = can_compare_header,
    .header_copy        = can_copy_header,
};
