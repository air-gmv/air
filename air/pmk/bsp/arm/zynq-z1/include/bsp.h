/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution.
 * ==========================================================================*/
/**
 * @file bsp.h
 * @author lumm
 * @brief Bsp headers and inline assembly functions.
 */

 #include <arm.h>

static inline uint32_t arm_cp15_get_sctlr(void) {
    uint32_t val;

    __asm__ volatile (
        THUMB_TO_ARM
        "mrc p15, 0, %[val], c1, c0, 0\n"
        ARM_TO_THUMB
        : [val] "=&r" (val)
    );

    return val;
}
