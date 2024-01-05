/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file smp.c
 * @author lumm
 * @brief SMP support
 */

#include <air.h>
#include <armv7.h>
#include <bare.h>
/**
 * @brief Core exception ISR tables
 */
volatile air_uptr_t *isr_table[PMK_MAX_CORES];
volatile air_u32_t __isr_table[PMK_MAX_CORES * ARM_IRQ_COUNT];

void wake_after(air_clocktick_t ticks) {
    if (ticks <= 0) return;

    air_clocktick_t it = air_syscall_get_elapsed_ticks();
    while (1) {
        air_clocktick_t ft = air_syscall_get_elapsed_ticks();
        if ((ft - it) >= ticks) break;
    }
}

void arm_pos_smp_init(void) {

    air_u32_t i;

    for (i = 0; i < PMK_MAX_CORES; ++i) {

        isr_table[i] = &__isr_table[i*ARM_IRQ_COUNT];
    }

    return;
}
