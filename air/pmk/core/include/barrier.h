/*
 * Copyright (C) 2013-2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 *  @file
 *  @author pfnf
 *  @brief Defines the type and functions to handle a Centralized
 *         Sense-Reversal Barrier
 */

#ifndef __BARRIER_H__
#define __BARRIER_H__

#include <atomic.h>

/**
 * @defgroup pmk_sync PMK Synchronization API
 * @brief Synchronization directives suck as exclusion locks and barriers
 * @{
 */

/**
 *  @brief Synchronization barrier
 */
typedef struct
{

    /** maximum of cores that barrier can hold                          */
    air_u32_t size;
    /** remaining number of cores to open the barrier                   */
    atomic_t count;
    /** current global sense                                            */
    volatile air_u32_t sense;
    /** core current local sense (local lock variable)                  */
    air_u32_t local_sense[PMK_MAX_CORES];

} pmk_barrier_t;

/**
 *  @brief Barrier initialize
 *  @param barrier Barrier to initialize
 *  @param size Number of process to wait on the barrier
 *  @note The value of cannot be higher than the number of supported cores
 */
void pmk_barrier_init(pmk_barrier_t *barrier, air_u32_t size);

/**
 * @brief Barrier wait
 * @param barrier Barrier to wait on
 * @param core_id Current core id
 */
void pmk_barrier_wait(pmk_barrier_t *barrier, air_u32_t core_id);

/** @} */

#endif /* __BARRIER_H__ */
