<%
    # device functions
    device_functions = dict(
          reader_task='spwrtr_reader',
          writer_task='spwrtr_writer',

          header_prebuild='NULL',
          header_compare='NULL',
          header_copy='NULL')
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
#include <IOPgrspw_router.h>
#include <spw_support.h>
#include <routing_table.h>

/** @brief  GRSPW control structure*/
static router_priv_t spwrtr_driver = ${'\\'}
{
	/* Router tables */
	.routes = &routes,
	.ps = &ps,
	.timer_reload = &timer_reload
};

/** @brief GRSPW driver configuration */
static iop_spw_router_device_t device_configuration = ${'\\'}
{
    /* device configuration */
    .dev        = {

        .driver         = (void *)&spwrtr_driver,
        .init           = router_initialize,
        .open           = router_open,
        .read           = NULL,
        .write          = NULL,
        .close          = router_close,
    },

    /* spacewire router configuration configuration */
	.flags			= ${device.setup.flags},
	.config			= ${device.setup.config},
	.iid			= ${device.setup.iid},
	.idiv			= ${device.setup.idiv},
	.prescaler		= ${device.setup.prescaler}
};

${iop_template.PhysicalDevice(iop_configuration, device, device_functions)}\