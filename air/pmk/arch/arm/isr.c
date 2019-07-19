/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file isr.c
 * @author lumm
 * @brief Interrupt Service Routines (ISR)
 */

#include <isr.h>
#ifdef PMK_DEBUG
#include <printk.h>
#endif

typedef void (*isr)(arm_interrupt_stack_frame_t *frame, pmk_core_ctrl_t *core);

air_uptr_t arm_isr_handler_table[ZYNQ_MAX_INT];

void arm_irq_default_handler(arm_interrupt_stack_frame_t *frame,
        pmk_core_ctrl_t *core) {

    air_u32_t ack = arm_acknowledge_int();

    air_u16_t id = (ack & 0x3ff);

#ifdef PMK_DEBUG
    printk("\n  :: IRQ #%d acknowledge\n\n", id);
    triple_timer_cnt_t *ttc = (triple_timer_cnt_t *)0xf8001000;
    ic_distributor_t *ic_dist = (ic_distributor_t *)IC_DIST_BASE_MEMORY;

    printk("    ttc->cnt_val_1 = 0x%x\n", ttc->cnt_val_1);
    printk("  * ic_dist->icdiser[1] = 0x%x\n", ic_dist->icdiser[1]);
    printk("  * ic_dist->icdispr[1] = 0x%x\n\n", ic_dist->icdispr[1]);
#endif
    return;
}

air_uptr_t arm_irq_install_handler(air_u32_t vector, void *handler) {

    air_uptr_t old_handler = (air_uptr_t)arm_isr_handler_table[vector];
    arm_isr_handler_table[vector] = (air_uptr_t)handler;
    return old_handler;
}

void arm_irq_table_initialize(void) {

    for (air_u32_t i = 0; i < arm_get_int_count(); ++i) {
        arm_irq_install_handler(i, arm_irq_default_handler);
    }
}

void arm_irq_handler(arm_interrupt_stack_frame_t *frame,
        pmk_core_ctrl_t* core) {

    /* Acknowledge Interrupt */
    air_u32_t ack = arm_acknowledge_int();

    air_u16_t id = (ack & 0x3ff);
    // not used    air_u8_t cpu = ((ack << 10U) & 0x7);

#ifdef PMK_DEBUG
    printk("\n :: IRQ #%d acknowledge\n\n", id);
    triple_timer_cnt_t *ttc = (triple_timer_cnt_t *)0xf8001000;
    ic_distributor_t *ic_dist = (ic_distributor_t *)IC_DIST_BASE_MEMORY;

    air_u32_t ttc_ack_1 = (ttc->int_cnt_1 & 0x3f);

    printk("    ttc_ack_1 = 0x%x\n", ttc_ack_1);

    printk("    ttc->cnt_val_1 = 0x%x\n", ttc->cnt_val_1);
    printk("  * ic_dist->icdiser[1] = 0x%x\n", ic_dist->icdiser[1]);
    printk("  * ic_dist->icdispr[1] = 0x%x\n\n", ic_dist->icdispr[1]);
#endif

    /* Spurious interrupt */
    if(id == 1023) {
        return;
    }

    if (arm_isr_handler_table[id] != (air_uptr_t)NULL) {
        ((isr)arm_isr_handler_table[id])(frame, core);
    }

    /* core point may change if the execution was preempted */
    core = pmk_get_current_core_ctrl();

    /* check if context switch was performed */
    if (core->partition_switch == 1) {
        /* in sparc they reassign the *frame, but it is loaded again in asm
         * so dunno.
         */
        core->partition_switch = 0;
    }

    /* TODO ROUTE THE IRQ TO THE PARTITION IF REQUIRED */

    arm_end_of_int(ack);
}
