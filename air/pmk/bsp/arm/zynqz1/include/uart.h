/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file uart.h
 * @author lumm
 * @brief UART setup and handler functions.
 */

#ifndef ARM_UART_H
#define ARM_UART_H

#include <air_arch.h>
#include <slcr.h>

#define UART0_BASE_MEMORY               0xe0000000
#define UART1_BASE_MEMORY               0xe0001000

typedef struct {
    air_u32_t ctrl;
    air_u32_t mode;
    air_u32_t int_enable;
    air_u32_t int_disable;
    air_u32_t int_mask;
    air_u32_t int_status;
    air_u32_t baud_rate_gen;
    air_u32_t rx_timeout;
    air_u32_t rx_fifo_trigger;
    air_u32_t modem_ctrl;
    air_u32_t modem_status;
    air_u32_t status;
    air_u32_t tx_rx_fifo;
    air_u32_t baud_rate_div;
    air_u32_t flow_ctrl_delay;
    air_u32_t reserved[2];
    air_u32_t tx_fifo_trigger;
} uart_t;

#define ARM_UART_CTRL_RXRST             (1U << 0)
#define ARM_UART_CTRL_TXRST             (1U << 1)
#define ARM_UART_CTRL_RX_EN             (1U << 2)
#define ARM_UART_CTRL_RX_DIS            (1U << 3)
#define ARM_UART_CTRL_TX_EN             (1U << 4)
#define ARM_UART_CTRL_TX_DIS            (1U << 5)
#define ARM_UART_CTRL_RSTTO             (1U << 6)
#define ARM_UART_CTRL_STTBRK            (1U << 7)
#define ARM_UART_CTRL_STPBRK            (1U << 8)

#define ARM_UART_MODE_CLKSEL(val)       (val << 0)
#define ARM_UART_MODE_CHRL(val)         (val << 1)
#define ARM_UART_MODE_PAR(val)          (val << 3)
#define ARM_UART_MODE_NBSTOP(val)       (val << 6)
#define ARM_UART_MODE_CHMODE(val)       (val << 8)

#define ARM_UART_INT_EN_RTRIG           (1U << 0)
#define ARM_UART_INT_EN_RXEMPTY         (1U << 1)
#define ARM_UART_INT_EN_RXFULL          (1U << 2)
#define ARM_UART_INT_EN_TXEMPTY         (1U << 3)
#define ARM_UART_INT_EN_TXFULL          (1U << 4)
#define ARM_UART_INT_EN_RXOVER          (1U << 5)
#define ARM_UART_INT_EN_RXFRAMING       (1U << 6)
#define ARM_UART_INT_EN_RXPARITY        (1U << 7)
#define ARM_UART_INT_EN_RXTOUT          (1U << 8)
#define ARM_UART_INT_EN_DMS             (1U << 9)
#define ARM_UART_INT_EN_TTRIG           (1U << 10)
#define ARM_UART_INT_EN_TNFUL           (1U << 11)
#define ARM_UART_INT_EN_TOVER           (1U << 12)

#define ARM_UART_INT_DIS_RTRIG          (1U << 0)
#define ARM_UART_INT_DIS_RXEMPTY        (1U << 1)
#define ARM_UART_INT_DIS_RXFULL         (1U << 2)
#define ARM_UART_INT_DIS_TXEMPTY        (1U << 3)
#define ARM_UART_INT_DIS_TXFULL         (1U << 4)
#define ARM_UART_INT_DIS_RXOVER         (1U << 5)
#define ARM_UART_INT_DIS_RXFRAMING      (1U << 6)
#define ARM_UART_INT_DIS_RXPARITY       (1U << 7)
#define ARM_UART_INT_DIS_RXTOUT         (1U << 8)
#define ARM_UART_INT_DIS_DMS            (1U << 9)
#define ARM_UART_INT_DIS_TTRIG          (1U << 10)
#define ARM_UART_INT_DIS_TNFUL          (1U << 11)
#define ARM_UART_INT_DIS_TOVER          (1U << 12)

#define ARM_UART_INT_RTRIG              (1U << 0)
#define ARM_UART_INT_RXEMPTY            (1U << 1)
#define ARM_UART_INT_RXFULL             (1U << 2)
#define ARM_UART_INT_TXEMPTY            (1U << 3)
#define ARM_UART_INT_TXFULL             (1U << 4)
#define ARM_UART_INT_RXOVR              (1U << 5)
#define ARM_UART_INT_RXFRAMING          (1U << 6)
#define ARM_UART_INT_RXPARITY           (1U << 7)
#define ARM_UART_INT_RXTOUT             (1U << 8)
#define ARM_UART_INT_DMS                (1U << 9)
#define ARM_UART_INT_TTRIG              (1U << 10)
#define ARM_UART_INT_TNFUL              (1U << 11)
#define ARM_UART_INT_TOVR               (1U << 12)

#define ARM_UART_STATUS_RXOVR           (1U << 0)
#define ARM_UART_STATUS_RXEMPTY         (1U << 1)
#define ARM_UART_STATUS_RXFULL          (1U << 2)
#define ARM_UART_STATUS_TXEMPTY         (1U << 3)
#define ARM_UART_STATUS_TXFULL          (1U << 4)
/* more...  (unused) */

void arm_start_uart(void);
void arm_uart_transmit(char ch);

#endif /* ARM_UART_H */
