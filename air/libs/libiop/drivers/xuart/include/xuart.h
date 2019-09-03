/*
 * Copyright (C) 2008-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author trcpse
 * @brief UART support structures and function definitions
 */
#ifndef __XUART_H__
#define __XUART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <air.h>
#include <iop.h>
#include <xuart_support.h>

/*-----------------------------------------------------------------------*/

#define UART0_BASE_MEMORY               0xe0000000
#define UART1_BASE_MEMORY               0xe0001000
#define UART_RST_CTRL                   0xf8000228
#define UART_FIFO_MAX_SIZE              64

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
} uart_ctrl_t;

typedef struct {
    uart_ctrl_t *reg;

    air_u32_t BaudRate;

} uart_priv;


//typedef struct {
//    u16 DeviceId;             /**< Unique ID of device */
//    u32 BaseAddress;          /**< Base address of device (IPIF) */
//    u32 InputClockHz;         /**< Input clock frequency */
//    s32 ModemPinsConnected;   /** Specifies whether modem pins are connected
//                                *  to MIO or FMIO */
//} XUart_Config;
//
//typedef void (*XUart_Handler) (void *CallBackRef, u32 Event,
//                  u32 EventData);
//
//typedef struct {
//    XUart_Config Config;  /* Configuration data structure */
//    u32 InputClockHz;     /* Input clock frequency */
//    u32 IsReady;          /* Device is initialized and ready */
//    u32 BaudRate;         /* Current baud rate */
//
//    XUartBuffer SendBuffer;
//    XUartBuffer ReceiveBuffer;
//
//    XUart_Handler Handler;
//    void *CallBackRef;    /* Callback reference for event handler */
//    u32 Platform;
//    u8 is_rxbs_error;
//} XUart;



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
#define ARM_UART_STATUS_TTRIG           (1U << 13)


air_u32_t iop_xuart_init(iop_device_driver_t *iop_dev, void *arg);
air_u32_t iop_xuart_open(iop_device_driver_t *iop_dev, void *arg);
air_u32_t iop_xuart_read(iop_device_driver_t *iop_dev, void *arg);
air_u32_t iop_xuart_write(iop_device_driver_t *iop_dev, void *arg);
air_u32_t iop_xuart_close(iop_device_driver_t *iop_dev, void *arg);


uart_ctrl_t *define_uart(air_u32_t port);
void arm_setup_xuart( uart_ctrl_t *uart, air_u32_t BaudRate);
void arm_xuart_transmit(uart_ctrl_t *uart, char ch);
void set_uart_mode(uart_ctrl_t *uart, char parity, int data_bits, int stop_bits);

/*-----------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif
