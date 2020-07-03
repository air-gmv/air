/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/AIR/raw/master/air/LICENSE
 */
/**
 * \file mmu.h
 * \author lumm
 * \brief ARM virtualization MMU tables
 */

#ifndef MMU_H_
#define MMU_H_

#include <armv7.h>

/* Division between the two Translation Tables (N == 0 means only one tt) */
#define TTBR_N              0 /* 0 <= N < 8 */

#define TTBR0_SIZE          (1U << (14 - TTBR_N))
#if TTBR_N
#define TTBR1_SIZE          (1U << 14)
#else
#define TTBR1_SIZE          0
#endif
#define TTBR0_ENTRIES       (1U << (12-TTBR_N))
#define TTBR1_ENTRIES       (1U << 12)

#define TTL1_SECT_SIZE      (1U << 20)
#define TTL2_SIZE           (1U << 10)
#define TTL2_ENTRIES        (1U << 8)
#define TTL2_MEM            (1U << 20)
#define TTL2_LPAGE_SIZE     (1U << 16)
#define TTL2_PAGE_SIZE      (1U << 12)

#define TTBCR_PD            0 /* if tbl miss performs a table walk. 0 -> performs walk */

/*************************** page related functions ***************************
 * \brief Paging functions
 *      Small page will have 4K and be on the level 2 page
 *      Big page (section) will be on 1st level and has 1MB
 */
#define PAGE_SIZE           (4*1024)    /* 4KB */
#define LPAGE_SIZE          (64*1024)   /* 64KB */
#define SECTION_SIZE        (1024*1024) /* 1MB */

#define DEFAULT_DOMAIN      15U

#define DAC_NO_ACCESS       0b00
#define DAC_CLIENT          0b01
#define DAC_MANAGER         0b11

#define VA2L1TABLE(address) ((address & 0xfff00000) >> 20)
#define VA2L2TABLE(address) ((address & 0x000ff000) >> 12)

/**
 * \brief Descriptor properties
 */
#define MMU_L1              0x0
#define MMU_L2              0x1

#define MMU_D_TYPE_MASK     0x3

#define MMU_INVALID_ENTRY   0x0
#define MMU_PTD             0x1
#define MMU_SECTION         0x2
#define MMU_SUPERSECTION    0x40002 /* NOT USED */
#define MMU_RESERVED_ENTRY  0x3

#define MMU_INVALID_PAGE    0x0
#define MMU_LPAGE           0x1
#define MMU_PAGE1           0x2
#define MMU_PAGE2           0x3

/* mmu access permissions for level 1 and 2 tables*/
#define MMU_ACCESS_RO_SECT  0x8000  /* AP[2]: disable write */
#define MMU_ACCESS_UP_SECT  0x800   /* AP[1]: enable unprivileged */
#define MMU_ACCESS_A_SECT   0x400   /* AP[0]: access flag */
#define MMU_ACCESS_XN_SECT  0x10    /* XN: execute never */
#define MMU_ACCESS_NG_SECT  0x20000 /* nG: non global */

#define MMU_ACCESS_RO_PAGE  0x200   /* AP[2]: disable write */
#define MMU_ACCESS_UP_PAGE  0x20    /* AP[1]: enable unprivileged */
#define MMU_ACCESS_A_PAGE   0x10    /* AP[0]: access flag */
#define MMU_ACCESS_XN_LPAGE 0x8000  /* XN: execute never for large page*/
#define MMU_ACCESS_XN_PAGE  0x1     /* XN: execute never for small page*/
#define MMU_ACCESS_NG_PAGE  0x800   /* nG: non global */

/* functions for code clarity */
#define MMU_ACCESS_RO(level, type) \
    ( (level == MMU_L1) ? MMU_ACCESS_RO_SECT: MMU_ACCESS_RO_PAGE )
#define MMU_ACCESS_UP(level, type) \
    ( (level == MMU_L1) ? MMU_ACCESS_UP_SECT: MMU_ACCESS_UP_PAGE )
#define MMU_ACCESS_A(level, type) \
    ( (level == MMU_L1) ? MMU_ACCESS_A_SECT: MMU_ACCESS_A_PAGE )
#define MMU_ACCESS_XN(level, type) \
    ( (level == MMU_L1) ? MMU_ACCESS_XN_SECT: \
    ( (type == MMU_LPAGE) ? MMU_ACCESS_XN_LPAGE: MMU_ACCESS_XN_PAGE ) )
#define MMU_ACCESS_NG(level, type) \
    ( (level == MMU_L1) ? MMU_ACCESS_NG_SECT: MMU_ACCESS_NG_PAGE )


/* mmu region attributes */
#define MMU_ATTR_B          0x4     /* bufferable */
#define MMU_ATTR_C          0x8     /* cacheable */
#define MMU_ATTR_S_L1       0x10000 /* shareable */
#define MMU_ATTR_S_L2       0x400   /* shareable */
#define MMU_ATTR_S(level, type) \
    ((level == MMU_L1) ? MMU_ATTR_S_L1: MMU_ATTR_S_L2)
#define MMU_TEX_OFFSET_     12
#define MMU_TEX_OFFSET_PAGE 6
#define MMU_TEX_OFFSET(level, type) \
    ( (level == MMU_L2 && (type == MMU_PAGE1 || type == MMU_PAGE2)) \
    ? MMU_TEX_OFFSET_PAGE: MMU_TEX_OFFSET_)


/*************************** function declarations ***************************/
void arm_mmu_init(void);
void arm_mmu_disable(void);
void arm_mmu_enable(arm_mmu_context_t *ctx);
air_u32_t arm_is_mmu_enabled(void);
void arm_mmu_change_context(arm_mmu_context_t *ctx);
air_u32_t arm_segregation_memcpy(
        arm_core_context_t *core_ctx, void *dst, void *src, air_sz_t size);

#endif /* MMU_H_ */
