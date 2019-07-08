/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file gic.h
 * @author lumm
 * @brief Zynq-Board Specifics for the int Controller
 */

#ifndef ARM_ZYNQ_Z1_GIC_H
#define ARM_ZYNQ_Z1_GIC_H

#include <gic-irq.h>
#include <gic-pl390.h>
#include <gic-zynq.h>

volatile ic_distributor_t *ic_dist = (ic_distributor_t *)IC_DIST_BASE_MEMORY;
volatile ic_cpu_interface_t *ic_cpu = (ic_cpu_interface_t *)IC_CPU_BASE_MEMORY;


#define ZYNQ_MAX_INT 256
#define MAX_INT ZYNQ_MAX_INT
static inline air_u32_t get_int_count(void) {

    air_u32_t int_count = ((ic_dist->icdictr & 0x1f) + 1) * 32;
    return int_count <= MAX_INT ? int_count : MAX_INT;
}

static inline void ppi_disable(void) {
    ic_dist->icdicer[0] = (0xffff << 16);
}

static inline void spi_disable(air_u32_t int_count) {
    for(air_u32_t i = 1; i < int_count/32; ++i) {
        ic_dist->icdicer[i] = 0xffffffff;
    }
}

static inline void int_set_priority(air_u32_t line, air_u32_t mask) {
    ic_dist->icdipr[line] = mask;
}

static inline air_u32_t int_get_priority(air_u32_t line) {
    return ic_dist->icdipr[line];
}

static inline void int_set_target(air_u32_t line, air_u32_t mask) {
    ic_dist->icdiptr[line] = mask;
}

static inline air_u32_t int_get_target(air_u32_t line) {
    return ic_dist->icdiptr[line];
}

static inline void gic_disable(void) {
    ic_dist->icddcr = 0x0;
}

static inline void gic_enable(void) {
    ic_dist->icddcr = 0x1;
}

static inline air_u32_t is_gic_enabled(void) {
    return (ic_dist->icddcr & 0x1);
}

static inline air_u32_t acknowledge_int(void) {
    return (ic_cpu->icciar & 0x1fff);
}

static inline air_u32_t ic_processor_count(void) {
    return ((ic_dist->icdictr << 5U) & 0x7) + 1;
}

void gic_init(air_u32_t cpu_id);

#endif /* ARM_ZYNQ_Z1_GIC_H */
