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

void arm_disable_preemption(void) {

    __asm__ volatile("cpsid if\n");
}

void arm_enable_preemption(void) {

    __asm__ volatile("cpsie if\n");
}

void arm_lock(air_uptr_t hash) {

    arm_disable_preemption();

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

void arm_unlock(air_uptr_t hash) {

    *hash = 0;
    arm_enable_preemption();
}
