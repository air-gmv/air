/*
 * Copyright (C) 2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief Abstractions for the SPARC cache control
 */

#include <air.h>
#include <printk.h>

#define LEON_CACHE_DATA_MASK        0xC
#define LEON_CACHE_INST_MASK        0x3
#define LEON_CACHE_SNOOPING_MASK    0x800000

/**
 * @brief Enable cache
 * @param type cache type
 * @returns NO_ACTION if no change in cache
 *          INVALID_CONFIG if the partition don't have cache control permission
 *          AIR_INVALID_PARAM if invalid cache type
 */
air_status_code_e air_syscall_enable_cache(air_cache_e type) {

    /* get action to apply */
    air_u32_t action = 0;
    switch (type) {

        case AIR_CACHE_ALL:
            action = LEON_CACHE_DATA_MASK | LEON_CACHE_INST_MASK | LEON_CACHE_SNOOPING_MASK;
            break;
        case AIR_CACHE_DATA:
            action = LEON_CACHE_DATA_MASK | LEON_CACHE_SNOOPING_MASK;
            break;
        case AIR_CACHE_INSTRUCTION:
            action = LEON_CACHE_INST_MASK | LEON_CACHE_SNOOPING_MASK;
            break;
        default:
            return AIR_INVALID_PARAM;
    }

    /* apply action */
    air_u32_t cache = air_sparc_get_cache_register();
    cache = (cache & ~action) | action;
    return air_sparc_set_cache_register(cache);
}

/**
 * @brief Disable cache
 * @param type cache type
 * @returns NO_ACTION if no change in cache
 *          INVALID_CONFIG if the partition don't have cache control permission
 *          AIR_INVALID_PARAM if invalid cache type
 */
air_status_code_e air_syscall_disable_cache(air_cache_e type) {

    /* get action to apply */
    air_u32_t action = 0;
    switch (type) {

        case AIR_CACHE_ALL:
            action = LEON_CACHE_DATA_MASK | LEON_CACHE_INST_MASK;
            break;
        case AIR_CACHE_DATA:
            action = LEON_CACHE_DATA_MASK;
            break;
        case AIR_CACHE_INSTRUCTION:
            action = LEON_CACHE_INST_MASK;
            break;
        default:
            return AIR_INVALID_PARAM;
    }

    /* apply action */
    air_u32_t cache = air_sparc_get_cache_register();
    cache = (cache & ~action);
    return air_sparc_set_cache_register(cache);
}

/**
 * @brief Freeze cache
 * @param type cache type
 * @returns NO_ACTION if no change in cache
 *          INVALID_CONFIG if the partition don't have cache control permission
 *          AIR_INVALID_PARAM if invalid cache type
 */
air_status_code_e air_syscall_freeze_cache(air_cache_e type) {

    /* get action to apply */
    air_u32_t clear = 0;
    air_u32_t action = 0;
    air_u32_t cache = air_sparc_get_cache_register();

    switch (type) {

        case AIR_CACHE_ALL:
            clear = LEON_CACHE_DATA_MASK | LEON_CACHE_INST_MASK;
            action = 0x5;
            break;
        case AIR_CACHE_DATA:
            clear = LEON_CACHE_DATA_MASK;
            action = 0x4;
            break;
        case AIR_CACHE_INSTRUCTION:
            clear = LEON_CACHE_INST_MASK;
            action = 0x1;
            break;
        default:
            return AIR_INVALID_PARAM;
    }

    /* check if the action is valid */
    if ((cache & clear) == 0) {

        return AIR_INVALID_MODE;
    }

    /* apply action */
    cache = (cache & ~clear) | action;
    return air_sparc_set_cache_register(cache);
}

/**
 * @brief Flush cache
 * @param type cache type
 * @returns NO_ACTION if no change in cache
 *          INVALID_CONFIG if the partition don't have cache control permission
 *          AIR_INVALID_PARAM if invalid cache type
 */
air_status_code_e air_syscall_flush_cache(air_cache_e type) {

    /* get action to apply */
    air_u32_t valid = 0;
    air_u32_t action = 0;
    switch (type) {

        case AIR_CACHE_ALL:
            valid = LEON_CACHE_DATA_MASK | LEON_CACHE_INST_MASK;
            action = 0x600000;
            break;
        case AIR_CACHE_DATA:
            valid = LEON_CACHE_DATA_MASK;
            action = 0x400000;
            break;
        case AIR_CACHE_INSTRUCTION:
            valid = LEON_CACHE_INST_MASK;
            action = 0x200000;
            break;
        default:
            return AIR_INVALID_PARAM;
    }

    /* check if flush is valid */
    air_u32_t cache = air_sparc_get_cache_register();
    if ((cache & valid) != valid) {

        return AIR_INVALID_MODE;
    }

    /* apply action */
    return air_sparc_set_cache_register((cache & ~action) | action);
}

