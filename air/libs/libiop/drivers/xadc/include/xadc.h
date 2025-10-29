/*
 * Copyright (C) 2021  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/air-dev/-/blob/dev/LICENSE
 */
/**
 * @file xadc.h
 * @author TTAO
 * @brief XADC Driver - Macros and Definitions
 */

#ifndef __XADC_H__
#define __XADC_H__

#include <air.h>
#include <iop.h>
#include <xadcps.h>
#include <xparameters.h>
#include <iop_support.h>
#include <xadc_support.h>

air_u32_t iop_xadc_init(iop_device_driver_t *iop_dev, void *arg);
air_u32_t iop_xadc_open(iop_device_driver_t *iop_dev, void *arg);
air_u32_t iop_xadc_read(iop_device_driver_t *iop_dev, void *arg);
air_u32_t iop_xadc_close(iop_device_driver_t *iop_dev, void *arg);

#endif /* __XADC_H__ */
