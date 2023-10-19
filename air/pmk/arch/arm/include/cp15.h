/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file cp15.h
 * \author lumm
 * \brief CP15 System Control coprocessor
 */

#ifndef CP15_H_
#define CP15_H_

#include <armv7.h>

/**
 * \brief System Control Register on CP15 definitions
 */
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


__FORCE_INLINE static air_u32_t arm_cp15_get_system_control(void) {
    air_u32_t val;
    __asm__ volatile ("mrc p15, 0, %0, c1, c0, 0\n":"=r" (val));
    return val;
}

__FORCE_INLINE static void arm_cp15_set_system_control(air_u32_t val) {
    arm_data_synchronization_barrier();
    arm_instruction_synchronization_barrier();
    __asm__ volatile ("mcr p15, 0, %0, c1, c0, 0\n"::"r" (val));
    arm_data_synchronization_barrier();
    arm_instruction_synchronization_barrier();
}

__FORCE_INLINE static air_u32_t arm_cp15_get_level_of_coherency(air_u32_t clidr) {
    return (clidr >> 24) & 0x7;
}

__FORCE_INLINE static void air_arm_cp15_instruction_cache_invalidate(void) {
    __asm__ volatile ("mcr p15, 0, r4, c7, c5, 0\n");
}

__FORCE_INLINE static air_u32_t air_arm_cp15_get_cache_level_id(void) {
    air_u32_t val;
    __asm__ volatile ("mrc p15, 1, %0, c0, c0, 1\n":"=r" (val));
    return val;
}

__FORCE_INLINE static air_u32_t air_arm_cp15_get_cache_size_id(void) {
    air_u32_t val;
    __asm__ volatile ("mrc p15, 1, %0, c0, c0, 0\n":"=r" (val));
    return val;
}

/* CSSELR */
__FORCE_INLINE static void air_arm_cp15_set_cache_size_selection(air_u32_t val) {
    __asm__ volatile ("mcr p15, 2, %0, c0, c0, 0\n"::"r" (val));
}

/* Cache Clean by Set/Way DCCSW */
__FORCE_INLINE static void air_arm_cp15_data_cache_clean_line_by_set_and_way(air_u32_t set_way) {
    __asm__ volatile ("mcr p15, 0, %0, c7, c10, 2\n"::"r" (set_way));
}

__FORCE_INLINE static void air_arm_cp15_data_cache_invalidate_line_by_set_and_way(air_u32_t set_way) {
    __asm__ volatile ("mcr p15, 0, %0, c7, c6, 2\n"::"r" (set_way));
}

/* BPIALL, Branch Predictor Invalidate All */
__FORCE_INLINE static void air_arm_cp15_branch_predictor_invalidate_all(void) {
    __asm__ volatile ("mcr p15, 0, r4, c7, c5, 6\n");
}

/* TLBIALL, TLB Invalidate All */
__FORCE_INLINE static void air_arm_cp15_tlb_invalidate(void) {
    __asm__ volatile ("mcr p15, 0, r4, c8, c7, 0\n");
}

/* MPIDR, Multiprocessor Affinity Register */
__FORCE_INLINE static air_u32_t arm_cp15_get_multiprocessor_cpu_id(void) {
    air_u32_t mpidr;
    __asm__ volatile ("mrc p15, 0, %0, c0, c0, 5\n":"=r" (mpidr));
    return (mpidr & 0xff);
}

/**
 * \brief ACTLR Auxiliary Control. Refer to ARM Cortex A9 TRM, section 4.3.10
 */
#define CP15_ACTLR_FW (1U << 0)
#define CP15_ACTLR_L1_PREFETCH_EN (1U << 2)
#define CP15_ACTLR_WR_FULL_LINE_OF_ZEROS_MODE (1U << 3)
#define CP15_ACTLR_SMP (1U << 6)
#define CP15_ACTLR_EXCL (1U << 7)
#define CP15_ACTLR_ALLOC_IN_ONE_WAY (1U << 8)
#define CP15_ACTLR_PARITY_ON (1U << 0)

__FORCE_INLINE static air_u32_t air_arm_cp15_get_auxiliary_control(void) {
    air_u32_t val;
    __asm__ volatile ("mrc p15, 0, %0, c1, c0, 1\n":"=r" (val)::"memory");
    return val;
}

__FORCE_INLINE static void air_arm_cp15_set_auxiliary_control(air_u32_t val) {
    __asm__ volatile ("mcr p15, 0, %0, c1, c0, 1\n"::"r" (val):"memory");
}

/* VBAR, Vector Base Address Register */
__FORCE_INLINE static void air_arm_cp15_set_vector_base_address(void *start) {
    __asm__ volatile ("mcr p15, 0, %0, c12, c0, 0\n"::"r" (start):"memory");
}

__FORCE_INLINE static void arm_cp15_set_translation_table_control(air_u32_t val) {
    __asm__ volatile ("mcr p15, 0, %0, c2, c0, 2\n"::"r" (val):"memory");
}

__FORCE_INLINE static air_u32_t arm_cp15_get_translation_table_control(void) {
    air_u32_t val;
    __asm__ volatile ("mrc p15, 0, %0, c2, c0, 2\n":"=r" (val)::"memory");
    return val;
}

__FORCE_INLINE static void air_arm_cp15_set_domain_access_control(air_u32_t val) {
    __asm__ volatile ("mcr p15, 0, %0, c3, c0, 0\n"::"r" (val):"memory");
}

