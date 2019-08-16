/**
 * @file
 * @author trcpse
 * @brief UART support structures and function definitions
 */
#include <bsp.h>
#include <xuart.h>

static volatile uart_ctrl_t *uart = (uart_ctrl_t *) UART0_BASE_MEMORY;
static volatile uart_ctrl_t *uart1 = (uart_ctrl_t *) UART1_BASE_MEMORY;

/* air_u32_t iop_xuart_initialize(iop_device_driver_t *iop_dev, void *arg){

    if(AIR_SUCCESSFUL != iop_grcan_device_init(iop_dev)){
        // Couldn't initialize the device
        iop_debug("Internal error on grcan_device_init\n");
        return AIR_DEVICE_ERROR;
    }

    return AIR_SUCCESSFUL;
}*/

/*----------------------------------------------------------------------*/

air_u32_t iop_xuart_init(iop_device_driver_t *iop_dev, void *arg) {

//    iop_uart_device_t *device = (iop_uart_device_t *) iop_dev;
//    uart_priv *pDev = (uart_priv*) (device->dev.driver);
//    amba_apb_dev_t xuartdev;
//    timer_regmap_t *tregs;
//
//    amba_confarea_t * ambabus = (amba_confarea_t *)air_syscall_get_ambaconf();
//
//    memset(&xuartdev, 0, sizeof(amba_apb_dev_t));
//
//    if(!amba_get_apb_slave(ambabus, VENDOR_GAISLER, GAISLER_GRCAN, device->uart_core, &xuartdev))       // DEFINIR XUART!!!
//        return AIR_DEVICE_NOT_FOUND;
//
//    pDev->irq = xuartdev.irq;
//    pDev->regs = (struct uart_ctrl_t *) (xuartdev.start);
//    pDev->minor = device->uart_core;
//
//    /*
//     * Auto Detect the UART core frequency by assuming that the system frequency is
//     * is the same as the UART core frequency.
//     */
//    if (amba_get_apb_slave(ambabus,VENDOR_GAISLER,GAISLER_GPTIMER,0,&gptimer))
//    {
//        /*Timer memory mapped registers*/
//        tregs = (timer_regmap_t*)gptimer.start;
//
//        /*Calculate System frequency based on the timer register*/
//        pDev->corefreq_hz = (tregs->scaler_reload+1)*1000000;
//    }
//    else
//    {
//        pDev->corefreq_hz = 250000000;
//        iop_debug("XUART: Failed to detect system frequency\n\r");
//    }
//
//    /* Reset Hardware*/
//    //grcan_hw_reset(pDev->regs);
    return AIR_SUCCESSFUL;

}

air_u32_t iop_xuart_open(iop_device_driver_t *iop_dev, void *arg) {   // a partir de iop_grcan_open e de iop_grcan_open_internal

    //iop_uart_device_t *device = (iop_uart_device_t *) iop_dev;

    return AIR_SUCCESSFUL;
}

air_u32_t iop_xuart_read(iop_device_driver_t *iop_dev, void *arg) {

    air_u32_t receive_count;

    iop_debug("\n\n XUART READ \n\n");

    /*Current UART device*/
    iop_uart_device_t *device = (iop_uart_device_t *) iop_dev;
    uart_ctrl_t *pDev = (uart_ctrl_t *) (device->dev.driver);

    if (arg == NULL)
        return AIR_INVALID_PARAM;

    //get IOP buffer
    iop_wrapper_t *wrapper = (iop_wrapper_t *) arg;
    iop_buffer_t *iop_buffer = wrapper->buffer;

    iop_buffer->payload_off = iop_buffer->header_size = sizeof(uart_header_t);
    iop_buffer->header_off = iop_buffer->header_size - sizeof(uart_header_t);
    iop_buffer->payload_size = 1024;

    char *b = (char *) iop_buffer->v_addr;

    if (iop_buffer->v_addr == NULL) {
        iop_debug("iop_xuart_write error iop_buffer\n");
        return AIR_INVALID_SIZE;
    }

    for (receive_count = 0; receive_count < iop_buffer->payload_size;
            receive_count++) {
        b[receive_count] = arm_uart_receive();
        iop_debug("\n char: %c \n", b);
    }
    iop_debug("\n\n MSG: %s  [END OF MSG] \n\n", b);

    return AIR_SUCCESSFUL;
}

air_u32_t arm_uart_receive() {

    //while((uart->status & ARM_UART_STATUS_RXFULL) == 0);

    if ((uart->status & ARM_UART_STATUS_RXEMPTY) == 0) {
        return (air_u32_t) (uart->tx_rx_fifo);
    }
    return AIR_SUCCESSFUL;
}

air_u32_t iop_xuart_write(iop_device_driver_t *iop_dev, void *arg) {

    air_u32_t sent_count = 0;

    iop_debug("\n\n XUART WRITE \n\n");

    /*Current UART device*/
    iop_uart_device_t *device = (iop_uart_device_t *) iop_dev;
    uart_ctrl_t *pDev = (uart_ctrl_t *) (device->dev.driver);

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

    air_u8_t *b = (air_u8_t *) iop_buffer->v_addr;

    for (sent_count = 0; sent_count < (iop_buffer->payload_size);
            sent_count++) {
        arm_xuart_transmit(b[sent_count]);
    }
    return AIR_SUCCESSFUL;
}

void arm_xuart_transmit(char ch) {

    while ((uart->status & ARM_UART_STATUS_TXEMPTY) == 0);

    if ((uart->status & ARM_UART_STATUS_TXFULL) == 0) {
        uart->tx_rx_fifo = ((air_u32_t) ch & 0xff);
    }
}

void iop_xuart_close(iop_device_driver_t *iop_dev, void *arg) {   //can_close e can_stop e can_hw stop (sw não interessa, é semáforos)

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

void arm_define_uart(air_u32_t port) {
    if (port == 1) {
        uart = uart1;
    }
}

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
