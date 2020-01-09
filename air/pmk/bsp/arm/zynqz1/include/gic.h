/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file gic.h
 * \author lumm
 * \brief Zynq-Board Specifics for the int Controller
 */

#ifndef GIC_H_
#define GIC_H_

#include <a9mpcore.h>
#include <gicv1.h>
#include <parameters.h>

#define ICD ((ic_distributor_t *)XPAR_PS7_SCUGIC_0_DIST_BASEADDR)
#define ICC ((ic_cpu_t *)XPAR_PS7_SCUGIC_0_BASEADDR)

/**< ************************ ICD Register Access ****************************/
/**< ICDDCR - Distributor Control Register */
__FORCE_INLINE static void arm_gic_disable(void) {
    ICD->icddcr &= ~(3U << 0);
}

__FORCE_INLINE static void arm_gic_enable(void) {
    ICD->icddcr |= (3U << 0);
}

__FORCE_INLINE static air_u32_t arm_is_gic_enabled(void) {
    return (ICD->icddcr & 0x3);
}

/**< ICDICTR - Interrupt Controller Type Register */
__FORCE_INLINE static air_u32_t arm_get_it_lines(void) {
    return (ICD->icdictr & 0x1f);
}

#define ZYNQ_MAX_INT 256
__FORCE_INLINE static air_u32_t arm_get_int_count(void) {
    air_u32_t int_count = (arm_get_it_lines() + 1) * 32;
    return (int_count <= ZYNQ_MAX_INT) ? int_count : ZYNQ_MAX_INT;
}

__FORCE_INLINE static air_u32_t arm_ic_processor_count(void) {
    return ((ICD->icdictr >> 5U) & 0x7) + 1;
}

__FORCE_INLINE static void arm_int_enable(air_u32_t id) {
    ICD->icdiser[id/32] = (1U << (id & 0x1f));
}

/**< ICDISRn - Interrupt Security Registers */
__FORCE_INLINE static void arm_int_set_grp1(void) {
    for (air_u32_t i = 0; i < arm_get_it_lines(); ++i) {
        ICD->icdisr[i] = 0xffffffff;
    }
}

__FORCE_INLINE static void arm_int_set_grp0(air_u32_t int_id) {
    ICD->icdisr[int_id/32] = (1U << int_id%32);
}

/**< ICDICER - Interrupt Clear-Enable Register */
__FORCE_INLINE static void arm_int_clear() {
    for (air_u32_t i = 0; i < arm_get_it_lines(); ++i) {
        ICD->icdicer[i] = 0xffffffff;
    }
}

__FORCE_INLINE static void arm_sgi_ppi_disable(void) {
    ICD->icdicer[0] = 0xffffffff;
}

__FORCE_INLINE static void arm_spi_disable(void) {
    for (air_u32_t i = 1; i < arm_get_it_lines(); ++i) {
        ICD->icdicer[i] = 0xffffffff;
    }
}

/**< ICDICPR  - Interrupt Clear-Pending Register*/
__FORCE_INLINE static void arm_clear_pending(void) {
    for (air_u32_t i = 1; i < arm_get_it_lines(); ++i) {
        ICD->icdicpr[i] = 0xffffffff;
    }
}

/**< ICDIPR - Interrupt Priority Register */
__FORCE_INLINE static void arm_int_set_priority(air_u32_t id, air_u8_t priority) {
    ICD->icdipr[id] = priority;
}

__FORCE_INLINE static air_u32_t arm_int_get_priority(air_u32_t id) {
    return ICD->icdipr[id];
}

__FORCE_INLINE static void arm_int_set_target(air_u32_t id, air_u32_t target) {
    ICD->icdiptr[id] = target;
}

__FORCE_INLINE static air_u32_t arm_int_get_target(air_u32_t id) {
    return ICD->icdiptr[id];
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

__FORCE_INLINE static void arm_set_ic_cpu_ctrl(air_u32_t iccicr) {
    ICC->iccicr = iccicr;
}

__FORCE_INLINE static void arm_gic_disable_interrupts(void) {
    /* not used. masking irq in psr is the way to go */
    ICC->iccicr &= ~0x1;
}

__FORCE_INLINE static void arm_gic_enable_interrupts(void) {
    /* not used. masking irq in psr is the way to go */
    ICC->iccicr |= 0x1;
}

/**< ICCPMR (GICC_PMR) - Interrupt Priority Mask Register  */
__FORCE_INLINE static air_u32_t arm_get_int_mask(void) {
    return (ICC->iccpmr & 0xff);
}

__FORCE_INLINE static void arm_set_int_mask(air_u32_t mask) {
    ICC->iccpmr = (mask & 0xff);
}

/**< ICCBPR (GICC_BPR) - Binary Point Register */
#define PREEMPTION(n_bits) ( 7 - n_bits ? (n_bits >= 0 && n_bits < 8) : 0)
__FORCE_INLINE static void arm_set_ic_cpu_preemption(air_u32_t iccbpr) {
    ICC->iccbpr = iccbpr;
}

/**< ICCIAR (GICC_IAR) - Interrupt Acknowledge Register */
__FORCE_INLINE static air_u32_t arm_acknowledge_int(void) {
    return (ICC->icciar & 0x1fff);
}

__FORCE_INLINE static void arm_end_of_int(air_u32_t ack) {
    ICC->icceoir = ack;
}

/*****************************************************************************/

void gic_init(air_u32_t cpu_id);
void arm_generate_swi(air_u32_t cpu_filter,
        air_u32_t cpu_list, air_u32_t nonsecure, air_u32_t sgi_id);

#endif /* GIC_H_ */
