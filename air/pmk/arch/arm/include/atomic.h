/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/AIR/raw/master/air/LICENSE
 */
/**
 * \file atomic.h
 * \author lumm
 * \brief Atomic type and atomic operations definitions
 */

#ifndef ATOMIC_H_
#define ATOMIC_H_

#include <armv7.h>

/**
 * \brief ARM atomic integer
 */
typedef struct { volatile int value; } atomic_t;

air_i32_t arm_atomic_add(air_i32_t i, atomic_t *v);
air_i32_t arm_atomic_swap(air_i32_t i, atomic_t *v);
air_i32_t arm_atomic_or(air_i32_t i, atomic_t *v);
air_i32_t arm_atomic_and(air_i32_t i, atomic_t *v);
air_i32_t arm_atomic_xor(air_i32_t i, atomic_t *v);

#define atomic_set(i, v)                ((void)arm_atomic_swap((int)(i), (v)))
#define atomic_swap(i, v)               (arm_atomic_swap((int)(i), (v)))
#define atomic_fetch(v)                 ((v)->value)

#define atomic_add(i, v)                ((void)arm_atomic_add( (int)(i), (v)))
#define atomic_sub(i, v)                ((void)arm_atomic_add(-(int)(i), (v)))
#define atomic_inc(v)                   ((void)arm_atomic_add(        1, (v)))
#define atomic_dec(v)                   ((void)arm_atomic_add(       -1, (v)))

#define atomic_or(i, v)                 ((void)arm_atomic_or((int)(i), (v)))
#define atomic_and(i, v)                ((void)arm_atomic_and((int)(i), (v)))
#define atomic_xor(i, v)                ((void)arm_atomic_xor((int)(i), (v)))

#define atomic_fetch_add(i, v)          (arm_atomic_add( (int)(i), (v)))
#define atomic_fetch_sub(i, v)          (arm_atomic_add(-(int)(i), (v)))
#define atomic_fetch_inc(v)             (arm_atomic_add(        1, (v)))
#define atomic_fetch_dec(v)             (arm_atomic_add(       -1, (v)))

#define atomic_fetch_or(i, v)           (arm_atomic_or((int)(i), (v)))
#define atomic_fetch_and(i, v)          (arm_atomic_and((int)(i), (v)))
#define atomic_fetch_xor(i, v)          (arm_atomic_xor((int)(i), (v)))

#endif /* ATOMIC_H_ */
