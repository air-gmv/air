/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file tripe_timer.c
 * \author lumm
 * \brief Triple Timer Counters (TTC)
 */

#include <timer.h>
#include <parameters.h>
#include <configurations.h>
#include <gic.h>
#include <bsp.h>
#ifdef PMK_DEBUG
#include <printk.h>
#endif

#define TTC_0 ((triple_timer_cnt_t *)XPAR_PS7_TTC_0_BASEADDR)

#define PRESCALER_N 7
#define PRESCALER (1U << (PRESCALER_N + 1))
#define CLK_FREQ 133 // TODO get correct freq value for prescaler op.

void arm_init_ttc(air_u32_t timer_id)
{
#ifdef PMK_DEBUG
    printk("\n :: Triple Timer #%d setup\n", timer_id);
#endif

    if (timer_id < 0 || timer_id > 3)
    {
        return;
    }

    //  volatile air_uptr clk_621_true = (air_uptr_t *)(XPAR_PS7_SLCR_0_S_AXI_BASEADDR + 0x1c4);
    //  reset value 621

    air_u32_t us_per_tick = pmk_get_usr_us_per_tick();

    air_u32_t counter = pmk_get_usr_us_per_tick() * CLK_FREQ / PRESCALER;

    if (counter >= (1 << 16))
    { // 16-bit counter
#ifdef PMK_DEBUG
        printk("    ! timer excedes 16bit counter\n"
               "    set to 0xffff\n");
#endif
        counter = (1 << 16) - 1;
    }
#ifdef PMK_DEBUG
    printk("    us_per_tick = %d\n"
           "    counter = %d\n\n",
           us_per_tick, counter);
#endif
    if (timer_id == 1)
    {

        TTC_0->clk_ctrl_1 = ARM_TTC_CLK_CTRL_PS_EN | ARM_TTC_CLK_CTRL_PS(PRESCALER_N);
        TTC_0->cnt_ctrl_1 = ARM_TTC_CNT_CTRL_DIS;
        arm_data_synchronization_barrier();
        TTC_0->intv_cnt_1 = ARM_TTC_INTV_CNT(counter);
        TTC_0->int_en_1 = ARM_TTC_INT_EN_INTV;
    }

#ifdef PMK_DEBUG_TIMER
    printk(" :: triple_timer_cnt_t\n"
           "    TTC_0 = 0x%x\n"
           "    TTC_0->clk_ctrl_1 = 0x%x\n"
           "    TTC_0->cnt_ctrl_1 = 0x%x\n"
           "    TTC_0->intv_cnt_1 = 0x%x\n"
           "    TTC_0->int_en_1   = 0x%x\n\n",
           TTC_0, TTC_0->clk_ctrl_1, TTC_0->cnt_ctrl_1, TTC_0->intv_cnt_1, TTC_0->int_en_1);
#endif
}

#define arm_read_ttc TTC_0->cnt_val_1

void arm_start_ttc(air_u32_t timer_id)
{
    if (timer_id < 0 || timer_id > 3)
    {
        return;
    }

    if (timer_id == 1)
    {
        TTC_0->cnt_ctrl_1 = ARM_TTC_CNT_CTRL_INTV; // since no 1 in dis, it is en
    }

#ifdef PMK_DEBUG_TIMER
    printk(" :: triple_timer_cnt_t\n"
           "    TTC_0 = 0x%x\n"
           "    TTC_0->clk_ctrl_1 = 0x%x\n"
           "    TTC_0->cnt_ctrl_1 = 0x%x\n"
           "    TTC_0->intv_cnt_1 = 0x%x\n"
           "    TTC_0->int_en_1   = 0x%x\n\n",
           TTC_0, TTC_0->clk_ctrl_1, TTC_0->cnt_ctrl_1, TTC_0->intv_cnt_1, TTC_0->int_en_1);
#endif
}

air_u32_t arm_acknowledge_ttc(void)
{
    return (TTC_0->int_cnt_1 & 0x3f);
}
