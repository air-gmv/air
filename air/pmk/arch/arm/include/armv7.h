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

#ifdef ASM

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
.thumb
.align 2
    bx      pc
    nop
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


#define ARM_CORE_CONTEXT_TRASHED    1


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


/* @brief Synchronization barries
 * TODO specific compiler optimizations
 */
inline static void arm_instruction_synchronization_barrier(void) {
#if defined(__CC_ARM)
    __isb(15);
//#elif defined(__GNUC__)
//  __ISB();
#else
    __asm__ volatile ("isb");
#endif
}

inline static void arm_data_synchronization_barrier(air_u32_t intrinsic) {
#if defined(__CC_ARM)
    __dsb(intrinsic);
//#elif defined(__GNUC__)
//  __DSB();
#else
    __asm__ volatile ("dsb");
#endif
}

inline static void arm_data_memory_barrier(air_u32_t intrinsic) {
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
    air_u32_t reserved;                 /**< svc immediate. UND if other    */
} arm_interrupt_stack_frame_t;

/**
 *  @brief Context saved on stack for a supervisor call.
 */
typedef struct {
    air_u32_t r0;
    air_u32_t r1;
    air_u32_t r2;
    air_u32_t r3;
    air_u32_t r4;
    air_u32_t r5;
    air_u32_t reserved;
    air_u32_t lr;
} arm_supervisor_stack_frame_t;

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
    void *isf_pointer;                  /**< core stack pointer             */
    void *idle_isf_pointer;             /**< core ISF stack                 */
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
    air_uptr_t ttbr0;                   /**< context id                     */
    air_uptr_t ttbr1;                   /**< context id                     */
    air_u32_t ttbcr;
} arm_mmu_context_t;

/**
 * @brief ARM MMU Configuration
 * TODO COMPATIBILITY ONLY. NOT USED
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

/****************************** CPU Register Access ***************************
 * @brief assembly inline functions to access cpu regs
 */
static inline void arm_disable_fpu(void) {
    ARM_SWITCH_REGISTERS;
    air_u32_t reg = 0;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "vmrs %[reg], FPEXC\n\t"
        "bic %[reg], %[reg], #30\n\t"
        "vmsr FPEXC, %[reg]\n\t"
        ARM_SWITCH_BACK
        : ARM_SWITCH_OUTPUT
        : [reg] "r" (reg)
    );
}

static inline void arm_enable_fpu(void) {
    ARM_SWITCH_REGISTERS;
    air_u32_t reg = 0;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "vmrs %[reg], FPEXC\n\t"
        "orr %[reg], %[reg], #(1 << 30U)\n\t"
        "vmsr FPEXC, %[reg]\n\t"
        ARM_SWITCH_BACK
        : ARM_SWITCH_OUTPUT
        : [reg] "r" (reg)
    );
}

static inline air_u32_t arm_get_exception_base_address(void) {
    ARM_SWITCH_REGISTERS;
    air_u32_t reg;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "mrc p15, 0, %[reg], c1, c0, 0\n\t"
        "tst %[reg], #(1 << 13)\n\t"
        "beq hyvecs\n\t"
        "mrc p15, 0, %[reg], c12, c0, 0\n\t"
        "hyvecs:\n\t"
        "mov %[reg], #0x0000\n\t"
        "movt %[reg], #0xffff\n\t"
        ARM_SWITCH_BACK
        : [reg] "=&r" (reg) ARM_SWITCH_ADDITIONAL_OUTPUT
    );
    return reg;
}

static inline void arm_set_exception_base_address(air_u32_t val) {
    ARM_SWITCH_REGISTERS;
    air_u32_t reg = 0;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "mrc p15, 0, %[reg], c1, c0, 0\n\t"
        "bic %[reg], %[reg], #13\n\t"
        "mcr p15, 0, %[reg], c1, c0, 0\n\t"
        "mcr p15, 0, %[val], c12, c0, 0\n\t"
        ARM_SWITCH_BACK
        : ARM_SWITCH_OUTPUT
        : [val] "r" (val), [reg] "r" (reg)
    );
}

static inline air_u32_t arm_get_cpsr(void) {
    ARM_SWITCH_REGISTERS;
    air_u32_t reg;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "mrs %[reg], cpsr\n\t"
        ARM_SWITCH_BACK
        : [reg] "=&r" (reg) ARM_SWITCH_ADDITIONAL_OUTPUT
    );
    return reg;
}

static inline void arm_set_cpsr(air_u32_t val) {
    ARM_SWITCH_REGISTERS;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "msr cpsr, %[val]\n\t"
        ARM_SWITCH_BACK
        : ARM_SWITCH_OUTPUT
        : [val] "r" (val)
    );
}

static inline void arm_disable_preemption(air_u32_t flags) {
    ARM_SWITCH_REGISTERS;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "cpsid aif\n\t"
        ARM_SWITCH_BACK
        : ARM_SWITCH_OUTPUT
    );
}

static inline void arm_enable_preemption(air_u32_t flags) {
    ARM_SWITCH_REGISTERS;

    __asm__ volatile (
        ARM_SWITCH_TO_ARM
        "cpsie aif\n\t"
        ARM_SWITCH_BACK
        : ARM_SWITCH_OUTPUT
    );
}

/******************************************************************************/
/*********************** Assembly functions declaration ***********************/
//void arm_core_context_save(void *core);
//void arm_core_context_restore(void *core);

void exception_undef(void);
void exception_svc(void);
void exception_pref_abort(void);
void exception_data_abort(void);
void exception_irq(void);
void exception_fiq(void);

#endif /* not ASM */
#endif /* ARM_ARMv7_H */
