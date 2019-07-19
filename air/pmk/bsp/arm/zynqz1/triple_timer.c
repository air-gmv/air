/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
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

static volatile triple_timer_cnt_t *ttc = (triple_timer_cnt_t *)0xf8001000;
void pmk_ipc_handler(void *isf, pmk_core_ctrl_t *core);

void arm_init_ttc(air_u32_t timer_id) {

    if(timer_id < 0 || timer_id > 3) {
        return;
    }

    if (timer_id == 1) {

        ttc->clk_ctrl_1 = ARM_TTC_CLK_CTRL_PS_EN | ARM_TTC_CLK_CTRL_PS(0xf);
        ttc->cnt_ctrl_1 |= ARM_TTC_CNT_CTRL_DIS;
        ttc->intv_cnt_1 = ARM_TTC_INTV_CNT(0x1000);
        ttc->int_en_1 = ARM_TTC_INT_EN_INTV;
    }

#ifdef PMK_DEBUG
    printk(" :: triple_timer_cnt_t\n"
            "    ttc = 0x%x\n"
            "    ttc->clk_ctrl_1 = 0x%x\n"
            "    ttc->cnt_ctrl_1 = 0x%x\n"
            "    ttc->intv_cnt_1 = 0x%x\n"
            "    ttc->int_en_1   = 0x%x\n\n",
            ttc, ttc->clk_ctrl_1, ttc->cnt_ctrl_1, ttc->intv_cnt_1, ttc->int_en_1);
#endif
}

void arm_start_ttc(air_u32_t timer_id) {

    if(timer_id < 0 || timer_id > 3) {
        return;
    }

#ifdef PMK_DEBUG
    printk("      :: before ttc enable");
    arm_read_ttc();
#endif

    if (timer_id == 1) {
        ttc->cnt_ctrl_1 = ARM_TTC_CNT_CTRL_INTV; //since no 1 in dis, it is en
    }

#ifdef PMK_DEBUG
    printk("      :: after ttc enable");
    arm_read_ttc();
#endif
}


air_u32_t arm_read_ttc(void) {

    air_u32_t cnt;

    cnt = ttc->cnt_val_1;

#ifdef PMK_DEBUG
    printk("  :: ttc->cnt_val_1 = 0x%x\n", cnt);
#endif
    return cnt;
}

void arm_setup_interprocessor_irq(air_u32_t cpu_id) {

    if (cpu_id == 0) {
        arm_irq_install_handler(ZYNQ_IRQ_TTC_0_0, pmk_partition_scheduler);
        arm_irq_install_handler(ZYNQ_IRQ_TTC_0_1, pmk_partition_scheduler);
        arm_irq_install_handler(ZYNQ_IRQ_TTC_0_2, pmk_partition_scheduler);
        arm_irq_install_handler(BSP_IPC_IRQ, pmk_partition_scheduler);
        arm_irq_install_handler(BSP_IPC_PCS, pmk_ipc_handler);
    }

    arm_int_set_priority(ZYNQ_IRQ_TTC_0_0, 0);
    arm_int_set_priority(ZYNQ_IRQ_TTC_0_1, 0);
    arm_int_set_priority(ZYNQ_IRQ_TTC_0_2, 0);
    arm_int_set_priority(BSP_IPC_IRQ, 0);
    arm_int_set_priority(BSP_IPC_PCS, 0);

    arm_int_enable(ZYNQ_IRQ_TTC_0_0);
    arm_int_enable(ZYNQ_IRQ_TTC_0_1);
    arm_int_enable(ZYNQ_IRQ_TTC_0_2);
    arm_int_enable(BSP_IPC_IRQ);
    arm_int_enable(BSP_IPC_PCS);
}
