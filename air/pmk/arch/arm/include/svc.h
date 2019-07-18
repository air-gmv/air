/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file svc.h
 * @author lumm
 * @brief SVC
 */

#ifndef ARM_SVC_H
#define ARM_SVC_H

#include <air_arch.h>
#include <armv7.h>
#include <mmu.h>
#include <syscall.h>

/**************************** syscalls declarations ***************************/
void arm_syscall_disable_interrupts(void);
void arm_syscall_enable_interrupts(void);
void arm_syscall_disable_traps(void);
void arm_syscall_enable_traps(void);
void arm_syscall_disable_fpu(void);
void arm_syscall_enable_fpu(void);
air_u32_t arm_syscall_get_tbr(void);
void arm_syscall_set_tbr(air_u32_t val);
air_u32_t arm_syscall_get_psr(void);
void arm_syscall_set_psr(air_u32_t val);
//air_u32_t AIR_SYSCALL_ARM_RETT
//air_u32_t arm_syscall_get_cache_register(void);
//air_u32_t arm_syscall_set_cache_register(void);
//air_u32_t AIR_SYSCALL_ARM_RESTORE_CACHE_REGISTER
air_u32_t arm_syscall_get_irq_mask_register(void);
void arm_syscall_set_irq_mask_register(air_u32_t val);
//air_u32_t arm_syscall_set_irq_force_register(void);

air_u32_t arm_syscall_get_core_id(void);

/**************************** function declarations ***************************/
void arm_svc_handler(air_u32_t svc_id, arm_supervisor_stack_frame_t *frame,
        pmk_core_ctrl_t *core);

#endif /* ARM_SVC_H */
