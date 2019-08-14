/*
 * Copyright (C) 2012-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 *  @file gr1553_support.c
 *
 *  @author Claudio Silva
 *
 *  @brief IOP STD-MIL-1553B Support Functions
 */
#include <gr1553_support.h>
#include <bsp.h>

void gr1553_copy_header(
        iop_physical_device_t *iop_dev,
        iop_wrapper_t *wrapper,
        iop_header_t *header)
{
    /* get underlying IOP buffer */
    iop_buffer_t *iop_buf = wrapper->buffer;

    /* set the header size and offsets */
    iop_buf->header_off = iop_buf->header_size - sizeof(milstd_header_t);
    iop_buf->header_size = sizeof(milstd_header_t);

    /* copy header from the route */
    memcpy(get_header(iop_buf), header, iop_buf->header_size);
}

uint32_t gr1553_compare_header(iop_wrapper_t *wrapper, iop_header_t *header) {

    milstd_header_t *src = (milstd_header_t *)header;
    milstd_header_t *rcv = (milstd_header_t *)get_header(wrapper->buffer);

    if ((rcv->desc != src->desc) || (rcv->address != src->address)) {
        return 0;
    }

    return 1;
}
