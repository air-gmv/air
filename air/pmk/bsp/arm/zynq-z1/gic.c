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
#include <bsp.h>

#define DEFAULT_PRIORITY 0xf7f7f7f7
#define INT_TARGET_CPU0 0x1
#define INT_TARGET_CPU1 0x2
#define INT_TARGET_BOTH_CPU (INT_TARGET_CPU0 | INT_TARGET_CPU1)
#define DEFAULT_TARGET (INT_TARGET_CPU0 << 24 | INT_TARGET_CPU0 << 16 | \
                        INT_TARGET_CPU0 << 8 | INT_TARGET_CPU0 << 0)


void gic_init(air_u32_t cpu_id) {

    if (cpu_id == 0 && gic_is_enabled()) {
        gic_disable();
    }

    /* SGIs cannot be disabled individually */
    ppi_disable();
    for (air_u32_t i = 0; i < 8; ++i) {
        int_set_priority(i, DEFAULT_PRIORITY);
    }
    /* Since AIR is running on core 0, all SGIs should go to it */
    for (air_u32_t i = 0; i < 4; ++i) {
        int_set_target(i, DEFAULT_TARGET);
    }
    /* PPIs are targeted to their cpu */

    if (cpu_id == 0) {
        air_u32_t int_count = get_int_count();
        spi_disable(int_count);
        /* There are 4 interrupts per priority and target register */
        for (air_u32_t i = 8; i < int_count/4; ++i) {
            int_set_priority(i, DEFAULT_PRIORITY);
            int_set_target(i, DEFAULT_TARGET);
        }
    }

    /**< Priority mask */
    ic_cpu->iccpmr = 0xff;
    /**< Binary point */
    ic_cpu->iccbpr = 0x0;
    /**< CPU enable of secure interrupts */
    ic_cpu->iccicr = 0b1;

    if (cpu_id == 0) {
        gic_enable();
    } else {
        while(!is_gic_enabled){}
    }
}
