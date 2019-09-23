/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file global_timer.c
 * @author lumm
 * @brief Global Timer setup and start.
 */

#include <isr.h>
#include <bsp.h>
#include <timer.h>
#include <pmk.h>
#include <configurations.h>
#ifdef PMK_DEBUG
#include <printk.h>
#endif

#define CPU_FREQ 650000000

static volatile global_timer_t *gt = (global_timer_t *)GT_BASE_MEMORY;
void pmk_ipc_handler(void *isf, pmk_core_ctrl_t *core);

void arm_init_global_timer(void) {

    /* in range 0-255 */
    air_u32_t us_per_tick = pmk_get_usr_us_per_tick();
    air_u32_t prescaler = arm_determine_gt_prescaler(us_per_tick);

    air_u32_t ctrl = ARM_GT_CTRL_PRESCALER(prescaler);
    gt->ctrl |= ctrl;

    air_u32_t autoinc = (25 * pmk_get_usr_us_per_tick())/256 - 1;
    gt->autoinc = autoinc;
#ifdef PMK_DEBUG
    printk("\n :: global_timer_t\n"
            "    gt = 0x%x\n"
            "    gt->autoinc = 0x%x\n\n", gt, gt->autoinc);
#endif
}

void arm_start_global_timer(void) {

    /* Start GT and set period to autoinc=usr_per_tick adjusted for frequency */
    gt->ctrl &= ~(ARM_GT_CTRL_COMP_EN | ARM_GT_CTRL_TIMER_EN);
    gt->cnt_lower = 0;
    gt->cnt_upper = 0;
    gt->comp_lower = gt->autoinc; // TODO should it start at 0 or gt->autoinc??
    gt->comp_upper = 0;

#ifdef PMK_DEBUG
    printk("    :: before gt enable;\n");
#endif

    gt->ctrl |= ARM_GT_CTRL_AUTOINC_EN | ARM_GT_CTRL_IRQ_EN |
            ARM_GT_CTRL_COMP_EN | ARM_GT_CTRL_TIMER_EN;

    arm_read_global_timer();
    for (air_u32_t i = 0; i < 0xffffff; ++i) {}
    arm_read_global_timer();
}

air_u64_t arm_read_global_timer(void) {

    air_u32_t lower = 0;
    air_u32_t upper_1 = 0;
    air_u32_t upper_2 = 0;
    do {
        upper_1 = gt->cnt_upper;
        lower = gt->cnt_lower;
        upper_2 = gt->cnt_upper;
    } while (upper_1 != upper_2);

    air_u64_t result = (air_u64_t) upper_2 << 32 | lower;

#ifdef PMK_DEBUG
    printk("\n    :: gt->cnt = 0x%x\n", upper_2);
    printk("\n    :: gt->cnt = 0x%x\n", lower);
#endif
    return result;
}

void arm_setup_interprocessor_irq(air_u32_t cpu_id) {

    if (cpu_id == 0) {
        arm_isr_install_handler(ARM_A9MPCORE_IRQ_GT, pmk_partition_scheduler);
        arm_isr_install_handler(BSP_IPC_IRQ, pmk_partition_scheduler);
        arm_isr_install_handler(BSP_IPC_PCS, pmk_ipc_handler);
    }

    arm_int_set_priority(ARM_A9MPCORE_IRQ_GT, 0);
    arm_int_set_priority(BSP_IPC_IRQ, 0);
    arm_int_set_priority(BSP_IPC_PCS, 0);

    arm_int_enable(ARM_A9MPCORE_IRQ_GT);
    arm_int_enable(BSP_IPC_IRQ);
    arm_int_enable(BSP_IPC_PCS);
}

void arm_acknowledge_gt(void) {
    gt->irq_status = 0x1;
}


air_u32_t arm_determine_gt_prescaler(air_u32_t us_per_tick) {

    air_u32_t prescaler_n, us_per_tick_calc, error;
    air_u64_t freq = CPU_FREQ / 2;
    air_u32_t prescaler = 0;
    air_u64_t counter;
    air_u32_t best_error = ~(0x0);

    for (prescaler_n = 256; prescaler_n > 0; prescaler_n--) {

        counter = us_per_tick * freq / (prescaler_n);
        us_per_tick_calc = counter * (prescaler_n) / freq;

        error = us_per_tick - us_per_tick_calc;
        if (error == 0)
            return prescaler_n;

        if ((error < best_error)) {
            best_error = error;
            prescaler = prescaler_n;
        }
    }

    return (prescaler - 1);
}
