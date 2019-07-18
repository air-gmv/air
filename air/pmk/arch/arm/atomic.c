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
 * TODO do this right
 */

#include <atomic.h>
#include <gic.h>

air_i32_t arm_atomic_add(air_i32_t i, atomic_t *v) {

    air_i32_t ret;
    arm_disable_interrupts();
    v->value += i;
    ret = v->value;
    arm_enable_interrupts();

    return ret;
}

air_i32_t arm_atomic_swap(air_i32_t i, atomic_t *v) {

    air_i32_t ret;

    arm_disable_interrupts();
    ret = v->value;
    v->value = i;
    arm_enable_interrupts();

    return ret;
}

air_i32_t arm_atomic_and(air_i32_t i, atomic_t *v) {

    air_i32_t ret;

    arm_disable_interrupts();
    v->value &= i;
    ret = v->value;
    arm_enable_interrupts();

    return ret;
}

air_i32_t arm_atomic_or(air_i32_t i, atomic_t *v) {

    air_i32_t ret;

    arm_disable_interrupts();
    v->value |= i;
    ret = v->value;
    arm_enable_interrupts();

    return ret;
}

air_i32_t arm_atomic_xor(air_i32_t i, atomic_t *v) {

    air_i32_t ret;

    arm_disable_interrupts();
    v->value ^= i;
    ret = v->value;
    arm_enable_interrupts();

    return ret;
}
