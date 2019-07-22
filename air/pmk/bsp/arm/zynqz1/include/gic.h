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

#include <air_arch.h>
#include <gic-irq.h>
#include <gic-pl390.h>
#include <gic-zynq.h>
#include <bsp.h>

static volatile ic_distributor_t *ic_dist = (ic_distributor_t *)IC_DIST_BASE_MEMORY;
static volatile ic_cpu_interface_t *ic_cpu = (ic_cpu_interface_t *)IC_CPU_BASE_MEMORY;

/**< ************************ ICD Register Access ****************************/
/**< ICDDCR */
static inline void arm_gic_disable(void) {
    ic_dist->icddcr &= ~(1U << 0);
}

static inline void arm_gic_enable(void) {
    ic_dist->icddcr |= (3U << 0);
}

static inline air_u32_t arm_is_gic_enabled(void) {
    return (ic_dist->icddcr & 0x1);
}

/**< ICDICTR */
#define ZYNQ_MAX_INT 256
static inline air_u32_t arm_get_int_count(void) {
    air_u32_t int_count = ((ic_dist->icdictr & 0x1f) + 1) * 32;
    return int_count <= ZYNQ_MAX_INT ? int_count : ZYNQ_MAX_INT;
}

static inline air_u32_t arm_ic_processor_count(void) {
    return ((ic_dist->icdictr << 5U) & 0x7) + 1;
}

static inline void arm_int_enable(air_u32_t id) {
    ic_dist->icdiser[id/32] = (1U << (id & 0x1f));
}

/**< ICDICER */
static inline void arm_sgi_ppi_disable(void) {
    ic_dist->icdicer[0] = 0xffffffff;
}

static inline void arm_spi_disable(air_u32_t int_count) {
    for(air_u32_t i = 32; i < int_count; i += 32) {
        ic_dist->icdicer[i/32] = 0xffffffff;
    }
}

/**< ICDICPR */
static inline void arm_clear_pending(air_u32_t int_count) {
    for(air_u32_t i = 32; i < int_count; i += 32) {
        ic_dist->icdicpr[i/32] = 0xffffffff;
    }
}

/**< ICDIPR */
static inline void arm_int_set_priority(air_u32_t id, air_u8_t priority) {
    ic_dist->icdipr[id] = priority;
}

static inline air_u32_t arm_int_get_priority(air_u32_t id) {
    return ic_dist->icdipr[id];
}

static inline void arm_int_set_target(air_u32_t id, air_u32_t target) {
    ic_dist->icdiptr[id] = target;
}

static inline air_u32_t arm_int_get_target(air_u32_t id) {
    return ic_dist->icdiptr[id];
}


/**< ************************ ICC Register Access ****************************/
/**< ICCICR (GICC_CTRL) - CPU interface control*/
#define ICCICR_ENABLE_GRP0      (1U << 0)
#define ICCICR_ENABLE_GRP1      (1U << 1)
#define ICCICR_FIQ_EN           (1U << 3)
#define ICCICR_CBPR             (1U << 4)
#define ICCICR_FIQ_BYP_GRP0     (1U << 5)
#define ICCICR_IRQ_BYP_GRP0     (1U << 6)
#define ICCICR_FIQ_BYP_GRP1     (1U << 7)
#define ICCICR_IRQ_BYP_GRP1     (1U << 8)
#define ICCICR_EOI_S            (1U << 9)
#define ICCICR_EOI_NS           (1U << 10)

static inline void arm_set_ic_cpu_ctrl(air_u32_t iccicr) {
    ic_cpu->iccicr = iccicr;
}

static inline void arm_gic_disable_interrupts(void) {
    /* not used. masking irq in psr is the way to go */
    ic_cpu->iccicr &= ~0x1;
}

static inline void arm_gic_enable_interrupts(void) {
    /* not used. masking irq in psr is the way to go */
    ic_cpu->iccicr |= 0x3;
}

/**< ICCPMR (GICC_PMR) */
static inline air_u32_t arm_get_int_mask(void) {
    return (ic_cpu->iccpmr & 0xff);
}

static inline void arm_set_int_mask(air_u32_t mask) {
    ic_cpu->iccpmr = (mask & 0xff);
}

//*< ICCBPR (GICC_BPR) */
#define PREEMPTION(n_bits) ( 7 - n_bits ? (n_bits >= 0 && n_bits < 8) : 0)
static inline void arm_set_ic_cpu_preemption(air_u32_t iccbpr) {
    ic_cpu->iccbpr = iccbpr;
}

/**< ICCIAR (GICC_IAR) */
static inline air_u32_t arm_acknowledge_int(void) {
    return (ic_cpu->icciar & 0x1fff);
}

static inline void arm_end_of_int(air_u32_t ack) {
    ic_cpu->icceoir = ack;
}

/*****************************************************************************/

void gic_init(air_u32_t cpu_id);
void arm_generate_swi(air_u32_t cpu_filter,
        air_u32_t cpu_list, air_u32_t nonsecure, air_u32_t sgi_id);

#endif /* ARM_ZYNQ_Z1_GIC_H */
