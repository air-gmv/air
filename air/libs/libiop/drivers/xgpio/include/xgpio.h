/*
 * Copyright (C) 2019-2020  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file
 * \author jmda
 * \brief GPIO driver adaptar for LIBIOP
 */

#ifndef __XILGPIO_H__
#define __XILGPIO_H__

#include <air.h>
#include <iop.h>
#include <gpio_support.h>

typedef enum
{
    DEVICE_NONE = -1,
    DEVICE_ZYNQ,
    DEVICE_ULTRASCALE
} gpioDeviceType;

air_u32_t iop_gpio_init(iop_device_driver_t *iop_dev, void *arg);
air_u32_t iop_gpio_read(iop_device_driver_t *iop_dev, void *arg);
air_u32_t iop_gpio_write(iop_device_driver_t *iop_dev, void *arg);

#endif /* __XILGPIO_H__ */
