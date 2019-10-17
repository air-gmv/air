/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file arm_cp15.h
 * \author lumm
 * \brief CP15 System Control Coprocessor
 */

#ifndef ARM_CP15_H
#define ARM_CP15_H

#include <air_arch.h>
#include <armv7.h>

/* System Control Register on CP15 definitions */
#define ARM_SCTLR_TE        (1U << 30)
#define ARM_SCTLR_AFE       (1U << 29)
#define ARM_SCTLR_TRE       (1U << 28)
#define ARM_SCTLR_NMFI      (1U << 27)
#define ARM_SCTLR_EE        (1U << 25)
#define ARM_SCTLR_VE        (1U << 24)
#define ARM_SCTLR_XP        (1U << 23)
#define ARM_SCTLR_U         (1U << 22)
#define ARM_SCTLR_FI        (1U << 21)
#define ARM_SCTLR_UWXN      (1U << 20)
#define ARM_SCTLR_WXN       (1U << 19)
#define ARM_SCTLR_HA        (1U << 17)
#define ARM_SCTLR_L4        (1U << 15)
#define ARM_SCTLR_RR        (1U << 14)
#define ARM_SCTLR_V         (1U << 13)
#define ARM_SCTLR_I         (1U << 12)
#define ARM_SCTLR_Z         (1U << 11)
#define ARM_SCTLR_SW        (1U << 10)
#define ARM_SCTLR_R         (1U << 9)
#define ARM_SCTLR_S         (1U << 8)
#define ARM_SCTLR_B         (1U << 7)
#define ARM_SCTLR_CP15BEN   (1U << 5)
#define ARM_SCTLR_C         (1U << 2)
#define ARM_SCTLR_A         (1U << 1)
#define ARM_SCTLR_M         (1U << 0)


static inline air_u32_t arm_cp15_get_system_control(void) {
    air_u32_t val;
    __asm__ volatile ("mrc p15, 0, %0, c1, c0, 0\n":"=r" (val)::"memory");
    return val;
}

static inline void arm_cp15_set_system_control(air_u32_t val) {
    __asm__ volatile ("mcr p15, 0, %0, c1, c0, 0\n"::"r" (val):"memory");
}

static inline air_u32_t arm_cp15_get_level_of_coherency(air_u32_t clidr) {
    return (clidr >> 24) & 0x7;
}

static inline void arm_cp15_instruction_cache_invalidate(void) {
    __asm__ volatile ("mcr p15, 0, r4, c7, c5, 0\n");
}

static inline air_u32_t arm_cp15_get_cache_level_id(void) {
    air_u32_t val;
    __asm__ volatile ("mrc p15, 1, %0, c0, c0, 1\n":"=r" (val));
    return val;
}

static inline air_u32_t arm_cp15_get_cache_size_id(void) {
    air_u32_t val;
    __asm__ volatile ("mrc p15, 1, %0, c0, c0, 0\n":"=r" (val));
    return val;
}

/* CSSELR */
static inline void arm_cp15_set_cache_size_selection(air_u32_t val) {
    __asm__ volatile ("mcr p15, 2, %0, c0, c0, 0\n"::"r" (val):"memory");
}

/* Cache Clean by Set/Way DCCSW */
static inline void arm_cp15_data_cache_clean_line_by_set_and_way(air_u32_t set_way) {
    __asm__ volatile ("mcr p15, 0, %0, c7, c10, 2\n"::"r" (set_way));
}

static inline air_u32_t arm_cp15_get_cache_size_id_for_level(air_u32_t level) {
    // TODO in RTEMS they disable interrupts for some reason
    // possibly because of the security extensions?
    air_u32_t ccsidr;

    arm_cp15_set_cache_size_selection(level);
    arm_instruction_synchronization_barrier();
    ccsidr = arm_cp15_get_cache_size_id();

    return ccsidr;
}

/*  L = log2(#words * 4(32bits))
    LineSize = log(#words) - 2
    => L = log2(2^(LineSize + 2) * 4) */
static inline air_u32_t arm_cp15_get_line_shift(air_u32_t ccsidr) {
    return (ccsidr & 0x7) + 4;
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
    __asm__ volatile ("mcr p15, 0, %0, c7, c6, 2\n"::"r" (set_way));
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
    __asm__ volatile ("mcr p15, 0, r4, c7, c5, 6\n");
}

/* TLBIALL, TLB Invalidate All */
static inline void arm_cp15_tlb_invalidate(void) {
    __asm__ volatile ("mcr p15, 0, r4, c8, c7, 0\n");

    arm_data_synchronization_barrier();
    arm_instruction_synchronization_barrier();
}

/* MPIDR, Multiprocessor Affinity Register */
static inline air_u32_t arm_cp15_get_multiprocessor_cpu_id(void) {
    air_u32_t mpidr;
    __asm__ volatile ("mrc p15, 0, %0, c0, c0, 5\n":"=r" (mpidr));
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
    air_u32_t val;
    __asm__ volatile ("mrc p15, 0, %0, c1, c0, 1\n":"=r" (val)::"memory");
    return val;
}

