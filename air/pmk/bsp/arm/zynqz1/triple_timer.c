/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file tripe_timer.c
 * @author lumm
 * @brief Triple Timer Counters (TTC)
 */

#include <isr.h>
#include <bsp.h>
#include <timer.h>
#include <pmk.h>
#include <configurations.h>
#ifdef PMK_DEBUG
#include <printk.h>
#endif

#define PRESCALER_N 7
#define PRESCALER (1U << (PRESCALER_N + 1))
#define CLK_FREQ 133

static volatile triple_timer_cnt_t *ttc = (triple_timer_cnt_t *)0xf8001000;
void pmk_ipc_handler(void *isf, pmk_core_ctrl_t *core);

void arm_init_ttc(air_u32_t timer_id) {
#ifdef PMK_DEBUG
    printk("\n :: Triple Timer #%d setup\n", timer_id);
#endif

    if(timer_id < 0 || timer_id > 3) {
        return;
    }

//  volatile air_uptr clk_621_true = (air_uptr_t)(SLCR_BASE_MEMORY + 0x1c4);
//  reset value 621

    air_u32_t us_per_tick = pmk_get_usr_us_per_tick();

    air_u32_t counter = pmk_get_usr_us_per_tick() * CLK_FREQ/PRESCALER;

    if (counter >= (1 << 16)) {//16-bit counter
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
    if (timer_id == 1) {

        ttc->clk_ctrl_1 = ARM_TTC_CLK_CTRL_PS_EN | ARM_TTC_CLK_CTRL_PS(PRESCALER_N);
        ttc->cnt_ctrl_1 = ARM_TTC_CNT_CTRL_DIS;
        arm_data_synchronization_barrier(15);
        ttc->intv_cnt_1 = ARM_TTC_INTV_CNT(counter);
        ttc->int_en_1 = ARM_TTC_INT_EN_INTV;
    }

#ifdef PMK_DEBUG_TIMER
    printk(" :: triple_timer_cnt_t\n"
            "    ttc = 0x%x\n"
            "    ttc->clk_ctrl_1 = 0x%x\n"
            "    ttc->cnt_ctrl_1 = 0x%x\n"
            "    ttc->intv_cnt_1 = 0x%x\n"
            "    ttc->int_en_1   = 0x%x\n\n",
            ttc, ttc->clk_ctrl_1, ttc->cnt_ctrl_1, ttc->intv_cnt_1, ttc->int_en_1);
#endif
}

#define arm_read_ttc ttc->cnt_val_1

void arm_start_ttc(air_u32_t timer_id) {
    if(timer_id < 0 || timer_id > 3) {
        return;
    }

    if (timer_id == 1) {
        ttc->cnt_ctrl_1 = ARM_TTC_CNT_CTRL_INTV; //since no 1 in dis, it is en
    }

#ifdef PMK_DEBUG_TIMER
    printk(" :: triple_timer_cnt_t\n"
            "    ttc = 0x%x\n"
            "    ttc->clk_ctrl_1 = 0x%x\n"
            "    ttc->cnt_ctrl_1 = 0x%x\n"
            "    ttc->intv_cnt_1 = 0x%x\n"
            "    ttc->int_en_1   = 0x%x\n\n",
            ttc, ttc->clk_ctrl_1, ttc->cnt_ctrl_1, ttc->intv_cnt_1, ttc->int_en_1);
#endif
}

air_u32_t arm_acknowledge_ttc(void) {
    return (ttc->int_cnt_1 & 0x3f);
}

//void arm_setup_interprocessor_irq(air_u32_t cpu_id) {
//
//    if (cpu_id == 0) {
//        arm_isr_install_handler(ZYNQ_IRQ_TTC_0_0, pmk_partition_scheduler);
//        arm_isr_install_handler(ZYNQ_IRQ_TTC_0_1, pmk_partition_scheduler);
//        arm_isr_install_handler(ZYNQ_IRQ_TTC_0_2, pmk_partition_scheduler);
//        arm_isr_install_handler(BSP_IPC_IRQ, pmk_partition_scheduler);
//        arm_isr_install_handler(BSP_IPC_PCS, pmk_ipc_handler);
//    }
//
//    arm_int_set_priority(ZYNQ_IRQ_TTC_0_0, 0);
//    arm_int_set_priority(ZYNQ_IRQ_TTC_0_1, 0);
//    arm_int_set_priority(ZYNQ_IRQ_TTC_0_2, 0);
//    arm_int_set_priority(BSP_IPC_IRQ, 0);
//    arm_int_set_priority(BSP_IPC_PCS, 0);
//
//    arm_int_enable(ZYNQ_IRQ_TTC_0_0);
//    arm_int_enable(ZYNQ_IRQ_TTC_0_1);
//    arm_int_enable(ZYNQ_IRQ_TTC_0_2);
//    arm_int_enable(BSP_IPC_IRQ);
//    arm_int_enable(BSP_IPC_PCS);
//}
