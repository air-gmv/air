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

#define UART_TX_SIZE   16
#define UART_RX_SIZE   48

 ${iop_template.RemotePortList(iop_configuration)}\

/**
 *@brief UART data buffer allocation
 */
static uint8_t uart_msg_buffer[UART_RX_SIZE*16*4+UART_TX_SIZE*16*4+1024];

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
};

${iop_template.PhysicalDevice(iop_configuration, device, device_functions)}\
