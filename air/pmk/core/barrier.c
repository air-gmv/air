/*
 * Copyright (C) 2013-2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file barrier.c
 * @author pfnf
 * @brief Sense-reversal barrier implementation
 */

#include <barrier.h>


void pmk_barrier_init(pmk_barrier_t *barrier, air_u32_t size) {

    int i;
    barrier->sense = 0;
    barrier->size = size;

    atomic_set(size, &barrier->count);
    for (i = 0; i < PMK_MAX_CORES; ++i) {
        barrier->local_sense[i] = barrier->sense;
    }
}

void pmk_barrier_wait(pmk_barrier_t *barrier, air_u32_t core_id) {

    /* swap local sense */
    barrier->local_sense[core_id] = ~barrier->local_sense[core_id];

    /* last core remaining ? */
    if (atomic_fetch_dec(&barrier->count) == 0) {

        atomic_set(barrier->size, &barrier->count);
        barrier->sense = barrier->local_sense[core_id];

    /* no? wait for last core */
    } else {

        while (barrier->sense != barrier->local_sense[core_id]);
    }
}
