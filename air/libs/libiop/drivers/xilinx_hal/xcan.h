/*
 * Copyright (C) 2019-2020  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file
 * \author lumm
 * \brief CAN driver adaptar for LIBIOP
 */

#ifndef __XILCAN_H__
#define __XILCAN_H__

#include <air.h>
#include <iop.h>
#include <can_support.h>

air_u32_t iop_can_init(iop_device_driver_t *iop_dev, void *arg);
air_u32_t iop_can_open(iop_device_driver_t *iop_dev, void *arg);
air_u32_t iop_can_receive(iop_device_driver_t *iop_dev, void *arg);
air_u32_t iop_can_transmit(iop_device_driver_t *iop_dev, void *arg);
air_u32_t iop_can_close(iop_device_driver_t *iop_dev, void *arg);

#endif /* __XILCAN_H__ */
