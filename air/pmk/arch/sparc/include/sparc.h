/*
 * Copyright (C) 2013-2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief
 */

#ifndef __SPARC_H__
#define __SPARC_H__

#include <air_arch.h>

#define SPARC_STACK_ALIGNMENT (16)
#define SPARC_TRAP_COUNT (256)
#define SPARC_STACK_MINIMUM_SIZE (1024 * 4)
#define SPARC_REAL_TRAP_NUMBER(_trap) ((_trap))
/**
 *  @brief determine the number of register windows for SPARC
 */
#define SPARC_REGISTER_WINDOWS_COUNT 8

/*
 *  Miscellaneous constants
 */

/**
 *  PSR masks and starting bit positions
 *
 *  NOTE: Reserved bits are ignored.
 */
#if (SPARC_REGISTER_WINDOWS_COUNT == 8)
#define SPARC_PSR_CWP_MASK 0x07 /* bits  0 -  4 */
#elif (SPARC_REGISTER_WINDOWS_COUNT == 16)
#define SPARC_PSR_CWP_MASK 0x0F /* bits  0 -  4 */
#elif (SPARC_REGISTER_WINDOWS_COUNT == 32)
#define SPARC_PSR_CWP_MASK 0x1F /* bits  0 -  4 */
#else
#error "Unsupported number of register windows for this cpu"
#endif

#define SPARC_PSR_ET_MASK 0x00000020   /* bit   5 */
#define SPARC_PSR_PS_MASK 0x00000040   /* bit   6 */
#define SPARC_PSR_S_MASK 0x00000080    /* bit   7 */
#define SPARC_PSR_PIL_MASK 0x00000F00  /* bits  8 - 11 */
#define SPARC_PSR_EF_MASK 0x00001000   /* bit  12 */
#define SPARC_PSR_EC_MASK 0x00002000   /* bit  13 */
#define SPARC_PSR_ICC_MASK 0x00F00000  /* bits 20 - 23 */
#define SPARC_PSR_VER_MASK 0x0F000000  /* bits 24 - 27 */
#define SPARC_PSR_IMPL_MASK 0xF0000000 /* bits 28 - 31 */
#define SPARC_PSR_POS_MASK 0x00F03040

#define SPARC_PSR_CWP_BIT_POSITION 0   /* bits  0 -  4 */
#define SPARC_PSR_ET_BIT_POSITION 5    /* bit   5 */
#define SPARC_PSR_PS_BIT_POSITION 6    /* bit   6 */
#define SPARC_PSR_S_BIT_POSITION 7     /* bit   7 */
#define SPARC_PSR_PIL_BIT_POSITION 8   /* bits  8 - 11 */
#define SPARC_PSR_EF_BIT_POSITION 12   /* bit  12 */
#define SPARC_PSR_EC_BIT_POSITION 13   /* bit  13 */
#define SPARC_PSR_ICC_BIT_POSITION 20  /* bits 20 - 23 */
#define SPARC_PSR_VER_BIT_POSITION 24  /* bits 24 - 27 */
#define SPARC_PSR_IMPL_BIT_POSITION 28 /* bits 28 - 31 */

#if PMK_FPU_SUPPORT
#define SPARC_PSR_INIT 0x00001FE0 /* Initial PSR w EF */
#define SPARC_WIM_INIT 0x00000002 /* Initial WIM */
#else
#define SPARC_PSR_INIT 0x00000FE0 /* Initial PSR wo EF */
#define SPARC_WIM_INIT 0x00000002 /* Initial WIM */
#endif

#ifndef ASM

#include <air_arch.h>

/**
 *  @brief SPARC minimum stack frame
 *  @note As defined by the SPARC ABI (minimum stack frame)
 */
