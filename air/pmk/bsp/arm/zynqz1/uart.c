/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file uart.c
 * @author lumm
 * @brief UART setup and handler functions.
 */

#include <uart.h>

static volatile uart_t *uart0 = (uart_t *)UART0_BASE_MEMORY;

inline void arm_reset_uart(void) {
    volatile air_uptr_t uart_rst_ctrl = (air_uptr_t)(SLCR_BASE_MEMORY + 0x228);
    *uart_rst_ctrl = 0xf;
}

void arm_start_uart(void) {

    /* 1. Reset Controller */
    arm_reset_uart();

    /* 2. Configure I/O signal routing */
    volatile air_uptr_t mio_pin_46 = (air_uptr_t)(SLCR_BASE_MEMORY + 0x7b8);
    volatile air_uptr_t mio_pin_47 = (air_uptr_t)(SLCR_BASE_MEMORY + 0x7bc);

    *mio_pin_46 = (MIO_PIN_TRI_ENABLE |
            MIO_PIN_L0_SEL(0) |
            MIO_PIN_L1_SEL(0) |
            MIO_PIN_L2_SEL(0) |
            MIO_PIN_L3_SEL(0x7) |
            MIO_PIN_SPEED(MIO_PIN_SLOW_CMOS_EDGE) |
            MIO_PIN_IO_TYPE(MIO_PIN_IO_LVCMOS18) |
            MIO_PIN_PULLUP(1) |
            MIO_PIN_DISABLE_RCVR);

    *mio_pin_47 = (MIO_PIN_L0_SEL(0) |
            MIO_PIN_L1_SEL(0) |
            MIO_PIN_L2_SEL(0) |
            MIO_PIN_L3_SEL(0x7) |
            MIO_PIN_SPEED(MIO_PIN_SLOW_CMOS_EDGE) |
            MIO_PIN_IO_TYPE(MIO_PIN_IO_LVCMOS18) |
            MIO_PIN_PULLUP(1) |
            MIO_PIN_DISABLE_RCVR);

    /* 3. Configure reference clock */
    volatile air_uptr_t uart_clk_ctrl = (air_uptr_t)(SLCR_BASE_MEMORY + 0x154);
    *uart_clk_ctrl = ((UART_CLK_CTRL_CLKACT0_ENABLE |
            UART_CLK_CTRL_SRCSEL(0x0) |
            UART_CLK_CTRL_DIVISOR(0x14)) & //50MHz
            ~UART_CLK_CTRL_CLKACT1_ENABLE);

    /* 4. Configure controller functions */
    uart0->mode = (ARM_UART_MODE_PAR(0x4));

    uart0->ctrl = ((ARM_UART_CTRL_RX_DIS |
            ARM_UART_CTRL_TX_DIS) &
            ~ARM_UART_CTRL_RX_EN &
            ~ARM_UART_CTRL_TX_EN);
    /* Baud Rate of 230400. Values on page 594 of zynq-7000-trm */
    uart0->baud_rate_gen = 31;
    uart0->baud_rate_div = 6;
    uart0->ctrl = ((ARM_UART_CTRL_RXRST |
            ARM_UART_CTRL_TXRST |
            ARM_UART_CTRL_RX_EN |
            ARM_UART_CTRL_TX_EN) &
            ~ARM_UART_CTRL_RX_DIS &
            ~ARM_UART_CTRL_TX_DIS);

    uart0->tx_fifo_trigger = 0; // no trigger

    uart0->ctrl = ((ARM_UART_CTRL_RXRST |
            ARM_UART_CTRL_TXRST |
            ARM_UART_CTRL_RX_EN |
            ARM_UART_CTRL_TX_EN |
            ARM_UART_CTRL_RSTTO |
            ARM_UART_CTRL_STPBRK) &
            ~ARM_UART_CTRL_RX_DIS &
            ~ARM_UART_CTRL_TX_DIS &
            ~ARM_UART_CTRL_STTBRK);

    uart0->rx_timeout = 0; //timeout disabled

    /* 5. Configure interrupts (none for now) */
    do {
        uart0->int_disable = ARM_UART_INT_DIS_RTRIG;
    } while ((uart0->int_status && ARM_UART_INT_RTRIG) != 0);

    /* 6. Configure modem controls (optional) */
}

void arm_uart_transmit(char ch) {

    while((uart0->status && ARM_UART_STATUS_TXEMPTY) == 0);

    uart0->tx_rx_fifo = ((air_u32_t)ch & 0xff);
}
