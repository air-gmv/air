/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file slcr.c
 * \author trcpse
 * \brief functions to handle the slcr
 *
 */

#include <slcr.h>

#define PSS_RST_CTRL *(XPAR_PS7_SLCR_0_S_AXI_BASEADDR + 0x200)
#define A9_CPU_RST_CTRL *(XPAR_PS7_SLCR_0_S_AXI_BASEADDR + 0x244)

void arm_peripheral_soft_reset(void) {
    if (arm_slcr_is_locked()) {
        arm_slcr_unlock();
    }
    A9_CPU_RST_CTRL |= 0x100;

    A9_CPU_RST_CTRL &= ~(0x100);

    if (!arm_slcr_is_locked()) {
        arm_slcr_lock();
    }

}

void arm_ps_reset(void) {

    if (ARM_SLCR_IS_LOCKED) arm_slcr_unlock();

    while(true) PSS_RST_CTRL = 1;

    /* shouldn't exit from this function */
}
