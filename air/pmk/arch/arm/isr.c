/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file isr.c
 * @author lumm
 * @brief Interrupt Service Routines (ISR)
 */

#include <air_arch.h>
#include <armv7.h>
#include <gic.h>
#include <isr.h>

air_uptr_t arm_isr_handler_table[MAX_INT];

void arm_irq_default_handler(arm_exception_frame_t *frame) {
    return;
}

void arm_irq_table_initialize(void) {

    air_u32_t i = 0;

    for (air_u32_t i = 0; i < get_int_count(); ++i) {
        arm_interrupt_handler_install(i, arm_irq_default_handler);
    }
}

air_uptr_t arm_irq_handler_install(air_u32_t vector, void *handler) {

    air_uptr_t old_handler = arm_isr_handler_table[vector];
    arm_isr_handler_table[vector] = handler;
    return old_handler;
}