typedef struct
{

    air_u32_t l0;
    air_u32_t l1;
    air_u32_t l2;
    air_u32_t l3;
    air_u32_t l4;
    air_u32_t l5;
    air_u32_t l6;
    air_u32_t l7;
    air_u32_t i0;
    air_u32_t i1;
    air_u32_t i2;
    air_u32_t i3;
    air_u32_t i4;
    air_u32_t i5;
    air_u32_t i6_fp;
    air_u32_t i7;
    void *struct_addr;    /**< structure return address                  */
    air_u32_t saved_arg0; /**<                                           */
    air_u32_t saved_arg1;
    air_u32_t saved_arg2;
    air_u32_t saved_arg3;
    air_u32_t saved_arg4;
    air_u32_t saved_arg5;
    air_u32_t dummy; /**< dummy entry for ldd and std instructions */

} sparc_stack_frame_t;

/**
 *  @brief Context saved on stack for an interrupt.
 */
typedef struct
{

    sparc_stack_frame_t stack_frame;

    air_u32_t psr;
    air_u32_t pc;
    air_u32_t nkpc;
    air_u32_t tpc;
    air_u32_t psp;

    air_u32_t g1;
    air_u32_t g2;
    air_u32_t g3;
    air_u32_t g4;
    air_u32_t g5;
    air_u32_t g6;
    air_u32_t g7;

    air_u32_t i0;
    air_u32_t i1;
    air_u32_t i2;
    air_u32_t i3;
    air_u32_t i4;
    air_u32_t i5;
    air_u32_t i6_fp;
    air_u32_t i7;

    air_u32_t y;
    air_u32_t dummy; /**< dummy entry for ldd and std instructions */

} sparc_interrupt_stack_frame_t;

/**
 *  @brief Floating Point Unit (FPU) registers context
 */
typedef struct
{

    double f0_f1;   /**< registers f0 and f1                        */
    double f2_f3;   /**< registers f2 and f3                        */
    double f4_f5;   /**< registers f4 and f5                        */
    double f6_f7;   /**< registers f6 and f6                        */
    double f8_f9;   /**< registers f8 and f9                        */
    double f10_f11; /**< registers f10 and f11                      */
    double f12_f13; /**< registers f12 and f13                      */
    double f14_f15; /**< registers f14 and f15                      */
    double f16_f17; /**< registers f16 and f19                      */
    double f18_f19; /**< registers f18 and f10                      */
    double f20_f21; /**< registers f20 and f21                      */
    double f22_f23; /**< registers f22 and f23                      */
    double f24_f25; /**< registers f24 and f25                      */
    double f26_f27; /**< registers f26 and f27                      */
    double f28_f29; /**< registers f28 and f29                      */
    double f30_f31; /**< registers f30 and f31                      */
    air_u32_t fsr;  /**< FPU status register                        */

} sparc_fpu_context_t;

/**
 * @brief Virtual SPARC CPU
 */
typedef struct
{

    air_u32_t id;       /**< virtual CPU id                 */
    air_u32_t tbr;      /**< virtual TBR                    */
    air_u32_t psr;      /**< virtual PSR                    */
    air_u32_t imask;    /**< interrupt mask                 */
    air_u32_t ipend;    /**< interrupts pending             */
    air_u32_t cctrl;    /**< cache control                  */
    air_u32_t mmu_ctrl; /**< MMU control register           */
    air_u32_t mmu_fsr;  /**< MMU fault register             */

} sparc_virtual_cpu_t;

/**
 * @brief Structure to hold a CPU partition context
 */
typedef struct
{

    sparc_virtual_cpu_t vcpu;         /**< virtual CPU control            */
    air_u32_t trash;                  /**< trash flag                     */
    void *entry_point;                /**< core entry point               */
    void *stack_pointer;              /**< core stack pointer             */
    void *isf_stack_pointer;          /**< core ISF stack                 */
    air_u32_t isr_nesting_level;      /**< core interrupt nesting level   */
    sparc_fpu_context_t *fpu_context; /**< floating point                 */
    air_u32_t ipc_event;              /**< IPC event                      */
    air_u32_t state;                  /**< system state                   */
    void *hm_event;                   /**< health-monitor event           */

} sparc_core_context_t;

