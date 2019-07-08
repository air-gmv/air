/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file arm.h
 * @author lumm
 * @brief Arm macros and typedef definitions.
 */

#ifndef ARM_ARMv7_H
#define ARM_ARMv7_H

#ifdef ASM

/**
 * @brief Program Status Register.
 * Each program mode has its own Saved PSR (SPSR). usr and sys shared theirs.
 */
#define ARM_PSR_N           (1 << 31)   /**< Negative condition flag        */
#define ARM_PSR_Z           (1 << 30)   /**< Zero condition flag            */
#define ARM_PSR_C           (1 << 29)   /**< Carry condition flag           */
#define ARM_PSR_V           (1 << 28)   /**< Overflow condition flag        */
#define ARM_PSR_Q           (1 << 27)   /**< Cumulative saturation          */
#define ARM_PSR_J           (1 << 24)   /**< Jazelle or ThumbEE (T also 1)  */
#define ARM_PSR_GE_MASK     (0xf << 16) /**< >= condition flags             */
#define ARM_PSR_E           (1 << 9)    /**< Endianness bit (0-low, 1-big)  */
#define ARM_PSR_EXC_MASK    (0x7 << 6)  /**< Exceptions mask                */
#define ARM_PSR_A           (1 << 8)    /**< Asynchronous abort             */
#define ARM_PSR_I           (1 << 7)    /**< IRQ mask                       */
#define ARM_PSR_F           (1 << 6)    /**< FIQ mask                       */
#define ARM_PSR_T           (1 << 5)    /**< Thumb execution state          */
#define ARM_PSR_MODE_MASK   (0x1f << 0) /**< Operating mode mask            */
#define ARM_PSR_USR         0x10        /**< User                           */
#define ARM_PSR_FIQ         0x11        /**< FIQ                            */
#define ARM_PSR_IRQ         0x12        /**< IRQ                            */
#define ARM_PSR_SVC         0x13        /**< Supervisor                     */
#define ARM_PSR_MON         0x16        /**< Monitor                        */
#define ARM_PSR_ABT         0x17        /**< Abort                          */
#define ARM_PSR_HYP         0x1a        /**< Hypervisor                     */
#define ARM_PSR_UND         0x1b        /**< Undefined                      */
#define ARM_PSR_SYS         0x1f        /**< System                         */

/* @brief If the bit 0 of the bx reg is 0 it will remain in ARM, if 1 it will
 * change to Thumb. The .thumb directive is for the assembler to compile the
 * following instructions as Thumb. Same way for .arm.
 */
.macro SWITCH_FROM_ARM_TO_THUMB r
#if defined(__thumb__)
    add     \r, pc, #1
    bx      \r
.thumb
#endif
.endm

.macro SWITCH_FROM_THUMB_TO_ARM
#if defined(__thumb__)
.align 2
    bx      pc
.arm
#endif
.endm

#if defined(PMK_FPU_SUPPORT)
    #if defined(ARM_NEON)
        #define VFP_D32
    #else
        #define VFP_D16
    #endif
#endif


/** @brief Size for the exception saved information. 20 uint32 */
#define ARM_EXCEPTION_FRAME_SIZE 80
#define ARM_EXCEPTION_FRAME_VFP_CONTEXT_OFFSET 72
#define ARM_VFP_CONTEXT_SIZE 264

#else /* ASM */
#include <air_arch.h>

#if defined(__thumb__)
    #define ARM_SWITCH_REGISTERS air_u32_t arm_switch_reg
    #define ARM_SWITCH_BACK "add %[arm_switch_reg], pc, #1\nbx %[arm_switch_reg]\n.thumb\n"
    #define ARM_SWITCH_TO_ARM ".align 2\nbx pc\n.arm\n"
    #define ARM_SWITCH_OUTPUT [arm_switch_reg] "=&r" (arm_switch_reg)
    #define ARM_SWITCH_ADDITIONAL_OUTPUT , ARM_SWITCH_OUTPUT
#else
    #define ARM_SWITCH_REGISTERS
    #define ARM_SWITCH_BACK
    #define ARM_SWITCH_TO_ARM
    #define ARM_SWITCH_OUTPUT
    #define ARM_SWITCH_ADDITIONAL_OUTPUT
#endif /* defined(__thumb__) */


// TODO
inline static void instruction_synchronization_barrier(void) {
#if defined(__CC_ARM)
    __isb(15);
//#elif defined(__GNUC__)
//  __ISB();
#else
    __asm__ volatile ("isb");
#endif
}

inline static void data_synchronization_barrier(air_u32_t intrinsic) {
#if defined(__CC_ARM)
    __dsb(intrinsic);
//#elif defined(__GNUC__)
//  __DSB();
#else
    __asm__ volatile ("dsb");
#endif
}

inline static void data_memory_barrier(air_u32_t intrinsic) {
#if defined(__CC_ARM)
    __dmb(intrinsic);
//#elif defined(__GNUC__)
//  __DMB();
#else
    __asm__ volatile ("dmb");
#endif
}

