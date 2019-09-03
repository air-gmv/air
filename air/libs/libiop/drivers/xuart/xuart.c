/*
 * Copyright (C) 2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * https://spass-git-ext.gmv.com/AIR/AIR/raw/master/air/LICENSE
 */
/**
 * @file
 * @author trcpse
 * @brief UART support structures and function definitions
 */
#include <bsp.h>
#include <xuart.h>

air_u32_t iop_xuart_init(iop_device_driver_t *iop_dev, void *arg) {

    iop_uart_device_t *device = (iop_uart_device_t *) iop_dev;
    uart_priv *pDev = (uart_priv *) (device->dev.driver);

    int uart_id = device->uart_core;
    pDev->reg = define_xuart(uart_id);

    char parity = device->parity;
    int data_bits = device->data_bits;
    int stop_bits = device->stop_bits;

    set_uart_mode(pDev->reg, parity, data_bits, stop_bits);


    air_u32_t baud = (air_u32_t) device->baud_rate;
    arm_setup_xuart(pDev->reg, baud);

    pDev->reg->rx_fifo_trigger = 0;

    return AIR_SUCCESSFUL;

}

air_u32_t iop_xuart_open(iop_device_driver_t *iop_dev, void *arg) {

    return AIR_SUCCESSFUL;
}

air_u32_t iop_xuart_read(iop_device_driver_t *iop_dev, void *arg) {

    static air_u32_t receive_count = 0;
    int i;
    /*Current UART device*/
    iop_uart_device_t *device = (iop_uart_device_t *) iop_dev;
    uart_priv *pDev = (uart_priv *) (device->dev.driver);


    if (arg == NULL)
        return AIR_INVALID_PARAM;

    //get IOP buffer
    iop_wrapper_t *wrapper = (iop_wrapper_t *) arg;
    iop_buffer_t *iop_buffer = wrapper->buffer;

    static char d[UART_FIFO_MAX_SIZE];

    if (iop_buffer->v_addr == NULL) {
        iop_debug("iop_xuart_write error iop_buffer\n");
        return AIR_INVALID_SIZE;
    }


    while( (receive_count<(device->data_bytes)) && (!(pDev->reg->status & ARM_UART_STATUS_RXEMPTY)) ){
        d[receive_count] = (char) (pDev->reg->tx_rx_fifo);
        receive_count++;
    }

    if(receive_count<(device->data_bytes))
        return AIR_INVALID_SIZE;

    iop_buffer->payload_off = iop_buffer->header_size = sizeof(iop_header_t);//sizeof(uart_header_t);
    iop_buffer->header_off = iop_buffer->header_size - sizeof(iop_header_t);
    iop_buffer->payload_size = device->data_bytes;

    char *b = (char *) get_payload(iop_buffer);
    memcpy(b, d, iop_buffer->payload_size);

    iop_buffer->payload_size = receive_count;
    //iop_debug("\n\n READ str: %s | Receive count = %d \n\n", b, (int) receive_count);
    receive_count=0;
    return AIR_SUCCESSFUL;
}


air_u32_t iop_xuart_write(iop_device_driver_t *iop_dev, void *arg) {

    air_u32_t sent_count = 0;
    /*Current UART device*/
    iop_uart_device_t *device = (iop_uart_device_t *) iop_dev;
    uart_priv *pDev = (uart_priv *) (device->dev.driver);

    if (arg == NULL){
        iop_debug("\n\n Invalid Param \n\n");
        return AIR_INVALID_PARAM;
    }
    //get IOP buffer
    iop_wrapper_t *wrapper = (iop_wrapper_t *) arg;
    iop_buffer_t *iop_buffer = wrapper->buffer;

    if (iop_buffer->v_addr == NULL) {
        iop_debug("iop_xuart_write error iop_buffer\n");
        return AIR_INVALID_SIZE;
    }

    char *b = (char *) iop_buffer->v_addr + sizeof(iop_header_t);

    for (sent_count = 0; sent_count < (iop_buffer->payload_size);
            sent_count++) {
        arm_xuart_transmit(pDev->reg, b[sent_count]);
    }
    arm_xuart_transmit(pDev->reg, '\n');

    //iop_debug("\nWRITE = %s", b);

    return AIR_SUCCESSFUL;
}

