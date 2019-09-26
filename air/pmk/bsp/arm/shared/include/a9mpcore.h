/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file a9mpcore.h
 * \author lumm
 * \brief A9MPCore specifics. Refer to the ARM® Cortex-A9 MPCore TRM
 */

#ifndef A9MPCORE_H_
#define A9MPCORE_H_

#include <air_arch.h>
#include <armv7.h>
#include <cp15.h>
#include <bsp.h>

#define CTRL_SCU_EN                 (1U << 0)
#define CTRL_ADDR_FILTER_EN         (1U << 1)
#define CTRL_SCU_RAM_PAR_EN         (1U << 2)
#define CTRL_SCU_SCPE_LINEFILLS_EN  (1U << 3)
#define CTRL_FORCE_PORT0_EN         (1U << 4)
#define CTRL_SCU_STANDBY_EN         (1U << 5)
#define CTRL_IC_STANDBY_EN          (1U << 6)

#define CFG_CPU_NUMBER              (0b11 << 0)
#define CFG_CPU0_SMP                (1U << 4)
#define CFG_CPU1_SMP                (1U << 5)
#define CFG_CPU2_SMP                (1U << 6)
#define CFG_CPU3_SMP                (1U << 7)
#define CFG_CPU0_TAG_RAM_SIZE       (0b11 << 8)
#define CFG_CPU1_TAG_RAM_SIZE       (0b11 << 10)
#define CFG_CPU2_TAG_RAM_SIZE       (0b11 << 12)
#define CFG_CPU3_TAG_RAM_SIZE       (0b11 << 14)

#define CPU_POW_ST_CPU0             (0b11 << 0)
#define CPU_POW_ST_CPU1             (0b11 << 8)
#define CPU_POW_ST_CPU2             (0b11 << 16)
#define CPU_POW_ST_CPU3             (0b11 << 24)

#define INV_REG_SS_CPU0             (0xf << 0)
#define INV_REG_SS_CPU1             (0xf << 4)
#define INV_REG_SS_CPU2             (0xf << 8)
#define INV_REG_SS_CPU3             (0xf << 12)

#define FILTER_START_REG_ADDR       (0xfff << 20)

#define FILTER_END_REG_ADDR         (0xfff << 20)

#define SAC_CPU0                    (1U << 0)
#define SAC_CPU1                    (1U << 2)
#define SAC_CPU2                    (1U << 2)
#define SAC_CPU3                    (1U << 3)

#define SNSAC_CPU0_REGS             (1U << 0)
#define SNSAC_CPU1_REGS             (1U << 2)
#define SNSAC_CPU2_REGS             (1U << 2)
#define SNSAC_CPU3_REGS             (1U << 3)
#define SNSAC_CPU0_TIMERS           (1U << 4)
#define SNSAC_CPU1_TIMERS           (1U << 5)
#define SNSAC_CPU2_TIMERS           (1U << 6)
#define SNSAC_CPU3_TIMERS           (1U << 7)
#define SNSAC_CPU0_GLOBAL_TIMER     (1U << 8)
#define SNSAC_CPU1_GLOBAL_TIMER     (1U << 9)
#define SNSAC_CPU2_GLOBAL_TIMER     (1U << 10)
#define SNSAC_CPU3_GLOBAL_TIMER     (1U << 11)

typedef struct {
    air_u32_t ctrl;
    air_u32_t cfg;  /* RO */
    air_u32_t cpu_pow_st;
    air_u32_t inv_reg_ss;
    air_u32_t filter_start;
    air_u32_t reserved_09[12];
    air_u32_t filter_start_reg;
    air_u32_t filter_end_reg;
    air_u32_t reserved_48[2];
    air_u32_t sac;
    air_u32_t snsac;
} scu_t;

static volatile scu_t *scu = (volatile scu_t *)SCU_BASE_MEMORY;

static inline void scu_invalidate(air_u32_t cpu_id) {
    scu->inv_reg_ss = ( (0xf) << ( (cpu_id & 0x3) * 4) );
}

static inline void arm_a9mpcore_start_hook(void) {

    air_u32_t cpu_id = arm_cp15_get_multiprocessor_cpu_id();

    if (cpu_id == 0) {
        /* Enable SCU */
        scu->ctrl |= CTRL_SCU_EN;
    }

#if PMK_SMP
    /* Enable cache coherency and cache/MMU maintenance broadcasts for
     * this processor.
     */
    air_u32_t actlr = arm_cp15_get_auxiliary_control();
    actlr |= CP15_ACTLR_SMP | CP15_ACTLR_FW;
    arm_cp15_set_auxiliary_control(actlr);
#endif

    scu_invalidate(cpu_id);
}


#endif /* A9MPCORE_H_ */
