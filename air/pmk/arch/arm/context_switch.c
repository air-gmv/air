/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file context_switch.c
 * \author lumm
 * \brief Contains the routines to perform a partition core context save
 *        and partition core context restore on a ARMv7
 *
 * The context save/restore is handled in exception_irq.S and schedule.c,
 * and because only one ISF needs to be saved, the only thing happening here
 * is the save/restore of additional fields besides the general-purpose
 * registers, such as the MMU and GIC registers.
 */

#include <mmu.h>
#include <workspace.h>

/**
 * \details
 * The context save is performed in the exception_irq.S (only one ISF needs
 * to be saved). The change is then handled by the pmk_partition_scheduler(),
 * so nothing really happens here.
 *
 * \note the core context shouldn't be trashed at this point
 */
void arm_core_context_save(pmk_core_ctrl_t *core) {

    void **isf = &(core->partition->context->isf_pointer);
    register air_u32_t r1, r2;

    __asm__ volatile (
            "mov %0, #18\n"
            "bfi %1, %0, #0, #5\n"
            "mrs %0, cpsr\n"
            "msr cpsr_c, %1\n"
            "str sp, [%2]\n"
            "msr cpsr_c, %0"
            : "=&r" (r1), "=&r" (r2)
            : "r" (isf)
            : "memory");

    arm_disable_fpu();
}

/**
 * \details
 * The rest of the context restore is performed in exception.S (only the IRQ sp needs to be set
 * here). The MMU context also needs to be switched.
 */
void arm_core_context_restore(pmk_core_ctrl_t *core) {

    void *isf = core->context->isf_pointer;
    register air_u32_t r1, r2;

    __asm__ volatile (
            "mov %0, #18\n"
            "bfi %1, %0, #0, #5\n"
            "mrs %0, cpsr\n"
            "msr cpsr_c, %1\n"
            "mov sp, %2\n"
            "msr cpsr_c, %0"
            : "=&r" (r1), "=&r" (r2)
            : "r" (isf));

    pmk_partition_t *partition = core->partition;

    if (partition != NULL) {

        if (arm_is_mmu_enabled()) {

            arm_mmu_change_context(partition->mmu_ctrl);
        } else {

            arm_mmu_enable(partition->mmu_ctrl);
        }
    }

    __asm__ volatile ("clrex");
}
