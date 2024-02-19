/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file uart.h
 * \author lumm
 * \brief UART setup and handler functions.
 */

#ifndef UART_H_
#define UART_H_
#if DEBUG_MONITOR != 2

#include <air_arch.h>
#include <armv7.h>
#include <slcr.h>

#define ARM_UART_CTRL_RXRST (1U << 0)
#define ARM_UART_CTRL_TXRST (1U << 1)
#define ARM_UART_CTRL_RX_EN (1U << 2)
#define ARM_UART_CTRL_RX_DIS (1U << 3)
#define ARM_UART_CTRL_TX_EN (1U << 4)
#define ARM_UART_CTRL_TX_DIS (1U << 5)
#define ARM_UART_CTRL_RSTTO (1U << 6)
#define ARM_UART_CTRL_STTBRK (1U << 7)
#define ARM_UART_CTRL_STPBRK (1U << 8)

#define ARM_UART_MODE_CLKSEL(val) (val << 0)
#define ARM_UART_MODE_CHRL(val) (val << 1)
#define ARM_UART_MODE_PAR(val) (val << 3)
#define ARM_UART_MODE_NBSTOP(val) (val << 6)
#define ARM_UART_MODE_CHMODE(val) (val << 8)

#define ARM_UART_INT_EN_RTRIG (1U << 0)
#define ARM_UART_INT_EN_RXEMPTY (1U << 1)
#define ARM_UART_INT_EN_RXFULL (1U << 2)
#define ARM_UART_INT_EN_TXEMPTY (1U << 3)
#define ARM_UART_INT_EN_TXFULL (1U << 4)
#define ARM_UART_INT_EN_RXOVER (1U << 5)
#define ARM_UART_INT_EN_RXFRAMING (1U << 6)
#define ARM_UART_INT_EN_RXPARITY (1U << 7)
#define ARM_UART_INT_EN_RXTOUT (1U << 8)
#define ARM_UART_INT_EN_DMS (1U << 9)
#define ARM_UART_INT_EN_TTRIG (1U << 10)
#define ARM_UART_INT_EN_TNFUL (1U << 11)
#define ARM_UART_INT_EN_TOVER (1U << 12)

#define ARM_UART_INT_DIS_RTRIG (1U << 0)
#define ARM_UART_INT_DIS_RXEMPTY (1U << 1)
#define ARM_UART_INT_DIS_RXFULL (1U << 2)
#define ARM_UART_INT_DIS_TXEMPTY (1U << 3)
#define ARM_UART_INT_DIS_TXFULL (1U << 4)
#define ARM_UART_INT_DIS_RXOVER (1U << 5)
#define ARM_UART_INT_DIS_RXFRAMING (1U << 6)
#define ARM_UART_INT_DIS_RXPARITY (1U << 7)
#define ARM_UART_INT_DIS_RXTOUT (1U << 8)
#define ARM_UART_INT_DIS_DMS (1U << 9)
#define ARM_UART_INT_DIS_TTRIG (1U << 10)
#define ARM_UART_INT_DIS_TNFUL (1U << 11)
#define ARM_UART_INT_DIS_TOVER (1U << 12)

#define ARM_UART_INT_RTRIG (1U << 0)
#define ARM_UART_INT_RXEMPTY (1U << 1)
#define ARM_UART_INT_RXFULL (1U << 2)
#define ARM_UART_INT_TXEMPTY (1U << 3)
#define ARM_UART_INT_TXFULL (1U << 4)
#define ARM_UART_INT_RXOVR (1U << 5)
#define ARM_UART_INT_RXFRAMING (1U << 6)
#define ARM_UART_INT_RXPARITY (1U << 7)
#define ARM_UART_INT_RXTOUT (1U << 8)
#define ARM_UART_INT_DMS (1U << 9)
#define ARM_UART_INT_TTRIG (1U << 10)
#define ARM_UART_INT_TNFUL (1U << 11)
#define ARM_UART_INT_TOVR (1U << 12)

#define ARM_UART_STATUS_RXOVR (1U << 0)
#define ARM_UART_STATUS_RXEMPTY (1U << 1)
#define ARM_UART_STATUS_RXFULL (1U << 2)
#define ARM_UART_STATUS_TXEMPTY (1U << 3)
#define ARM_UART_STATUS_TXFULL (1U << 4)
/* more...  (unused) */

typedef struct
{
    volatile air_u32_t ctrl;
    volatile air_u32_t mode;
    volatile air_u32_t int_enable;
    volatile air_u32_t int_disable;
    volatile air_u32_t int_mask;
    volatile air_u32_t int_status;
    volatile air_u32_t baud_rate_gen;
    volatile air_u32_t rx_timeout;
    volatile air_u32_t rx_fifo_trigger;
    volatile air_u32_t modem_ctrl;
    volatile air_u32_t modem_status;
    volatile air_u32_t status;
    volatile air_u32_t tx_rx_fifo;
    volatile air_u32_t baud_rate_div;
    volatile air_u32_t flow_ctrl_delay;
    volatile air_u32_t reserved[2];
    volatile air_u32_t tx_fifo_trigger;
} uart_t;

// void arm_start_uart(void);
void arm_setup_uart(air_u32_t port, air_u32_t BaudRate);
void arm_select_debug_uart(air_u32_t port);

#endif /* DEBUG_MONITOR != 2 */
#endif /* UART_H_ */
