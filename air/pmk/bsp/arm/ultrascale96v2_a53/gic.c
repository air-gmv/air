/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file gic.c
 * \author ansi
 * \brief Interrupt Controller routines.
 * SGIs are always enabled on the Zynq board, and writes and reads to the
 * Set-Enable Register (ICDISER) Clear-Enable Register (ICDICER) are
 * pointless. SPIs and PPIs are disabled by default.
 * This is the default state we want in AIR, so no point in changing it.
 *
 * TODO probably need to add timer handlers later
 */

#include <gic.h>
#include <cpu.h>
#include <slcr.h>

#ifdef PMK_DEBUG
#include <printk.h>
#include <timer.h>
#endif


#define PRIORITY_DEFAULT    0x78
#define INT_TARGET_CPU0     0x1
#define INT_TARGET_CPU1     0x2
#define TARGET_DEFAULT      INT_TARGET_CPU0

air_u32_t protected_isr_mask = 0;
air_u32_t pmk_isr_mask = 0;

void gic_init(air_u32_t cpu_id) {

    air_u32_t int_count = arm_get_int_count();
    air_u32_t id = 0;

    if (cpu_id == 0 && arm_is_gic_enabled()) {
        arm_gic_disable();
    }

    /* Clear */
    arm_sgi_ppi_disable();
    if (cpu_id == 0) {
        arm_spi_disable();
    }

    /* downgrade all interrupts to grp1 so that only timer is set to grp0 and uses FIQ */
    //arm_int_set_grp1();

    /* Priority */
    for (id = 0; id < int_count; ++id) {
        arm_int_set_priority(id, PRIORITY_DEFAULT);
    }

    /* Target */
    for (id = 32; id < int_count; ++id) {
        arm_int_set_target(id, TARGET_DEFAULT);
    }

    /* See the gicv2 architecture specification for an indepth explanation of the following fields */
    air_u32_t int_mask = 0xff; /* all interrupts accepted */
    
    // Setting a mask provides an interrupt priority filter.
    // Only interrupts with higher priority than the mask are signaled to the processor.
    arm_set_int_mask(int_mask);

    // The group priority field is used to determine interrupt preemption
    // Interrupt preemption is the signaling of higher priority 
    // pending interrupts to a target processor before an active interrupt completes.
    // arm_set_ic_cpu_preemption(PREEMPTION(MAX_NESTING_LEVEL));

    //Enable forwarding of interrupts in cpu interfaces
    air_u32_t iccicr = ICCICR_ENABLE_GRP0 | ICCICR_ENABLE_GRP1;//(ICCICR_FIQ_EN | ICCICR_ENABLE_GRP1 | ICCICR_ENABLE_GRP0);
    arm_set_ic_cpu_ctrl(iccicr);

    if (cpu_id == 0) {
        arm_disable_interrupts();
        arm_clear_pending();
        
        //Enable forwarding of interrupts in distributor
        arm_gic_enable();

        //Enable interrupts in CRF_APB Module
        arm_crf_apb_clear_status();
        arm_crf_apb_ir_enable();

        #ifdef PMK_DEBUG_GIC
            printk("\n :: GIC initialization\n"
            "    CRF_APB module at 0x%x\n"
            "    \t ir_status register at 0x%x = 0x%x\n"
            "    \t ir_mask register at 0x%x = 0x%x\n"
            "    \t ir_enable register at 0x%x = 0x%x\n"
            "    \t ir_disable register at 0x%x = 0x%x\n",
            CRF_APB, &CRF_APB->ir_status, CRF_APB->ir_status, &CRF_APB->ir_mask, CRF_APB->ir_mask, &CRF_APB->ir_enable, CRF_APB->ir_enable, &CRF_APB->ir_disable, CRF_APB->ir_disable);

            printk( "    ICD at 0x%x\n"
            "    \t gicd_ctlr register at 0x%x = 0x%x\n"
            "    \t gicd_typer at 0x%x = 0x%x\n"
            "    \t gicd_iidr register at 0x%x = 0x%x\n"
            "    \t gicd_igroup register at 0x%x\n"
            "    \t gicd_isenabler register at 0x%x\n"
            "    \t gicd_icenabler register at 0x%x\n"
            "    \t gicd_ispendr register at 0x%x\n"
            "    \t gicd_icpendr register at 0x%x\n"
            "    \t gicd_isactiver register at 0x%x\n"
            "    \t gicd_ipriority register at 0x%x\n"
            "    \t gicd_itargetsr register at 0x%x\n"
            "    \t gicd_icfgr register at 0x%x\n"
            "    \t gicd_ppisr register at 0x%x = 0x%x\n"
            "    \t gicd_spisr register at 0x%x = 0x%x\n"
            "    \t... \n",
            ICD, &ICD->gicd_ctlr, ICD->gicd_ctlr, &ICD->gicd_typer, ICD->gicd_typer,&ICD->gicd_iidr, ICD->gicd_iidr,&ICD->gicd_igroup, &ICD->gicd_isenabler, &ICD->gicd_icenabler, &ICD->gicd_ispendr, &ICD->gicd_icpendr, &ICD->gicd_isactiver, &ICD->gicd_ipriority, &ICD->gicd_itargetsr, &ICD->gicd_icfgr, &ICD->gicd_ppisr, ICD->gicd_ppisr, &ICD->gicd_spisr, ICD->gicd_spisr);

            printk( "    ICC at 0x%x\n"
            "    \t gicc_ctlr register at 0x%x = 0x%x\n"
            "    \t gicc_pmr register at 0x%x = 0x%x\n"
            "    ... \n",
            ICC, &ICC->gicc_ctlr, ICC->gicc_ctlr, &ICC->gicc_pmr, ICC->gicc_pmr);
        #endif
    } else {
        while(!arm_is_gic_enabled());
    }
}

void arm_generate_swi(air_u32_t cpu_filter,
        air_u32_t cpu_list, air_u32_t nonsecure, air_u32_t sgi_id) {

    if (cpu_filter == ARM_SGI_TARGET_LIST && cpu_list == 0)
        return;

    ICD->gicd_sgir =
            (ARM_SGI_TARGET(cpu_filter) |
            ARM_SGI_TARGET_MASK(cpu_list) |
            (nonsecure << 15) |
            sgi_id);
}
