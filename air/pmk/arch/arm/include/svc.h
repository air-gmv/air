/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file svc.h
 * \author lumm
 * \brief Supervisor Calls definitions
 */

#ifndef SVC_H_
#define SVC_H_

#include <armv7.h>
#include <workspace.h>

/**************************** syscalls declarations ***************************/
air_u32_t arm_syscall_disable_interrupts(pmk_core_ctrl_t *core);
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
air_u32_t arm_syscall_get_irq_mask_register(pmk_core_ctrl_t *core);
void arm_syscall_set_irq_mask_register(pmk_core_ctrl_t *core, air_u32_t val);
//air_u32_t arm_syscall_set_irq_force_register(void);

air_u32_t arm_syscall_get_core_id(pmk_core_ctrl_t *core);
air_u64_t arm_syscall_get_elapsed_ticks(pmk_core_ctrl_t *core);

/*trcpse*/
air_u32_t arm_syscall_acknowledge_int(pmk_core_ctrl_t *core);
/*----*/

/**************************** function declarations ***************************/
void arm_svc_handler(arm_interrupt_stack_frame_t *frame, pmk_core_ctrl_t *core);

#endif /* SVC_H_ */
