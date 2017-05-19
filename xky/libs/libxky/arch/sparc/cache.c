/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file
 * @author pfnf
 * @brief Abstractions for the SPARC cache control
 */

#include <xky.h>

#define LEON_CACHE_DATA_MASK        0xC
#define LEON_CACHE_INST_MASK        0x3

/**
 * @brief Enable cache
 * @param type cache type
 * @returns NO_ACTION if no change in cache
 *          INVALID_CONFIG if the partition don't have cache control permission
 *          XKY_INVALID_PARAM if invalid cache type
 */
xky_status_code_e xky_syscall_enable_cache(xky_cache_e type) {

    /* get action to apply */
    xky_u32_t action = 0;
    switch (type) {

        case XKY_CACHE_ALL:
            action = LEON_CACHE_DATA_MASK | LEON_CACHE_INST_MASK;
            break;
        case XKY_CACHE_DATA:
            action = LEON_CACHE_DATA_MASK;
            break;
        case XKY_CACHE_INSTRUCTION:
            action = LEON_CACHE_INST_MASK;
            break;
        default:
            return XKY_INVALID_PARAM;
    }

    /* apply action */
    xky_u32_t cache = xky_sparc_get_cache_register();
    cache = (cache & ~action) | action;
    return xky_sparc_set_cache_register(cache);
}

/**
 * @brief Disable cache
 * @param type cache type
 * @returns NO_ACTION if no change in cache
 *          INVALID_CONFIG if the partition don't have cache control permission
 *          XKY_INVALID_PARAM if invalid cache type
 */
xky_status_code_e xky_syscall_disable_cache(xky_cache_e type) {

    /* get action to apply */
    xky_u32_t action = 0;
    switch (type) {

        case XKY_CACHE_ALL:
            action = LEON_CACHE_DATA_MASK | LEON_CACHE_INST_MASK;
            break;
        case XKY_CACHE_DATA:
            action = LEON_CACHE_DATA_MASK;
            break;
        case XKY_CACHE_INSTRUCTION:
            action = LEON_CACHE_INST_MASK;
            break;
        default:
            return XKY_INVALID_PARAM;
    }

    /* apply action */
    xky_u32_t cache = xky_sparc_get_cache_register();
    cache = (cache & ~action);
    return xky_sparc_set_cache_register(cache);
}

/**
 * @brief Freeze cache
 * @param type cache type
 * @returns NO_ACTION if no change in cache
 *          INVALID_CONFIG if the partition don't have cache control permission
 *          XKY_INVALID_PARAM if invalid cache type
 */
xky_status_code_e xky_syscall_freeze_cache(xky_cache_e type) {

    /* get action to apply */
    xky_u32_t clear = 0;
    xky_u32_t action = 0;
    xky_u32_t cache = xky_sparc_get_cache_register();

    switch (type) {

        case XKY_CACHE_ALL:
            clear = LEON_CACHE_DATA_MASK | LEON_CACHE_INST_MASK;
            action = 0x5;
            break;
        case XKY_CACHE_DATA:
            clear = LEON_CACHE_DATA_MASK;
            action = 0x4;
            break;
        case XKY_CACHE_INSTRUCTION:
            clear = LEON_CACHE_INST_MASK;
            action = 0x1;
            break;
        default:
            return XKY_INVALID_PARAM;
    }

    /* check if the action is valid */
    if ((cache & clear) == 0) {

        return XKY_INVALID_MODE;
    }

    /* apply action */
    cache = (cache & ~clear) | action;
    return xky_sparc_set_cache_register(cache);
}

/**
 * @brief Flush cache
 * @param type cache type
 * @returns NO_ACTION if no change in cache
 *          INVALID_CONFIG if the partition don't have cache control permission
 *          XKY_INVALID_PARAM if invalid cache type
 */
xky_status_code_e xky_syscall_flush_cache(xky_cache_e type) {

    /* get action to apply */
    xky_u32_t valid = 0;
    xky_u32_t action = 0;
    switch (type) {

        case XKY_CACHE_ALL:
            valid = LEON_CACHE_DATA_MASK | LEON_CACHE_INST_MASK;
            action = 0x600000;
            break;
        case XKY_CACHE_DATA:
            valid = LEON_CACHE_DATA_MASK;
            action = 0x400000;
            break;
        case XKY_CACHE_INSTRUCTION:
            valid = LEON_CACHE_INST_MASK;
            action = 0x200000;
            break;
        default:
            return XKY_INVALID_PARAM;
    }

    /* check if flush is valid */
    xky_u32_t cache = xky_sparc_get_cache_register();
    if ((cache & valid) != valid) {

        return XKY_INVALID_MODE;
    }

    /* apply action */
    return xky_sparc_set_cache_register((cache & ~action) | action);
}

