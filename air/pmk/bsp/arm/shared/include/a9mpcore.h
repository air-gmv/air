/*
 * Copyright (C) 2018-2020  GMVIS Skysoft S.A.
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

#include <armv7.h>
#include <cp15.h>
#include <parameters.h>

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
    volatile air_u32_t ctrl;
    air_u32_t cfg; /* RO */
    volatile air_u32_t cpu_pow_st;
    volatile air_u32_t inv_reg_ss;
    volatile air_u32_t filter_start;
    volatile air_u32_t reserved_09[12];
    volatile air_u32_t filter_start_reg;
    volatile air_u32_t filter_end_reg;
    volatile air_u32_t reserved_48[2];
    volatile air_u32_t sac;
    volatile air_u32_t snsac;
} a9mpcore_scu_t;

#define SCU ((a9mpcore_scu_t *)XPAR_PS7_SCUC_0_S_AXI_BASEADDR)

__FORCE_INLINE static void arm_scu_invalidate(air_u32_t cpu_id) {
    SCU->inv_reg_ss = ( (0xf) << ( (cpu_id & 0x3) * 4) );
}

__FORCE_INLINE static void arm_scu_standy_enable() {
    SCU->ctrl |= CTRL_SCU_STANDBY_EN;
}

__FORCE_INLINE static void arm_cp15_enable_dynamic_clock_gating(void) {

    air_u32_t val;
    __asm__ volatile ("mrc p15, 0, %0, c15, c0, 0\n":"=r" (val));
    val |= 1;
    __asm__ volatile ("mcr p15, 0, %0, c15, c0, 0\n"::"r" (val));
}

void arm_a9mpcore_start_hook(air_u32_t cpu_id);

#endif /* A9MPCORE_H_ */
