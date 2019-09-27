/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/AIR/raw/master/air/LICENSE
 */
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
#include <configurations.h>

/**************************** syscalls declarations ***************************/
void arm_syscall_disable_interrupts(pmk_core_ctrl_t *core);
void arm_syscall_enable_interrupts(pmk_core_ctrl_t *core);
void arm_syscall_disable_traps(pmk_core_ctrl_t *core);
void arm_syscall_enable_traps(pmk_core_ctrl_t *core);
void arm_syscall_disable_fpu(pmk_core_ctrl_t *core);
void arm_syscall_enable_fpu(pmk_core_ctrl_t *core);
air_u32_t arm_syscall_get_tbr(pmk_core_ctrl_t *core);
void arm_syscall_set_tbr(pmk_core_ctrl_t *core, void *val);
air_u32_t arm_syscall_get_psr(pmk_core_ctrl_t *core);
void arm_syscall_set_psr(pmk_core_ctrl_t *core, air_u32_t val);
void arm_syscall_rett(pmk_core_ctrl_t *core);
//air_u32_t arm_syscall_get_cache_register(void);
//air_u32_t arm_syscall_set_cache_register(void);
//air_u32_t AIR_SYSCALL_ARM_RESTORE_CACHE_REGISTER
air_u32_t arm_syscall_get_irq_mask_register(void);
void arm_syscall_set_irq_mask_register(air_u32_t val);
//air_u32_t arm_syscall_set_irq_force_register(void);

air_u32_t arm_syscall_get_core_id(pmk_core_ctrl_t *core);
air_u64_t arm_syscall_get_elapsed_ticks(pmk_core_ctrl_t *core);

/**************************** function declarations ***************************/
void arm_svc_handler(air_u32_t svc_id, pmk_core_ctrl_t *core, arm_supervisor_stack_frame_t *frame);

#endif /* ARM_SVC_H */
