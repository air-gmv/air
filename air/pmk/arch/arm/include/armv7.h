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

#define ARM_VFP_FPEXC_ENABLE (1 << 30)

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
    #if defined(__ARM_NEON)
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
    air_u32_t fpexc;
    air_u32_t fpscr;
    air_u64_t d0;
    air_u64_t d1;
    air_u64_t d2;
    air_u64_t d3;
    air_u64_t d4;
    air_u64_t d5;
    air_u64_t d6;
    air_u64_t d7;
    air_u64_t d8;
    air_u64_t d9;
    air_u64_t d10;
    air_u64_t d11;
    air_u64_t d12;
    air_u64_t d13;
    air_u64_t d14;
    air_u64_t d15;
    air_u64_t d16;
    air_u64_t d17;
    air_u64_t d18;
    air_u64_t d19;
    air_u64_t d20;
    air_u64_t d21;
    air_u64_t d22;
    air_u64_t d23;
    air_u64_t d24;
    air_u64_t d25;
    air_u64_t d26;
    air_u64_t d27;
    air_u64_t d28;
    air_u64_t d29;
    air_u64_t d30;
    air_u64_t d31;
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
    air_u32_t usr_sp;                   /**< pre-exception sp               */
    air_u32_t usr_lr;                   /**< pre-exception lr               */
    air_u32_t ret_addr;                 /**< return addr after the exception*/
    air_u32_t ret_psr;                  /**< pre-exception cpsr             */
    symbolic_exception_name exception_name;
    arm_vfp_context_t *vfp_context;
    air_u32_t reserved;
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
    air_u32_t lr;                       /**< reserved                       */
} arm_supervisor_stack_frame_t;

/**
 * @brief Virtual SPARC CPU
 */
typedef struct {
    air_u32_t id;                       /**< virtual CPU id                 */
    air_u32_t psr;                      /**< virtual PSR                    */
    air_u32_t **vbar;                   /**< virtual vector base address    */
    air_u32_t cctrl;                    /**< cache control                  */
} arm_virtual_cpu_t;

#define ARM_ISR_ABT_PENDING (1 << 8)
#define ARM_ISR_IRQ_PENDING (1 << 7)
#define ARM_ISR_FIQ_PENDING (1 << 6)
typedef struct {
    air_u32_t isr;                      /**< Interrupt Status Register(cp15)*/
    air_u32_t iccpmr;                   /**< icc priority mask              */
    air_u32_t iccbpr;                   /**< icc binary point               */
    air_u32_t icciar;                   /**< icc interrupt accept           */
//  air_u32_t eoi;                      /**< end of interrupt               */
//  air_u32_t rp;                       /**< running priority               */
//  air_u32_t hppi;                     /**< highest pending priority       */
//  air_u32_t abp;                      /**< icc control                    */
//  air_u32_t iid;                      /**< interface id                   */
    air_u32_t icdiser[32];              /**< icd set-enable                 */
    air_u32_t icdispr[32];              /**< icd set-pending                */
    air_u32_t icdipr[1020];             /**< icd priority                   */
} arm_virtual_gic_t;

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
    air_u32_t ipc_event;                /**< IPC event                      */
    air_u32_t state;                    /**< system state                   */
    void *hm_event;                     /**< health-monitor event           */
    arm_virtual_gic_t vgic;             /**< virtual GIC CPU control        */
} arm_core_context_t;                   /*  vGIC MUST BE LAST DUE TO SIZE   */

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
    air_u32_t reg = 0;
    __asm__ volatile (
            "vmrs %0, FPEXC\n"
            "bic %0, #30\n"
            "vmsr FPEXC, %0\n"
            :
            : "r" (reg));
}

static inline void arm_enable_fpu(void) {
    air_u32_t reg = 0;
    __asm__ volatile (
            "vmrs %0, FPEXC\n"
            "orr %0, #(1 << 30U)\n"
            "vmsr FPEXC, %0\n"
            :
            : "r" (reg));
}

static inline air_u32_t arm_get_cpsr(void) {
    air_u32_t reg;
    __asm__ volatile ("mrs %0, cpsr\n":"=r" (reg));
    return reg;
}

static inline void arm_set_cpsr(air_u32_t val) {
    __asm__ volatile ("msr cpsr, %0\n"::"r" (val));
}

static inline void arm_disable_interrupts() {
    __asm__ volatile ("cpsid if\n");
}

static inline void arm_enable_interrupts() {
    __asm__ volatile ("cpsie aif\n");
}

static inline air_u32_t arm_is_vint_enabled(
        air_u32_t *icdiser,
        air_u32_t id) {

    return (icdiser[id/32] & (id & 0x1f));
}

static inline air_u32_t arm_get_vint_priority(
        air_u32_t *icdipr,
        air_u32_t id) {

    return icdipr[id];
}

static inline void arm_set_vint_pending(
        air_u32_t *icdispr,
        air_u32_t id) {

    icdispr[id/32] |= (1U << (id & 0x1f));
}

/******************************************************************************/
/*********************** Assembly functions declaration ***********************/
//void arm_core_context_save(void *core);
//void arm_core_context_restore(void *core);

//void arm_cp15_setup_Per_CPU(air_u32_t cpu_id);
void exception_undef(void);
void exception_svc(void);
void exception_pref_abort(void);
void exception_data_abort(void);
void exception_irq(void);
void exception_fiq(void);

#endif /* not ASM */
#endif /* ARM_ARMv7_H */
