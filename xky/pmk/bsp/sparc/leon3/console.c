/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file
 * @author pfnf
 * @brief UART device initialization and handling functions
 */

#include <pmk.h>
#include <amba.h>
#include <clock.h>
#include <console.h>

/**
 * @brief LEON UART Registers map
 * @ingroup bsp_leon_console
 */
typedef struct {

    volatile xky_u32_t data;
    volatile xky_u32_t status;
    volatile xky_u32_t ctrl;
    volatile xky_u32_t scaler;

} uart_regmap_t;

/**
 * @brief UART Receiver enable control bit
 * @ingroup bsp_leon_console
 */
#define UART_CTRL_RE                    (0x00000001u)
/**
 * @brief UART Transmitter enable control bit
 * @ingroup bsp_leon_console
 */
#define UART_CTRL_TE                    (0x00000002u)
/**
 * @brief UART Scaler setup
 * @param c Clock frequency in HZ
 * @ingroup bsp_leon_console
 */
#define UARC_SCALER_SETUP(c)            (((((c) * 10) / (115200 * 8)) - 5) / 10)
/**
 * @brief UART TX Hold Register Empty
 * @ingroup bsp_leon_console
 */
#define UART_STATUS_THE                 (0x00000004u)
/**
 *  @brief UART registers pointer
 *  @ingroup bsp_leon_console
 */
static volatile uart_regmap_t *uart_regs;

int console_init(void) {

    /* find and initialize the UART */
    amba_apb_dev_t uart_device;
    if (amba_get_apb_slave(
            &amba_confarea, VENDOR_GAISLER, GAISLER_APBUART, -1, &uart_device)){

        uart_regs = (volatile uart_regmap_t *)uart_device.start;
        uart_regs->ctrl |= UART_CTRL_RE | UART_CTRL_TE;
        uart_regs->status = 0;
        uart_regs->scaler = UARC_SCALER_SETUP(timer_ctrl.frequency);

        return 0;
    }

    return 1;
}

void console_outbyte(char ch) {

    /* busy wait until the UART transmit hold register is empty */
    while ((uart_regs->status & UART_STATUS_THE) == 0);

    /* send byte to the UART0 */
    uart_regs->data = (xky_u32_t)ch;
}
