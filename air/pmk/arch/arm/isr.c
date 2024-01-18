/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file isr.c
 * \author lumm
 * \brief Interrupt Service Routines (ISR)
 */

#include <isr.h>
#include <hm.h>
#include <svc.h>
#include <gic.h>
#include <timer.h>

#ifdef PMK_DEBUG_ISR
#include <cp15.h>
#include <printk.h>
air_u32_t counter = 0;
void arm_isr_handler_print_frame(arm_interrupt_stack_frame_t *frame, const char txt[5]);
#endif

typedef void (*isr)(arm_interrupt_stack_frame_t *frame, pmk_core_ctrl_t *core);

air_uptr_t * arm_isr_handler_table[ZYNQ_MAX_INT];

void arm_isr_table_init(void) {}

air_uptr_t * arm_isr_handler(arm_interrupt_stack_frame_t *frame, pmk_core_ctrl_t *core) {

    /* Return Value */
    air_uptr_t * ret = NULL;

    /* Acknowledge Interrupt */
    air_u32_t ack = arm_acknowledge_int();

    air_u32_t id = (ack & 0x3ff); // Get bits 0:9
    air_u8_t cpu = ((ack << 10U) & 0x7); //Get bits 10:12

#ifdef PMK_DEBUG_ISR
    printk("interrupt ID = %d for cpu = %d (ack = 0x%08x)\n", id, cpu, ack);
#endif

    /* GT Interrupt*/
    #ifdef PMK_ARM_ULTRASCALE96V2_A53
    if (id == ARM_A53_IRQ_SPT) { //Ultrascale timer generates interrupt with id 29 instead of 27
        arm_acknowledge_gt();
    }
    #endif
    if (id == 27) {

        arm_acknowledge_gt();

#ifdef PMK_DEBUG_ISR
#ifdef PMK_DEBUG_TICKS
    printk("|");
    if(!(++counter % 20))
        printk(" %d\n", counter);
#endif
#ifdef PMK_DEBUG_TIMER
#define GT ((global_timer_t *)XPAR_PS7_GLOBALTIMER_0_S_AXI_BASEADDR)

        arm_read_global_timer();
#endif
#endif
    }

    /* Spurious interrupt */
    if (id == 1023) {
        return ret;
    }

    if (arm_isr_handler_table[id] != (air_uptr_t *)NULL) {
        ((isr)arm_isr_handler_table[id])(frame, core);
    }

    /* core point may change if the execution was preempted. */
//  core = (pmk_core_ctrl_t *)arm_cp15_get_Per_CPU();

    /* check if context switch was performed */
    if (core->partition_switch == 1) {
#ifdef PMK_DEBUG_ISR
        arm_isr_handler_print_frame(frame, "ENTRY");
#endif

        frame = (arm_interrupt_stack_frame_t *)core->context->isf_pointer;
        core->partition_switch = 0;
#ifdef PMK_DEBUG_ISR
        if (core->partition != NULL) {
            printk("       ISR :: Switching to Partition %d \n\n", core->partition->id);
        } else {
            printk("       ISR :: Switching to Partition IDLE\n\n");
        }
        arm_isr_handler_print_frame(frame, "EXIT ");
#endif
    }

    /* will it route to the partition */
    if (core->partition != NULL) {
        air_u32_t mode = frame->ret_psr & ARM_PSR_MODE_MASK;
        // only perform virtual handler if the previous mode was USR or SYS
        if ( (mode == ARM_PSR_USR) || (mode == ARM_PSR_SYS))
            ret = arm_partition_isr_handler(id, core);
    }

    arm_end_of_int(ack);

    return ret;
}

air_uptr_t * arm_isr_install_handler(air_u32_t vector, void *handler) {

    air_uptr_t * old_handler = (air_uptr_t *)arm_isr_handler_table[vector];

    if (handler != old_handler) {
        arm_isr_handler_table[vector] = (air_uptr_t *)handler;
    }
    return old_handler;
}