typedef enum {
    ARM_EXCEPTION_RESET = 0,
    ARM_EXCEPTION_UNDEF = 1,
    ARM_EXCEPTION_SWI = 2,
    ARM_EXCEPTION_PREF_ABORT = 3,
    ARM_EXCEPTION_DATA_ABORT = 4,
    ARM_EXCEPTION_RESERVED = 5,
    ARM_EXCEPTION_IRQ = 6,
    ARM_EXCEPTION_FIQ = 7,
    MAX_EXCEPTIONS = 8,
} symbolic_exception_name;

/**
 *  @brief Floating Point Unit (FPU) registers context
 */
typedef struct {
    air_u32_t register_fpexc;
    air_u32_t register_fpscr;
    air_u64_t register_d0;
    air_u64_t register_d1;
    air_u64_t register_d2;
    air_u64_t register_d3;
    air_u64_t register_d4;
    air_u64_t register_d5;
    air_u64_t register_d6;
    air_u64_t register_d7;
    air_u64_t register_d8;
    air_u64_t register_d9;
    air_u64_t register_d10;
    air_u64_t register_d11;
    air_u64_t register_d12;
    air_u64_t register_d13;
    air_u64_t register_d14;
    air_u64_t register_d15;
    air_u64_t register_d16;
    air_u64_t register_d17;
    air_u64_t register_d18;
    air_u64_t register_d19;
    air_u64_t register_d20;
    air_u64_t register_d21;
    air_u64_t register_d22;
    air_u64_t register_d23;
    air_u64_t register_d24;
    air_u64_t register_d25;
    air_u64_t register_d26;
    air_u64_t register_d27;
    air_u64_t register_d28;
    air_u64_t register_d29;
    air_u64_t register_d30;
    air_u64_t register_d31;
} arm_vfp_context_t;

/**
 *  @brief Context saved on stack for an interrupt.
 */
typedef struct {
    air_u32_t r0;
    air_u32_t r1;
    air_u32_t r2;
    air_u32_t r3;
    air_u32_t r4;
    air_u32_t r5;
    air_u32_t r6;
    air_u32_t r7;
    air_u32_t r8;
    air_u32_t r9;
    air_u32_t r10;
    air_u32_t r11;
    air_u32_t r12;
    air_uptr_t orig_sp;                 /**< pre-exception sp               */
    air_uptr_t orig_lr;                 /**< pre-exception lr               */
    air_uptr_t lr;                      /**< return addr after the exception*/
    air_u32_t orig_cpsr;                /**< pre-exception cpsr             */
    symbolic_exception_name exception_name;
    const arm_vfp_context_t *vfp_context;
    air_u32_t svc_imm; /* used in Supervisor Calls (SVC) */
} arm_exception_frame_t;

/**
* @brief Virtual SPARC CPU
*/
typedef struct {
    air_u32_t id;                       /**< virtual CPU id                 */
    air_u32_t tbr;                      /**< virtual TBR                    */
    air_u32_t psr;                      /**< virtual PSR                    */
    air_u32_t imask;                    /**< interrupt mask                 */
    air_u32_t ipend;                    /**< interrupts pending             */
    air_u32_t cctrl;                    /**< cache control                  */
    air_u32_t mmu_ctrl;                 /**< MMU control register           */
    air_u32_t mmu_fsr;                  /**< MMU fault register             */
} arm_virtual_cpu_t;

/**
 * @brief Structure to hold a CPU partition context
 */
typedef struct {
    arm_virtual_cpu_t vcpu;             /**< virtual CPU control            */
    air_u32_t trash;                    /**< trash flag                     */
    void *entry_point;                  /**< core entry point               */
    void *stack_pointer;                /**< core stack pointer             */
    void *isf_stack_pointer;            /**< core ISF stack                 */
    air_u32_t isr_nesting_level;        /**< core interrupt nesting level   */
    arm_vfp_context_t *fpu_context;     /**< floating point                 */
    air_u32_t ipc_event;                /**< IPC event                      */
    air_u32_t state;                    /**< system state                   */
    void *hm_event;                     /**< health-monitor event           */
} arm_core_context_t;

/**
 * @brief ARM MMU control
 */
typedef struct {
    air_u32_t context;                  /**< context id                     */
    air_u32_t *l1_tables;               /**< pointer to the L1 tables       */
} arm_mmu_context_t;

/**
 * @brief ARM MMU Configuration
 */
typedef struct {
    air_u32_t mmu_context_entries;
    air_u32_t mmu_l1_tables_entries;
    air_u32_t mmu_l2_tables_entries;
    air_u32_t mmu_l3_tables_entries;
} arm_mmu_configuration_t;

/* SVC defines */
#define MAX_SVC_A32 16777216            /**< number of svc calls in A32     */
#define MAX_SVC_T32 256                 /**< number of svc calls in thumb   */
#if defined(__thumb__)
#define MAX_SVC MAX_SVC_T32
#else
#define MAX_SVC MAX_SVC_A32
#endif

#endif /* ASM */

#endif /* ARM_ARMv7_H */
