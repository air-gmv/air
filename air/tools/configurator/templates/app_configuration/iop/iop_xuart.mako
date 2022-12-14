<%
    # device functions
    device_functions = dict(
        reader_task='xuart_reader',
        writer_task='xuart_writer',

        header_prebuild='NULL',
        header_compare='uart_compare_header',
        header_copy='uart_copy_header')
%>\
<%namespace name="iop_template" file="/iop_templates.mako"/>\
/*
* ============================================================================
* This file was generated by the GMV's Configurator Tool and is part of an I/O
* partition configuration.
* File Purpose:    IO Partition user configuration. Contains the configuration
*                     options for XUART driver of UART devices.
* ============================================================================
*/

#include <iop.h>
#include <xuart.h>
#include <xuart_support.h>

 ${iop_template.RemotePortList(iop_configuration)}\

static uart_priv xuart_driver;

/**  @brief XUART control strucutre */
static iop_uart_device_t device_configuration = ${'\\'}
{
    /* device configuration */
    .dev         = {
        .driver         = (void *)&xuart_driver,
        .init           = iop_xuart_init,
        .open           = iop_xuart_open,
        .read           = iop_xuart_read,
        .write          = iop_xuart_write,
        .close          = iop_xuart_close,
    },
    .uart_core    = ${device.setup.uart_core},
    .baud_rate    = ${device.setup.baud},
    .parity       = '${device.setup.parity}',
    .data_bits    = ${device.setup.data_bits},
    .stop_bits    = ${device.setup.stop_bits},
    .data_bytes   = ${device.setup.data_bytes},
};

${iop_template.PhysicalDevice(iop_configuration, device, device_functions)}\
