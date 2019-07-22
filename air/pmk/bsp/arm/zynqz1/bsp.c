/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file bsp.c
 * @author lumm
 * @brief Bsp core routines.
 */
#include <bsp.h>
#ifdef PMK_DEBUG
#include <printk.h>
#endif

void bsp_start_hook(void) {

    air_u32_t sctlr = arm_cp15_get_system_control();

    /*
    * Current U-boot loader seems to start kernel image
    * with I and D caches on and MMU enabled.
    * If RTEMS application image finds that cache is on
    * during startup then disable caches.
    */
    if (sctlr & (ARM_SCTLR_I | ARM_SCTLR_C | ARM_SCTLR_M) ) {
        if ( sctlr & (ARM_SCTLR_C | ARM_SCTLR_M) ) {
            /*
            * If the data cache is on then ensure that it is clean
            * before switching off to be extra careful.
            */
            arm_cp15_data_cache_clean_all_levels();
        }
        arm_instruction_synchronization_barrier();
        sctlr &= ~( ARM_SCTLR_I | ARM_SCTLR_C | ARM_SCTLR_M | ARM_SCTLR_A );
        arm_cp15_set_system_control(sctlr);
    }

    arm_cp15_instruction_cache_invalidate();

    /*
    * The care should be taken there that no shared levels
    * are invalidated by secondary CPUs in SMP case.
    * It is not problem on Zynq because level of coherency
    * is L1 only and higher level is not maintained and seen
    * by CP15. So no special care to limit levels on the secondary
    * are required there. TODO understand
    */
    arm_cp15_data_cache_invalidate_all_levels();
    arm_cp15_branch_predictor_invalidate_all();
    arm_cp15_tlb_invalidate();

    air_u32_t cpu_id = arm_cp15_get_multiprocessor_cpu_id();

    if(cpu_id == 0) {
        /* If the vector table is already on the right memory position
        * then no copy needs to be done. Otherwise ...
        */
//      copy_vector_table();
        bsp_clear_bss();
    }
}

air_u32_t bsp_core_init(void) {

    arm_a9mpcore_start_hook();

    air_u32_t cpu_id = arm_cp15_get_multiprocessor_cpu_id();

    if(cpu_id == 0) {

        arm_set_vector_base();
        arm_irq_table_initialize();
        arm_init_ttc(1);
        //arm_start_uart();
    }

    gic_init(cpu_id);
    arm_mmu_init();

    //TODO Do I need to copy from LoadMA to VMA
    // bsp_start_copy_sections();

    return 0;
}

void bsp_core_ready(void) {

    air_u32_t cpu_id = arm_cp15_get_multiprocessor_cpu_id();

    arm_setup_interprocessor_irq(cpu_id);

    if(cpu_id == 0) {

        //CACHE
        // something about smp. not implement for now

        //CLOCK
        arm_start_ttc(1);
    }

    //MMU
    arm_mmu_enable();

    arm_enable_interrupts();
}

void bsp_boot_core(air_u32_t cpu_id, void *entry_point) {

    if (cpu_id != 0) {
        volatile air_uptr_t start_addr = (air_uptr_t)0xfffffff0;
        arm_data_synchronization_barrier(15);
        arm_instruction_synchronization_barrier();
        *start_addr = (air_u32_t)entry_point;
        arm_data_synchronization_barrier(15);
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

#ifdef PMK_DEBUG
    printk("    :: wfi\n");
#endif
    arm_data_synchronization_barrier(15);
    while(true) {
        __asm__ volatile ("wfi" ::: "memory");
    }
}
