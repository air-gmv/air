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
#include <slcr.h>

static volatile uart_t *uart = (uart_t *)UART0_BASE_MEMORY;
static volatile uart_t *uart1 = (uart_t *)UART1_BASE_MEMORY;

void arm_define_uart(air_u32_t port) {
    if (port==1) {
        uart = uart1;
    }
}

//void arm_reset_uart(void) {
//    volatile air_uptr_t uart_rst_ctrl = (air_uptr_t)(SLCR_BASE_MEMORY + 0x228);
//    *uart_rst_ctrl = 0xf;
//}
//
//void arm_start_uart(void) {
//
//    /* 1. Reset Controller */
//    arm_reset_uart();
//
//    /* 2. Configure I/O signal routing */
//    volatile air_uptr_t mio_pin_46 = (air_uptr_t)(SLCR_BASE_MEMORY + 0x7b8);
//    volatile air_uptr_t mio_pin_47 = (air_uptr_t)(SLCR_BASE_MEMORY + 0x7bc);
//
//    *mio_pin_46 = (MIO_PIN_TRI_ENABLE |
//            MIO_PIN_L0_SEL(0) |
//            MIO_PIN_L1_SEL(0) |
//            MIO_PIN_L2_SEL(0) |
//            MIO_PIN_L3_SEL(0x7) |
//            MIO_PIN_SPEED(MIO_PIN_SLOW_CMOS_EDGE) |
//            MIO_PIN_IO_TYPE(MIO_PIN_IO_LVCMOS18) |
//            MIO_PIN_PULLUP(1) |
//            MIO_PIN_DISABLE_RCVR);
//
//    *mio_pin_47 = (MIO_PIN_L0_SEL(0) |
//            MIO_PIN_L1_SEL(0) |
//            MIO_PIN_L2_SEL(0) |
//            MIO_PIN_L3_SEL(0x7) |
//            MIO_PIN_SPEED(MIO_PIN_SLOW_CMOS_EDGE) |
//            MIO_PIN_IO_TYPE(MIO_PIN_IO_LVCMOS18) |
//            MIO_PIN_PULLUP(1) |
//            MIO_PIN_DISABLE_RCVR);
//
//    /* 3. Configure reference clock */
//    volatile air_uptr_t uart_clk_ctrl = (air_uptr_t)(SLCR_BASE_MEMORY + 0x154);
//    *uart_clk_ctrl = ((UART_CLK_CTRL_CLKACT0_ENABLE |
//            UART_CLK_CTRL_SRCSEL(0x0) |
//            UART_CLK_CTRL_DIVISOR(0x14)) & //50MHz
//            ~UART_CLK_CTRL_CLKACT1_ENABLE);
//
//    /* 4. Configure controller functions */
//    uart0->mode = (ARM_UART_MODE_PAR(0x4));
//
//    uart0->ctrl = ((ARM_UART_CTRL_RX_DIS |
//            ARM_UART_CTRL_TX_DIS) &
//            ~ARM_UART_CTRL_RX_EN &
//            ~ARM_UART_CTRL_TX_EN);
//    arm_instruction_synchronization_barrier();
//    /* Baud Rate of 921,600. Values on page 594 of ug585-zynq-7000-trm */
//    uart0->baud_rate_gen = 9;
//    uart0->baud_rate_div = 5;
//    arm_data_synchronization_barrier();
//    uart0->ctrl = (ARM_UART_CTRL_RXRST |
//            ARM_UART_CTRL_TXRST);
//    arm_data_synchronization_barrier();
//    uart0->ctrl = ((ARM_UART_CTRL_RX_EN |
//                ARM_UART_CTRL_TX_EN) &
//                ~ARM_UART_CTRL_RX_DIS &
//                ~ARM_UART_CTRL_TX_DIS);
//
//    uart0->tx_fifo_trigger = 0; // no trigger
//
////    uart0->ctrl = ((ARM_UART_CTRL_RXRST |
////            ARM_UART_CTRL_TXRST |
////            ARM_UART_CTRL_RX_EN |
////            ARM_UART_CTRL_TX_EN |
////            ARM_UART_CTRL_RSTTO |
////            ARM_UART_CTRL_STPBRK) &
////            ~ARM_UART_CTRL_RX_DIS &
////            ~ARM_UART_CTRL_TX_DIS &
////            ~ARM_UART_CTRL_STTBRK);
////
////    uart0->rx_timeout = 0; //timeout disabled
////
////    /* 5. Configure interrupts (none for now) */
////    do {
////        uart0->int_disable = ARM_UART_INT_DIS_RTRIG;
////    } while ((uart0->int_status && ARM_UART_INT_RTRIG) != 0);
//
//    /* 6. Configure modem controls (optional) */
//}

void arm_setup_uart(air_u32_t BaudRate) {

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
    uart->ctrl = (ARM_UART_CTRL_RX_DIS | ARM_UART_CTRL_TX_DIS );

    //write baud rate:
    uart->baud_rate_gen = Best_BRGR;
    uart->baud_rate_div = Best_BAUDDIV;

    //reset tx and rx
    uart->ctrl = (ARM_UART_CTRL_RXRST | ARM_UART_CTRL_TXRST) ;
    //enable uart
    uart->ctrl = (ARM_UART_CTRL_RX_EN | ARM_UART_CTRL_TX_EN);

    arm_data_synchronization_barrier();
}

void arm_uart_transmit(char ch) {

    while((uart->status & ARM_UART_STATUS_TXEMPTY) == 0); // Waits until txFIFO is empty

    uart->tx_rx_fifo = ((air_u32_t)ch & 0xff);

}
