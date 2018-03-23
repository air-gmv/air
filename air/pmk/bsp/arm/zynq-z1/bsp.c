/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution.
 * ==========================================================================*/
/**
 * @file bsp.c
 * @author lumm
 * @brief Bsp core routines.
 */

#include <bsp.h>

int bsp_start_hook_0(void) {

    uint32_t sctlr = arm_cp15_get_sctlr();

    /* @brief Disable caches and mmu if they are enabled */
    if (sctlr & (ARM_CP15_CTRL_I | ARM_CP15_CTRL_C | ARM_CP15_CTRL_M)) {
        if (sctlr & (ARM_CP15_CTRL_C | ARM_CP15_CTRL_M)) {

        }
    }
}
