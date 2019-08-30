#include <iop.h>
#include <bsp.h>
#include <iop_support.h>
#include <xuart_support.h>

void uart_copy_header(
        iop_physical_device_t *iop_dev,
        iop_wrapper_t *wrapper,
        iop_header_t *header){

    /* get underlying UART device */
//    iop_can_device_t *uart_dev = (iop_uart_device_t *)iop_uart->driver;

    /* get underlying IOP buffer */
    iop_buffer_t *iop_buf = wrapper->buffer;

    /* set the header size and offsets */
    iop_buf->header_off = iop_buf->header_size - sizeof(uart_header_t);
    iop_buf->header_size = sizeof(uart_header_t);

    /* copy header from the route */
    memcpy(get_header(iop_buf), header, iop_buf->header_size);
}

/* This function compares the ID on the wrapper
 * to the ID  of a given header. */
air_u32_t uart_compare_header(
        iop_wrapper_t *wrapper,
        iop_header_t *header){

#if 0
    uart_header_t *uart_header1 = (uart_header_t *) header;
    uart_header_t *uart_header2 = (uart_header_t *) get_header(wrapper->buffer);

    iop_debug("\nComparing headers - H1:%d H2:%d\n", uart_header1->id , uart_header2->id);
    if (uart_header1->id == uart_header2->id){
        return 1;
    }
    return 0;
#endif

    return 1;
}

void uart_prebuild_header(uart_header_t *header){
    /* If ID not defined */
    if(header->id == 0){
        header->id = 255;
    }
}
