/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013 - 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file mmu.h
 * @author lumm
 * @brief ARM virtualization MMU tables
 */

#include <air_arch.h>


/* Division between the two Translation Tables (N == 0 means only one tt) */
#define TTBR_N      0 /* 0 <= N < 8 */
#define TTBCR_PD    0 /* if a tbl miss performs a table walk. 0 -> performs walk */


inline void arm_set_translation_table_control(air_u32_t val) {
    ARM_SWITCH_REGISTERS;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "mcr p15, 0, %[val], c2, c0, 2\n\t"
        ARM_SWITCH_BACK
        : ARM_SWITCH_OUTPUT
        : [val] "r" (val)
    );
}


inline air_u32_t arm_get_translation_table_control() {
    ARM_SWITCH_REGISTERS;
    air_u32_t val;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "mrc p15, 0, %[val], c2, c0, 2\n\t"
        ARM_SWITCH_BACK
        : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
    );

    return val;
}
