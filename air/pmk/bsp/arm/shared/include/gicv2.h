/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file gicv2.h
 * \author ansi
 * \brief General Interrupt Controller (GIC) architecture version 2
 */

#ifndef GICV2_H_
#define GICV2_H_

#include <armv7.h>

// CPU interface registers (table 9-2 on page 9-4 of Cortex-A53 TRM)
                                          /* OFFSETS */
typedef struct {                          
    air_u32_t gicc_ctlr;                  /* 0x000 */ //CPU Interface Control Register
    air_u32_t gicc_pmr;                   /* 0x004 */ //Interrupt Priority Mask Register
    air_u32_t gicc_bpr;                   /* 0x008 */ //Binary Point Register
    volatile air_u32_t gicc_iar;          /* 0x00c */ //Interrupt Acknowledge Register
    volatile air_u32_t gicc_eoir;         /* 0x010 */ //End of Interrupt Register
    volatile air_u32_t gicc_rpr;          /* 0x014 */ //Running Priority Register
    volatile air_u32_t gicc_hppir;        /* 0x018 */ //Highest Priority Pending Interrupt Register
    air_u32_t gicc_abpr;                  /* 0x01c */ //Aliased Binary Point Register
    air_u32_t gicc_aiar;                  /* 0x020 */ //Aliased Interrupt Acknowledge Register
    air_u32_t gicc_aeoir;                 /* 0x024 */ //Aliased End of Interrupt Register
    air_u32_t gicc_ahppir;                /* 0x028 */ //Aliased Highest Priority Pending Interrupt Register
    volatile air_u32_t reserved0[40];       /* 0x02c - 0x0cf */
    
    // GICV2 defines 3 gicc_apr registers, but the A53 TRM defines only one
    air_u32_t gicc_apr[3];                   /* 0x00D0 - 0x00dc*/ //Active Priority Register
    // The same happens with the gicc_nsapr registers
    air_u32_t gicc_nsapr[3];                 /* 0x00E0 - 0x00ec */ //Non-secure Active Priority Register
    volatile air_u32_t reserved1[3];       /* 0x00ed - 0x00f8 */
    
    // Implementation defined registers
    air_u32_t gicc_iidr;                  /* 0x0fc */ //CPU Interface Identification Register
    air_u32_t gicc_dir;                   /* 0x1000 */ //Deactivate Interrupt Register
    ////
} ic_cpu_t;

// GIC400 Module Registers (on Zynq Ultrascale+ Registers Reference)
                                           /* OFFSETS */
typedef struct {                           /* 0x00F9000000 (ACPU_GIC)*/
    volatile air_u32_t gicd_ctlr;          /* 0x10000 */ //Distributor Control Register
    volatile air_u32_t gicd_typer;         /* 0x10004 */ //Interrupt Controller Type Register
    volatile air_u32_t gicd_iidr;          /* 0x10008 */ //Distributor Implementer Identification Register
    volatile air_u32_t reserved0[29];      /* 0x1000c - 0x1007c */
    volatile air_u32_t gicd_igroup[7];     /* 0x10080 - 0x100fc */ //Interrupt Group Registers
    volatile air_u32_t gicd_isenabler[7];  /* 0x10100 - 0x1017c */ //Interrupt Set-Enable Registers
    volatile air_u32_t gicd_icenabler[7];  /* 0x10180 - 0x101fc */ //Interrupt Clear-Enable Registers
    volatile air_u32_t gicd_ispendr[7];    /* 0x10200 - 0x1027c */ //Interrupt Set-Pending Registers
    volatile air_u32_t gicd_icpendr[7];    /* 0x10280 - 0x102fc */ //Interrupt Clear-Pending Registers
    volatile air_u32_t gicd_isactiver[7];  /* 0x10300 - 0x1037c */ //Interrupt Set-Active Registers
    volatile air_u32_t gicd_icactiver[7];  /* 0x10380 - 0x103fc */ //Interrupt Clear-Active Registers
    volatile air_u8_t gicd_ipriority[48];  /* 0x10400 - 0x107f8 */ //Interrupt Priority Registers
    volatile air_u32_t reserved1;          /* 0x107fc */
    volatile air_u8_t gicd_itargetsr[48];  /* 0x10800 - 0x10bf8 */ //Interrupt Processor Targets Registers
    volatile air_u32_t reserved2;          /* 0x10bfc */
    volatile air_u32_t gicd_icfgr[12];     /* 0x10c00 - 0x10cfc */ //Interrupt Configuration Registers
    volatile air_u32_t reserved3[63];      /* 0x10d00 - 0x10dfc */

    // Implementation defined registers
    volatile air_u32_t gicd_ppisr;         /* 0x10d00 */ //Private Peripheral Interrupt Status Registers
    volatile air_u32_t gicd_spisr[5];      /* 0x10d04 - 0x10d14 */ //Shared Peripheral Interrupt Status Registers
    volatile air_u32_t reserved4[57];      /* 0x10d18 - 0x10dfc */
    ////
    
    volatile air_u32_t gicd_sgir;          /* 0x10f00 */ //Software Generated Interrupt Register (SGI)
    volatile air_u32_t reserved5[8];       /* 0x10f04 - 0x10f0c */
    volatile air_u32_t gicd_cpendsgir[4];  /* 0x10f10 - 0x10f1c */ //SGI Clear-Pendind Registers 
    volatile air_u32_t gicd_spendsgir[4];  /* 0x10f20 - 0x10f2c */ //SGI Set-Pendind Registers 
    volatile air_u32_t reserved6[39];      /* 0x10f30 - 0x10fcc */
    
    // Implementation defined registers
    volatile air_u32_t gicd_pidr4_7[4];   /* 0x10fd0 - 0x10fdc */ //Peripheral Identification 4 to 7 Registers 
    volatile air_u32_t gicd_pidr0_3[4];   /* 0x10fe0 - 0x10fec */ //Peripheral Identification 0 to 3 Registers 
    volatile air_u32_t gicd_cidr[4];      /* 0x10ff0 - 0x10ffc */ //Component Identification Registers 
    ////
} ic_distributor_t;

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

/* Fields of the gicd_sgir register */
/* See Table 4-21 on GICv2 Architecture Specification for details*/
#define ARM_SGI_TARGET(target)      (target << 24)
#define ARM_SGI_TARGET_LIST         0x0
#define ARM_SGI_TARGET_OTHERS       0x1
#define ARM_SGI_TARGET_SELF         0x2
#define ARM_SGI_TARGET_MASK(mask)   (mask << 16)


/* Private CPU Interrupts (PPI) */
// Defined in the Zynq Ultrascale+ Device TRM (Interrupts section)
#define ARM_A53_IRQ_VMI      25 // Virtual Maintenance Interrupt
#define ARM_A53_IRQ_HT       26 // Hypervisor Timer
#define ARM_A53_IRQ_VT       27 // Virtual Timer
#define ARM_A53_IRQ_LFIQ     28 // Legacy FIQ
#define ARM_A53_IRQ_SPT      29 // Secure Physical Timer
#define ARM_A53_IRQ_NSPT     30 // Non-Secure Physical Timer
#define ARM_A53_IRQ_LIRQ     31 // Legacy IRQ

/* Shared Peripheral Interrupts (SPI)*/
// Interrupt ids are defined in the "xparameters_ps.h" file

#endif /* GICV2_H_ */
