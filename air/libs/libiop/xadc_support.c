/*
 * Copyright (C) 2021  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/air-dev/-/blob/dev/LICENSE
 */
/**
 * @file xadc_support.c
 * @author TTAO
 * @brief XADC support functions
 */

#include <xadc_support.h>

void xadc_copy_header(iop_physical_device_t *iop_dev, iop_wrapper_t *wrapper, iop_header_t *header)
{

    iop_buffer_t *iop_buf = wrapper->buffer;

    iop_buf->header_off = iop_buf->header_size - sizeof(adc_header_t);
    iop_buf->header_size = sizeof(adc_header_t);

    memcpy(get_header(iop_buf), header, iop_buf->header_size);
}

uint32_t xadc_compare_header(iop_wrapper_t *wrapper, iop_header_t *header)
{

    adc_header_t *adc_header1 = (adc_header_t *)header;
    adc_header_t *adc_header2 = (adc_header_t *)get_header(wrapper->buffer);

    if (adc_header1->id == adc_header2->id)
    {
        return 1;
    }

    return 0;
}