air_uptr_t * arm_partition_isr_handler(air_u32_t id, pmk_core_ctrl_t *core) {

    air_uptr_t * ret = NULL;

    arm_virtual_cpu_t *vcpu = &core->context->vcpu;
    arm_virtual_gic_t *vgic = &core->context->vgic;
    arm_interrupt_stack_frame_t *frame = (arm_interrupt_stack_frame_t *)core->context->isf_pointer;

    air_u32_t psr = vcpu->psr;
    air_u32_t psr_i = ((psr & ARM_PSR_I) >> 7);
    air_u32_t psr_a = ((psr & ARM_PSR_A) >> 8);
    air_uptr_t * vbar = vcpu->vbar;

    if (vbar == NULL)
        return ret;

    /* abort have higher priority than IRQ, soo if any pending and cpu is accepting they go 1st */
    if (!psr_a) {

        pmk_hm_event_t *hm_event = (pmk_hm_event_t *)core->context->hm_event;
        if (hm_event->nesting > 0) {

            vcpu->psr |= (ARM_PSR_A | ARM_PSR_I);

            switch (hm_event->error_id) {
            case AIR_UNIMPLEMENTED_ERROR:
                ret = vbar + 1;
                break;

            case AIR_VIOLATION_ERROR:
                ret = vbar + 3;
                break;

            case AIR_SEGMENTATION_ERROR:
                ret = vbar + 4;
                break;

            case AIR_IO_ERROR:
                // TODO not sure in this case
                ret = vbar + 6;
                break;

            default:
                ret = NULL;
                break;
            }
        }

        if (ret != NULL) {
            vgic->ilist[id] |= (1U << 28);
            return ret;
        }
    }
    /*the IRQ exception can only be taken if the IRQ mask bit is 0*/
    if (!psr_i) {
        //virtual interrupts are enabled:
        if ((vgic->vm_ctrl & 0x1)) {
            //the current interrupt priority is higher than the priority mask:
            if (((vgic->ilist[id] >> 23) & 0x1f) < vgic->pmr) {

                vgic->pmr = ((vgic->ilist[id] >> 23) & 0x1f); //updates priority mask

                vgic->hppir = vgic->rpr;
                vgic->ilist[(vgic->iar & 0x3ff)] &= ~(1U << 29);
                vgic->ilist[(vgic->iar & 0x3ff)] |= (1U << 28);

                vgic->iar = id;//((core->idx << 10) & id);
                vgic->rpr = ((vgic->ilist[id] >> 23) & 0x1f);

                vcpu->psr |= (ARM_PSR_A | ARM_PSR_I);
                if (((frame->ret_psr & ARM_PSR_MODE_MASK) == ARM_PSR_USR) || ((frame->ret_psr & ARM_PSR_MODE_MASK) == ARM_PSR_SYS)){
                     if ((vcpu->psr & ARM_PSR_MODE_MASK) != ARM_PSR_IRQ){
                         vcpu->psr &= ~(ARM_PSR_MODE_MASK);
                         vcpu->psr |= ARM_PSR_IRQ;
                         core->context->sp_svc = frame->usr_sp;
                     }
                }

                ret = vbar + 6;

            } else {
                //set the state of the interrupt to pending
                vgic->ilist[id] |= (1U << 28);
                //change the highest priority pending if the current interrupt priority is higher
                if (((vgic->ilist[id] >> 23) & 0x1f) < ((vgic->ilist[(vgic->hppir & 0x3ff)] >> 23) & 0x1f)) {
                    vgic->hppir = ((vgic->ilist[id] >> 23) & 0x1f);
                }
            }
        }
    } else {
        // TODO: CHECK THIS INTERRUPT HANDLING, COMMENTING IT OUT FOR NOW (NOT NEEDED IN BARE)
        // vgic->ilist[id] |= (1U << 28); //Set the interrupt as pending
        // if (((vgic->ilist[id] >> 23) & 0x1f) < vgic->pmr) { //then checks for priority
        //     vgic->hppir = ((vgic->ilist[id] >> 23) & 0x1f);
        // }
    }

    return ret;
}

#ifdef PMK_DEBUG_ISR
void arm_isr_handler_print_frame(arm_interrupt_stack_frame_t *frame, const char txt[5]) {
    printk("\n        arm_interrupt_stack_frame_t %s content (0x%08x)\n"
            "\tr0   = 0x%08x\tr1   = 0x%08x\tr2   = 0x%08x\n"
            "\tr3   = 0x%08x\tr4   = 0x%08x\tr5   = 0x%08x\n"
            "\tr6   = 0x%08x\tr7   = 0x%08x\tr8   = 0x%08x\n"
            "\tr9   = 0x%08x\tr10  = 0x%08x\tr11  = 0x%08x\n"
            "\tr12  = 0x%08x\tsp   = 0x%08x\tlr   = 0x%08x\n"
            "\tp_lr = 0x%08x\tcpsr = 0x%08x\tttbr = 0x%08x exp #%d\n\n",
            txt, frame, frame->r0, frame->r1, frame->r2, frame->r3, frame->r4,
            frame->r5, frame->r6, frame->r7, frame->r8, frame->r9,
            frame->r10, frame->r11, frame->r12, frame->usr_sp,
            frame->usr_lr, frame->ret_addr, frame->ret_psr,
            arm_cp15_get_translation_table0_base(),
            frame->exception_name);
#ifdef PMK_DEBUG_PARTITION
    printk("\n        partition memory contents\n");
    for (air_u32_t i = 0; i < 0x400; i += 4) {
        if (!(i%16))
            printk("\n");
        printk("0x%08x: 0x%08x   ", (0x10000000+i), *((air_u32_t *)(0x10000000+i)));
    }
    printk("\n");
#endif
}
#endif
