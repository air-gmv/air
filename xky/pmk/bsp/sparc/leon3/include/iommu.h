/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2008-2015
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file
 * @author pfnf
 * @brief GRIOMMU control functions and initialization
 */

#ifndef __IOMMU_H__
#define __IOMMU_H__

/**
 *  @brief APV Page size (in KiB)
 */
#define APV_PAGE    (256)

/**
 *  @brief APV Vector size (in 4KiB)
 */
#define APV_SIZE    ((4 * 1024 * 1024 / APV_PAGE) / (sizeof(air_u32_t) * 8))

/**
 *  @brief Number of available groups
 */
#define APV_SIZE_GROUPS  (8 * (APV_SIZE - 1));

/**
 *  @brief IOMMU Control registers
 */
typedef struct  {

    volatile unsigned int cap[3];       /* 0x00 - 0x08 */
    volatile unsigned int dummy0;       /* 0x0C */
    volatile unsigned int ctrl;         /* 0x10 */
    volatile unsigned int flush;        /* 0x14 */
    volatile unsigned int stat;         /* 0x18 */
    volatile unsigned int mask;         /* 0x1C */
    volatile unsigned int fail;         /* 0x20 */
    volatile unsigned int dummy1[7];    /* 0x24 - 0x3C */
    volatile unsigned int mstcfg[16];   /* 0x40 - 0x7C */
    volatile unsigned int grpctrl[16];  /* 0x80-0xBC */
    volatile unsigned int diaga;        /* 0xC0 */
    volatile unsigned int diagd[8];     /* 0xC4 - 0xE0 */
    volatile unsigned int diagt;        /* 0xE4 */
    volatile unsigned int derri;        /* 0xE8 */
    volatile unsigned int terri;        /* 0xEC */
    volatile unsigned int dummy2[4];    /* 0xF0 - 0xFC */
    volatile unsigned int asmpacc[15];  /* 0x100 - 0x138 */

} IOMMU_Regs_Map;

typedef struct {

    int partity_error;
    int flush_completed;
    int flush_started;
    int access_denied;
    int translation_error;

} IOMMU_Status;

/**
 *  @brief IOMMU Page size
 */
typedef enum {

    PAGE_4K         = 0,    /**< Page size of 4 KiB      */
    PAGE_8K         = 1,    /**< Page size of 8 KiB      */
    PAGE_16K        = 2,    /**< Page size of 16 KiB     */
    PAGE_32K        = 3,    /**< Page size of 32 KiB     */
    PAGE_64K        = 4,    /**< Page size of 64 KiB     */
    PAGE_128K       = 5,    /**< Page size of 128 KiB    */
    PAGE_256K       = 6,    /**< Page size of 256 KiB    */
    PAGE_512K       = 7,    /**< Page size of 512 KiB    */

} IOMMU_Page_Size;

/**
 *  @brief IOMMU Interrupt Mask
 */
typedef enum {

    INT_MASK_PEI    = 1 << 5,
    INT_MASK_FCI    = 1 << 3,
    INT_MASK_FLI    = 1 << 2,
    INT_MASK_ADI    = 1 << 1,
    INT_MASK_TEI    = 1 << 0,

} IOMMU_Interrupt_Mask_t;

/**
 *  @brief IOMMU Status register
 */
typedef enum {

    STAT_PE         = 1 << 5,
    STAT_DE         = 1 << 4,
    STAT_FC         = 1 << 3,
    STAT_FL         = 1 << 2,
    STAT_AD         = 1 << 1,
    STAT_TE         = 1 << 0,

} IOMMU_Status_t;

typedef enum {

    PASS_TROUGH     = 1 << 1,
    ACTIVE          = 1 << 0,

} IOMMU_Group_Ctrl_t;

/**
 *  @brief GRIOMMU Control Register
 */
typedef enum {

    CTRL_SP         = 1 << 16,  /**< SPLIT Support            */
    CTRL_DP         = 1 << 11,  /**< Disable Prefetch         */
    CTRL_SIV        = 1 << 10,  /**< Save Invalid IOPTE       */
    CTRL_AU         = 1 << 7,   /**< Always Update            */
    CTRL_WP         = 1 << 6,   /**< Write Protection Only    */
    CTRL_DM         = 1 << 5,   /**< Diagnostic Mode          */
    CTRL_GS         = 1 << 4,   /**< Group-Set-Addressing     */
    CTRL_CE         = 1 << 3,   /**< Cache/TBL                */
    CTRL_EN         = 1 << 0,   /**< Core Enable              */

} IOMMU_Ctrl_t;

/**
 *  @brief GRIOMMU Protection Mode
 */
typedef enum {

    APV             = 0x0 << 1,  /**< Access Protection Vector       */
    IOMMU           = 0x1 << 1,  /**< IOMMU Mode                     */

} IOMMU_PM_t;

/**
 *  @brief GRIOMMU Internal control structure
 *
 *  This strcuture provides access to the GRIOMMU control registers, along
 *  with relevant information about its operation
 */
