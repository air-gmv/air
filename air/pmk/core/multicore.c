/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013 - 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file 
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE. 
 * ==========================================================================*/
/**
 *  @file
 *  @author pfnf
 *  @brief Multicore
 */

#include <pmk.h>
#include <bsp.h>
#include <error.h>
#include <printk.h>
#include <barrier.h>
#include <workspace.h>
#include <multicore.h>
#include <configurations.h>

/**
 * @brief Assembly entry point for the secondary core initialization
 */
void pmk_trap_table(void);


/**
 * @brief Core idle context's
 */
core_context_t *pmk_core_idle_context;

/**
 * @brief Multi-core synchronization barrier
 * @ingroup pmk_multicore
 */
static pmk_barrier_t initialization_barrier;

/**
 * @brief
 */
void pmk_multicore_init(void) {
    
    air_u32_t i;
    air_u32_t available_cores = 0;
    air_u32_t configured_cores = 0;

#ifdef PMK_DEBUG
    printk(" :: Multicore support initialization\n");
#endif

    /* get the hardware available cores, and the required cores */
    available_cores  = bsp_get_number_of_cores();
    configured_cores = pmk_get_usr_configured_cores();

    /* invalid configuration */
    if (0 == configured_cores || configured_cores > available_cores) {
        pmk_fatal_error(PMK_INTERNAL_ERROR_CONFIG, __func__, __FILE__, __LINE__);
    }

#ifdef PMK_DEBUG
    printk("    Configured cores: %i\n", configured_cores);
#endif

    /* setup configured cores */
    air_shared_area.configured_cores = configured_cores;

    /* allocate the required core structures and its respective interrupt stack
     * pointers in the workspace shared area
     */
    pmk_core_idle_context =
            pmk_workspace_alloc(sizeof(core_context_t) * configured_cores);
    air_shared_area.core =
            pmk_workspace_alloc(sizeof(pmk_core_ctrl_t) * configured_cores);
    for (i = 0; i < configured_cores; ++i){

#ifdef PMK_DEBUG
        printk("    core control %i (0x%08x):\n", i, &air_shared_area.core[i]);
#endif

        /* initialize idle contexts */
        core_context_init(&pmk_core_idle_context[i], i);
        core_context_setup_idle(&pmk_core_idle_context[i]);

        /* initialize core control */
        air_shared_area.core[i].context = &pmk_core_idle_context[i];
        air_shared_area.core[i].partition = NULL;
        air_shared_area.core[i].schedule = NULL;
        air_shared_area.core[i].idx = i;
    }

    /* initialize the synchronization barriers between all cores */
    pmk_barrier_init(&initialization_barrier, configured_cores);

    /* boot configured cores */
    for(i = 1; i < configured_cores; ++i) {
        bsp_boot_core(i, pmk_trap_table);
    }

    /* synchronize cores initialization */
    pmk_barrier_wait(&initialization_barrier, 0);

#ifdef PMK_DEBUG
    pmk_workspace_debug();
#endif

    return;
}

/**
 * @brief C initialization function for secondary cores.
 * @note called from pmk_init_core
 */
void pmk_core_initialize(void){

    /* perform per-core BSP initialization */
    bsp_core_init();

    /* wait for other cores to initialize */
    pmk_barrier_wait(&initialization_barrier, bsp_get_core_id());

    /* mark core as ready */
    bsp_core_ready();

    /* loop until the next clock tick interrupt */
    bsp_idle_loop();
}

pmk_core_ctrl_t *pmk_get_current_core_ctrl(void) {

    air_u32_t idx = bsp_get_core_id();
    return &air_shared_area.core[idx];
}