static inline void arm_cp15_set_auxiliary_control(air_u32_t val) {
    __asm__ volatile ("mcr p15, 0, %0, c1, c0, 1\n"::"r" (val):"memory");
}

/* VBAR, Vector Base Address Register */
static inline void arm_cp15_set_vector_base_address(void *start) {
    __asm__ volatile ("mcr p15, 0, %0, c12, c0, 0\n"::"r" (start):"memory");
}

static inline void arm_cp15_set_translation_table_control(air_u32_t val) {
    __asm__ volatile ("mcr p15, 0, %0, c2, c0, 2\n"::"r" (val):"memory");
}

static inline air_u32_t arm_cp15_get_translation_table_control(void) {
    air_u32_t val;
    __asm__ volatile ("mrc p15, 0, %0, c2, c0, 2\n":"=r" (val)::"memory");
    return val;
}

static inline void arm_cp15_set_domain_access_control(air_u32_t val) {
    __asm__ volatile ("mcr p15, 0, %0, c3, c0, 0\n"::"r" (val):"memory");
}

static inline void arm_cp15_set_translation_table0_base(air_u32_t base) {
    __asm__ volatile ("mcr p15, 0, %0, c2, c0, 0\n"::"r" (base):"memory");
}

static inline air_u32_t arm_cp15_get_translation_table0_base(void) {
    air_u32_t base;
    __asm__ volatile ("mrc p15, 0, %0, c2, c0, 0\n":"=r" (base)::"memory");
    return base;
}

static inline void arm_cp15_set_translation_table1_base(air_u32_t base) {
    __asm__ volatile ("mcr p15, 0, %0, c2, c0, 1\n"::"r" (base):"memory");
}

static inline air_u32_t arm_is_hivecs(void) {
    air_u32_t sctlr = arm_cp15_get_system_control();
    return (sctlr & ARM_SCTLR_V);
}

static inline void arm_disable_hivecs(void) {
    air_u32_t sctlr = arm_cp15_get_system_control();
    if (sctlr & ARM_SCTLR_V) {
        sctlr &= ~(ARM_SCTLR_V);
        arm_cp15_set_system_control(sctlr);
    }
}

static inline void arm_enable_hivecs(void) {
    air_u32_t sctlr = arm_cp15_get_system_control();
    if (!(sctlr & ARM_SCTLR_V)) {
        sctlr |= ARM_SCTLR_V;
        arm_cp15_set_system_control(sctlr);
    }
}

// TODO arm_get_monitor_exception_base_address

static inline air_u32_t arm_cp15_get_exception_base_address(void) {

    if (arm_is_hivecs()) {
        return 0xffff0000;
    }

// if () TODO no security extensions

    air_u32_t val;
    __asm__ volatile ("mrc p15, 0, %0, c12, c0, 0\n":"=r" (val)::"memory");
    return val;
}

static inline void arm_cp15_set_exception_base_address(air_u32_t val) {
    arm_disable_hivecs();
    __asm__ volatile ("mcr p15, 0, %0, c12, c0, 0\n"::"r" (val):"memory");
}

static inline void arm_cp15_setup_Per_CPU(air_u32_t cpu_id) {
    air_u32_t val;
    air_u32_t offset = 4;
    __asm__ volatile (
            "ldr %[rx], =air_shared_area\n"
            "ldr %[rx], [%[rx], %[offset]]\n"
            "add %[rx], %[rx], %[id], asl #5\n"
            "mcr p15, 0, %[rx], c13, c0, 4\n"
            : [rx] "=&r" (val)
            : [id] "r" (cpu_id), [offset] "r" (offset)
            : "memory");
}

static inline air_u32_t arm_cp15_get_Per_CPU(void) {
	air_u32_t val;
    __asm__ volatile ("mrc p15, 0, %0, c13, c0, 4\n":"=r" (val));
    return val;
}

static inline air_u32_t arm_cp15_get_CPACR(void) {
    air_u32_t val;
    __asm__ volatile ("mrc p15, 0, %0, c1, c0, 2\n":"=r" (val));
    return val;
}

static inline void arm_cp15_set_CPACR(air_u32_t val) {
    __asm__ volatile ("mcr p15, 0, %0, c1, c0, 2\n"::"r" (val):"memory");
}

static inline air_u32_t arm_cp15_get_CONTEXTIDR(void) {
    air_u32_t val;
    __asm__ volatile ("mrc p15, 0, %0, c13, c0, 1\n":"=r" (val));
    return val;
}

static inline void arm_cp15_set_CONTEXTIDR(air_u32_t val) {
    __asm__ volatile ("mcr p15, 0, %0, c13, c0, 1\n"::"r" (val):"memory");
}

#endif /* ARM_CP15_H */
