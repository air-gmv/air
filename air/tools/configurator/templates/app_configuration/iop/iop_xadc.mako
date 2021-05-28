<%
    # device functions
    device_functions = dict(
          reader_task='xadc_reader',
          writer_task='xadc_writer',

          header_prebuild='NULL',
          header_compare='xadc_compare_header',
          header_copy='xadc_copy_header')
%>\
<%namespace name="iop_template" file="/iop_templates.mako"/>\
/*
 * ============================================================================
 * This file was generated by the GMV's Configurator Tool and is part of an I/O
 * partition configuration. 
 * File Purpose: IO Partition user configuration. Contains the configuration 
 *               options for XADC driver of ADC devices.
 * ============================================================================
 */
#include <iop.h>
#include <xadc.h>
#include <xadcps.h>
#include <xadc_support.h>


${iop_template.RemotePortList(iop_configuration)}\


/** @brief XADC control structure*/
static XAdcPs XAdcInst;


/** @brief XADC driver configuration */
static iop_adc_device_t device_configuration = ${'\\'}
{
    /* device configuration */
    .dev        = {
        .driver         = (void *)&XAdcInst,
        .init           = iop_xadc_init,
        .open           = iop_xadc_open,
        .read           = iop_xadc_read,
        .write          = NULL,
        .close          = iop_xadc_close,
    },

    /* adc configuration */
    .id         		= ${device.setup.id},
};


${iop_template.PhysicalDevice(iop_configuration, device, device_functions)}\
