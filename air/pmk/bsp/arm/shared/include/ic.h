/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file ic.h
 * @author lumm
 * @brief Interrupt Controller (IC) available to the CPU. Based on ARM® Generic
  Interrupt Controller Architecture version 2.0 Architecture Specification
 */

#ifndef ARM_IC_H
#define ARM_IC_H

#include <air_arch.h>
#include <bsp.h>

typedef struct {
    air_u32_t iccicr;
    air_u32_t iccpmr;
    air_u32_t iccbpr;
    air_u32_t icciar;
    air_u32_t icceoir;
    air_u32_t iccrpr;
    air_u32_t icchpir;
    air_u32_t iccabpr;
    air_u32_t reserved_20[55];
    air_u32_t iccidr;
} ic_cpu_interface_t;

typedef struct {
    air_u32_t icddcr;
    air_u32_t icdictr;
    air_u32_t icdiidr;
    air_u32_t reserved_0c[29];
    air_u32_t icdisr[32];
    air_u32_t icdiser[32];
    air_u32_t icdicer[32];
    air_u32_t icdispr[32];
    air_u32_t icdicpr[32];
    air_u32_t icdabr[32];
    air_u32_t reserved_380[32];
    air_u32_t icdipr[64];
    air_u32_t reserved_500[192];
    air_u32_t icdiptr[64];
    air_u32_t reserved_900[192];
    air_u32_t icdicfr[64];
    air_u32_t reserved_d00[128];
    air_u32_t icdsgir;
} ic_distributor_t;

#define NUMBER_INTERRUPTS 96
#define MAX_ICDICER ( (NUMBER_INTERRUPTS%32) ? \
                    (NUMBER_INTERRUPTS/32) + 1 : \
                    (NUMBER_INTERRUPTS/32) )

static inline air_u32_t ic_processor_count(void) {

    volatile ic_distributor_t *ic_dist = (ic_distributor_t *)IC_DIST_BASE_MEMORY;

    return ((ic_dist->icdictr << 5U) & 0x7) + 1;
}


static inline void clean_ppi(void) {

    volatile ic_distributor_t *ic_dist = (ic_distributor_t *)IC_DIST_BASE_MEMORY;
    ic_dist->icdicer[0] = (0xffff << 16);
}

static inline void clean_spi(void) {

    volatile ic_distributor_t *ic_dist = (ic_distributor_t *)IC_DIST_BASE_MEMORY;
    for(air_u32_t i = 1; i < MAX_ICDICER; ++i) {
        ic_dist->icdicer[i] = 0xffffffff;
    }
}

static inline void gic_disable(void) {

    volatile ic_distributor_t *ic_dist = (ic_distributor_t *)IC_DIST_BASE_MEMORY;
    ic_dist->icddcr = 0x0;
}

static inline void gic_enable(void) {

    volatile ic_distributor_t *ic_dist = (ic_distributor_t *)IC_DIST_BASE_MEMORY;
    ic_dist->icddcr = 0x1;
}

#endif /* ARM_IC_H */
