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
 * 					options for OCCAN driver of CAN devices.
 * ============================================================================
 */
 
 #include <iop.h>
 #include <iop_occan.h>
 #include <can_support.h>
 
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
 * @brief Allocation of the occan driver internal
 * message queue
 */
 
static CANMsg rx_msg_fifo[${device.setup.internal_queue}];
 
static CANMsg tx_msg_fifo[${device.setup.internal_queue}]; 

/**
 * @brief RX and TX descriptor table
 * @warning this should be 2048, but we need 3072 to ensure the 0x400 alignment
 */
//static uint8_t descriptor_table[3072];

/** @brief OCCAN control structure*/
static occan_priv occan_driver = ${'\\'}
{
	.speed		= ${device.setup.speed}000,
	.filter		= {
		.single_mode	= ${1 \
							if device.setup.single_mode is True else \
							0},
		.code			= {${','.join('0x' + s for s in device.setup.code)}},
		.mask			= {${','.join('0x' + s for s in device.setup.mask)}},
	},
	
	.iop_buffers			= iop_buffers,
	.iop_buffers_storage	= iop_buffers_storage,
	
	.rx_fifo = {
		.max = ${device.setup.internal_queue},
		.fifo = rx_msg_fifo,
	},
	
	.tx_fifo = {
		.max = ${device.setup.internal_queue},
		.fifo = tx_msg_fifo,
	},
	.iop_buffers = iop_buffers,
		
	.tx_iop_buffer = tx_iop_buffer,
	.rx_iop_buffer = rx_iop_buffer,
};

/**  @brief OCCAN control strucutre */
static iop_can_device_t device_configuration = ${'\\'}
{
	/* device configuration */
	.dev 		= {
		.driver			= (void *)&occan_driver,
		.init			= occan_initialize,
		.open			= occan_open,
		.read			= occan_read,
		.write			= occan_write,
		.close			= occan_close,
	},
	.dev_name = "/dev/${device.device}",
};

${iop_template.PhysicalDevice(iop_configuration, device, device_functions)}\
