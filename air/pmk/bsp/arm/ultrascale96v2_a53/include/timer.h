/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file timer.h
 * \author ansi
 * \brief Generic Timer/Triple Timer definitions
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <armv7.h>

#define ARM_TIMER_ID ARM_A53_IRQ_SPT

/**
 * \brief Generic Timer
 */

typedef struct {
    volatile air_u32_t cnt_ctrl;
    volatile air_u32_t cnt_status;
    volatile air_u32_t current_cnt_lower;
    volatile air_u32_t current_cnt_upper;
    volatile air_u32_t reserved[4];
    volatile air_u32_t base_freq_id;
} iou_scntrs_module;

void arm_init_global_timer(void);

void arm_start_global_timer(void);

void arm_disable_global_timer(void);

air_u64_t arm_read_global_timer(void);

void arm_setup_interprocessor_irq(air_u32_t cpu_id);

void arm_acknowledge_gt(void);

air_u32_t arm_determine_gt_prescaler(air_u32_t us_per_tick);

#endif /* TIMER_H_ */