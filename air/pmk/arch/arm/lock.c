/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file lock.c
 * \author lumm
 * \brief lock operations
 *
 * TODO do this right. possibly PIL has to be used
 */

#include <lock.h>

air_u32_t arm_lock(air_uptr_t *hash) {

    air_u32_t pil = arm_save_preemption();

    air_u32_t token, lock;

    __asm__ volatile(
            "mov %1, #0x1\n"
            "try:\n"
            "ldrex %0, [%[hash]]\n"
            "cmp %0, %1\n"
            "it eq\n"
            "beq try\n"
            "strex %0, %1, [%[hash]]\n"
            "cmp %0, #0\n"
            "it ne\n"
            "bne try\n"
            : "=&r" (lock), "=&r" (token)
            : [hash] "r" (hash)
            : "memory");
    arm_data_memory_barrier();

    return pil;
}

air_u32_t arm_unlock(air_uptr_t *hash, air_u32_t pil) {

    arm_data_memory_barrier();
    *hash = 0;
    arm_restore_preemption(pil);
    return 0;
}

air_u32_t arm_save_preemption(void) {

    air_u32_t psr = arm_get_cpsr();
    air_u32_t pil = psr & ARM_PSR_I;

    if ((psr & ARM_PSR_MODE_MASK) != ARM_PSR_USR) {

        if (!pil) {
            arm_disable_interrupts();
        }
    }

    return pil;
}

void arm_restore_preemption(air_u32_t pil) {

    air_u32_t psr = arm_get_cpsr();

    if ((psr & ARM_PSR_MODE_MASK) != ARM_PSR_USR) {

        if (!pil && (psr & ARM_PSR_I)) {
            arm_enable_interrupts();
        }
    }
}