void arm_xuart_transmit(uart_ctrl_t *uart, char ch) {

    while ((uart->status & ARM_UART_STATUS_TXEMPTY) == 0);

    if ((uart->status & ARM_UART_STATUS_TXFULL) == 0) {
        uart->tx_rx_fifo = ((air_u32_t) ch & 0xff);
    }
}

air_u32_t iop_xuart_close(iop_device_driver_t *iop_dev, void *arg) {

//    iop_uart_device_t *device = (iop_uart_device_t *) iop_dev;
//    uart_priv *pDev = (uart_priv *) (device->dev.driver);
//
//    if (pDev->started == STATE_STOPPED)
//        return -1;
//
//    if (pDev->started == STATE_STARTED) {
//        grcan_hw_stop(pDev);
//        do_sw_stop = 1;
//    } else {
//        /*
//         * started == STATE_[STOPPED|BUSOFF|AHBERR] so grcan_hw_stop()
//         * might already been called from ISR.
//         */
//        do_sw_stop = 0;
//    }
//    pDev->started = STATE_STOPPED;
//
//    return 0;
    return AIR_SUCCESSFUL;

}

/*----------------------------------------------------------------------*/

void arm_reset_uart(void) {
    volatile air_uptr_t uart_rst_ctrl = (air_uptr_t) (UART_RST_CTRL);

    if (arm_slcr_is_locked()) {
        arm_slcr_unlock();
    }

    *uart_rst_ctrl |= 0xf;
    *uart_rst_ctrl &= ~(0xf);

    if (!arm_slcr_is_locked()) {
        arm_slcr_lock();
    }

}

uart_ctrl_t *define_xuart(air_u32_t port) {

    if (port == 1)
        return (uart_ctrl_t *) UART1_BASE_MEMORY;
    else
        return (uart_ctrl_t *) UART0_BASE_MEMORY;
}

void arm_setup_xuart(uart_ctrl_t *uart, air_u32_t BaudRate) {

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
        } else {
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
    uart->ctrl = (ARM_UART_CTRL_RX_DIS | ARM_UART_CTRL_TX_DIS);

    //write baud rate:
    uart->baud_rate_gen = Best_BRGR;
    uart->baud_rate_div = Best_BAUDDIV;

    //reset tx and rx
    uart->ctrl = (ARM_UART_CTRL_RXRST | ARM_UART_CTRL_TXRST);
    //enable uart
    uart->ctrl = (ARM_UART_CTRL_RX_EN | ARM_UART_CTRL_TX_EN);

    arm_data_synchronization_barrier(15);
}


void set_uart_mode(uart_ctrl_t *uart, char parity, int data_bits, int stop_bits) {

    air_u32_t mode = ((uart->mode)&(0xFFFFFC01));


   // mode |=  0x200;   NORMAL: 000; AUTOMATIC ECHO: 100; LOCAL LOOPBACK: 200; REMOTE LOOPBACK: 300

    //PARITY -> default is EVEN (000)
    if (parity=='O')
        mode |= ARM_UART_MODE_PAR(001);
    else if (parity=='N')
        mode |= ARM_UART_MODE_PAR(100);

    //DATA BITS -> default is 8 (0x)
    if (data_bits==6)
        mode |= ARM_UART_MODE_CHRL(11);
    else if (data_bits==7)
        mode |= ARM_UART_MODE_CHRL(10);

    //STOP BITS -> default is 1 (00)
    if (stop_bits==2)
        mode |= ARM_UART_MODE_CHRL(10);

    uart->mode = mode;

}
