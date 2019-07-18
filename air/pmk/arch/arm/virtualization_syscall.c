/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013 - 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file
 * @author pfnf
 * @brief ARM virtualization system calls (PMK side)
 */

#include <air_arch.h>
#include <armv7.h>
#include <svc.h>
#include <gic.h>

extern pmk_sharedarea_t air_shared_area;

void arm_syscall_disable_interrupts(void) {

    arm_disable_interrupts();
}
void arm_syscall_enable_interrupts(void) {

    arm_enable_interrupts();
}

void arm_syscall_disable_traps(void) {

    arm_disable_interrupts();
}

void arm_syscall_enable_traps(void) {

    arm_enable_interrupts();
}

void arm_syscall_disable_fpu(void) {
#ifdef PMK_FPU_SUPPORT
    arm_disable_fpu();
#endif
}

void arm_syscall_enable_fpu(void) {
#ifdef PMK_FPU_SUPPORT
    arm_enable_fpu();
#endif
}

air_u32_t arm_syscall_get_tbr(void) {
    return arm_get_exception_base_address();
}

void arm_syscall_set_tbr(air_u32_t val) {
    arm_set_exception_base_address(val);
}

air_u32_t arm_syscall_get_psr(void) {
    return arm_get_cpsr();
}
void arm_syscall_set_psr(air_u32_t val) {
    arm_set_cpsr(val);
}

//air_u32_t AIR_SYSCALL_ARM_RETT
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

air_u32_t arm_syscall_get_core_id(void) {
    ARM_SWITCH_REGISTERS;
    air_u32_t core_id;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "mrc p15, 0, %[core_id], c0, c0, 5\n"
        ARM_SWITCH_BACK
        : [core_id] "=&r" (core_id) ARM_SWITCH_ADDITIONAL_OUTPUT
    );

    pmk_core_ctrl_t core = air_shared_area.core[core_id];

    return core.context->vcpu.id;
}
