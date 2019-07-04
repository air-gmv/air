/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file arm_a9mpcore.h
 * @author lumm
 * @brief A9MPCore specifics. Refer to the ARM® Cortex-A9 MPCore TRM
 */

#include <air_arch.h>
#include <armv7.h>
#include <bsp.h>
#include <arm_cp15.h>

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

#define CTRL_TIMER_EN               (1U << 0)
#define CTRL_COMP_EN                (1U << 1)
#define CTRL_IRQ_EN                 (1U << 2)
#define CTRL_AUTOINC_EN             (1U << 3)
#define CTRL_PRESCALER              (0xff << 8)

typedef struct {
    air_u32_t counter_low;
    air_u32_t counter_high;
    air_u32_t ctrl;
    air_u32_t irq_st;
    air_u32_t comp_value_low;
    air_u32_t comp_value_high;
    air_u32_t autoinc;
} global_timer_t

static inline void scu_invalidate(volatile scu_t *scu, air_u32_t cpu_id) {

    scu->inv_reg_ss = ( (0xf) << ( (cpu_id & 0x3) * 4) )
}

static inline void a9mpcore_start_hook(void) {

    volatile scu_t *scu = (volatile scu_t *)SCU_BASE_MEMORY;

    air_u32_t cpu_id = get_multiprocessor_cpu_id();

    if (cpu_id == 0) {
        /* Enable SCU */
        scu->ctrl |= CTRL_SCU_EN;
    }

#ifdef PMK_SMP
    /* Enable cache coherency and cache/MMU maintenance broadcasts for
     * this processor.
     */
    air_u32_t actlr = get_auxiliary_control();
    actlr |= ACTLR_SMP | ACTLR_FW;
    set_auxiliary_control(actlr);
#endif

    scu_invalidate(scu, cpu_id);
}

static inline void start_global_timer(void) {

    volatile global_timer_t *gt = (global_timer_t *)GT_BASE_MEMORY;

    gt->ctrl = 0;
    gt->counter_lower = 0;
    gt->counter_higher = 0;
    gt->ctrl = CTRL_TIMER_EN;
}

static inline void set_vector_base(void) {

    /* Do not use bsp_vector_table_begin == 0, since this will get optimized
    * away. Font: rtems source code
    */
    if (bsp_vector_table_end != bsp_vector_table_size) {
        air_u32_t ctrl;

        /* Assumes every core has Security Extensions */
        set_vector_base_address(bsp_vector_table_begin);

        ctrl = get_system_control();
        ctrl &= ~CP15_CTRL_V;
        set_system_control(ctrl);
    }
}
