/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2015
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file 
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE. 
 * ==========================================================================*/
/**
 *  @file
 *  @author pfnf
 *  @brief PMK Initialization
 *  
 *  - Low level initialization:
 *      - Setup trap base trap tables
 *      - Architecture dependent operations
 *      - Clear PMK BSS area
 *  - High level initialization:
 *      - BSP base initialization (cpu, clock, irqmp, uart, ect)
 *      - Application configuration initialization
 *      - Workspace initialization
 *      - Segregation initialization
 *      - Multi-Core support initialization
 *      - Health-Monitoring tables initialization
 *      - Partitions, Schedules and Channels initialization
 *      - BSP specific devices initialization
 *  	
 *  After the initialization, all cores remain in idle-loop until the first
 *  context switch is triggered by an interrupt
 */

#include <pmk.h>
#include <bsp.h>
#include <tod.h>
#include <error.h>
#include <ports.h>
#include <printk.h>
#include <schedule.h>
#include <multicore.h>
#include <workspace.h>
#include <partition.h>
#include <segregation.h>
#include <configurations.h>
#include <health_monitor.h>

/** 
 * @brief PMK initialization
 */
void pmk_init(void) {

	/* BSP Initialization */
    if (bsp_core_init() != 0) {
        pmk_fatal_error(PMK_INTERNAL_ERROR_BSP, __func__, __FILE__, __LINE__);
    }

    printk(
"\033[0m"
"-------------------------------------------------------------------------------\n" \
" ** AIR OS v0.42 \n" \
"-------------------------------------------------------------------------------\n");

	/* USR configuration initialization */
	pmk_configurations_int();

    /* Workspace initialization */
    pmk_workspace_init();

    /* initializes the PMK memory segregation */
    pmk_segregation_int();

    /* initialize other cores */
    pmk_multicore_init();

	/* Initialize user health monitor tables */
	pmk_hm_init();

    /* Partition initialization */
    pmk_partitions_init();

    /* Schedule Initialization */
    pmk_scheduler_init();

    /* communication channels initialization */
    pmk_channels_init();
	
    /* Initialize Time of Day */
    pmk_tod_initialization();

    /* Set HM state */
    pmk_hm_set_state(AIR_STATE_MODULE_EXEC);

    printk("    :: Initialization complete!\n");

	/* enable preemption */
    bsp_core_ready();

	/* Idle loop */
	bsp_idle_loop();
	return;
}