__FORCE_INLINE static void arm_cp15_set_translation_table0_base(air_u32_t base) {
    __asm__ volatile ("mcr p15, 0, %0, c2, c0, 0\n"::"r" (base):"memory");
}

__FORCE_INLINE static air_u32_t arm_cp15_get_translation_table0_base(void) {
    air_u32_t base;
    __asm__ volatile ("mrc p15, 0, %0, c2, c0, 0\n":"=r" (base)::"memory");
    return base;
}

__FORCE_INLINE static void arm_cp15_set_translation_table1_base(air_u32_t base) {
    __asm__ volatile ("mcr p15, 0, %0, c2, c0, 1\n"::"r" (base):"memory");
}

__FORCE_INLINE static air_u32_t arm_is_hivecs(void) {
    air_u32_t sctlr = arm_cp15_get_system_control();
    return (sctlr & ARM_SCTLR_V);
}

__FORCE_INLINE static void arm_disable_hivecs(void) {
    air_u32_t sctlr = arm_cp15_get_system_control();
if((sctlr & ARM_SCTLR_V) != 0) {
        sctlr &= ~(ARM_SCTLR_V);
        arm_cp15_set_system_control(sctlr);
    }
}

__FORCE_INLINE static void arm_enable_hivecs(void) {
    air_u32_t sctlr = arm_cp15_get_system_control();
    if (!(sctlr & ARM_SCTLR_V)) {
        sctlr |= ARM_SCTLR_V;
        arm_cp15_set_system_control(sctlr);
    }
}

// TODO arm_get_monitor_exception_base_address
__FORCE_INLINE static air_u32_t arm_cp15_get_exception_base_address(void) {

if((arm_is_hivecs()) != 0) {
        return 0xffff0000;
    }

//    if () TODO no security extensions

    air_u32_t val;
    __asm__ volatile ("mrc p15, 0, %0, c12, c0, 0\n":"=r" (val)::"memory");
    return val;
}

__FORCE_INLINE static void arm_cp15_set_exception_base_address(air_u32_t val) {
    arm_disable_hivecs();
    __asm__ volatile ("mcr p15, 0, %0, c12, c0, 0\n"::"r" (val):"memory");
}

/**
 * \details
 * loads the i-th core from the pmk_sharedarea_t
 * the pmk_core_ctrl_t * points to a contiguous memory of n cores
 * since pmk_core_ctrl_t holds 8 4byte values, access to the next cores can be done by
 * adding the sizeof pmk_core_ctrl_t i times. since 8*4 = 32 = 2⁵, the value can more efficiently
 * be shifted left by 5
 */
__FORCE_INLINE static void arm_cp15_setup_Per_CPU(air_u32_t cpu_id) {
    air_u32_t val;
    air_u32_t offset = 4;
    __asm__ volatile (
            "ldr %[rx], =air_shared_area\n"
            "ldr %[rx], [%[rx], %[offset]]\n"
            "add %[rx], %[rx], %[id], lsl #5\n"
            "mcr p15, 0, %[rx], c13, c0, 4\n"
            : [rx] "=&r" (val)
            : [id] "r" (cpu_id), [offset] "r" (offset)
            : "memory");
}

__FORCE_INLINE static air_u32_t arm_cp15_get_Per_CPU(void) {
	air_u32_t val;
    __asm__ volatile ("mrc p15, 0, %0, c13, c0, 4\n":"=r" (val));
    return val;
}

__FORCE_INLINE static air_u32_t arm_cp15_get_CPACR(void) {
    air_u32_t val;
    __asm__ volatile ("mrc p15, 0, %0, c1, c0, 2\n":"=r" (val));
    return val;
}

__FORCE_INLINE static void arm_cp15_set_CPACR(air_u32_t val) {
    __asm__ volatile ("mcr p15, 0, %0, c1, c0, 2\n"::"r" (val));
}

__FORCE_INLINE static air_u32_t arm_cp15_get_CONTEXTIDR(void) {
    air_u32_t val;
    __asm__ volatile ("mrc p15, 0, %0, c13, c0, 1\n":"=r" (val));
    return val;
}

__FORCE_INLINE static void arm_cp15_set_CONTEXTIDR(air_u32_t val) {
    __asm__ volatile ("mcr p15, 0, %0, c13, c0, 1\n"::"r" (val));
}

/**
 * Fault status/address registers
 */
__FORCE_INLINE static air_u32_t arm_cp15_get_IFSR(void) {
    air_u32_t val;
    __asm__ volatile ("mrc p15, 0, %0, c5, c0, 1\n":"=r" (val));
    return val;
}

__FORCE_INLINE static air_u32_t arm_cp15_get_IFAR(void) {
    air_u32_t val;
    __asm__ volatile ("mrc p15, 0, %0, c6, c0, 2\n":"=r" (val));
    return val;
}

__FORCE_INLINE static air_u32_t arm_cp15_get_DFSR(void) {
    air_u32_t val;
    __asm__ volatile ("mrc p15, 0, %0, c5, c0, 0\n":"=r" (val));
    return val;
}

__FORCE_INLINE static air_u32_t arm_cp15_get_DFAR(void) {
    air_u32_t val;
    __asm__ volatile ("mrc p15, 0, %0, c6, c0, 0\n":"=r" (val));
    return val;
}
#endif /* CP15_H_ */
