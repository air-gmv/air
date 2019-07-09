/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013 - 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file segregation.c
 * @author lumm
 * @brief ARM virtualization MMU tables and segregation
 */

#include <mmu.h>
#include <configurations.h>
#include <partition.h>

void arm_mmu_init() {

    air_u32_t ttbcr = arm_get_translation_table_control();
    ttbcr = (ttbcr & ~0b110111) | (TTBCR_PD << 4U | TTBR_N);
    arm_set_translation_table_control(ttbrc);

    air_u32_t tt0_size = (2U << (12 - TTBR_N - 1)) * sizeof(air_u32_t);
    air_u32_t ttbr0 = pmk_workspace_alloc(tt0_size);


#ifdef TTBR_N
    air_u32_t tt1_size = (2U << (12-1))* sizeof(air_u32_t);
    air_u32_t ttbr1 = pmk_workspace_alloc(tt1_size);
#else
    air_u32_t ttbr1 = NULL;
#endif

    /* TODO only here for compatibility reasons */
    /* setup partition MMU control structure for each partition */
    pmk_list_t *list = pmk_get_usr_partitions();
    for(i = 0; i < list->length; ++i) {

        pmk_partition_t *partition = &((pmk_partition_t *)list->elements)[i];

        /* get a MMU control structure for the partition */
        mmu_context_t *ctrl = pmk_workspace_alloc(sizeof(mmu_context_t));
        ctrl->ttbr0 = ttbr0;
        ctrl->ttbr1 = ttbr1;
        partition->mmu_ctrl = ctrl;
    }
}
