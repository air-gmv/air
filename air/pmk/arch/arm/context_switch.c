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
 */
void arm_core_context_save(pmk_core_ctrl_t *core) {

    if (core->context->trash) return;

//    pmk_partition_t *partition = core->partition;
}

/**
 * \details
 * The context restore is performed in the exception_irq.S (only one ISF needs
 * to be restored). In here, only the MMU context needs to be switched.
 */
void arm_core_context_restore(pmk_core_ctrl_t *core) {

    __asm__ volatile ("clrex\n");

    if (((pmk_core_ctrl_t *)core)->context->trash) return;

    pmk_partition_t *partition = core->partition;

    if (arm_is_mmu_enabled()) {
        arm_mmu_change_context(partition->mmu_ctrl);
    } else {
        arm_mmu_enable(partition->mmu_ctrl);
    }
}
