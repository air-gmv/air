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

void arm_isr_default_handler(arm_interrupt_stack_frame_t *frame,
        pmk_core_ctrl_t *core) {

    air_u32_t ack = arm_acknowledge_int();

    air_u16_t id = (ack & 0x3ff);

#ifdef PMK_DEBUG_ISR
    printk("\n  :: IRQ #%d acknowledge\n\n", id);
#endif
    arm_end_of_int(ack);
}

air_uptr_t arm_isr_install_handler(air_u32_t vector, void *handler) {

    air_uptr_t old_handler = (air_uptr_t)arm_isr_handler_table[vector];
    arm_isr_handler_table[vector] = (air_uptr_t)handler;
    return old_handler;
}

void arm_isr_table_initialize(void) {

    for (air_u32_t i = 0; i < arm_get_int_count(); ++i) {
        arm_isr_install_handler(i, arm_isr_default_handler);
    }
}

air_uptr_t arm_partition_isr_handler(
        arm_interrupt_stack_frame_t *frame,
        pmk_core_ctrl_t *core,
        air_u32_t id) {

    arm_virtual_cpu_t *vcpu = &core->context->vcpu;
    arm_virtual_gic_t *vgic = &core->context->vgic;

    air_u32_t psr = vcpu->psr;
    air_u32_t psr_i = (psr & ARM_PSR_I);
    air_u32_t **vbar = vcpu->vbar;

    air_u32_t iccicr = vgic->iccicr;
    air_u32_t iccpmr = vgic->iccpmr;
    air_uptr_t icdiser = vgic->icdiser;
    air_uptr_t icdispr = vgic->icdispr;
    air_uptr_t icdipr = vgic->icdipr;

    if (psr_i && (iccicr & 0x1) && (vbar != NULL)) {

        pmk_hm_event_t *hm_event = (pmk_hm_event_t *)core->context->hm_event;
        if (hm_event->nesting > 0 && vbar[1] && vbar[3] && vbar[4]) {

            vcpu->psr |= (ARM_PSR_A | ARM_PSR_I);
            vgic->icciar = ((core->idx << 10) & id);
            switch (hm_event->error_id) {
            case AIR_UNIMPLEMENTED_ERROR:
                return vbar[1];

            case AIR_VIOLATION_ERROR:
                return vbar[3];

            case AIR_SEGMENTATION_ERROR:
                return vbar[4];

            case AIR_IO_ERROR:
                // TODO not sure in this case
                return vbar[6];

            default:
                return NULL;
            }
        }

        if (arm_is_vint_enabled(icdiser, id)) {

            if (arm_get_vint_priority(icdipr, id) < iccpmr) {

                vcpu->psr |= (ARM_PSR_A | ARM_PSR_I);
                return vbar[6];
            } else {
                arm_set_vint_pending(icdispr, id);
            }
        }
    } else {

        arm_set_vint_pending(icdispr, id);
    }

    return NULL;
}

#ifdef PMK_DEBUG_ISR
void arm_isr_handler_print_frame(arm_interrupt_stack_frame_t *frame, const char txt[5]) {
    printk("\n        arm_interrupt_stack_frame_t %s content\n"
            "\tr0   = 0x%08x\tr1   = 0x%08x\tr2   = 0x%08x\n"
            "\tr3   = 0x%08x\tr4   = 0x%08x\tr5   = 0x%08x\n"
            "\tr6   = 0x%08x\tr7   = 0x%08x\tr8   = 0x%08x\n"
            "\tr9   = 0x%08x\tr10  = 0x%08x\tr11  = 0x%08x\n"
            "\tr12  = 0x%08x\tsp   = 0x%08x\tlr   = 0x%08x\n"
            "\tp_lr = 0x%08x\tcspr = 0x%08x\tttbr = 0x%08x\n\n",
            txt, frame->r0, frame->r1, frame->r2, frame->r3, frame->r4,
            frame->r5, frame->r6, frame->r7, frame->r8, frame->r9,
            frame->r10, frame->r11, frame->r12, frame->usr_sp,
            frame->usr_lr, frame->ret_addr, frame->ret_psr,
            arm_cp15_get_translation_table0_base());
#ifdef PMK_DEBUG_PARTITION
    printk("\n        partition memory contents\n");
    for (air_u32_t i = 0; i < 0x400; i += 4) {
        if (!(i%16))
            printk("\n");
        printk("0x%08x: 0x%08x   ", (0x10000000+i), *((air_uptr_t)(0x10000000+i)));
    }
    printk("\n");
#endif
}
#endif

air_uptr_t arm_isr_handler(arm_interrupt_stack_frame_t *frame,
        pmk_core_ctrl_t *core) {

    /* Return Value */
    air_uptr_t ret = NULL;
    /* Acknowledge Interrupt */
    air_u32_t ack = arm_acknowledge_int();

    air_u16_t id = (ack & 0x3ff);
    // not used    air_u8_t cpu = ((ack << 10U) & 0x7);

#ifdef PMK_DEBUG_ISR
    printk("\n :: IRQ #%d acknowledge\n\n", id);
#endif
    /* TTC interrupt */
    if (id >= 42 && id <= 44) {

        air_u32_t ttc_ack_1 = arm_acknowledge_ttc();

#ifdef PMK_DEBUG_TIMER
        printk("    ttc_ack_1 = 0x%x\n", ttc_ack_1);

        triple_timer_cnt_t *ttc = (triple_timer_cnt_t *)0xf8001000;
        ic_distributor_t *ic_dist = (ic_distributor_t *)IC_DIST_BASE_MEMORY;

        printk("    ttc->cnt_val_1 = 0x%x\n", ttc->cnt_val_1);
        printk("  * ic_dist->icdiser[1] = 0x%x\n", ic_dist->icdiser[1]);
        printk("  * ic_dist->icdispr[1] = 0x%x\n\n", ic_dist->icdispr[1]);
#endif
    }

    /* Spurious interrupt */
    if(id == 1023) {
        return ret;
    }
#ifdef PMK_DEBUG_ISR
    arm_isr_handler_print_frame(frame, "ENTRY");
#endif

    if (arm_isr_handler_table[id] != (air_uptr_t)NULL) {
        ((isr)arm_isr_handler_table[id])(frame, core);
    }

    /* core point may change if the execution was preempted. WTF? */
//  core = (pmk_core_ctrl_t *)arm_cp15_get_Per_CPU();

    /* check if context switch was performed */
    if (core->partition_switch == 1) {
#ifdef PMK_DEBUG
        printk("      Switching Partitions\n");
#endif /* PMK_DEBUG */

        frame = core->context->isf_pointer;
        core->partition_switch = 0;
    }

#ifdef PMK_DEBUG_ISR
    arm_isr_handler_print_frame(frame, "EXIT ");
#endif

    /* route to the partition */
    if (core->partition != NULL &&
            (id < 42 && id > 44)) {

        ret = arm_partition_isr_handler(frame, core, id);
    }

    arm_end_of_int(ack);

    return ret;
}

