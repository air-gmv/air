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

// /**
//  * \brief Triple Timer Counter
//  */
// #define ARM_TTC_CLK_CTRL_PS_EN          (1U << 0)
// #define ARM_TTC_CLK_CTRL_PS(val)        ((val & 0xf) << 1)
// #define ARM_TTC_CLK_CTRL_SRC            (1U << 5)
// #define ARM_TTC_CLK_CTRL_EXT_EDGE       (1U << 6)

// #define ARM_TTC_CNT_CTRL_DIS            (1U << 0)
// #define ARM_TTC_CNT_CTRL_INTV           (1U << 1)
// #define ARM_TTC_CNT_CTRL_DEC            (1U << 2)
// #define ARM_TTC_CNT_CTRL_MTC            (1U << 3)
// #define ARM_TTC_CNT_CTRL_RST            (1U << 4)
// #define ARM_TTC_CNT_CTRL_EN_WAVE        (1U << 5)
// #define ARM_TTC_CNT_CTRL_POL_WAVE       (1U << 6)

// #define ARM_TTC_INTV_CNT(val)           ((val & 0xffff) << 0)
// #define ARM_TTC_MTC_CNT(val)            ((val & 0xffff) << 0)

// #define ARM_TTC_INT_EN_INTV             (1U << 0)
// #define ARM_TTC_INT_EN_MTC1             (1U << 1)
// #define ARM_TTC_INT_EN_MTC2             (1U << 2)
// #define ARM_TTC_INT_EN_MTC3             (1U << 3)
// #define ARM_TTC_INT_EN_OVR              (1U << 4)
// #define ARM_TTC_INT_EN_E                (1U << 5)


// typedef struct {
//     volatile air_u32_t clk_ctrl_1;
//     volatile air_u32_t clk_ctrl_2;
//     volatile air_u32_t clk_ctrl_3;
//     volatile air_u32_t cnt_ctrl_1;
//     volatile air_u32_t cnt_ctrl_2;
//     volatile air_u32_t cnt_ctrl_3;
//     volatile air_u32_t cnt_val_1;
//     volatile air_u32_t cnt_val_2;
//     volatile air_u32_t cnt_val_3;
//     volatile air_u32_t intv_cnt_1;
//     volatile air_u32_t intv_cnt_2;
//     volatile air_u32_t intv_cnt_3;
//     volatile air_u32_t mtc_1_cnt_1;
//     volatile air_u32_t mtc_1_cnt_2;
//     volatile air_u32_t mtc_1_cnt_3;
//     volatile air_u32_t mtc_2_cnt_1;
//     volatile air_u32_t mtc_2_cnt_2;
//     volatile air_u32_t mtc_2_cnt_3;
//     volatile air_u32_t mtc_3_cnt_1;
//     volatile air_u32_t mtc_3_cnt_2;
//     volatile air_u32_t mtc_3_cnt_3;
//     volatile air_u32_t int_cnt_1;
//     volatile air_u32_t int_cnt_2;
//     volatile air_u32_t int_cnt_3;
//     volatile air_u32_t int_en_1;
//     volatile air_u32_t int_en_2;
//     volatile air_u32_t int_en_3;
//     volatile air_u32_t e_ctrl_timer_1;
//     volatile air_u32_t e_ctrl_timer_2;
//     volatile air_u32_t e_ctrl_timer_3;
//     volatile air_u32_t e_cnt_1;
//     volatile air_u32_t e_cnt_2;
//     volatile air_u32_t e_cnt_3;
// } triple_timer_cnt_t;

// void arm_init_ttc(air_u32_t timer_id);
// void arm_start_ttc(air_u32_t timer_id);
// air_u32_t arm_read_ttc(void);
// air_u32_t arm_acknowledge_ttc(void);

#endif /* TIMER_H_ */