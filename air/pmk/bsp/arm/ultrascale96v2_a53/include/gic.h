/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file gic.h
 * \author ansi
 * \brief Ultrascale-Board Specifics for the Interrupt Controller
 */

#ifndef GIC_H_
#define GIC_H_

#include <gicv2.h>
#include <parameters.h>

#define ICD ((ic_distributor_t *)XPAR_PSU_ACPU_GIC_DIST_BASEADDR)
#define ICC ((ic_cpu_t *)XPAR_PSU_ACPU_GIC_BASEADDR)

/**< ************************ ICD Register Access ****************************/
/**< ICDDCR - Distributor Control Register */
__FORCE_INLINE static void arm_gic_disable(void) {
    ICD->gicd_ctlr &= ~(3U << 0); //0b11..1100
}

__FORCE_INLINE static void arm_gic_enable(void) {
    ICD->gicd_ctlr |= 0b11; // (3U << 0)
}

__FORCE_INLINE static air_u32_t arm_is_gic_enabled(void) {
    return (ICD->gicd_ctlr & 0x3);
}

/**< GICD_TYPER - Interrupt Controller Type Register */
__FORCE_INLINE static air_u32_t arm_get_it_lines(void) {
    // Indicates the maximum number of SPIs that the gic supports
    // Determines the number of instances of registers like the isenabler, icenabler and so on 
    return (ICD->gicd_typer & 0b0011111); //& 00011111, aka get 5 first bits 
}

#define ZYNQ_MAX_INT 256
__FORCE_INLINE static air_u32_t arm_get_int_count(void) {
    //Maximum number of interrupts that the GIC Supports
    air_u32_t int_count = (arm_get_it_lines() + 1) * 32;
    return int_count;
}

__FORCE_INLINE static air_u32_t arm_ic_processor_count(void) {
    return ((ICD->gicd_typer >> 5U) & 0x7) + 1;
}

/* GICD_ISENABLER - Interrupt Set-Enable Registers*/
__FORCE_INLINE static void arm_int_enable(air_u32_t id) {
    ICD->gicd_isenabler[id/32] = (1U << (id % 32));
}

/**< GICD_IGROUP - Interrupt Group Registers */
__FORCE_INLINE static void arm_int_set_grp1(void) {
    for (air_u32_t i = 0; i < arm_get_it_lines()+1; ++i) {
        ICD->gicd_igroup[i] = 0xffffffff;
    }
}

__FORCE_INLINE static void arm_int_set_grp0(air_u32_t int_id) {
    ICD->gicd_igroup[int_id/32] = (1U << int_id % 32);
}

/**< ICDICER - Interrupt Clear-Enable Register */
__FORCE_INLINE static void arm_int_clear() {
    for (air_u32_t i = 0; i < arm_get_it_lines()+1; ++i) {
        ICD->gicd_icenabler[i] = 0xffffffff;
    }
}

// Disables SGIs with ids 15 - 0
// Disables PPIs with ids 31 - 16
__FORCE_INLINE static void arm_sgi_ppi_disable(void) {
    ICD->gicd_icenabler[0] = 0xffffffff;
}

// Disables SPIs with ids starting at id 32
__FORCE_INLINE static void arm_spi_disable(void) {
    for (air_u32_t i = 1; i < arm_get_it_lines()+1; ++i) {
        ICD->gicd_icenabler[i] = 0xffffffff;
    }
}

/*Interrupt Clear-Pending Register*/
// Clears all pending interrupts
__FORCE_INLINE static void arm_clear_pending(void) {
    for (air_u32_t i = 0; i < arm_get_it_lines()+1; ++i) {
        ICD->gicd_icpendr[i] = 0xffffffff;
    }
}

/**< ICDIPR - Interrupt Priority Register */
__FORCE_INLINE static void arm_int_set_priority(air_u32_t id, air_u8_t priority) {
    ICD->gicd_ipriority[id] = priority;
}

__FORCE_INLINE static air_u32_t arm_int_get_priority(air_u32_t id) {
    return ICD->gicd_ipriority[id];
}

__FORCE_INLINE static void arm_int_set_target(air_u32_t id, air_u32_t target) {
    ICD->gicd_itargetsr[id] = target;
}

__FORCE_INLINE static air_u32_t arm_int_get_target(air_u32_t id) {
    return ICD->gicd_itargetsr[id];
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

__FORCE_INLINE static void arm_set_ic_cpu_ctrl(air_u32_t gicc_ctlr) {
    ICC->gicc_ctlr = gicc_ctlr;
}

__FORCE_INLINE static void arm_gic_disable_interrupts(void) {
    /* not used. masking irq in psr is the way to go */
    ICC->gicc_ctlr &= ~0x1;
}

__FORCE_INLINE static void arm_gic_enable_interrupts(void) {
    /* not used. masking irq in psr is the way to go */
    ICC->gicc_ctlr |= 0x1;
}

/**< gicc_pmr - Interrupt Priority Mask Register  */
__FORCE_INLINE static air_u32_t arm_get_int_mask(void) {
    return (ICC->gicc_pmr & 0xff);
}

__FORCE_INLINE static void arm_set_int_mask(air_u32_t mask) {
    ICC->gicc_pmr = (mask & 0xff);
}

/**< gicc_bpr - Binary Point Register */
#define PREEMPTION(n_bits) ( 7 - n_bits ? (n_bits >= 0 && n_bits < 8) : 0)
__FORCE_INLINE static void arm_set_ic_cpu_preemption(air_u32_t gicc_bpr) {
    ICC->gicc_bpr = gicc_bpr;
}

/**< gicc_iar - Interrupt Acknowledge Register */
__FORCE_INLINE static air_u32_t arm_acknowledge_int(void) {
    return (ICC->gicc_iar & 0x1fff); // 0x1fff = 0001 1111 1111 1111, aka get 13 first bits (CPUID and INTID)
}

__FORCE_INLINE static void arm_end_of_int(air_u32_t ack) {
    ICC->gicc_eoir = ack;
}

/*****************************************************************************/

void gic_init(air_u32_t cpu_id);

void arm_generate_swi(air_u32_t cpu_filter,
        air_u32_t cpu_list, air_u32_t nonsecure, air_u32_t sgi_id);

#endif /* GIC_H_ */
