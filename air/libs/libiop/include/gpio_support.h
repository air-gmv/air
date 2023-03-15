/*
 * Copyright (C) 2017-2022  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 *  \author gmvs@gmv.com
 *
 *  Modified on: December, 2022
 *      Author: jmda <jmda@gmv.com>
 *
 *
 * \brief GPIO support structures and functions definitions
 *  to integrate the gpio driver into AIR
 */

#ifndef __GPIO_SUPPORT_H__
#define __GPIO_SUPPORT_H__

#include <iop.h>
#include <iop_support.h>

typedef struct {

    /* Generic device configuration */
    iop_device_driver_t dev;

    uint8_t port;
    uint32_t writeMask;

} iop_gpio_device_t;

void gpio_copy_header(
        iop_physical_device_t *iop_dev,
        iop_wrapper_t *wrapper,
        iop_header_t *header);

uint32_t gpio_compare_header(
        iop_wrapper_t *wrapper,
        iop_header_t *header);

void gpio_writer(air_uptr_t arg);
void gpio_reader(air_uptr_t arg);

#endif
