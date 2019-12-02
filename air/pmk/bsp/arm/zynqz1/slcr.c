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

#define PSS_RST_CTRL *(air_uptr_t)(XPAR_PS7_SLCR_0_S_AXI_BASEADDR + 0x200)
#define A9_CPU_RST_CTRL *(air_uptr_t)(XPAR_PS7_SLCR_0_S_AXI_BASEADDR + 0x244)

void arm_peripheral_soft_reset(void) {

    if (arm_slcr_is_locked()) arm_slcr_unlock();

    A9_CPU_RST_CTRL |= PERI_RST;
    arm_instruction_synchronization_barrier();
    A9_CPU_RST_CTRL &= ~PERI_RST;

    if (!arm_slcr_is_locked()) arm_slcr_lock();
}   

void arm_ps_reset(void) {

    if (arm_slcr_is_locked()) arm_slcr_unlock();

    while(1) {
        PSS_RST_CTRL = 1;
        arm_instruction_synchronization_barrier();
    }

    /* shouldn't exit from this function */
}
