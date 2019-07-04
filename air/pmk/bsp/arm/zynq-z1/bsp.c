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
#include <armcp15.h>

void bsp_start_hook(void *vector_table_begin)
{
    air_u32_t sctlr;

    sctlr = get_system_control();

    /*
    * Current U-boot loader seems to start kernel image
    * with I and D caches on and MMU enabled.
    * If RTEMS application image finds that cache is on
    * during startup then disable caches.
    */
    if ( sctlr & (CP15_CTRL_I | CP15_CTRL_C | CP15_CTRL_M) ) {
        if ( sctlr & (CP15_CTRL_C | CP15_CTRL_M) ) {
            /*
            * If the data cache is on then ensure that it is clean
            * before switching off to be extra carefull.
            */
            data_cache_clean_all_levels();
        }
        instruction_synchronization_barrier();
        sctlr &= ~ ( CP15_CTRL_I | CP15_CTRL_C | CP15_CTRL_M | CP15_CTRL_A );
        set_system_control(sctlr);
    }

    instruction_cache_invalidate();

    /*
    * The care should be taken there that no shared levels
    * are invalidated by secondary CPUs in SMP case.
    * It is not problem on Zynq because level of coherency
    * is L1 only and higher level is not maintained and seen
    * by CP15. So no special care to limit levels on the secondary
    * are required there. TODO understand
    */
    data_cache_invalidate_all_levels();
    branch_predictor_invalidate_all();
    tlb_invalidate();

    /* If the vector table is already on the right memory position
    * then no copy needs to be done. Otherwise ...
    */
    copy_vector_table()

    clear_bss();
}

air_u32_t bsp_core_init() {

    a9mpcore_start_hook_0();
    a9mpcore_start_hook_1();

    //TODO Do I need to copy from LoadMA to VMA
    // bsp_start_copy_sections();

    //TODO done in the pmk
    // zynq_setup_mmu_and_cache();

    return 0;
}
