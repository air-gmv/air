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
 * @author pfnf
 * @brief Implements SPARC atomic operations
 *
 * @note Based on asm-parisc/atomic.h Copyright (C) 2000 Philipp Rumpf
 */

#include <sparc.h>
#include <atomic.h>

/**
 * @brief Number of spin locks to be used by the atomic operations
 */
#define ATOMIC_HASH_SIZE                       (4)

/**
 * @brief Spin lock hash function selector
 */
#define ATOMIC_HASH(a) \
        (&__atomic_hash[(((xky_uptr_t)a) >> 8) & (ATOMIC_HASH_SIZE - 1)])

/**
 * @brief Array of spin locks to be used by the atomic operations
 */
xky_u32_t __atomic_hash[ATOMIC_HASH_SIZE] = { 0 };

/**
 * @brief SPARC atomic addition
 * @param i integer to add
 * @param v atomic value pointer
 * @return modified atomic value
 */
xky_i32_t sparc_atomic_add(xky_i32_t i, atomic_t *v) {

    xky_i32_t ret;
    xky_u32_t isr_level;

    /* enter critical area (disable local preemption and lock) */
    isr_level = sparc_lock_irqsave(ATOMIC_HASH(v));

    v->value += i;
    ret = v->value;

    /* leave critical area (restore local preemption state and unlock) */
    sparc_unlock_irqrestore(ATOMIC_HASH(v), isr_level);
    return ret;
}

/**
 * @brief SPARC atomic swap
 * @param i integer to swap
 * @param v atomic value pointer
 * @return previous atomic value
 */
xky_i32_t sparc_atomic_swap(xky_i32_t i, atomic_t *v) {

    xky_i32_t ret;
    xky_u32_t isr_level;

    /* enter critical area (disable local preemption and lock) */
    isr_level = sparc_lock_irqsave(ATOMIC_HASH(v));

    ret = v->value;
    v->value = i;

    /* leave critical area (restore local preemption state and unlock) */
    sparc_unlock_irqrestore(ATOMIC_HASH(v), isr_level);
    return ret;
}

/**
 * @brief SPARC atomic bitwise AND
 * @param i integer to bitwise AND
 * @param v atomic value pointer
 * @return modified atomic value
 */
xky_i32_t sparc_atomic_and(xky_i32_t i, atomic_t *v) {

    xky_i32_t ret;
    xky_u32_t isr_level;

    /* enter critical area (disable local preemption and lock) */
    isr_level = sparc_lock_irqsave(ATOMIC_HASH(v));

    v->value &= i;
    ret = v->value;

    /* leave critical area (restore local preemption state and unlock) */
    sparc_unlock_irqrestore(ATOMIC_HASH(v), isr_level);
    return ret;
}

/**
 * @brief SPARC atomic bitwise OR
 * @param i integer to bitwise OR
 * @param v atomic value pointer
 * @return modified atomic value
 */
xky_i32_t sparc_atomic_or(xky_i32_t i, atomic_t *v) {

    xky_i32_t ret;
    xky_u32_t isr_level;

    /* enter critical area (disable local preemption and lock) */
    isr_level = sparc_lock_irqsave(ATOMIC_HASH(v));

    v->value |= i;
    ret = v->value;

    /* leave critical area (restore local preemption state and unlock) */
    sparc_unlock_irqrestore(ATOMIC_HASH(v), isr_level);
    return ret;
}

/**
 * @brief SPARC atomic bitwise XOR
 * @param i integer to bitwise XOR
 * @param v atomic value pointer
 * @return modified atomic value
 */
xky_i32_t sparc_atomic_xor(xky_i32_t i, atomic_t *v) {

    xky_i32_t ret;
    xky_u32_t isr_level;

    /* enter critical area (disable local preemption and lock) */
    isr_level = sparc_lock_irqsave(ATOMIC_HASH(v));

    v->value ^= i;
    ret = v->value;

    /* leave critical area (restore local preemption state and unlock) */
    sparc_unlock_irqrestore(ATOMIC_HASH(v), isr_level);
    return ret;
}
