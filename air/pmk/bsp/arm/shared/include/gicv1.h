/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file gicv1.h
 * \author lumm
 * \brief General Interrupt Controller (GIC) architecture version 1
 */

#ifndef GICV1_H_
#define GICV1_H_

#ifndef PMK_ARM_ULTRASCALE96V2_A53
#include <a9mpcore.h>
#endif /* PMK_ARM_ULTRASCALE96V2_A53 */

                                        /* OFFSETS */
typedef struct {                        /* 0xf8f0 0100 */
    air_u32_t iccicr;                   /* 0x000 */
    air_u32_t iccpmr;                   /* 0x004 */
    air_u32_t iccbpr;                   /* 0x008 */
    volatile air_u32_t icciar;          /* 0x00c */
    volatile air_u32_t icceoir;         /* 0x010 */
    volatile air_u32_t iccrpr;          /* 0x014 */
    volatile air_u32_t icchpir;         /* 0x018 */
    air_u32_t iccabpr;                  /* 0x01c */
    air_u32_t reserved[55];             /* 0x020 - 0x0f8 */
    air_u32_t icciidr;                  /* 0x0fc */
} ic_cpu_t;

typedef struct
{                                     /* 0xf8f0 1000*/
    volatile air_u32_t icddcr;        /* 0x000 */
    volatile air_u32_t icdictr;       /* 0x004 */
    volatile air_u32_t icdiidr;       /* 0x008 */
    volatile air_u32_t reserved0[29]; /* 0x00c - 0x07c */
    volatile air_u32_t icdisr[32];    /* 0x080 - 0x0fc */
    volatile air_u32_t icdiser[32];   /* 0x100 - 0x17c */
    volatile air_u32_t icdicer[32];   /* 0x180 - 0x1fc */
    volatile air_u32_t icdispr[32];   /* 0x200 - 0x27c */
    volatile air_u32_t icdicpr[32];   /* 0x280 - 0x2fc */
    volatile air_u32_t icdabr[32];    /* 0x300 - 0x37c */
    volatile air_u32_t reserved1[32]; /* 0x380 - 0x3fc */
    volatile air_u8_t icdipr[1020];   /* 0x400 - 0x7fb */
    volatile air_u32_t reserved2;     /* 0x7fc */
    volatile air_u8_t icdiptr[1020];  /* 0x800 - 0xbfb */
    volatile air_u32_t reserved3;     /* 0xbfc */
    volatile air_u32_t icdicfr[64];   /* 0xc00 - 0xcfc */
    volatile air_u32_t reserved4[64]; /* 0xd00 - 0xdfc */
    volatile air_u32_t reserved5[64]; /* 0xe00 - 0xefc */
    volatile air_u32_t icdsgir;       /* 0xf00 */
    volatile air_u32_t reserved6[51]; /* 0xf04 - 0xfcc */
    air_u32_t icpidr[8];              /* 0xfd0 - 0xfec */
    air_u32_t iccidr[4];              /* 0xff0 - 0xfff */
} ic_distributor_t;

/* Software Interrupts (SGI)*/
#define ARM_GIC_IRQ_SGI_0 0
#define ARM_GIC_IRQ_SGI_1 1
#define ARM_GIC_IRQ_SGI_2 2
#define ARM_GIC_IRQ_SGI_3 3
#define ARM_GIC_IRQ_SGI_5 5
#define ARM_GIC_IRQ_SGI_6 6
#define ARM_GIC_IRQ_SGI_7 7
#define ARM_GIC_IRQ_SGI_8 8
#define ARM_GIC_IRQ_SGI_9 9
#define ARM_GIC_IRQ_SGI_10 10
#define ARM_GIC_IRQ_SGI_11 11
#define ARM_GIC_IRQ_SGI_12 12
#define ARM_GIC_IRQ_SGI_13 13
#define ARM_GIC_IRQ_SGI_14 14
#define ARM_GIC_IRQ_SGI_15 15

#define ARM_SGI_TARGET(target) (target << 24)
#define ARM_SGI_TARGET_LIST 0x0
#define ARM_SGI_TARGET_OTHERS 0x1
#define ARM_SGI_TARGET_SELF 0x2
#define ARM_SGI_TARGET_MASK(mask) (mask << 16)

