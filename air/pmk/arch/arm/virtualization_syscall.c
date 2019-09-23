/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file virtualization_syscall.c
 * @author lumm
 * @brief ARM virtualization system calls (PMK side)
 */

#include <air_arch.h>
#include <armv7.h>
#include <svc.h>
#include <gic.h>
#include <workspace.h>

extern pmk_sharedarea_t air_shared_area;

void arm_syscall_disable_interrupts(pmk_core_ctrl_t *core) {

    core->context->vcpu.psr |= ARM_PSR_I;
}
void arm_syscall_enable_interrupts(pmk_core_ctrl_t *core) {

    core->context->vcpu.psr &= ~ARM_PSR_I;
}

void arm_syscall_disable_traps(pmk_core_ctrl_t *core) {

    core->context->vcpu.psr |= (ARM_PSR_A | ARM_PSR_I);
}

void arm_syscall_enable_traps(pmk_core_ctrl_t *core) {

    core->context->vcpu.psr &= ~(ARM_PSR_A | ARM_PSR_I);
}

void arm_syscall_disable_fpu(pmk_core_ctrl_t *core) {

    arm_interrupt_stack_frame_t *isf = (arm_interrupt_stack_frame_t *)core->context->isf_pointer;
    isf->vfp_context->fpexc &= ~ARM_VFP_FPEXC_ENABLE;
}

void arm_syscall_enable_fpu(pmk_core_ctrl_t *core) {

    arm_interrupt_stack_frame_t *isf = (arm_interrupt_stack_frame_t *)core->context->isf_pointer;
    isf->vfp_context->fpexc |= ARM_VFP_FPEXC_ENABLE;
}

air_u32_t arm_syscall_get_tbr(pmk_core_ctrl_t *core) {

    air_uptr_t vbar = core->context->vcpu.vbar;
    return (air_u32_t)vbar;
}

void arm_syscall_set_tbr(pmk_core_ctrl_t *core, void *val) {

    core->context->vcpu.vbar = (air_uptr_t)val;
}

air_u32_t arm_syscall_get_psr(pmk_core_ctrl_t *core) {

    air_u32_t psr = core->context->vcpu.psr;
    return psr;
}

void arm_syscall_set_psr(pmk_core_ctrl_t *core, air_u32_t val) {

    core->context->vcpu.psr = val;
}

void arm_syscall_rett(pmk_core_ctrl_t *core) {

    core->context->vcpu.psr &= ~(ARM_PSR_A | ARM_PSR_I);
//  core->context->state = AIR_STATE_PARTITION_EXEC; // its already in core_context_remove_hm_event
}

//air_u32_t arm_syscall_get_cache_register(void);
//air_u32_t arm_syscall_set_cache_register(void);
//air_u32_t AIR_SYSCALL_ARM_RESTORE_CACHE_REGISTER
air_u32_t arm_syscall_get_irq_mask_register(void) {
    return arm_get_int_mask();
}

void arm_syscall_set_irq_mask_register(air_u32_t val) {
    arm_set_int_mask(val);
}
//air_u32_t arm_syscall_set_irq_force_register(void);

air_u32_t arm_syscall_get_core_id(pmk_core_ctrl_t *core) {

    return core->context->vcpu.id;
}

air_u64_t arm_syscall_get_elapsed_ticks(pmk_core_ctrl_t *core) {

    return core->partition->elapsed_ticks;
}
