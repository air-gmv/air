/*
 * Copyright (C) 2021  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/air-dev/-/blob/dev/LICENSE
 */
/**
 * @file xsd_support.c
 * @author TTAO
 * @brief XSD support functions
 */

#include <xsd_support.h>

void xsd_copy_header(iop_physical_device_t *iop_dev, iop_wrapper_t *wrapper, iop_header_t *header)
{

    iop_buffer_t *iop_buf = wrapper->buffer;

    iop_buf->header_off = iop_buf->header_size - sizeof(sd_header_t);
    iop_buf->header_size = sizeof(sd_header_t);

    memcpy(get_header(iop_buf), header, iop_buf->header_size);
}

uint32_t xsd_compare_header(iop_wrapper_t *wrapper, iop_header_t *header)
{

    sd_header_t *sd_header1 = (sd_header_t *)header;
    sd_header_t *sd_header2 = (sd_header_t *)get_header(wrapper->buffer);

    if (sd_header1->size == sd_header2->size && strcmp(sd_header1->file, sd_header2->file) == 0)
    {
        return 1;
    }

    return 0;
}
