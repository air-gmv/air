/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file atomic.h
 * @author pfnf
 * @brief Atomic type and atomic operations definitions
 *
 * @note Based on asm-parisc/atomic.h Copyright (C) 2000 Philipp Rumpf
 */

#ifndef __ATOMIC_H__
#define __ATOMIC_H__

#include <air_arch.h>

/**
 * @brief SPARC atomic integer
 */
typedef struct { volatile int value; } atomic_t;

extern air_i32_t sparc_atomic_add(air_i32_t i, atomic_t *v);
extern air_i32_t sparc_atomic_swap(air_i32_t i, atomic_t *v);
extern air_i32_t sparc_atomic_or(air_i32_t i, atomic_t *v);
extern air_i32_t sparc_atomic_and(air_i32_t i, atomic_t *v);
extern air_i32_t sparc_atomic_xor(air_i32_t i, atomic_t *v);

#define atomic_set(i, v)                ((void)sparc_atomic_swap((int)(i), (v)))
#define atomic_swap(i, v)               (sparc_atomic_swap((int)(i), (v)))
#define atomic_fetch(v)                 ((v)->value)

#define atomic_add(i, v)                ((void)sparc_atomic_add( (int)(i), (v)))
#define atomic_sub(i, v)                ((void)sparc_atomic_add(-(int)(i), (v)))
#define atomic_inc(v)                   ((void)sparc_atomic_add(        1, (v)))
#define atomic_dec(v)                   ((void)sparc_atomic_add(       -1, (v)))

#define atomic_or(i, v)                 ((void)sparc_atomic_or((int)(i), (v)))
#define atomic_and(i, v)                ((void)sparc_atomic_and((int)(i), (v)))
#define atomic_xor(i, v)                ((void)sparc_atomic_xor((int)(i), (v)))

#define atomic_fetch_add(i, v)          (sparc_atomic_add( (int)(i), (v)))
#define atomic_fetch_sub(i, v)          (sparc_atomic_add(-(int)(i), (v)))
#define atomic_fetch_inc(v)             (sparc_atomic_add(        1, (v)))
#define atomic_fetch_dec(v)             (sparc_atomic_add(       -1, (v)))

#define atomic_fetch_or(i, v)           (sparc_atomic_or((int)(i), (v)))
#define atomic_fetch_and(i, v)          (sparc_atomic_and((int)(i), (v)))
#define atomic_fetch_xor(i, v)          (sparc_atomic_xor((int)(i), (v)))

#endif /* __ATOMIC_H__ */