#ifndef PMK_ARM_ULTRASCALE96V2_A53
/* Private CPU Interrupts (PPI) (for A9MPCORE) */
<<<<<<< HEAD
#define ARM_A9MPCORE_IRQ_GT         27
#define ARM_A9MPCORE_IRQ_NFIQ       28
#define ARM_A9MPCORE_IRQ_PT         29
#define ARM_A9MPCORE_IRQ_PW         30
#define ARM_A9MPCORE_IRQ_NIRQ       31
#endif /* PMK_ARM_ULTRASCALE96V2_A53 */
=======
#define ARM_A9MPCORE_IRQ_GT 27
#define ARM_A9MPCORE_IRQ_NFIQ 28
#define ARM_A9MPCORE_IRQ_PT 29
#define ARM_A9MPCORE_IRQ_PW 30
#define ARM_A9MPCORE_IRQ_NIRQ 31
>>>>>>> dev

/* Shared Peripheral Interrupts (SPI)*/
#define ZYNQ_IRQ_CPU_0 32
#define ZYNQ_IRQ_CPU_1 33
#define ZYNQ_IRQ_L2_CACHE 34
#define ZYNQ_IRQ_OCM 35
#define ZYNQ_IRQ_PMU_0 37
#define ZYNQ_IRQ_PMU_1 38
#define ZYNQ_IRQ_XADC 39
#define ZYNQ_IRQ_DVI 40
#define ZYNQ_IRQ_SWDT 41
#define ZYNQ_IRQ_TTC_0_0 42
#define ZYNQ_IRQ_TTC_0_1 43
#define ZYNQ_IRQ_TTC_0_2 44
#define ZYNQ_IRQ_DMAC_ABORT 45
#define ZYNQ_IRQ_DMAC_0 46
#define ZYNQ_IRQ_DMAC_1 47
#define ZYNQ_IRQ_DMAC_2 48
#define ZYNQ_IRQ_DMAC_3 49
#define ZYNQ_IRQ_SMC 50
#define ZYNQ_IRQ_QUAD_SPI 51
#define ZYNQ_IRQ_GPIO 52
#define ZYNQ_IRQ_USB_0 53
#define ZYNQ_IRQ_ETHERNET_0 54
#define ZYNQ_IRQ_ETHERNET_0_WAKEUP 55
#define ZYNQ_IRQ_SDIO_0 56
#define ZYNQ_IRQ_I2C_0 57
#define ZYNQ_IRQ_SPI_0 58
#define ZYNQ_IRQ_UART_0 59
#define ZYNQ_IRQ_CAN_0 60
#define ZYNQ_IRQ_FPGA_0 61
#define ZYNQ_IRQ_FPGA_1 62
#define ZYNQ_IRQ_FPGA_2 63
#define ZYNQ_IRQ_FPGA_3 64
#define ZYNQ_IRQ_FPGA_4 65
#define ZYNQ_IRQ_FPGA_5 66
#define ZYNQ_IRQ_FPGA_6 67
#define ZYNQ_IRQ_FPGA_7 68
#define ZYNQ_IRQ_TTC_1_0 69
#define ZYNQ_IRQ_TTC_1_1 70
#define ZYNQ_IRQ_TTC_1_2 71
#define ZYNQ_IRQ_DMAC_4 72
#define ZYNQ_IRQ_DMAC_5 73
#define ZYNQ_IRQ_DMAC_6 74
#define ZYNQ_IRQ_DMAC_7 75
#define ZYNQ_IRQ_USB_1 76
#define ZYNQ_IRQ_ETHERNET_1 77
#define ZYNQ_IRQ_ETHERNET_1_WAKEUP 78
#define ZYNQ_IRQ_SDIO_1 79
#define ZYNQ_IRQ_I2C_1 80
#define ZYNQ_IRQ_SPI_1 81
#define ZYNQ_IRQ_UART_1 82
#define ZYNQ_IRQ_CAN_1 83
#define ZYNQ_IRQ_FPGA_8 84
#define ZYNQ_IRQ_FPGA_9 85
#define ZYNQ_IRQ_FPGA_10 86
#define ZYNQ_IRQ_FPGA_11 87
#define ZYNQ_IRQ_FPGA_12 88
#define ZYNQ_IRQ_FPGA_13 89
#define ZYNQ_IRQ_FPGA_14 90
#define ZYNQ_IRQ_FPGA_15 91
#define ZYNQ_IRQ_PARITY 92

#endif /* GICV1_H_ */
