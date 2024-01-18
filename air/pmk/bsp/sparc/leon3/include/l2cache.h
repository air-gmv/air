/*
 * Copyright (C) 2008-2015  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief L2 Cache support definitions and functions
 */

#ifndef __L2CAHCE_H__
#define __L2CAHCE_H__

/**
 * @defgroup bsp_leon_l2cache BSP - LEON 3 & 4 L2 Cache
 * @brief Board Support package L2 Cache support functions
 * @{
 */

/**
 *  @brief L2Cache Control bits enumeration
 */
typedef enum
{

    CTRL_EN = 1 << 31,   /**< Enable cache       */
    CTRL_EDAC = 1 << 30, /**< Enable EDAC        */
    CTRL_HPRHB = 1 << 5, /**< */
    CTRL_HPB = 1 << 4,   /**< */
    CTRL_UC = 1 << 3,    /**< Bus usage status   */
    CTRL_HC = 1 << 2,    /**< Hit rate status    */
    CTRL_WP = 1 << 1,    /**< Write Policy       */
    CTRL_HP = 1 << 0,    /**< */

} l2cacge_ctrl_e;

/**
 *  @brief L2 Cache Writing Policy enumeration
 */
typedef enum
{

    COPY_BACK = 0,     /**< Copy back mode     */
    WRITE_THROUGH = 1, /**< Write through mode */

} l2cache_writepol_e;

/**
 *  @brief Bus usage and hit rate status mode enumeration
 */
typedef enum
{

    SHIFTING_MODE = 0, /**< Shifting mode      */
    WRAPPING_MODE = 1, /**< Wrapping mode      */

} l2cacge_statys_mode_e;

/**
 *  @brief L2 Cache Flush mode enumeration
 */
typedef enum
{

    INVALIDATE_ONE_LINE = 0x01,
    WRITE_BACK_ONE_LINE = 0x02,
    INV_AND_WB_ONE_LINE = INVALIDATE_ONE_LINE | WRITE_BACK_ONE_LINE,
    INVALIDATE_ALL_LINES = 0x05,
    WRITE_BACK_ALL_LINES = 0x06,
    INV_AND_WB_ALL_LINES = INVALIDATE_ALL_LINES | WRITE_BACK_ALL_LINES,

} l2cache_rflush_mode_e;

/**
 *  @brief L2Cache Replacement Policy enumeration
 */
typedef enum
{

    LRU = 0,            /**< Least Recently Used                 */
    PSEUDO_RANDOM = 1,  /**< Pseudo-Random                       */
    MASTER_INDEX_0 = 2, /**< Master-Index using index replace    */
    MASTER_INDEX_1 = 3, /**< Master-Index using modules function */

} l2cache_reppol_e;

/**
 * @brief Initializes the L2 Cache if available
 * @return 0 if the device was found, 1 otherwise
 */
int l2cache_init(void);

/**
 * @brief L2 Cache control set replacement policy
 * @param repol replacement policy
 */
void l2cache_set_replacement_policy(l2cache_reppol_e repol);

/**
 * @brief Flush L2 Cache
 * @param address Address to flush (0 to flush all cache)
 * @param mode Flush mode
 * @param disable Flag to disable the cache
 */
void l2cache_flush(air_uptr_t address, l2cache_rflush_mode_e mode, int disable);

/**
 * @brief Set L2 Cache Write Policy
 * @param wp Write policy
 * @note Changing the write policy, will force an cache flush
 */
void l2cache_set_write_policy(l2cache_writepol_e wp);

/**
 * @brief Set the number of locked ways
 * @param locked Number of locked ways
 */
void l2cache_set_number_of_locked_ways(int locked);

/**
 * @brief Set the BUS usage mode
 * @param mode Mode to be seted
 */
void l2cache_set_bus_usage_mode(l2cacge_statys_mode_e mode);

/**
 * @brief Set the BUS usage mode
 * @param mode Mode to be set
 */
void l2cache_hit_rate_mode(l2cacge_statys_mode_e mode);

/**
 * @brief Enables the L2 Cache core
 * @note Enable or disable the l2 cache core, will force an cache flush
 */
void l2cache_enable_core(void);

/**
 * @brief Disable the L2 Cache core
 * @note Enable or disable the l2 cache core, will force an cache flush
 */
void l2cache_disable_core(void);

/** @} */

#endif /* __L2CAHCE_H__ */
