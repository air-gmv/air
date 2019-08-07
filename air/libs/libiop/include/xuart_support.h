/**
 * @file
 * @author trcpse
 * @brief UART support structures and function definitions
 */

#ifndef __UART_SUPPORT_H__
#define __UART_SUPPORT_H__

#include <iop.h>
#include <iop_support.h>

/**
 * @brief UART device
 */
typedef struct {
    /* Generic device configuration */
    iop_device_driver_t dev;

    int uart_core; // Identify which UART core corresponds to (either UART0 or UART1)
    int baud_rate;

} iop_uart_device_t;

void uart_copy_header(
        iop_physical_device_t *iop_dev,
        iop_wrapper_t *wrapper,
        iop_header_t *header);

air_u32_t uart_compare_header(
        iop_wrapper_t *wrapper,
        iop_header_t *header);

void uart_prebuild_header(
        uart_header_t *header);

void xuart_writer(air_uptr_t arg);
void xuart_reader(air_uptr_t arg);

#endif
