/*
 * Copyright (C) 2018-2020  GMVIS Skysoft S.A.
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
#include <a9mpcore.h>
#include <cache.h>
#include <isr.h>
#include <hm.h>
#include <mmu.h>
#include <gic.h>
#include <timer.h>
#include <slcr.h>
#include <uart.h>
#include <ddr.h>
#include <printk.h>

/**
 * \warning
 * Care should be taken that no shared levels are invalidated by secondary CPUs
 * in SMP case. It is not problem on Zynq-700 because the level of coherency is
 * L1 only and higher levels are not maintained and seen by the CP15. So no
 * special care to limit levels on the secondary are required there.
 */
void bsp_start_hook(void)
{

    air_u32_t sctlr = arm_cp15_get_system_control();

    /*
     * If the I and D caches on and MMU are enabled, then disable them
     */
    if (sctlr & (ARM_SCTLR_I | ARM_SCTLR_C | ARM_SCTLR_M))
    {
        if (sctlr & (ARM_SCTLR_C | ARM_SCTLR_M))
        {
            /*
             * If the data cache is on then ensure that it is clean
             * before switching off to be extra careful.
             */
            arm_data_cache_clean();
        }
        sctlr &= ~(ARM_SCTLR_I | ARM_SCTLR_C | ARM_SCTLR_M);
        arm_cp15_set_system_control(sctlr);
    }

    /* Low interrupt latency configuration & Alignment fault enable*/
    sctlr |= (ARM_SCTLR_FI & ARM_SCTLR_A);
    arm_cp15_set_system_control(sctlr);

    arm_tlb_invalidate();
    arm_instruction_cache_invalidate();
    arm_branch_predictor_invalidate();
    arm_data_cache_invalidate();

    arm_instruction_synchronization_barrier();
}

air_u32_t bsp_core_init(void)
{

    air_u32_t cpu_id = arm_cp15_get_multiprocessor_cpu_id();

 //   if(cpu_id != 0)
 //       bsp_wait_for_event();

    arm_a9mpcore_start_hook(cpu_id);
    arm_set_vector_base();

    if (cpu_id == 0)
    {
#if DEBUG_MONITOR != 2
        arm_select_debug_uart(0);
#endif /* DEBUG_MONITOR != 2	*/
        arm_isr_table_init();
        arm_hm_init();
#if DEBUG_MONITOR != 2
        arm_setup_uart(0, 115200);
#endif /* DEBUG_MONITOR != 2	*/
    
    arm_peripheral_soft_reset();

   }

#ifdef PMK_DEBUG
    printk("start! bsp_core_init::cpu_id = %d\n", cpu_id);
#endif

    gic_init(cpu_id);
    arm_mmu_init();

    // TODO Do I need to copy from LoadMA to VMA???
    //  bsp_start_copy_sections();

    return 0;
}

void bsp_core_ready(void)
{

    air_u32_t cpu_id = arm_cp15_get_multiprocessor_cpu_id();
    arm_cp15_setup_Per_CPU(cpu_id);

    arm_setup_ipc(cpu_id);

    // MMU
    // arm_mmu_enable(partition->mmu_ctrl);

    if (cpu_id == 0)
    {

        arm_init_global_timer();
        arm_clear_pending();
    }

    arm_enable_interrupts();

    if (cpu_id == 0)
    {

        arm_start_global_timer();
    }
}

void bsp_boot_core(air_u32_t cpu_id, void *entry_point)
{

    if (cpu_id != 0)
    {
        volatile air_uptr_t *start_addr = (air_uptr_t *)0xfffffff0;
        arm_data_synchronization_barrier();
        arm_instruction_synchronization_barrier();
        *start_addr = (air_u32_t)entry_point;
        arm_data_synchronization_barrier();
        arm_instruction_synchronization_barrier();
        bsp_send_event();
    }
}

/* Resets all cores */
void bsp_restart_core(pmk_core_ctrl_t *core_ctx)
{
    arm_ps_reset();
}

void bsp_shutdown_core(pmk_core_ctrl_t *core_ctx)
{

    printk("\nAIR has been terminaded by the application.\n");

    /* 1. disable interrupts */
    arm_disable_interrupts();

    /* 2. configure wake-up device */
    /* 3. enable l2 cache dynamic clock gating */
    /* 4. enable scu standby mode */
    arm_scu_standy_enable();

    /* 5. enable topswitch clock stop */
    *(air_uptr_t *)(XPAR_PS7_SLCR_0_S_AXI_BASEADDR + 0x16c) |= 1;

    /* 6. enable cortex-a9 dynamic clock gating */
    arm_cp15_enable_dynamic_clock_gating();

    /* 7. put external DDR into self-refresh mode */
    arm_ddr_self_refresh();

    /* 8. put PLLs into bypass mode */
    arm_pll_bypass_mode();

    /* 9. shutdown PLLs */
    arm_pll_shutdown();

    /* 10. increase clock divider (0x3f) and slow down CPU clock */
    arm_set_cpu_clock_divisor(0x3f);

    while (1)
    {
        ;
    }

    return;
}

void bsp_interrupt_broadcast(air_u32_t dummy)
{
    arm_generate_swi(ARM_SGI_TARGET_OTHERS, 0, 0, BSP_IPC_PCS);
}

void bsp_idle_loop(void)
{

#ifdef PMK_DEBUG_IDLE
    printk("\n    wfi w/ ttbr at 0x%x\n\n", arm_cp15_get_translation_table0_base());
#endif
    arm_data_synchronization_barrier();
    while (true)
    {
        __asm__ volatile("wfi");
    }
}
