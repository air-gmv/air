/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file arm_cp15.h
 * @author lumm
 * @brief CP15 System Control Coprocessor
 */

#ifndef ARM_CP15_H
#define ARM_CP15_H

#include <air_arch.h>
#include <armv7.h>

/* System Control Register on CP15 definitions */
#define CP15_CTRL_TE        (1U << 30)
#define CP15_CTRL_AFE       (1U << 29)
#define CP15_CTRL_TRE       (1U << 28)
#define CP15_CTRL_NMFI      (1U << 27)
#define CP15_CTRL_EE        (1U << 25)
#define CP15_CTRL_VE        (1U << 24)
#define CP15_CTRL_XP        (1U << 23)
#define CP15_CTRL_U         (1U << 22)
#define CP15_CTRL_FI        (1U << 21)
#define CP15_CTRL_UWXN      (1U << 20)
#define CP15_CTRL_WXN       (1U << 19)
#define CP15_CTRL_HA        (1U << 17)
#define CP15_CTRL_L4        (1U << 15)
#define CP15_CTRL_RR        (1U << 14)
#define CP15_CTRL_V         (1U << 13)
#define CP15_CTRL_I         (1U << 12)
#define CP15_CTRL_Z         (1U << 11)
#define CP15_CTRL_SW        (1U << 10)
#define CP15_CTRL_R         (1U << 9)
#define CP15_CTRL_S         (1U << 8)
#define CP15_CTRL_B         (1U << 7)
#define CP15_CTRL_CP15BEN   (1U << 5)
#define CP15_CTRL_C         (1U << 2)
#define CP15_CTRL_A         (1U << 1)
#define CP15_CTRL_M         (1U << 0)


static inline air_u32_t arm_cp15_get_system_control(void) {
    ARM_SWITCH_REGISTERS;
    air_u32_t val;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "mrc p15, 0, %[val], c1, c0, 0\n"
        ARM_SWITCH_BACK
        : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
    );

    return val;
}

static inline void arm_cp15_set_system_control(air_u32_t val) {
    ARM_SWITCH_REGISTERS;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "mcr p15, 0, %[val], c1, c0, 0\n"
        "nop\n"
        "nop\n"
        ARM_SWITCH_BACK
        : ARM_SWITCH_OUTPUT
        : [val] "r" (val)
        : "memory"
    );
}

static inline air_u32_t arm_cp15_get_level_of_coherency(air_u32_t clidr) {
    return (clidr >> 24) & 0x7;
}

static inline void arm_cp15_instruction_cache_invalidate(void) {
    ARM_SWITCH_REGISTERS;
    air_u32_t sbz = 0;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "mcr p15, 0, %[sbz], c7, c5, 0\n"
        ARM_SWITCH_BACK
        : ARM_SWITCH_OUTPUT
        : [sbz] "r" (sbz)
        : "memory"
    );
}

static inline air_u32_t arm_cp15_get_cache_level_id(void) {
    ARM_SWITCH_REGISTERS;
    air_u32_t val;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "mrc p15, 1, %[val], c0, c0, 1\n"
        ARM_SWITCH_BACK
        : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
    );

    return val;
}

static inline air_u32_t arm_cp15_get_cache_size_id(void) {
    ARM_SWITCH_REGISTERS;
    air_u32_t val;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "mrc p15, 1, %[val], c0, c0, 0\n"
        ARM_SWITCH_BACK
        : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
    );

    return val;
}

/* CSSELR */
static inline void arm_cp15_set_cache_size_selection(air_u32_t val) {
    ARM_SWITCH_REGISTERS;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "mcr p15, 2, %[val], c0, c0, 0\n"
        ARM_SWITCH_BACK
        : ARM_SWITCH_OUTPUT
        : [val] "r" (val)
        : "memory"
    );
}

/* Cache Clean by Set/Way DCCSW */
static inline void arm_cp15_data_cache_clean_line_by_set_and_way(air_u32_t set_way) {
    ARM_SWITCH_REGISTERS;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "mcr p15, 0, %[set_way], c7, c10, 2\n"
        ARM_SWITCH_BACK
        : ARM_SWITCH_OUTPUT
        : [set_way] "r" (set_way)
        : "memory"
    );
}