typedef struct {

    int found;
    int minor;
    /**
     *  @brief Number of masters supported by the core
     */
    int nmsts;
    /**
     *  @brief Number of groups supported by the core
     */
    int ngrps;

    unsigned int irq;
    IOMMU_Regs_Map *regs;

} IOMMU_internal;

/* Capability register 0 */
#define GRIOMMU_CAP0_A (1 << 31)
#define GRIOMMU_CAP0_AC (1 << 30)
#define GRIOMMU_CAP0_CA (1 << 29)
#define GRIOMMU_CAP0_CP (1 << 28)
#define GRIOMMU_CAP0_NARB 20
#define GRIOMMU_CAP0_CS 19
#define GRIOMMU_CAP0_FT 17
#define GRIOMMU_CAP0_ST (1 << 16)
#define GRIOMMU_CAP0_I (1 << 15)
#define GRIOMMU_CAP0_IT (1 << 14)
#define GRIOMMU_CAP0_IA (1 << 13)
#define GRIOMMU_CAP0_IP (1 << 12)
#define GRIOMMU_CAP0_MB (1 << 8)
#define GRIOMMU_CAP0_GRPS 4
#define GRIOMMU_CAP0_MSTS 0

/* Capability register 1 */
#define GRIOMMU_CAP1_CADDR 20
#define GRIOMMU_CAP1_CMASK 16
#define GRIOMMU_CAP1_CTAGB 8
#define GRIOMMU_CAP1_CSIZE 5
#define GRIOMMU_CAP1_CLINES 0

/* Capability register 2 */
#define GRIOMMU_CAP2_TMASK 24
#define GRIOMMU_CAP2_MTYPE 18
#define GRIOMMU_CAP2_TTYPE 16
#define GRIOMMU_CAP2_TTAGB 8
#define GRIOMMU_CAP2_ISIZE 5
#define GRIOMMU_CAP2_TLBENT 0

/* Control register */
#define GRIOMMU_CTRL_PGSZ 18
#define GRIOMMU_CTRL_LB (1 << 17)
#define GRIOMMU_CTRL_SP (1 << 16)
#define GRIOMMU_CTRL_ITR 12
#define GRIOMMU_CTRL_DP (1 << 11)
#define GRIOMMU_CTRL_SIV (1 << 10)
#define GRIOMMU_CTRL_HPROT 8
#define GRIOMMU_CTRL_AU (1 << 7)
#define GRIOMMU_CTRL_WP (1 << 6)
#define GRIOMMU_CTRL_DM (1 << 5)
#define GRIOMMU_CTRL_GS (1 << 4)
#define GRIOMMU_CTRL_CE (1 << 3)
#define GRIOMMU_CTRL_PM (1 << 1)
#define GRIOMMU_CTRL_EN (1 << 0)

/* Flush register */
#define GRIOMMU_FLUSH_FGRP 4
#define GRIOMMU_FLUSH_GF (1 << 1)
#define GRIOMMU_FLUSH_F (1 << 0)

/* Status register */
#define GRIOMMU_STAT_PE (1 << 5)
#define GRIOMMU_STAT_DE (1 << 4)
#define GRIOMMU_STAT_FC (1 << 3)
#define GRIOMMU_STAT_FL (1 << 2)
#define GRIOMMU_STAT_AD (1 << 1)
#define GRIOMMU_STAT_TE (1 << 0)

/* Mask register */
#define GRIOMMU_MASK_PEI (1 << 5)
#define GRIOMMU_MASK_FCI (1 << 3)
#define GRIOMMU_MASK_FLI (1 << 2)
#define GRIOMMU_MASK_ADI (1 << 1)
#define GRIOMMU_MASK_TEI (1 << 0)

/* Failing access register */
#define GRIOMMU_FAIL_FADDR 5
#define GRIOMMU_FAIL_FW (1 << 4)
#define GRIOMMU_FAIL_FMASTER 0

/* Master configuration register(s) */
#define GRIOMMU_MST_VENDOR 24
#define GRIOMMU_MST_DEVICE 12
#define GRIOMMU_MST_BS (1 << 4)
#define GRIOMMU_MST_GROUP 0

/* Group control register(s) */
#define GRIOMMU_GRPCTRL_BASE 2
#define GRIOMMU_GRPCTRL_P (1 << 1)
#define GRIOMMU_GRPCTRL_AG (1 << 0)

/* Diagnostic cache access register */
#define GRIOMMU_DIAGA_DA (1 << 31)
#define GRIOMMU_DIAGA_RW (1 << 30)
#define GRIOMMU_DIAGA_DP (1 << 21)
#define GRIOMMU_DIAGA_TP (1 << 20)


/* ASMP Register Block access control register */
#define GRIOMMU_ASMPCC_FC (1 << 18)
#define GRIOMMU_ASMPCC_SC (1 << 17)
#define GRIOMMU_ASMPCC_MC (1 << 16)
#define GRIOMMU_ASMPCC_GRP 0

#endif /* __IOMMU_H__ */
