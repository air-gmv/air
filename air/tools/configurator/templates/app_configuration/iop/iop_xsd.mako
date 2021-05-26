<%
    # device functions
    device_functions = dict(
          reader_task='xsd_reader',
          writer_task='xsd_writer',

          header_prebuild='NULL',
          header_compare='xsd_compare_header',
          header_copy='xsd_copy_header')
%>\
<%namespace name="iop_template" file="/iop_templates.mako"/>\
/*
 * ============================================================================
 * This file was generated by the GMV's Configurator Tool and is part of an I/O
 * partition configuration. 
 * File Purpose: IO Partition user configuration. Contains the configuration 
 *               options for XSD driver of SD Card devices.
 * ============================================================================
 */
#include <iop.h>
#include <xsd.h>
#include <xsd_support.h>


${iop_template.RemotePortList(iop_configuration)}\


/** @brief XSD control structure*/
static xsd_struct xsd_driver;


/** @brief XSD driver configuration */
static iop_sd_device_t device_configuration = ${'\\'}
{
    /* device configuration */
    .dev        = {
        .driver         = (void *)&xsd_driver,
        .init           = iop_xsd_init,
        .open           = iop_xsd_open,
        .read           = iop_xsd_read,
        .write          = iop_xsd_write,
        .close          = iop_xsd_close,
    },

    /* sd configuration */
    .id         		= ${device.setup.id},
    .format         	= ${device.setup.format},
    .number_files        = ${device.setup.number_files}
};


${iop_template.PhysicalDevice(iop_configuration, device, device_functions)}\
