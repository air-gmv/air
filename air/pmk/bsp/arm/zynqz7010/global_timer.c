/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file global_timer.c
 * \author lumm
 * \brief Global Timer setup and start.
 */

#include <timer.h>
#include <bsp.h>
#include <isr.h>
#include <parameters.h>
#include <configurations.h>
#ifdef PMK_DEBUG
#include <printk.h>
#endif

#define GT ((global_timer_t *)XPAR_PS7_GLOBALTIMER_0_S_AXI_BASEADDR)

void arm_init_global_timer(void) {

    /* in range 0-255 */
    air_u32_t us_per_tick = pmk_get_usr_us_per_tick();
    air_u32_t prescaler = arm_determine_gt_prescaler(us_per_tick);

    air_u32_t ctrl = ARM_GT_CTRL_PRESCALER(prescaler);
    GT->ctrl = ctrl;

    air_u32_t freq = XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2000000;

    air_u32_t autoinc = (air_u32_t)( (freq * us_per_tick) / ((prescaler + 1)) - 1 );
    GT->autoinc = autoinc;

#ifdef PMK_DEBUG
    printk("\n :: Global Timer initialization\n"
            "    global_timer_t at 0x%x\n"
            "    GT ctrl = 0x%08x\n"
            "    GT status = 0x%08x\n"
            "    gt freq = %dMHz\n"
            "    prescaler = %d\n"
            "    us per tick = %d\n"
            "    gt comparator = 0x%x\n\n",
            GT, GT->ctrl, GT->irq_status, freq, prescaler, us_per_tick, GT->autoinc);
#endif
}

void arm_start_global_timer(void) {

    /* Start GT and set period to autoinc=usr_per_tick adjusted for frequency */
    GT->ctrl &= ~(ARM_GT_CTRL_COMP_EN | ARM_GT_CTRL_TIMER_EN);
    GT->cnt_lower = 0;
    GT->cnt_upper = 0;
    GT->comp_lower = GT->autoinc; // TODO should it start at 0 or GT->autoinc??
    GT->comp_upper = 0;

    GT->ctrl |= ARM_GT_CTRL_AUTOINC_EN | ARM_GT_CTRL_IRQ_EN |
            ARM_GT_CTRL_COMP_EN | ARM_GT_CTRL_TIMER_EN;

    arm_instruction_synchronization_barrier();
}

air_u64_t arm_read_global_timer(void) {

    air_u32_t lower = 0;
    air_u32_t upper_1 = 0;
    air_u32_t upper_2 = 0;
    do {
        upper_1 = GT->cnt_upper;
        lower = GT->cnt_lower;
        upper_2 = GT->cnt_upper;
    } while (upper_1 != upper_2);

    air_u64_t result = (air_u64_t) upper_2 << 32U | lower;

#ifdef PMK_DEBUG
    printk("\n    :: gt->cnt = 0x%x%08x\n", upper_2, lower);
#endif
    return result;
}

void arm_acknowledge_gt(void) {
    GT->irq_status = 0x1;
}

air_u32_t arm_determine_gt_prescaler(air_u32_t us_per_tick) {

    air_u32_t freq = XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2000000;
    air_u32_t prescaler_n, us_per_tick_calc;
    air_u32_t prescaler = 0;
    air_u64_t counter;
    air_u32_t error;
    air_u32_t best_error = ~(0x0);

#ifdef PMK_DEBUG_TIMER
    printk("        :: global timer prescaler calculation\n"
           "           us_per_tick = %d\n"
           "           freq = %dMHz\n\n", us_per_tick, freq);
#endif

    for (prescaler_n = 256; prescaler_n > 0; prescaler_n--) {

        counter = (air_u64_t)( ((us_per_tick * freq) / (prescaler_n + 1)) - 1 );

#ifdef PMK_DEBUG_TIMER
        air_u32_t c_l = (air_u32_t)(counter & 0xffffffff);
        air_u32_t c_h = (air_u32_t)(counter >> 32U);
        printk("        counter = 0x%x %08x\n", c_h, c_l);
#endif

        us_per_tick_calc = (air_u32_t)(((counter + 1) * (prescaler_n + 1)) / freq);

        error = us_per_tick - us_per_tick_calc;
#ifdef PMK_DEBUG_TIMER
        printk("        prescaler %ld -> error %ld\n"
               "        us_per_tick %ld, us_per_tick_calc %ld\n",
               prescaler_n, error, us_per_tick, us_per_tick_calc);
#endif
        if (error == 0) {
            best_error = error;
            prescaler = prescaler_n;
            break;
        }

        if (error < best_error) {
            best_error = error;
            prescaler = prescaler_n;
        }
    }

    return prescaler;
}
