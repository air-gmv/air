/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file bsp.c
 * \author lumm
 * \brief BSP core routines
 */
#include <bsp.h>
#include <cache.h>
#include <isr.h>
#include <hm.h>
#include <mmu.h>
#include <gic.h>
#include <timer.h>

#ifdef PMK_DEBUG
#include <printk.h>
#endif


/**
 * \warning
 * Care should be taken that no shared levels are invalidated by secondary CPUs
 * in SMP case. It is not problem on Zynq-700 because the level of coherency is
 * L1 only and higher levels are not maintained and seen by the CP15. So no
 * special care to limit levels on the secondary are required there.
 */
void bsp_start_hook(void) {

    air_u32_t sctlr = arm_cp15_get_system_control();

    /*
     * If the I and D caches on and MMU are enabled, then disable them
     */
    if (sctlr & (ARM_SCTLR_I | ARM_SCTLR_C | ARM_SCTLR_M) ) {
        if ( sctlr & (ARM_SCTLR_C | ARM_SCTLR_M) ) {
            /*
            * If the data cache is on then ensure that it is clean
            * before switching off to be extra careful.
            */
            arm_data_cache_clean_all_levels();
        }
        arm_instruction_synchronization_barrier();
        sctlr &= ~( ARM_SCTLR_I | ARM_SCTLR_C | ARM_SCTLR_M | ARM_SCTLR_A );
        arm_cp15_set_system_control(sctlr);
    }

    /* Low interrupt latency configuration */
    sctlr |= ARM_SCTLR_FI;
    arm_cp15_set_system_control(sctlr);

    arm_instruction_cache_invalidate();
    arm_data_cache_invalidate_all_levels();
    arm_cp15_branch_predictor_invalidate_all();
    arm_cp15_tlb_invalidate();
}

air_u32_t bsp_core_init(void) {

    arm_a9mpcore_start_hook();
    arm_define_uart(0);

    air_u32_t cpu_id = arm_cp15_get_multiprocessor_cpu_id();

#ifdef PMK_DEBUG
    printk("bsp_core_init::cpu_id = %d\n", cpu_id);
#endif

    arm_set_vector_base();
    if(cpu_id == 0) {

        arm_isr_table_init();
        arm_hm_init();
        arm_setup_uart(115200);
//      arm_start_uart(); //TODO screws up in qemu. and everywhere else it seems
    }

    arm_peripheral_soft_reset();
    gic_init(cpu_id);
    arm_mmu_init();

    //TODO Do I need to copy from LoadMA to VMA???
    // bsp_start_copy_sections();

    return 0;
}

void bsp_core_ready(void) {

    air_u32_t cpu_id = arm_cp15_get_multiprocessor_cpu_id();

    arm_setup_ipc(cpu_id);
    arm_cp15_setup_Per_CPU(cpu_id);

    //MMU
    //arm_mmu_enable(partition->mmu_ctrl);

    if(cpu_id == 0) {

        //CACHE
        // something about smp. not implement for now

        //arm_init_ttc(1);

        arm_init_global_timer();

        //CLOCK
        //arm_start_ttc(1);
    }

    arm_enable_interrupts();

    arm_start_global_timer();
}

void bsp_boot_core(air_u32_t cpu_id, void *entry_point) {

    if (cpu_id != 0) {
        volatile air_uptr_t start_addr = (air_uptr_t)0xfffffff0;
        arm_data_synchronization_barrier();
        arm_instruction_synchronization_barrier();
        *start_addr = (air_u32_t)0x00100020;
        arm_data_synchronization_barrier();
        arm_instruction_synchronization_barrier();
        bsp_send_event();
    }
}

/* Resets all cores */
void bsp_restart_core(pmk_core_ctrl_t *core_ctx) {

    volatile air_uptr_t pss_rst_ctrl = (air_uptr_t)(SLCR_BASE_MEMORY + 0x200);
    while(true) {
        *pss_rst_ctrl = 0x1;
    }
}

/* TODO possibly go to standby mode */
void bsp_shutdown_core(pmk_core_ctrl_t *core_ctx) {
    return;
}

void bsp_interrupt_broadcast(air_u32_t dummy) {
    arm_generate_swi(ARM_SGI_TARGET_OTHERS, 0, 0, BSP_IPC_PCS);
}


void bsp_idle_loop(void) {

#ifdef PMK_DEBUG_IDLE
    printk("\n    wfi: 0x%x\n\n", arm_cp15_get_translation_table0_base());
#endif

//#ifdef PMK_DEBUG_TIMER
//    //triple_timer_cnt_t *ttc = (triple_timer_cnt_t *)0xf8001000;
//    global_timer_t *gt = (global_timer_t *)GT_BASE_MEMORY;
//
//    ic_distributor_t *ic_dist = (ic_distributor_t *)IC_DIST_BASE_MEMORY;
//    ic_cpu_interface_t *ic_cpu = (ic_cpu_interface_t *)IC_CPU_BASE_MEMORY;
//
//    //printk("\n\n    ttc->cnt_val_1 = 0x%x\n", ttc->cnt_val_1);
//    printk("\n\n    gt->cnt_ = 0x%x\n", gt->cnt_upper);
//    printk("\n\n    gt->cnt_lower = 0x%x\n", gt->cnt_lower);
//
//    printk("    ic_cpu->iccicr = 0x%x\n", ic_cpu->iccicr);
//    printk("    ic_cpu->iccpmr = 0x%x\n", ic_cpu->iccpmr);
//    printk("    ic_cpu->iccrpr = 0x%x\n", ic_cpu->iccrpr);
//    printk("    ic_cpu->icchpir = 0x%x\n", ic_cpu->icchpir);
//    printk("    ic_dist->icddcr = 0x%x\n", ic_dist->icddcr);
//    printk("    ic_dist->icdisr[0] = 0x%x\n", ic_dist->icdisr[0]);
//    printk("    ic_dist->icdisr[1] = 0x%x\n", ic_dist->icdisr[1]);
//    printk("    ic_dist->icdisr[2] = 0x%x\n", ic_dist->icdisr[2]);
//    printk("    ic_dist->icdiser[0] = 0x%x\n", ic_dist->icdiser[0]);
//    printk("  * ic_dist->icdiser[1] = 0x%x\n", ic_dist->icdiser[1]);
//    printk("    ic_dist->icdiser[2] = 0x%x\n", ic_dist->icdiser[2]);
//    printk("    ic_dist->icdispr[0] = 0x%x\n", ic_dist->icdispr[0]);
//    printk("  * ic_dist->icdispr[1] = 0x%x\n", ic_dist->icdispr[1]);
//    printk("    ic_dist->icdispr[2] = 0x%x\n", ic_dist->icdispr[2]);
//    printk("    ic_dist->icdabr[0] = 0x%x\n", ic_dist->icdabr[0]);
//    printk("    ic_dist->icdabr[1] = 0x%x\n", ic_dist->icdabr[1]);
//    printk("    ic_dist->icdabr[2] = 0x%x\n\n", ic_dist->icdabr[2]);
//#endif

    arm_data_synchronization_barrier();
    while(true) {
        __asm__ volatile ("wfi" ::: "memory");
    }
}
