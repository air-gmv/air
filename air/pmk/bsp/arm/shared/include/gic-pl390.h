/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file gic-pl390.h
 * @author lumm
 * @brief Interrupt Controller (IC) available to the CPU. Based on ARM® Generic
  Interrupt Controller Architecture version 2.0 Architecture Specification and
  the PrimeCell Controller (PL390)
 */

#ifndef ARM_SHARED_GIC_PL390_H
#define ARM_SHARED_GIC_PL390_H

#include <air_arch.h>
                                    /* OFFSETS */

typedef struct {                    /* 0xf8f0 0100 */
    air_u32_t iccicr;               /* 0x000 */
    air_u32_t iccpmr;               /* 0x004 */
    air_u32_t iccbpr;               /* 0x008 */
    air_u32_t icciar;               /* 0x00c */
    air_u32_t icceoir;              /* 0x010 */
    air_u32_t iccrpr;               /* 0x014 */
    air_u32_t icchpir;              /* 0x018 */
    air_u32_t iccabpr;              /* 0x01c */
    air_u32_t reserved[55];         /* 0x020 - 0x0f8 */
    air_u32_t iccidr;               /* 0x0fc */
} ic_cpu_interface_t;

typedef struct {                    /* 0xf8f0 1000*/
    air_u32_t icddcr;               /* 0x000 */
    air_u32_t icdictr;              /* 0x004 */
    air_u32_t icdiidr;              /* 0x008 */
    air_u32_t reserved0[29];        /* 0x00c - 0x07c */
    air_u32_t icdisr[32];           /* 0x080 - 0x0fc */
    air_u32_t icdiser[32];          /* 0x100 - 0x17c */
    air_u32_t icdicer[32];          /* 0x180 - 0x1fc */
    air_u32_t icdispr[32];          /* 0x200 - 0x27c */
    air_u32_t icdicpr[32];          /* 0x280 - 0x2fc */
    air_u32_t icdabr[32];           /* 0x300 - 0x37c */
    air_u32_t reserved1[32];        /* 0x380 - 0x3fc */
    air_u8_t icdipr[1020];          /* 0x400 - 0x7fb */
    air_u32_t reserved2;            /* 0x7fc */
    air_u8_t icdiptr[1020];         /* 0x800 - 0xbfb */
    air_u32_t reserved3;            /* 0xbfc */
    air_u32_t icdicfr[64];          /* 0xc00 - 0xcfc */
    air_u32_t reserved4[128];       /* 0xd00 - 0xefc */
    air_u32_t icdsgir;              /* 0xf00 */
} ic_distributor_t;

#endif /* ARM_SHARED_GIC_PL390_H */