static inline air_u32_t arm_cp15_get_cache_size_id_for_level(air_u32_t level) {
    // TODO in RTEMS they disable interrupts for some reason
    // possibly because of the security extensions?
    air_u32_t ccsidr;

    arm_cp15_set_cache_size_selection(level);
    instruction_synchronization_barrier();
    ccsidr = arm_cp15_get_cache_size_id();

    return ccsidr;
}

/*  L = log2(#words * 4(32bits))
    LineSize = log(#words) - 2
    => L = log2(2^(LineSize + 2) * 4) */
static inline air_u32_t arm_cp15_get_line_shift(air_u32_t ccsidr) {
    return (ccsidr &0x7) + 4;
}

/* Associativity + 1 */
static inline air_u32_t arm_cp15_get_associativity(air_u32_t ccsidr) {
    return ((ccsidr >> 3U) & 0x3ff) + 1;
}

/* NumSets + 1 */
static inline air_u32_t arm_cp15_get_number_sets(air_u32_t ccsidr) {
    return ((ccsidr >> 13U) & 0x7fff) + 1;
}

/* Register Size - Associativity */
static inline air_u32_t arm_cp15_get_way_shift(air_u32_t associativity) {
    return __builtin_clz(associativity - 1);
}

static inline void arm_cp15_data_cache_clean_level(air_u32_t level) {
    air_u32_t ccsidr;
    air_u32_t line_shift;
    air_u32_t num_sets;
    air_u32_t set;
    air_u32_t way_shift;
    air_u32_t associativity;
    air_u32_t way;

    /* bitwise shift due to CSSELR level starting at bit 1 */
    ccsidr = arm_cp15_get_cache_size_id_for_level((level << 1U));

    line_shift = arm_cp15_get_line_shift(ccsidr);
    num_sets = arm_cp15_get_number_sets(ccsidr);
    associativity = arm_cp15_get_associativity(ccsidr);
    way_shift = arm_cp15_get_way_shift(associativity);

    for (set = 0; set < num_sets; set++) {
        for (way = 0; way < associativity; way++) {

            air_u32_t set_way = (way << way_shift)
                | (set << line_shift)
                | (level << 1U);

            arm_cp15_data_cache_clean_line_by_set_and_way(set_way);
        }
    }
}

static inline void arm_cp15_data_cache_clean_all_levels(void) {
    air_u32_t clidr = arm_cp15_get_cache_level_id();
    air_u32_t loc = (clidr >> 24U) & 0b111;
    air_u32_t level = 0;

    for (level = 0; level < loc; ++level) {
        air_u32_t ctype = (clidr >> (3U * level)) & 0b111;

        /* Check if this level has a data cache or unified cache */
        if ( (ctype == 0b010) || (ctype == 0b100) ) {
            arm_cp15_data_cache_clean_level(level);
        }
    }
}

static inline void arm_cp15_data_cache_invalidate_line_by_set_and_way(air_u32_t set_way) {
    ARM_SWITCH_REGISTERS;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "mcr p15, 0, %[set_way], c7, c6, 2\n"
        ARM_SWITCH_BACK
        : ARM_SWITCH_OUTPUT
        : [set_way] "r" (set_way)
        : "memory"
    );
}

#define CP15_CSSELR_DATA 0
#define CP15_CSSELR_INST 1
static inline void arm_cp15_cache_invalidate_level(air_u32_t level, air_u8_t inst_or_data) {
    air_u32_t ccsidr;
    air_u32_t line_shift;
    air_u32_t num_sets;
    air_u32_t set;
    air_u32_t way_shift;
    air_u32_t associativity;
    air_u32_t way;

    /* bitwise shift due to CSSELR level starting at bit 1 */
    ccsidr = arm_cp15_get_cache_size_id_for_level((level << 1U) | inst_or_data);

    line_shift = arm_cp15_get_line_shift(ccsidr);
    num_sets = arm_cp15_get_number_sets(ccsidr);
    associativity = arm_cp15_get_associativity(ccsidr);
    way_shift = arm_cp15_get_way_shift(associativity);

    for (set = 0; set < num_sets; set++) {
        for (way = 0; way < associativity; way++) {

            air_u32_t set_way = (way << way_shift)
                | (set << line_shift)
                | (level << 1U);

            arm_cp15_data_cache_invalidate_line_by_set_and_way(set_way);
        }
    }
}

