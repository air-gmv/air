/*
 * Copyright (C) 2017-2020  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file gpio_support.c
 *
 *  Created on: Dec 15, 2022
 *      Author: gmvs@gmv.com
 *
 *
 */

#include <bsp.h>
#include <gpio_support.h>

void gpio_copy_header(iop_physical_device_t *iop_dev, iop_wrapper_t *wrapper, iop_header_t *header)
{

    /* get underlying IOP buffer */
    iop_buffer_t *iop_buf = wrapper->buffer;

    /* set the header size and offsets */
    iop_buf->header_off = iop_buf->header_size - sizeof(gpio_header_t);
    iop_buf->header_size = sizeof(gpio_header_t);

    gpio_header_t *gpio_header = (gpio_header_t *)header;
    iop_debug("Source header: pin:%d\n", gpio_header->pin);
    /* copy header from the route */
    (void)memcpy(get_header(iop_buf), header, get_header_size(iop_buf));
}

/* This function compares the ID on the wrapper
 * to the ID  of a given header. */
uint32_t gpio_compare_header(iop_wrapper_t *wrapper, iop_header_t *header)
{

    gpio_header_t *gpio_header1 = (gpio_header_t *)get_header(wrapper->buffer);
    gpio_header_t *gpio_header2 = (gpio_header_t *)header;
    iop_debug("Comparing headers - H1:%d, H2:%d\n", gpio_header1->pin, gpio_header2->pin);

    if (gpio_header1->pin == gpio_header2->pin)
    {

        return 1;
    }

    return 0;
}
