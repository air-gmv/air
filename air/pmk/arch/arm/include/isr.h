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

void arm_irq_default_handler(arm_exception_frame_t *frame);
void arm_irq_table_initialize(void);
void arm_*irq_handler_install(air_u32_t vector, void *handler);
void arm_irq_handler(arm_exception_frame_t *frame);

#endif /* ARM_ISR_H */
