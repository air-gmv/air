/*
 * Copyright (C) 2011-2017  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
#include <iop.h>
#include <bsp.h>
#include <iop_support.h>
#include <spw_support.h>

void spw_prebuild_header(iop_header_t *buf)
{

    /* the total length will be inserted when we have the data */
    // buf->hlen = 0;
}

// void spw_complete_header(iop_buffer_t *buf) {

void spw_copy_header(iop_physical_device_t *iop_dev, iop_wrapper_t *wrapper, iop_header_t *header)
{

    uint32_t i;
    uint32_t header_size = 1;

    /* get underlying IOP buffer */
    iop_buffer_t *iop_buf = wrapper->buffer;

    spw_header_t *spw_header = (spw_header_t *)header;

    for (i = 1; i < sizeof(spw_header_t); i++)
    {
        if ((*spw_header).hdr[i] != 0)
        {
            header_size++;
        }
        else
        {
            break;
        }
    }

    /* set the header size and offsets */
    iop_buf->header_off = iop_buf->header_size - header_size;

    /* copy header from the route */
    (void)memcpy(get_header(iop_buf), header, header_size);

    /* complete header with the device parameters. TODO depending on how this is used
     *, the removing bytes might be done here
     */
    // spw_complete_header();
}

uint32_t spw_compare_header(iop_wrapper_t *wrapper, iop_header_t *header)
{

    return 1;
}
