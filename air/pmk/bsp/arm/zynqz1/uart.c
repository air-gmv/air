/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file uart.c
 * \author lumm
 * \brief UART setup and handler functions.
 */

#include <uart.h>
#include <parameters.h>
#include <slcr.h>

#define UART(port) ((uart_t *)(XPAR_PS7_UART_0_BASEADDR + port*0x1000))
air_u32_t arm_uart_debug_port = 0;

void arm_setup_uart(air_u32_t port, air_u32_t BaudRate) {

    air_u32_t IterBAUDDIV;          /* Iterator for available baud divisor values */
    air_u32_t BRGR_Value;           /* Calculated value for baud rate generator */
    air_u32_t CalcBaudRate;         /* Calculated baud rate */
    air_u32_t BaudError;            /* Diff between calculated and requested baud rate */
    air_u32_t Best_BRGR = 0U;       /* Best value for baud rate generator */
    air_u32_t Best_BAUDDIV = 0U;    /* Best value for baud divisor */
    air_u32_t Best_Error = 0xFFFFFFFFU;
    air_u32_t InputClk = 100000000;


    for (IterBAUDDIV = 4; IterBAUDDIV < 255; IterBAUDDIV++) {

        /* Calculate the value for BRGR register */
        BRGR_Value = InputClk / (BaudRate * (IterBAUDDIV + 1));

        /* Calculate the baud rate from the BRGR value */
        CalcBaudRate = InputClk / (BRGR_Value * (IterBAUDDIV + 1));

        /* Avoid unsigned integer underflow */
        if (BaudRate > CalcBaudRate) {
            BaudError = BaudRate - CalcBaudRate;
        }
        else {
            BaudError = CalcBaudRate - BaudRate;
        }

        /* Find the calculated baud rate closest to requested baud rate. */
        if (Best_Error > BaudError) {

            Best_BRGR = BRGR_Value;
            Best_BAUDDIV = IterBAUDDIV;
            Best_Error = BaudError;
        }
    }

    arm_instruction_synchronization_barrier();

    //disable uart:
    UART(port)->ctrl = (ARM_UART_CTRL_RX_DIS | ARM_UART_CTRL_TX_DIS );

    //write baud rate:
    UART(port)->baud_rate_gen = Best_BRGR;
    UART(port)->baud_rate_div = Best_BAUDDIV;

    //reset tx and rx
    UART(port)->ctrl = (ARM_UART_CTRL_RXRST | ARM_UART_CTRL_TXRST) ;
    //enable uart
    UART(port)->ctrl = (ARM_UART_CTRL_RX_EN | ARM_UART_CTRL_TX_EN);

    arm_data_synchronization_barrier();
}

void arm_select_debug_uart(air_u32_t port) {

    arm_uart_debug_port = port;
}

void arm_uart_transmit(char ch) {

    // Waits until txFIFO is empty
    while((UART(arm_uart_debug_port)->status & ARM_UART_STATUS_TXEMPTY) == 0);

    UART(arm_uart_debug_port)->tx_rx_fifo = ((air_u32_t)ch & 0xff);
}
