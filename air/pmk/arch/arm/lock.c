/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file atomic.c
 * @author lumm
 * @brief Atomic type and atomic operations definitions
 * TODO do this right. possibly PIL has to be used
 */

#include <lock.h>

void arm_lock(air_uptr_t hash) {

//    arm_disable_preemption(0);

    air_u32_t token, lock;

    __asm__ volatile(
            "mov %1, #0x1\n"
            "try:\n"
            "ldrex %0, [%[hash]]\n"
            "cmp %0, #0\n"
            "itte eq\n"
            "strexeq %0, %1, [%[hash]]\n"
            "cmpeq %0, #0\n"
            "bne try\n"
            : "=&r" (lock), "=&r" (token)
            : [hash] "r" (hash)
            : "memory");
}

air_u32_t arm_unlock(air_uptr_t hash) {

    *hash = 0;
//    air_u32_t flag = arm_enable_preemption();
    return 0;
}

void arm_disable_preemption(air_u32_t irq_mask) {

    air_u32_t psr = arm_get_cpsr();

    if ((psr & ARM_PSR_MODE_MASK) != ARM_PSR_USR) {

        if (irq_mask && !(psr & ARM_PSR_I)) {
            arm_disable_interrupts();
        }
    }
}

air_u32_t arm_enable_preemption(void) {

    air_u32_t psr = arm_get_cpsr();
    air_u32_t irq_mask = psr & ARM_PSR_I;

    if ((psr & ARM_PSR_MODE_MASK) != ARM_PSR_USR) {

        if (irq_mask) {
            arm_enable_interrupts();
        }
    }

    return irq_mask;
}
