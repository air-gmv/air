/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file timer.h
 * @author lumm
 * @brief Timers defs
 */

#ifndef ARM_TIMER_H
#define ARM_TIMER_H

#include <air_arch.h>

/******************************** Global Timer *******************************/
#define GT_BASE_MEMORY                  0xf8f00200

#define ARM_GT_CTRL_TIMER_EN            (1U << 0)
#define ARM_GT_CTRL_COMP_EN             (1U << 1)
#define ARM_GT_CTRL_IRQ_EN              (1U << 2)
#define ARM_GT_CTRL_AUTOINC_EN          (1U << 3)
#define ARM_GT_CTRL_PRESCALER(value) \
        ((value <= 255) ? value << 8 : (255U << 8))


typedef struct {
    air_u32_t cnt_lower;
    air_u32_t cnt_upper;
    air_u32_t ctrl;
    air_u32_t irq_status;
    air_u32_t comp_lower;
    air_u32_t comp_upper;
    air_u32_t autoinc;
} global_timer_t;

void arm_init_global_timer(void);
void arm_start_global_timer(void);
air_u64_t arm_read_global_timer(void);
air_u32_t arm_acknowledge_ttc(void);
void arm_setup_interprocessor_irq(air_u32_t cpu_id);
void arm_acknowledge_gt(void);
air_u32_t arm_determine_gt_prescaler(air_u32_t us_per_tick);

/**************************** Triple Timer Counter ***************************/
#define TTC0_BASE_MEMORY                0xf8001000
#define TTC1_BASE_MEMORY                0xf8002000

#define ARM_TTC_CLK_CTRL_PS_EN          (1U << 0)
#define ARM_TTC_CLK_CTRL_PS(val)        ((val & 0xf) << 1)
#define ARM_TTC_CLK_CTRL_SRC            (1U << 5)
#define ARM_TTC_CLK_CTRL_EXT_EDGE       (1U << 6)

#define ARM_TTC_CNT_CTRL_DIS            (1U << 0)
#define ARM_TTC_CNT_CTRL_INTV           (1U << 1)
#define ARM_TTC_CNT_CTRL_DEC            (1U << 2)
#define ARM_TTC_CNT_CTRL_MTC            (1U << 3)
#define ARM_TTC_CNT_CTRL_RST            (1U << 4)
#define ARM_TTC_CNT_CTRL_EN_WAVE        (1U << 5)
#define ARM_TTC_CNT_CTRL_POL_WAVE       (1U << 6)

#define ARM_TTC_INTV_CNT(val)           ((val & 0xffff) << 0)
#define ARM_TTC_MTC_CNT(val)            ((val & 0xffff) << 0)

#define ARM_TTC_INT_EN_INTV             (1U << 0)
#define ARM_TTC_INT_EN_MTC1             (1U << 1)
#define ARM_TTC_INT_EN_MTC2             (1U << 2)
#define ARM_TTC_INT_EN_MTC3             (1U << 3)
#define ARM_TTC_INT_EN_OVR              (1U << 4)
#define ARM_TTC_INT_EN_E                (1U << 5)


typedef struct {
    air_u32_t clk_ctrl_1;
    air_u32_t clk_ctrl_2;
    air_u32_t clk_ctrl_3;
    air_u32_t cnt_ctrl_1;
    air_u32_t cnt_ctrl_2;
    air_u32_t cnt_ctrl_3;
    air_u32_t cnt_val_1;
    air_u32_t cnt_val_2;
    air_u32_t cnt_val_3;
    air_u32_t intv_cnt_1;
    air_u32_t intv_cnt_2;
    air_u32_t intv_cnt_3;
    air_u32_t mtc_1_cnt_1;
    air_u32_t mtc_1_cnt_2;
    air_u32_t mtc_1_cnt_3;
    air_u32_t mtc_2_cnt_1;
    air_u32_t mtc_2_cnt_2;
    air_u32_t mtc_2_cnt_3;
    air_u32_t mtc_3_cnt_1;
    air_u32_t mtc_3_cnt_2;
    air_u32_t mtc_3_cnt_3;
    air_u32_t int_cnt_1;
    air_u32_t int_cnt_2;
    air_u32_t int_cnt_3;
    air_u32_t int_en_1;
    air_u32_t int_en_2;
    air_u32_t int_en_3;
    air_u32_t e_ctrl_timer_1;
    air_u32_t e_ctrl_timer_2;
    air_u32_t e_ctrl_timer_3;
    air_u32_t e_cnt_1;
    air_u32_t e_cnt_2;
    air_u32_t e_cnt_3;
} triple_timer_cnt_t;

void arm_init_ttc(air_u32_t timer_id);
void arm_start_ttc(air_u32_t timer_id);
air_u32_t arm_read_ttc(void);

#endif /* ARM_TIMER_H */