/**
 * @brief SPARC MMU control
 */
typedef struct
{

    air_u32_t context;    /**< context id                     */
    air_u32_t *l1_tables; /**< pointer to the L1 tables       */

} sparc_mmu_context_t;

/**
 * @brief SPARC Configuration
 */
typedef struct
{

    air_u32_t mmu_context_entries;
    air_u32_t mmu_l1_tables_entries;
    air_u32_t mmu_l2_tables_entries;
    air_u32_t mmu_l3_tables_entries;

} sparc_configuration_t;

/**
 * @brief Enable Preemption
 * @return PSR control field
 */
air_u32_t sparc_enable_preemption();

/**
 * @brief Disable Preemption
 * @param flags PSR control field
 */
void sparc_disable_preemption(air_u32_t flags);

/**
 * @brief Install a raw handler in the trap table
 * @param n Trap number
 * @param handler Function handler
 */
void *sparc_install_raw_trap_handler(air_u32_t n, void *handler);

/**
 * @brief Install a C trap handler in the trap table
 * @param n Trap number
 * @param handler Function handler
 */
void *sparc_install_vector_trap_handler(air_u32_t n, void *handler);

/**
 * @brief Register a function handler in the C trap handler vector
 * @param n Trap number
 * @param handler Function handler
 */
void *sparc_register_vector_trap_handler(air_u32_t n, void *handler);

/**
 * @brief Initializes the health-monitor traps handlers
 */
void sparc_healthmonitor_init();

/**
 * @brief Initializes the memory segregation
 */
void sparc_segregation_init();

/**
 * @brief Maps virtual address to physical address with given permissions
 * @param context MMU context
 * @param p_addr Initial physical address
 * @param v_addr Initial virtual address
 * @param size Size of the memory block
 * @param unit MMU unit of division
 * @param permissions Memory block permissions
 */
void sparc_map_memory(sparc_mmu_context_t *context, void *p_addr, void *v_addr, air_sz_t size, air_u32_t unit,
                      air_u32_t permissions);

/**
 * @brief Copies a block of memory to the user-land
 * @param core_ctx Executing core context
 * @param dst Destination address
 * @param src Source address
 * @param size Size of the memory block
 * @return zero if no faults, non-zero  otherwise
 */
air_u32_t sparc_copy_to_user(sparc_core_context_t *core_ctx, void *dst, void *src, air_sz_t size);

/**
 * @brief Copies a block of memory from the user-land
 * @param core_ctx Executing core context
 * @param dst Destination address
 * @param src Source address
 * @param size Size of the memory block
 * @return zero if no faults, non-zero  otherwise
 */
air_u32_t sparc_copy_from_user(sparc_core_context_t *core_ctx, void *dst, void *src, air_sz_t sizd);

/**
 * @brief Get the physical address from the virtual one
 * @param context MMU context
 * @param v_addr Virtual address
 * @return Physical address if available, NULL otherwise
 */
void *sparc_get_physical_addr(sparc_mmu_context_t *context, void *v_addr);

/**
 * @brief Enables MMU
 */
void sparc_enable_mmu();

/**
 * @brief Saves the core context
 * @param core_ctrl control control pointer
 */
void sparc_core_context_save(void *core);

/**
 * @brief Restores the core context
 * @param core_ctrl control control pointer
 */
void sparc_core_context_restore(void *core);

/**
 * @brief Spin lock with no preemption
 * @param lock Lock variable
 * @return ISR level (PIL)
 */
air_u32_t sparc_lock_irqsave(air_u32_t *lock);

/**
 * @brief Spin unlock with no preemption
 * @param lock Lock variable
 * @param isr_level ISR level (PIL) to restore
 */
void sparc_unlock_irqrestore(air_u32_t *lock, air_u32_t isr_level);

#endif /* ASM */
#endif /* __SPARC_H__ */
