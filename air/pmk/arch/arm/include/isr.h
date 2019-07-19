/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file isr.h
 * @author lumm
 * @brief Interrupt Service Handler
 */

#ifndef ARM_ISR_H
#define ARM_ISR_H

#include <air_arch.h>
#include <armv7.h>
#include <gic.h>
#include <pmk.h>
#include <workspace.h>

void arm_exception_default_handler(void);
void arm_irq_default_handler(arm_interrupt_stack_frame_t *frame,
        pmk_core_ctrl_t *core);
air_uptr_t arm_irq_install_handler(air_u32_t vector, void *handler);
void arm_irq_table_initialize(void);
void arm_irq_handler(arm_interrupt_stack_frame_t *frame,
        pmk_core_ctrl_t* core);

#endif /* ARM_ISR_H */
