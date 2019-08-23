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
 */

#include <atomic.h>

/**
 * @brief Number of spin locks to be used by the atomic operations
 */
#define ATOMIC_HASH_SIZE                       (4)

/**
 * @brief Spin lock hash function selector
 */
#define ATOMIC_HASH(a) \
        (&__atomic_hash[(((air_u32_t)a) >> 8) & (ATOMIC_HASH_SIZE - 1)])

/**
 * @brief Array of spin locks to be used by the atomic operations
 */
air_u32_t __atomic_hash[ATOMIC_HASH_SIZE] = { 0 };


air_i32_t arm_atomic_add(air_i32_t i, atomic_t *v) {

    air_i32_t ret;
    air_u32_t pil = arm_lock(ATOMIC_HASH(v));
    v->value += i;
    ret = v->value;
    arm_unlock(ATOMIC_HASH(v), pil);

    return ret;
}

air_i32_t arm_atomic_swap(air_i32_t i, atomic_t *v) {

    air_i32_t ret;

    air_u32_t pil = arm_lock(ATOMIC_HASH(v));
    ret = v->value;
    v->value = i;
    arm_unlock(ATOMIC_HASH(v), pil);

    return ret;
}

air_i32_t arm_atomic_and(air_i32_t i, atomic_t *v) {

    air_i32_t ret;

    air_u32_t pil = arm_lock(ATOMIC_HASH(v));
    v->value &= i;
    ret = v->value;
    arm_unlock(ATOMIC_HASH(v), pil);

    return ret;
}

air_i32_t arm_atomic_or(air_i32_t i, atomic_t *v) {

    air_i32_t ret;

    air_u32_t pil = arm_lock(ATOMIC_HASH(v));
    v->value |= i;
    ret = v->value;
    arm_unlock(ATOMIC_HASH(v), pil);

    return ret;
}

air_i32_t arm_atomic_xor(air_i32_t i, atomic_t *v) {

    air_i32_t ret;

    air_u32_t pil = arm_lock(ATOMIC_HASH(v));
    v->value ^= i;
    ret = v->value;
    arm_unlock(ATOMIC_HASH(v), pil);

    return ret;
}
