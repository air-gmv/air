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

/// @brief Wait for @param clock ticks 
/// @param ticks_to_wait 
void wake_after(air_clocktick_t ticks_to_wait) {
    air_clocktick_t now = air_syscall_get_elapsed_ticks();
    air_clocktick_t deadline = now + ticks_to_wait ;
    while (now < deadline) {
        now = air_syscall_get_elapsed_ticks();
    }
}

/// @brief Active wait time. 
/// @param delay_seconds 
void delay(float delay_seconds){
	// Time constants
	int tps = 1000000 / air_syscall_get_us_per_tick(); // 1 sec
    air_u64_t ticks_to_wait = (air_u64_t) (delay_seconds *tps)   ;
    wake_after(ticks_to_wait); 
}

void arm_pos_smp_init(void) {

    air_u32_t i;

    for (i = 0; i < PMK_MAX_CORES; ++i) {

        isr_table[i] = &__isr_table[i*ARM_IRQ_COUNT];
    }

    return;
}
