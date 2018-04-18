<%	
	# device functions
	device_functions = dict(
		reader_task='can_reader',
		writer_task='can_writer',
		
		header_prebuild='can_prebuild_header',
		header_compare='can_compare_header',
		header_copy='can_copy_header')
%>\
<%namespace name="iop_template" file="/iop_templates.mako"/>\
 /*
 * ============================================================================
 * This file was generated by the GMV's Configurator Tool and is part of an I/O
 * partition configuration. 
 * File Purpose:	IO Partition user configuration. Contains the configuration 
 * 					options for GRCAN driver of CAN devices to be used on the
 *					gr740 board.
 * ============================================================================
 */
 
 #include <iop.h>
 #include <grcan.h>
 #include <can_support.h>
 #include <iop_mms.h>
 #include <iop_error.h>
 
 ${iop_template.RemotePortList(iop_configuration)}\
 
 ${iop_template.IopBuffersStorage(device.setup.txd_count + device.setup.rxd_count)}\
 
/**
 * @brief TX descriptor to IOP buffer mapping
 */
static iop_buffer_t *tx_iop_buffer[${device.setup.txd_count}];
/**
 * @brief RX descriptor to IOP buffer mapping
 */
static iop_buffer_t *rx_iop_buffer[${device.setup.rxd_count}];

/**
 * @brief RX and TX descriptor table
 * @warning this should be 2048, but we need 3072 to ensure the 0x400 alignment
 */
static uint8_t descriptor_table[3072];

/**
 * @brief Allocation of the grcan driver internal
 * message queue
 */
/**
 *@brief Allocation of the receiver memory
 */
unsigned int rx_msg_fifo[${device.setup.txd_count*96+1024}];
/**
 *@brief Allocation of the transmiter memory
 */
unsigned int tx_msg_fifo[${device.setup.txd_count*96+1024}];

/** @brief GRCAN control structure*/
static grcan_priv grcan_driver = ${'\\'}
{
	.corefreq_hz	= 250000000, // Hardcoded here for now
	/* Acceptance filter */
	.afilter		= {
		.code			= ${device.setup.code},
		.mask			= ${device.setup.mask},
	},
	/* Synchronization filter */
	.sfilter		= {
		.code			= 0xffffffff,
		.mask			= 0x00000000,
	},
	
	.iop_buffers			= iop_buffers,
	.iop_buffers_storage	= iop_buffers_storage,
	
	.iop_buffers = iop_buffers,	
	
	.txbuf_adr = 0,
	.rxbuf_adr = 0,
	
	.txcomplete = 0,
	.rxcomplete = 0,
	
	.txblock = 0,
	.rxblock = 0,
	
	.tx_sem = 0,
	.rx_sem = 0,
	.txempty_sem = 0,
	.dev_sem = 0,
	
	.txbuf_size = ${device.setup.txd_count*96+1024},
	.rxbuf_size = ${device.setup.txd_count*96+1024},
	
	.config = {
		.selection = {
			.selection 	= ${device.setup.selection},
			.enable0 	= ${device.setup.enable0},
			.enable1 	= ${device.setup.enable1},
		},
	},
	
	._tx = tx_msg_fifo,
	._rx = rx_msg_fifo,
};

/**  @brief GRCAN control strucutre */
static iop_can_device_t device_configuration = ${'\\'}
{
	/* device configuration */
	.dev 		= {
		.driver			= (void *)&grcan_driver,
		.init			= grcan_initialize,
		.open			= grcan_open,
		.read			= grcan_read,
		.write			= grcan_write,
		.close			= grcan_close,
	},
	.can_core 	= ${device.setup.can_core},
	.baud_rate 	= ${device.setup.baud},
};

${iop_template.PhysicalDevice(iop_configuration, device, device_functions)}\
