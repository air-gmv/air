/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2008-2012
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/

#include <pmk.h>
#include <bsp.h>
#include <amba.h>
#include <clock.h>
#include <irqmp.h>
#include <l2cache.h>
#include <console.h>
#include <segregation.h>

/**
 * @brief AMBA Plug & Play configuration area
 * @ingroup bsp_leon_amba
 */
amba_confarea_t amba_confarea;


extern void leon_restart_core();
extern void leon_shutdown_core();
extern void sparc_trap_table_int(void);
extern air_u32_t sparc_enable_preemption(air_u32_t pil);


/**
 * @brief LEON I/O Scan area
 */
#define LEON3_IO_AREA                               (0xFFF00000)


int bsp_core_init(void) {

    int rc = 0;
    air_u32_t core_id = bsp_get_core_id();

    /* primary core, will initialize the BSP */
    if (core_id == 0) {

        /* SPARC trap table initialization */
        sparc_trap_table_int();

        /* setup the AMBA configuration area */
        amba_setup(&amba_confarea, LEON3_IO_AREA);

        rc |= clock_init();           /* initialize the clock               */
        rc |= console_init();         /* initialize the console             */
        rc |= irqmp_init();           /* initialize the IRQASMP             */
    }
    /* if it is a secondary core start it be setting 
     * mpstat - multiprocessor status */
    else
    {
        irqmp_interrupt_set_mprocessor_status(core_id);
    }

    return rc;
}

void bsp_shutdown_core(pmk_core_ctrl_t *core) {

    leon_shutdown_core();
}

void bsp_restart_core(pmk_core_ctrl_t *core) {

    /* mask all interrupts */
    irqmp_set_interrupt_mask(core->idx, 0x0);

    /* restart primary core */
    if (core->idx == 0) {
        leon_restart_core();

    /* shutdown secondary cores */
    } else {
        leon_shutdown_core();
    }
    return;
}

void bsp_core_ready(void) {

    air_u32_t core_id = bsp_get_core_id();

    /* primary core, will enable more BSP specific components */
    if (core_id == 0) {

        /* initialize & configure L2 cache */
        if (l2cache_init() == 0) {

            l2cache_set_replacement_policy(MASTER_INDEX_0);
            l2cache_set_write_policy(WRITE_THROUGH);
            l2cache_set_bus_usage_mode(WRAPPING_MODE);
            l2cache_hit_rate_mode(WRAPPING_MODE);
            l2cache_enable_core();
        }

        /* arm clock */
        bsp_clock_start();
    }

    /* enable MMU */
    sparc_enable_mmu();

    /* flush cache and enable interrupts */
    __asm__ __volatile__("flush" :::);
    sparc_enable_preemption(0);
    return;
}

void bsp_segregation(pmk_partition_t *partition) {

    /* map the PMK memory region for supervisor access */
    cpu_segregation_map_memory(
            partition->mmu_ctrl,
            &air_kernel_memory_start,
            &air_kernel_memory_start,
            (air_uptr_t)&air_kernel_memory_end - (air_uptr_t)&air_kernel_memory_start,
            0x01000000,
            PMK_MMU_SR | PMK_MMU_SW | PMK_MMU_SE | PMK_MMU_CACHEABLE);

    /**
     * Map devices IO area
     * @todo This is a security flaw, it must be fixed
     */
    cpu_segregation_map_memory(
            partition->mmu_ctrl,
            (void *)0x80000000,
            (void *)0x80000000,
            0x80000000,
            1 << 24,
            PMK_MMU_R | PMK_MMU_W | PMK_MMU_E | PMK_MMU_DEVICE);
}
