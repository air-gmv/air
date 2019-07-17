/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file timer.h
 * @author lumm
 * @brief Timers defs
 */

#ifndef ARM_TIMER_H
#define ARM_TIMER_H

#include <air_arch.h>

#define GT_BASE_MEMORY                  0xf8f00200

#define ARM_GT_CTRL_TIMER_EN            (1U << 0)
#define ARM_GT_CTRL_COMP_EN             (1U << 1)
#define ARM_GT_CTRL_IRQ_EN              (1U << 2)
#define ARM_GT_CTRL_AUTOINC_EN          (1U << 3)
#define ARM_GT_CTRL_PRESCALER(value) \
        ((value << 8) ? value <= 255 : (1U << 8))

typedef struct {
    air_u32_t counter_lower;
    air_u32_t counter_upper;
    air_u32_t ctrl;
    air_u32_t irq_status;
    air_u32_t comparator_lower;
    air_u32_t comparator_upper;
    air_u32_t autoinc;
} global_timer_t;

void arm_init_global_timer(void);
void arm_start_global_timer(void);
void arm_setup_interprocessor_irq(air_u32_t cpu_id);

#endif /* ARM_TIMER_H */
