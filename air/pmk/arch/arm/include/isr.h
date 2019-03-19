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

void arm_hm_init(void);
air_uptr_t arm_hm_handler(air_u32_t id, air_u32_t fsr, air_u32_t far, air_u32_t *instr);
air_uptr_t arm_partition_hm_handler(air_u32_t id, pmk_core_ctrl_t *core);

void arm_isr_table_init(void);
air_uptr_t arm_isr_handler(
        arm_interrupt_stack_frame_t *frame,
        pmk_core_ctrl_t *core);
air_uptr_t arm_partition_isr_handler(air_u32_t id, pmk_core_ctrl_t *core);
void arm_isr_default_handler(arm_interrupt_stack_frame_t *frame,
        pmk_core_ctrl_t *core);

air_uptr_t arm_isr_install_handler(air_u32_t vector, void *handler);

void arm_restore_fpu(arm_vfp_context_t *vfp_context);

#endif /* ARM_ISR_H */
