/*
 * Copyright (C) 2021  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/air-dev/-/blob/dev/LICENSE
 */
/**
 * @file xadc_support.h
 * @author TTAO
 * @brief XADC support structures and function definitions
 */

#ifndef __XADC_SUPPORT_H__
#define __XADC_SUPPORT_H__

#include <iop.h>
#include <iop_support.h>

/**
 * @brief ADC Device.
 */
typedef struct
{

    iop_device_driver_t dev;
    unsigned short id;

} iop_adc_device_t;

/**
 *
 * @param arg: Pointer to the Physical device struct.
 */
void xadc_writer(air_uptr_t arg);

/**
 *
 * @param arg: Pointer to the Physical device struct.
 */
void xadc_reader(air_uptr_t arg);

/**
 * Copies an ADC header to a wrapper.
 * @param iop_dev: Physical device, not used.
 * @param wrapper: IOP wrapper.
 * @param header: ADC header to copy to the wrapper.
 */
void xadc_copy_header(iop_physical_device_t *iop_dev, iop_wrapper_t *wrapper, iop_header_t *header);

/**
 * Compares two ADC headers.
 * @param wrapper: IOP wrapper with an ADC header.
 * @param header: ADC header.
 * @return 1 if the ADC headers match, 0 otherwise.
 */
uint32_t xadc_compare_header(iop_wrapper_t *wrapper, iop_header_t *header);

#endif /* __XADC_SUPPORT_H__ */
