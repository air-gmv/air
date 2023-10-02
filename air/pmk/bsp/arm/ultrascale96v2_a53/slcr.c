/*
 * Copyright (C) 2018-2020  GMVIS Skysoft S.A.
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

#define PSS_RST_CTRL (air_uptr_t *)(XPAR_PSU_FPD_SLCR_SECURE_S_AXI_BASEADDR + 0x200)
#define A9_CPU_RST_CTRL (air_uptr_t *)(XPAR_PSU_FPD_SLCR_SECURE_S_AXI_BASEADDR + 0x244)

void arm_peripheral_soft_reset(void) {

    if (arm_slcr_is_locked()) arm_slcr_unlock();

    *A9_CPU_RST_CTRL |= PERI_RST;
    arm_instruction_synchronization_barrier();
    *A9_CPU_RST_CTRL &= ~PERI_RST;

    if (!arm_slcr_is_locked()) arm_slcr_lock();
}

void arm_ps_reset(void) {

    if (arm_slcr_is_locked()) arm_slcr_unlock();

    while(1) {
        *PSS_RST_CTRL = 1;
        arm_instruction_synchronization_barrier();
    }

    /* shouldn't exit from this function */
}


#define ARM_PLL_CTRL (air_uptr_t *)(XPAR_PSU_FPD_SLCR_SECURE_S_AXI_BASEADDR + 0x100)
#define DDR_PLL_CTRL (air_uptr_t *)(XPAR_PSU_FPD_SLCR_SECURE_S_AXI_BASEADDR + 0x104)
#define IO_PLL_CTRL (air_uptr_t *)(XPAR_PSU_FPD_SLCR_SECURE_S_AXI_BASEADDR + 0x108)

void arm_pll_bypass_mode(void) {

    *ARM_PLL_CTRL |= (1 << 4);
    *DDR_PLL_CTRL |= (1 << 4);
    *IO_PLL_CTRL |= (1 << 4);
}

void arm_pll_shutdown(void) {

    *ARM_PLL_CTRL |= (1 << 1);
    *DDR_PLL_CTRL |= (1 << 1);
    *IO_PLL_CTRL |= (1 << 1);
}

#define ARM_CLK_CTRL (air_uptr_t *)(XPAR_PSU_FPD_SLCR_SECURE_S_AXI_BASEADDR + 0x120)

void arm_set_cpu_clock_divisor(air_u32_t divisor) {

    if (divisor < 0) divisor = 0;
    else if (divisor > 0x3f) divisor = 0x3f;

    *ARM_CLK_CTRL |= (divisor << 8);
}
