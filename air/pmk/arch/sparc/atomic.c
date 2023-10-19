/*
 * Copyright (C) 2000 Philipp Rumpf
 * Copyright (C) 2013-2014  GMVIS Skysoft S.A.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/**
 * @file atomic.c
 * @author pfnf
 * @brief Implements SPARC atomic operations
 */

#include <atomic.h>
#include <sparc.h>

/**
 * @brief Number of spin locks to be used by the atomic operations
 */
#define ATOMIC_HASH_SIZE (4)

/**
 * @brief Spin lock hash function selector
 */
#define ATOMIC_HASH(a) (&__atomic_hash[(((air_uptr_t)a) >> 8) & (ATOMIC_HASH_SIZE - 1)])

/**
 * @brief Array of spin locks to be used by the atomic operations
 */
air_u32_t __atomic_hash[ATOMIC_HASH_SIZE] = {0};

/**
 * @brief SPARC atomic addition
 * @param i integer to add
 * @param v atomic value pointer
 * @return modified atomic value
 */
air_i32_t sparc_atomic_add(air_i32_t i, atomic_t *v)
{

    air_i32_t ret;
    air_u32_t isr_level;

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
air_i32_t sparc_atomic_swap(air_i32_t i, atomic_t *v)
{

    air_i32_t ret;
    air_u32_t isr_level;

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
air_i32_t sparc_atomic_and(air_i32_t i, atomic_t *v)
{

    air_i32_t ret;
    air_u32_t isr_level;

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
air_i32_t sparc_atomic_or(air_i32_t i, atomic_t *v)
{

    air_i32_t ret;
    air_u32_t isr_level;

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
air_i32_t sparc_atomic_xor(air_i32_t i, atomic_t *v)
{

    air_i32_t ret;
    air_u32_t isr_level;

    /* enter critical area (disable local preemption and lock) */
    isr_level = sparc_lock_irqsave(ATOMIC_HASH(v));

    v->value ^= i;
    ret = v->value;

    /* leave critical area (restore local preemption state and unlock) */
    sparc_unlock_irqrestore(ATOMIC_HASH(v), isr_level);
    return ret;
}
