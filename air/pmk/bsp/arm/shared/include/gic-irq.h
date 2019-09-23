/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file gic-irq.h
 * @author lumm
 * @brief Interrupt Controller (IC) Interrupt Requests (IRQ)
 */

#ifndef ARM_SHARED_GIC_IRQ_H
#define ARM_SHARED_GIC_IRQ_H

/* Software Interrupts (SGI)*/
#define ARM_GIC_IRQ_SGI_0           0
#define ARM_GIC_IRQ_SGI_1           1
#define ARM_GIC_IRQ_SGI_2           2
#define ARM_GIC_IRQ_SGI_3           3
#define ARM_GIC_IRQ_SGI_5           5
#define ARM_GIC_IRQ_SGI_6           6
#define ARM_GIC_IRQ_SGI_7           7
#define ARM_GIC_IRQ_SGI_8           8
#define ARM_GIC_IRQ_SGI_9           9
#define ARM_GIC_IRQ_SGI_10          10
#define ARM_GIC_IRQ_SGI_11          11
#define ARM_GIC_IRQ_SGI_12          12
#define ARM_GIC_IRQ_SGI_13          13
#define ARM_GIC_IRQ_SGI_14          14
#define ARM_GIC_IRQ_SGI_15          15

#define ARM_SGI_TARGET(target)      (target << 24)
#define ARM_SGI_TARGET_LIST         0x0
#define ARM_SGI_TARGET_OTHERS       0x1
#define ARM_SGI_TARGET_SELF         0x2
#define ARM_SGI_TARGET_MASK(mask)   (mask << 16)

/* Private CPU Interrupts (PPI) (for A9MPCORE) */
#define ARM_A9MPCORE_IRQ_GT         27
#define ARM_A9MPCORE_IRQ_NFIQ       28
#define ARM_A9MPCORE_IRQ_PT         29
#define ARM_A9MPCORE_IRQ_PW         30
#define ARM_A9MPCORE_IRQ_NIRQ       31

#endif /* ARM_SHARED_GIC_IRQ_H */