static inline void arm_cp15_data_cache_invalidate_all_levels(void) {
    air_u32_t clidr = arm_cp15_get_cache_level_id();
    air_u32_t loc = (clidr >> 24U) & 0b111;
    air_u32_t level = 0;

    for (level = 0; level < loc; ++level) {
        air_u32_t ctype = (clidr >> (3U * level)) & 0b111;

        /* Check if this level has a data cache or unified cache */
        if (((ctype & (0x6)) == 2) || (ctype == 4)) {
            arm_cp15_cache_invalidate_level(level, CP15_CSSELR_DATA);
        }
    }
}

/* BPIALL, Branch Predictor Invalidate All */
static inline void arm_cp15_branch_predictor_invalidate_all(void) {
    ARM_SWITCH_REGISTERS;
    air_u32_t sbz = 0;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "mcr p15, 0, %[sbz], c7, c5, 6\n"
        ARM_SWITCH_BACK
        : ARM_SWITCH_OUTPUT
        : [sbz] "r" (sbz)
        : "memory"
    );
}

/* TLBIALL, TLB Invalidate All */
static inline void arm_cp15_tlb_invalidate(void) {
    ARM_SWITCH_REGISTERS;
    air_u32_t sbz = 0;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "mcr p15, 0, %[sbz], c8, c7, 0\n"
        ARM_SWITCH_BACK
        : ARM_SWITCH_OUTPUT
        : [sbz] "r" (sbz)
    );

    data_synchronization_barrier(15);
    instruction_synchronization_barrier();
}

/* MPIDR, Multiprocessor Affinity Register */
static inline air_u32_t arm_cp15_get_multiprocessor_cpu_id(void) {
    ARM_SWITCH_REGISTERS;
    air_u32_t mpidr;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "mrc p15, 0, %[mpidr], c0, c0, 5\n"
        ARM_SWITCH_BACK
        : [mpidr] "=&r" (mpidr) ARM_SWITCH_ADDITIONAL_OUTPUT
    );

    return (mpidr & 0xff);
}

/* ACTLR Auxiliary Control. Refer to ARM Cortex A9 TRM, section 4.3.10 */
#define CP15_ACTLR_FW (1U << 0)
#define CP15_ACTLR_L1_PREFETCH_EN (1U << 2)
#define CP15_ACTLR_WR_FULL_LINE_OF_ZEROS_MODE (1U << 3)
#define CP15_ACTLR_SMP (1U << 6)
#define CP15_ACTLR_EXCL (1U << 7)
#define CP15_ACTLR_ALLOC_IN_ONE_WAY (1U << 8)
#define CP15_ACTLR_PARITY_ON (1U << 0)

static inline air_u32_t arm_cp15_get_auxiliary_control(void) {
    ARM_SWITCH_REGISTERS;
    air_u32_t val;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "mrc p15, 0, %[val], c1, c0, 1\n"
        ARM_SWITCH_BACK
        : [val] "=&r" (val) ARM_SWITCH_ADDITIONAL_OUTPUT
    );

    return val;
}

static inline void arm_cp15_set_auxiliary_control(air_u32_t val) {
    ARM_SWITCH_REGISTERS;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "mcr p15, 0, %[val], c1, c0, 1\n"
        ARM_SWITCH_BACK
        : ARM_SWITCH_OUTPUT
        : [val] "r" (val)
    );
}

/* VBAR, Vector Base Address Register */
static inline void arm_cp15_set_vector_base_address(void *base) {
    ARM_SWITCH_REGISTERS;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "mcr p15, 0, %[base], c12, c0, 0\n"
        ARM_SWITCH_BACK
        : ARM_SWITCH_OUTPUT
        : [base] "r" (base)
    );
}

#endif /* ARM_CP15_H */
