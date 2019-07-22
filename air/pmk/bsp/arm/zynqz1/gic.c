/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file gic.c
 * @author lumm
 * @brief Interrupt Controller routines.
 * SGIs are always enabled on the Zynq board, and writes and reads to the
 * Set-Enable Register (ICDISER) Clear-Enable Register (ICDICER) are
 * pointless. SPIs and PPIs are disabled by default.
 * This is the default state we want in AIR, so no point in changing it.
 *
 * TODO probably need to add timer handlers later
 */

#include <gic.h>

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
        arm_spi_disable(int_count);
    }

    /* Priority */
    for (id = 0; id < int_count; ++id) {
        arm_int_set_priority(id, PRIORITY_DEFAULT);
    }

    /* Target */
    for (id = 32; id < int_count; ++id) {
        arm_int_set_target(id, TARGET_DEFAULT);
    }

    /* See the gicv2 for an indepth explanation of the following fields */
    air_u32_t int_mask = 0xff; /* all interrupts accepted */
    arm_set_int_mask(int_mask);

    arm_set_ic_cpu_preemption(PREEMPTION(PREEMPTION_LEVEL));

    air_u32_t iccicr = (ICCICR_ENABLE_GRP1 | ICCICR_ENABLE_GRP0);
    arm_set_ic_cpu_ctrl(iccicr);

    if (cpu_id == 0) {
        arm_disable_interrupts();
        arm_clear_pending(int_count);
        arm_gic_enable();
    } else {
        while(!arm_is_gic_enabled()){}
    }
}

void arm_generate_swi(air_u32_t cpu_filter,
        air_u32_t cpu_list, air_u32_t nonsecure, air_u32_t sgi_id) {

    if (cpu_filter == ARM_SGI_TARGET_LIST && cpu_list == 0)
        return;

    ic_dist->icdsgir =
            (ARM_SGI_TARGET(cpu_filter) |
            ARM_SGI_TARGET_MASK(cpu_list) |
            (nonsecure << 15) |
            sgi_id);
}
