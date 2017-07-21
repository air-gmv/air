<%
    # device functions
    device_functions = dict(
          reader_task='spw_reader',
          writer_task='spw_writer',

          header_prebuild='spw_prebuild_header',
          header_compare='spw_compare_header',
          header_copy='spw_copy_header')
%>\
<%namespace name="iop_template" file="/iop_templates.mako"/>\
/*
 * ============================================================================
 * This file was generated by the GMV's Configurator Tool and is part of an I/O
 * partition configuration. 
 * File Purpose:	IO Partition user configuration. Contains the configuration 
 * 					options for Ethernet's GRSPW devices.
 * ============================================================================
 */
#include <iop.h>
#include <iop_grspw.h>
#include <spw_support.h>

${iop_template.RemotePortList(iop_configuration)}\

${iop_template.IopBuffersStorage(2 * device.setup.txdesccnt + device.setup.rxdesccnt)}\

/**
 * @brief TX header descriptor to IOP buffer mapping
 */
static iop_buffer_t *tx_headers_iop_buffer[${device.setup.txdesccnt}];
/**
 * @brief TX descriptor to IOP buffer mapping
 */
static iop_buffer_t *tx_data_iop_buffer[${device.setup.txdesccnt}];
/**
 * @brief RX descriptor to IOP buffer mapping
 */
static iop_buffer_t *rx_iop_buffer[${device.setup.rxdesccnt}];

/**
 * @brief RX and TX descriptor table
 * @warning this should be 2048, but we need 3072 to ensure the 0x400 alignment
 */
static uint8_t descriptor_table[3072];

/** @brief  GRSPW control structure*/
static SPW_DEV grspw_driver = ${'\\'}
{
    .iop_buffers            = iop_buffers,
    .iop_buffers_storage    = iop_buffers_storage,

    /** @note descriptor table address are split and aligned at the runtime */
    .txdesc = descriptor_table,
    .rxdesc = descriptor_table,

    .tx_iop_buffer = tx_iop_buffer,
    .rx_iop_buffer = rx_iop_buffer
};

/** @brief GRSPW driver configuration */
static iop_spw_device_t device_configuration = ${'\\'}
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

    /* ethernet??? configuration */
	.nodeaddr		= ${device.setup.nodeaddr},
	.nodemask;		= ${device.setup.nodemask},
	.destkey;		= ${device.setup.destkey},
	.clkdiv; 		= ${device.setup.clkdiv},
	
	.rxmaxlen;		= ${device.setup.rxmaxlen},
	.promiscuous;	= ${device.setup.promiscuous},
	.rmapen;		= ${device.setup.rmapen},
	.rmapbufdis;	= ${device.setup.rmapbufdis},
	.rm_prot_id;	= ${device.setup.rm_prot_id},
	
/* The buffer size is in the var IOP_BUFFER_SIZE declared in iop.h */
//	.txdbufsize;	= ${device.setup.txdbufsize},
//	.txhbufsize;	= ${device.setup.txhbufsize},
//	.rxbufsize;		= ${device.setup.rxbufsize},
	.txdesccnt;		= ${device.setup.txdesccnt},
	.rxdesccnt;		= ${device.setup.rxdesccnt},
	
	.retry;			= ${device.setup.retry},
	.wait_ticks;	= ${device.setup.wait_ticks},
	
	.init_timeout;	= ${device.setup.init_timeout}
};

${iop_template.PhysicalDevice(iop_configuration, device, device_functions)}\
