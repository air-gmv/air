/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file exception_handler.c
 * @author lumm
 * @brief exception_handlers and default
 */

#include <air_arch.h>
#include <armv7.h>
#include <gic.h>
#include <isr.h>

extern air_uptr_t arm_isr_handler_table[MAX_INT];

typedef void (*interrupt_handler)(arm_exception_frame_t *frame);

void arm_irq_handler(arm_exception_frame_t *frame) {

    /* Acknowledge Interrupt */
    air_u32_t ack = acknowledge_int();

    air_u16_t id = (ack & 0x3ff);
    air_u8_t cpu = ((ack << 10U) & 0x7);

    /* Spurious interrupt */
    if(id == 1023) {
        return;
    }

    if (arm_isr_handler_table[id] != (void *)NULL) {
        ((interrupt_handler)arm_isr_handler_table[id])(frame);
    }

    return;
}

