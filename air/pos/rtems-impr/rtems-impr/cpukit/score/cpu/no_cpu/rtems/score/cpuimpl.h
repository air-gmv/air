/**
 * @file
 *
 * @brief CPU Port Implementation API
 */

/*
 * Copyright (c) 2013, 2016 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPUIMPL_H
#define _RTEMS_SCORE_CPUIMPL_H

#include <rtems/score/cpu.h>

/**
 * @brief The size of the CPU specific per-CPU control.
 *
 * This define must be visible to assember files since it is used to derive
 * structure offsets.
 */
#define CPU_PER_CPU_CONTROL_SIZE 0

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The CPU specific per-CPU control.
 *
 * The CPU port can place here all state information that must be available and
 * maintained for each processor in the system.  This structure must contain at
 * least one field for C/C++ compatibility.  In GNU C empty structures have a
 * size of zero.  In C++ structures have a non-zero size.  In case
 * CPU_PER_CPU_CONTROL_SIZE is defined to zero, then this structure is not
 * used.
 */
typedef struct {
  /* CPU specific per-CPU state */
} CPU_Per_CPU_control;

/**
 * @brief Special register pointing to the per-CPU control of the current
 * processor.
 *
 * This is optional.  Not every CPU port needs this.  It is only an optional
 * optimization variant.
 */
register struct Per_CPU_Control *_CPU_Per_CPU_current asm( "rX" );

/**
 * @brief Optional method to obtain the per-CPU control of the current processor.
 *
 * This is optional.  Not every CPU port needs this.  It is only an optional
 * optimization variant.  In case this macro is undefined, the default
 * implementation using the current processor index will be used.
 */
#define _CPU_Get_current_per_CPU_control() ( _CPU_Per_CPU_current )

/**
 * @brief Optional method to get the executing thread.
 *
 * This is optional.  Not every CPU port needs this.  It is only an optional
 * optimization variant.  In case this macro is undefined, the default
 * implementation uses the per-CPU information and the current processor index
 * to get the executing thread.
 */
#define _CPU_Get_thread_executing() ( _CPU_Per_CPU_current->executing )

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* _RTEMS_SCORE_CPUIMPL_H */
