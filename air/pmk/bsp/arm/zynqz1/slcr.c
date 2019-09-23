/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file slcr.c
 * @author trcpse
 * @brief functions to handle the slcr
 *
 */

#include <slcr.h>


static volatile air_u32_t *a9_cpu_rst_ctrl = (air_u32_t *)0xF8000244;

 void arm_peripheral_soft_reset(void) {
    if (arm_slcr_is_locked()) {
        arm_slcr_unlock();
    }
    *a9_cpu_rst_ctrl |= 0x100;

    *a9_cpu_rst_ctrl &= ~(0x100);

    if (!arm_slcr_is_locked()) {
        arm_slcr_lock();
    }

}
