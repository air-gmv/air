/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file ic.c
 * @author lumm
 * @brief Interrupt Controller routines.
 */

#include <ic.h>

static volatile ic_distributor_t *ic_dist =
                (ic_distributor_t *)IC_DIST_BASE_MEMORY;
static volatile ic_cpu_interface_t *ic_cpu =
                (ic_cpu_interface_t *)IC_CPU_BASE_MEMORY;

void gic_init(air_u32_t cpu_id) {

    clean_ppi();
    if (cpu_id == 0) {
        clean_spi();
        gic_disable();
    }


    /**< Priority mask */
    ic_cpu->iccpmr = 0xff;
    /**< Binary point */
    ic_cpu->iccbpr = 0x0;
    /**< Secure interrupts as FIQ and Global enable of secure interrupts */
    ic_cpu->iccicr = 0b101;

    if (cpu_id == 0) {
        gic_enable();
    }
}
