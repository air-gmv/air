/*
 * Copyright (C) 2017-2020  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file can_support.c
 *
 *  Created on: Sep 7, 2017
 *      Author: gmvs@gmv.com
 *
 *  Modified on: Jan 5, 2020
 *      Author: lumm <lmurta@gmv.com>
 *
 *      Revised to include ARM/Xilinx can drivers
 */

#include <bsp.h>
#include <can_support.h>

void can_copy_header(iop_physical_device_t *iop_dev, iop_wrapper_t *wrapper, iop_header_t *header)
{

    /* get underlying CAN device */
    //    iop_can_device_t *can_dev = (iop_can_device_t *)iop_dev->driver;

    /* get underlying IOP buffer */
    iop_buffer_t *iop_buf = wrapper->buffer;

    /* set the header size and offsets */
    iop_buf->header_off = iop_buf->header_size - sizeof(can_header_t);
    iop_buf->header_size = sizeof(can_header_t);

    /* copy header from the route */
    (void)memcpy(get_header(iop_buf), header, get_header_size(iop_buf));
}

/* This function compares the ID on the wrapper
 * to the ID  of a given header. */
uint32_t can_compare_header(iop_wrapper_t *wrapper, iop_header_t *header)
{

    can_header_t *can_header1 = (can_header_t *)header;
    can_header_t *can_header2 = (can_header_t *)get_header(wrapper->buffer);
    iop_debug("Comparing headers - H1:%d H2:%d\n", can_header1->id, can_header2->id);

    if (can_header1->id == can_header2->id)
    {

        return 1;
    }

    return 0;
}

void can_prebuild_header(can_header_t *header)
{

    /* If ID not defined */
    if (header->id == 0)
    {

        header->id = 255;
        header->extended = 0;
        header->rtr = 0;
        header->sshot = 0;
    }
}
