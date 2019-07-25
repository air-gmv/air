/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2019
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
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
