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
 * @brief SPARC virtualization system calls (PMK side)
 */

/* enforce ASM definition */
#ifndef ASM
#define ASM
#endif

#include <air.h>
#include <pmk.h>
#include <asm.h>
#include <bsp.h>
#include <sparc.h>
#include <partition.h>

/**
 * @brief SPARC specific system calls
 */
    GLOBAL(sparc_virt_syscall)
SYM(sparc_virt_syscall):

    /* the system call is synchronous */
    mov     %l2, %l1
    add     %l2, 0x04, %l2

    /* check if the system call is valid */
    cmp     %i5, AIR_SYSCALL_SPARC_COUNT
    bgeu    syscall_sparc_return
    nop

    /* get current core control */
    rd      %asr17, %l4
    srl     %l4, 28, %l4
    and     %l4, 0xff, %l4
    umul    %l4, CORE_CTRL_SIZE, %l4                    ! core control offset

    set     SYM(air_shared_area), %l3                   ! shared area
    ld      [%l3 + SHAREDAREA_CORE_CTRL], %l3           ! core control area
    add     %l3, %l4, %l3                               ! current core control

    /* offset within the system call branch table */
    set     syscall_sparc_branch_table, %l4
    umul    %i5, 0x04, %l5
    jmp     %l4 + %l5
    nop

/**
 * @brief System call branch table
 */
syscall_sparc_branch_table:

    ba,a    syscall_sparc_disable_interrupts
    ba,a    syscall_sparc_enable_interrupts
    ba,a    syscall_sparc_disable_traps
    ba,a    syscall_sparc_enable_traps
    ba,a    syscall_sparc_disable_fpu
    ba,a    syscall_sparc_enable_fpu
    ba,a    syscall_sparc_get_tbr
    ba,a    syscall_sparc_set_tbr
    ba,a    syscall_sparc_get_psr
    ba,a    syscall_sparc_set_psr
    ba,a    syscall_sparc_rett
    ba,a    syscall_sparc_get_cache_register
    ba,a    syscall_sparc_set_cache_register
    ba,a    syscall_sparc_restore_cache_register
    ba,a    syscall_sparc_get_irq_mask_register
    ba,a    syscall_sparc_set_irq_mask_register
    ba,a    syscall_sparc_set_irq_force_register
    ba,a    syscall_sparc_get_inst_cache_config_register
    ba,a    syscall_sparc_get_data_cache_config_register
    ba,a    syscall_sparc_set_ef

/**
 * @brief SPARC disable interrupts
 */
syscall_sparc_disable_interrupts:

    /* get virtual PSR */
    ld      [%l3 + CORE_CTRL_CONTEXT], %l3
    ld      [%l3 + CONTEXT_VCPU + SPARC_VCPU_PSR], %l4

    /* get current level */
    and     %l4, SPARC_PSR_PIL_MASK, %i0
    srl     %i0, SPARC_PSR_PIL_BIT_POSITION, %i0

    /* increase PIL to the highest level and return from trap */
    or      %l4, SPARC_PSR_PIL_MASK, %l4
    ba      syscall_sparc_return
    st      %l4, [%l3 + CONTEXT_VCPU + SPARC_VCPU_PSR]

/**
 * @brief SPARC enable interrupts
 */
syscall_sparc_enable_interrupts:

    /* get virtual PSR */
    ld      [%l3 + CORE_CTRL_CONTEXT], %l3
    ld      [%l3 + CONTEXT_VCPU + SPARC_VCPU_PSR], %l4

    /* clear level and apply the new one */
    andn    %l4, SPARC_PSR_PIL_MASK, %l4
    sll     %i0, SPARC_PSR_PIL_BIT_POSITION, %i0
    or      %l4, %i0, %l4

    /* get current level */
    ba      syscall_sparc_pedding_irq
    st      %l4, [%l3 + CONTEXT_VCPU + SPARC_VCPU_PSR]

/**
 * @brief SPARC disable traps
 */
syscall_sparc_disable_traps:

    /* get virtual PSR */
    ld      [%l3 + CORE_CTRL_CONTEXT], %l3
    ld      [%l3 + CONTEXT_VCPU + SPARC_VCPU_PSR], %l4

    /* disable traps */
    andn     %l4, SPARC_PSR_ET_MASK, %l4
    ba      syscall_sparc_return
    st      %l4,  [%l3 + CONTEXT_VCPU + SPARC_VCPU_PSR]

/**
 * @brief SPARC enable traps
 */
syscall_sparc_enable_traps:

    /* get virtual PSR */
    ld      [%l3 + CORE_CTRL_CONTEXT], %l3
    ld      [%l3 + CONTEXT_VCPU + SPARC_VCPU_PSR], %l4

    /* enable traps */
    or      %l4, SPARC_PSR_ET_MASK, %l4
    ba      syscall_sparc_pedding_irq
    st      %l4,  [%l3 + CONTEXT_VCPU + SPARC_VCPU_PSR]

/**
 * @brief SPARC disable FPU
 * @note This system call must be autorized by the configuration
 */
syscall_sparc_disable_fpu:

#if PMK_FPU_SUPPORT

    /* check the permissions to disable the FPU */
    ld      [%l3 + CORE_CTRL_PARTITION], %l4
    ld      [%l4 + PARTITION_PERMISSIONS], %l4
    btst    AIR_PERMISSION_FPU_CONTROL, %l4
    bz      syscall_sparc_return_invalid_config
    nop

    /* get EF mask */
    sethi   %hi(SPARC_PSR_EF_MASK), %l5
    or      %l5, %lo(SPARC_PSR_EF_MASK), %l5    ! EF Mask

    /* apply change to the virtual PSR */
    ld      [%l3 + CORE_CTRL_CONTEXT], %l3
    ld      [%l3 + CONTEXT_VCPU + SPARC_VCPU_PSR], %l4
    andn    %l4, %l5, %l4
    st      %l4, [%l3 + CONTEXT_VCPU + SPARC_VCPU_PSR]

    /* apply change to the real PSR */
    ba      syscall_sparc_return_no_error       ! finish trap
    andn    %l0, %l5, %l0                       ! delay slot

#else

    ba      syscall_sparc_return_invalid_config
    nop

#endif

/**
 * @brief SPARC enable FPU
 * @note This system call must be autorized by the configuration
 */
syscall_sparc_enable_fpu:

#if PMK_FPU_SUPPORT

    /* check the permissions to disable the FPU */
    ld      [%l3 + CORE_CTRL_PARTITION], %l4
    ld      [%l4 + PARTITION_PERMISSIONS], %l4
    btst    AIR_PERMISSION_FPU_CONTROL, %l4
    bz      syscall_sparc_return_invalid_config
    nop

    /* get EF mask */
    sethi   %hi(SPARC_PSR_EF_MASK), %l5
    or      %l5, %lo(SPARC_PSR_EF_MASK), %l5    ! EF Mask

    /* apply change to the virtual PSR */
    ld      [%l3 + CORE_CTRL_CONTEXT], %l3
    ld      [%l3 + CONTEXT_VCPU + SPARC_VCPU_PSR], %l4
    or      %l4, %l5, %l4
    st      %l4, [%l3 + CONTEXT_VCPU + SPARC_VCPU_PSR]

    /* apply change to the real PSR */
    ba      syscall_sparc_return_no_error       ! finish trap
    or      %l0, %l5, %l0                       ! delay slot

#else

    ba      syscall_sparc_return_invalid_config
    nop

#endif

/**
 * @brief SPARC get TBR
 */
syscall_sparc_get_tbr:

    /* get virtual TBR */
    ld      [%l3 + CORE_CTRL_CONTEXT], %l3
    ba      syscall_sparc_return
    ld      [%l3 + CONTEXT_VCPU + SPARC_VCPU_TBR], %i0

/**
 * @brief SPARC set TBR
 */
syscall_sparc_set_tbr:

    /* align address to 0x4 */
    andn    %i0, 0x3, %i0

    /* check if trap table lies within the partition space */
    ld      [%l3 + CORE_CTRL_PARTITION], %l4
    ld      [%l4 + PARTITION_MMAP], %l4

    /* check lower bound */
    ld      [%l4 + MMAP_V_ADDR], %l5
    cmp     %i0, %l5
    blu     syscall_sparc_return
    nop

    /* check top bound */
    ld      [%l4 + MMAP_SIZE], %l6
    add     %l5, %l6, %l5
    set     SPARC_TRAP_COUNT, %l6
    umul    %l6, 0x4, %l6
    add     %l6, %i0, %l6
    cmp     %l6, %l5
    bgeu    syscall_sparc_return
    nop

     /* set virtual TBR */
    ld      [%l3 + CORE_CTRL_CONTEXT], %l3
    ba      syscall_sparc_return
    st      %i0, [%l3 + CONTEXT_VCPU + SPARC_VCPU_TBR]

/**
 * @brief SPARC get PSR
 */
syscall_sparc_get_psr:

    /* get virtual PSR */
    ld      [%l3 + CORE_CTRL_CONTEXT], %l3
    ld      [%l3 + CONTEXT_VCPU + SPARC_VCPU_PSR], %i0

    /* get the CWP from the real PSR (psr is stored on %l0) */
    and     %l0, SPARC_PSR_CWP_MASK, %l4
    andn    %i0, SPARC_PSR_CWP_MASK, %i0
    or      %i0, %l4, %i0

    /* get the ICC from the real PSR (psr is stored on %l0) */
    sethi   %hi(SPARC_PSR_ICC_MASK), %l4
    or      %l4, %lo(SPARC_PSR_ICC_MASK), %l4   ! ICC mask

    andn    %i0, %l4, %i0                       ! clear ICCs
    and     %l0, %l4, %l4                       ! get real ICCs
    ba      syscall_sparc_return
    or      %i0, %l4, %i0                       ! optimized delay slot

/**
 * @brief SPARC set PSR
 */
syscall_sparc_set_psr:

#if PMK_FPU_SUPPORT

    /* check if the EF bit is being changed */
    xor     %i0, %l0, %l5
    set     SPARC_PSR_EF_MASK, %l6

    btst    %l6, %l5
    bz      1f
    nop

    /* check the permissions to disable the FPU */
    ld      [%l3 + CORE_CTRL_PARTITION], %l4
    ld      [%l4 + PARTITION_PERMISSIONS], %l4
    btst    AIR_PERMISSION_FPU_CONTROL, %l4
    bz      1f

    /* apply FPU change */
    and     %i0, %l6, %l5
    andn    %l0, %l6, %l0
    or      %l0, %l5, %l0
1:

#endif

    /* set virtual PSR */
    ld      [%l3 + CORE_CTRL_CONTEXT], %l3
    st      %i0, [%l3 + CONTEXT_VCPU + SPARC_VCPU_PSR]

    /* apply the ICC to the real PSR */
    set     SPARC_PSR_ICC_MASK, %l4
    and     %i0, %l4, %i0                   ! extract ICC from the virtual PSR
    andn    %l0, %l4, %l0                   ! clear ICC from the real PSR

    /* reset the PSR */
    ba      syscall_sparc_pedding_irq
    or      %l0, %i0, %l0                   ! apply the ICC to real PSR

/**
 * @brief SPARC virtual RETT
 */
syscall_sparc_rett:

    /* enable virtual traps */
    ld      [%l3 + CORE_CTRL_CONTEXT], %l3
    ld      [%l3 + CONTEXT_VCPU + SPARC_VCPU_PSR], %l4
    or      %l4, SPARC_PSR_ET_MASK, %l4
    st      %l4, [%l3 + CONTEXT_VCPU + SPARC_VCPU_PSR]

    /* increment the CWP to simulate the return of original trap window */
    add     %l0, 1, %l4                         ! increment the CWP
    andn    %l0, SPARC_PSR_CWP_MASK, %l0        ! clear current CWP
    and     %l4, SPARC_PSR_CWP_MASK, %l4        ! module CWP with window count
    or      %l0, %l4, %i3                       ! apply new CWP to the PSR and
                                                ! save it in register visible on the next window

    /* apply the new PSR */
    mov     %l3, %i4                            ! save core context
    mov     %i3, %psr
    nop; nop; nop                               ! WRPSR delay slots

    /*
     * now we are on the window of the original trap, which may contain guest OS
     * content (mainly in %i and %g registers, therefore these registers must
     * be preserved
     */
    mov     %g3, %l4
    mov     %g4, %l5
    mov     %g5, %l6
    mov     %g6, %l7

    /*
     * move the restore stack pointer to a global, we cannot use the input
     * registers because the guest OS may need its contents as part of its
     * ISR handler
     */
    mov     %o2, %g6

    /* check if the trap return will cause an window underflow */
    rd      %wim, %g4
    add     %o3, 1, %g5
    and     %g5, SPARC_PSR_CWP_MASK, %g5    ! g5 = cwp + 1
    srl     %g4, %g5, %g5                   ! g5 = win >> cwp + 1 ; shift count

    cmp     %g5, 1
    bne     2f                              ! no underflow will happen

    /* rotate WIM to the left */
    sll     %g4, 1, %g5
    srl     %g4, SPARC_REGISTER_WINDOWS_COUNT -1 , %g4
    or      %g4, %g5, %g4

    /* restore the window just as if we underflowed into it */
    mov     %g4,  %wim                      ! WIM = new WIM
    nop; nop; nop                           ! WRWIM delay slots

    restore

    /* 1st check if the address is aligned */
    andcc   %g6, 0x7, %g0
    bne     1f
    nop

    /* 2nd check if the address is in kernel zone */
    set     SYM(air_kernel_memory_end), %g3
    cmp     %g6, %g3
    bleu    1f
    nop

    /* 3rd disable MMU to prevent faults */
    mov     AC_M_SFSR, %g3
    lda     [%g3] ASI_LEON_MMUREGS, %g0         ! clear flauts
    lda     [%g0] ASI_LEON_MMUREGS, %g3         ! read MMU control
    or      %g3, 0x2, %g3                       ! set no_flaut bit
    sta     %g3, [%g0] ASI_LEON_MMUREGS         ! write MMU control

    /* restore the window contents */
    ldd     [%g6 + CPU_STACK_FRAME_L0_OFFSET], %l0
    ldd     [%g6 + CPU_STACK_FRAME_L2_OFFSET], %l2
    ldd     [%g6 + CPU_STACK_FRAME_L4_OFFSET], %l4
    ldd     [%g6 + CPU_STACK_FRAME_L6_OFFSET], %l6
    ldd     [%g6 + CPU_STACK_FRAME_I0_OFFSET], %i0
    ldd     [%g6 + CPU_STACK_FRAME_I2_OFFSET], %i2
    ldd     [%g6 + CPU_STACK_FRAME_I4_OFFSET], %i4
    ldd     [%g6 + CPU_STACK_FRAME_I6_FP_OFFSET], %i6

    /* check if any MMU faults occurred */
    andn    %g3, 0x2, %g3
    sta     %g3, [%g0] ASI_LEON_MMUREGS         ! clear no flaut bit
    mov     AC_M_SFAR, %g3
    lda     [%g3] ASI_LEON_MMUREGS, %g0
    mov     AC_M_SFSR, %g3
    lda     [%g3] ASI_LEON_MMUREGS, %g3         ! get the flaut information

1:
    /* return to trap window */
    save
    nop

2:
    /* restore the globals used by the underflow */
    mov     %o3, %l0                            ! PSR
    mov     %o0, %l1                            ! return PC
    mov     %o1, %l2                            ! return nPC
    mov     %o4, %l3                            ! current core context
    mov     %l4, %g3                            ! partition global registers
    mov     %l5, %g4
    mov     %l6, %g5
    mov     %l7, %g6

    /* process pending interrupts */
    ba      syscall_sparc_pedding_irq
    nop

/**
 * @brief SPARC get cache register
 */
syscall_sparc_get_cache_register:

    /* usable bits of the cache mask (leno3) */
    sethi   %hi(0xE0000F), %l6
    or      %l6, %lo(0xE0000F), %l6

    lda     [%g0]2, %i0                         ! get cache register value
    ba      syscall_sparc_return                ! finish trap
    and     %i0, %l6, %i0                       ! delay slot

/**
 * @brief SPARC set cache register
 */
syscall_sparc_set_cache_register:

    /* check if the partition have the permissions */
    ld      [%l3 + CORE_CTRL_PARTITION], %l4
    ld      [%l4 + PARTITION_PERMISSIONS], %l4               ! permissions
    andcc   %l4, AIR_PERMISSION_CACHE_CONTROL, %g0 ! can control cache?
    bz      syscall_sparc_return_invalid_config              ! no? then error
    nop                                                      ! delay slot

    /* usable bits of the cache mask (leon3 and leon4) */
    set     0xE0000F, %l4

    /* mask user register */
    and     %i0, %l4, %i0

    /* check if an action will occurr */
    lda     [%g0]2, %l5
    and     %l5, %l4, %l6
    cmp     %l6, %i0
    be      syscall_sparc_return_no_action
    nop

    /* save user options */
    ld      [%l3 + CORE_CTRL_CONTEXT], %l3
 //  st      %i0, [%l3 + CONTEXT_VCPU + SPARC_VCPU_CCTRL]

    /*Avoid store of FI & FD because these bits must read 0*/
    set     0x80000F, %l6
    and     %i0, %l6, %l6
    st      %l6, [%l3 + CONTEXT_VCPU + SPARC_VCPU_CCTRL]

    /* apply user options */
    andn    %l5, %l4, %l5
    or      %l5, %i0, %l5

    ba      syscall_sparc_return_no_error                   ! finish
    sta     %l5, [%g0]2                                     ! delay slot

/**
 * @brief SPARC restore cache register
 */
syscall_sparc_restore_cache_register:

    /* restore partition cache state (leon3) */
    ld      [%l3 + CORE_CTRL_CONTEXT], %l3
    ld      [%l3 + CONTEXT_VCPU + SPARC_VCPU_CCTRL], %l4
    set     0x810000, %l5
    or      %l5, %l4, %l5
    ba      syscall_sparc_return
    sta     %l5, [%g0]2

/**
 * @brief SPARC get IRQ mask register
 */
syscall_sparc_get_irq_mask_register:

    /* get partition core count */
    ld      [%l3 + CORE_CTRL_PARTITION], %l4
    ld      [%l4 + PARTITION_CORE_CNT], %l7         ! partition core count

    /* check if the virtual core id is valid */
    cmp     %i0, %l7                                ! check if the virtual core id is valid
    bge     syscall_sparc_return                    ! no? then its an error
    mov     %g0, %i0                                ! delay slot

    /* get the real core id */
    ld      [%l4 + PARTITION_CORE_MAPPING], %l6
    umul    %i0, 0x04, %i0                          ! virtual core memory offset
    ld      [%l6 + %i0], %l3                        ! real core id
    umul    %l3, 0x04, %l3                          ! relad core memory offset

    /* get the register value */
    sethi   %hi(IRQ_mask_register), %l5
    ld      [%l5 + %lo(IRQ_mask_register)], %l5
    ba      syscall_sparc_return
    ld      [%l5 + %l3], %i0                        ! delay slot

/**
 * @brief SPARC set IRQ mask register
 * @param %i1 virtual core id
 * @param %i2 mask register value
 */
syscall_sparc_set_irq_mask_register:

    /* get partition core count */
    ld      [%l3 + CORE_CTRL_PARTITION], %l4
    ld      [%l4 + PARTITION_CORE_CNT], %l5         ! partition core count

    /* check if the virtual core id is valid */
    cmp     %i0, %l5                                ! check if the virtual core id is valid
    bge     syscall_sparc_return_invalid_config     ! no? then its an error
    nop                                             ! delay slot

    /* store mask in core context */
    ld      [%l3 + CORE_CTRL_CONTEXT], %l3
    st      %i1, [%l3 + CONTEXT_VCPU + SPARC_VCPU_IMASK]

    /* get the real core id */
    ld      [%l4 + PARTITION_CORE_MAPPING], %l4
    umul    %i0, 0x04, %i0                          ! virtual core memory offset
    ld      [%l4 + %i0], %l4                        ! real core id
    umul    %l4, 0x04, %l4                          ! real core memory offset

    /* merge partition IRQ mask with the PMK IRQ mask */
    sethi   %hi(IRQ_mask_protected_interrupts), %l5
    ld      [%l5 + %lo(IRQ_mask_protected_interrupts)], %l5
    andn    %i1, %l5, %i1                           ! remove protected IRQs

    sethi   %hi(pmk_irq_mask), %l5
    or      %l5, %lo(pmk_irq_mask), %l5
    ld      [%l5 + %l4], %l5                        ! load core PMK mask
    or      %i1, %l5, %i1                           ! add it to the partition mask

    /* apply the new mask */
    sethi   %hi(IRQ_mask_register), %l5
    ld      [%l5 + %lo(IRQ_mask_register)], %l5
    ba      syscall_sparc_return_no_error           ! finish trap
    st      %i1, [%l5 + %l4]                        ! delay slot

/**
 * @brief SPARC set IRQ force register
 */
syscall_sparc_set_irq_force_register:

    /* get partition core count */
    ld      [%l3 + CORE_CTRL_PARTITION], %l4
    ld      [%l4 + PARTITION_CORE_CNT], %l5         ! partition core count

    /* check if the virtual core id is valid */
    cmp     %i0, %l5                                ! check if the virtual core id is valid
    bge     syscall_sparc_return_invalid_config     ! no? then its an error
    nop                                             ! delay slot

    /* get the real core id */
    ld      [%l4 + PARTITION_CORE_MAPPING], %l4
    umul    %i0, 0x04, %i0                          ! virtual core memory offset
    ld      [%l4 + %i0], %l4                        ! real core id
    umul    %l4, 0x04, %l4                          ! relad core memory offset

    /* prevent the partition force protected interrupts */
    sethi   %hi(IRQ_force_protected_interrupts), %l5
    ld      [%l5 + %lo(IRQ_force_protected_interrupts)], %l5
    and     %i1, %l5, %i1                           ! remove PMK interrupts from mask

    /* apply the new mask */
    sethi   %hi(IRQ_force_register), %l5
    ld      [%l5 + %lo(IRQ_force_register)], %l5
    ba      syscall_sparc_return_no_error           ! finish trap
    st      %i1, [%l5 + %l4]                        ! delay slot

/**
 * @brief PMK system call: process pedding interrupts
 *
 * This routine is used whenever the virtual interrupts and the virtual traps
 * are enabled
 */
syscall_sparc_pedding_irq:

    /* get virtual PSR and virtual interrupt pedding vector */
    ld      [%l3 + CONTEXT_VCPU + SPARC_VCPU_PSR], %l4
    ld      [%l3 + CONTEXT_VCPU + SPARC_VCPU_IPEND], %l5

    /* check if virtual traps are enabled */
    andcc   %l4, SPARC_PSR_ET_MASK, %g0
    bz      syscall_sparc_return                        ! no? then exit

    /* get virtual PIL */
    and     %l4, SPARC_PSR_PIL_MASK, %l4
    srl     %l4, SPARC_PSR_PIL_BIT_POSITION, %l4

    /* check if there are any pedding interrupts */
    tst     %l5
    bz      syscall_sparc_return                        ! no? then exit

    /* get witch interrupt will be served */
    mov     0x0F, %l6

1:

    /* decrement interrupt level */
    subcc   %l6, 1, %l6
    bz      syscall_sparc_return                        ! level 0? then exit

    /* check if the interrupt is greater than PIL */
    cmp     %l6, %l4
    bleu    syscall_sparc_return                        ! yes? then exit
    nop

    /* check if interrupt in %l6 is pedding */
    mov     0x01, %l7
    sll     %l7, %l6, %l7
    andcc   %l7, %l5, %g0
    bz      1b                                          ! no? check for next
    nop

/*
 * Pending interrupt was found, check if the partition has an handler for it
 *  %l3 - core contex
 *  %l4 - virtual pil
 *  %l5 - virtual pending vector
 *  %l6 - pending interrupt found
 */

    /* clear pedding bit */
    andn    %l5, %l7, %l5
    st      %l5, [%l3 + CONTEXT_VCPU + SPARC_VCPU_IPEND]

    /* get virtual TBR and entry point for that interrupt */
    ld      [%l3 + CONTEXT_VCPU + SPARC_VCPU_TBR], %l4
    tst     %l4                                         ! check if TBR is set
    bz      syscall_sparc_return                        ! no? then exit
    nop

    /* check if the partition have an handler set */
    add     %l6, 0x10, %l6          ! place interrupt number on %l6
    umul    %l6, 0x04, %l7          ! interrupt TBR memory offset
    ld      [%l4 + %l7], %l4        ! interrupt handler
    tst     %l4                     ! check if the partition have an handler
    bz      syscall_sparc_return  ! no? then exit
    nop

    /* disable traps and place trap number on the correct register */
    ld      [%l3 + CONTEXT_VCPU + SPARC_VCPU_PSR], %l7
    andn    %l7, SPARC_PSR_ET_MASK, %l7
    st      %l7, [%l3 + CONTEXT_VCPU + SPARC_VCPU_PSR]
    mov     %l6, %l3

    /* save the globals used by this routine */
    mov     %g4, %l6
    mov     %g5, %l7

    /* check if we are at the invalid window */
    mov     %wim, %g4
    srl     %g4, %l0, %g5

    cmp     %g5, 1
    bne     2f

    /* compute the new WIM */
    srl     %g4, 1, %g5
    sll     %g4, SPARC_REGISTER_WINDOWS_COUNT - 1 , %g4
    or      %g4, %g5, %g4

    /* save window as if we overflowed into it */
    save
    mov     %g4, %wim             ! WIM = new WIM
    nop; nop; nop                 ! WRWIM delay slots

    /* 1st check if the address is aligned */
    andcc   %sp, 0x7, %g0
    bne     1f
    nop

    /* 2nd check if the address is in kernel zone */
    set     SYM(air_kernel_memory_end), %g4
    cmp     %sp, %g4
    bleu    1f
    nop

    /* 3rd disable MMU to prevent faults */
    mov     AC_M_SFSR, %g4
    lda     [%g4] ASI_LEON_MMUREGS, %g0         ! clear flauts
    lda     [%g0] ASI_LEON_MMUREGS, %g4         ! read MMU control
    or      %g4, 0x2, %g4                       ! set no_flaut bit
    sta     %g4, [%g0] ASI_LEON_MMUREGS         ! write MMU control

    /* dump the window contents */
    std     %l0, [%sp + CPU_STACK_FRAME_L0_OFFSET]
    std     %l2, [%sp + CPU_STACK_FRAME_L2_OFFSET]
    std     %l4, [%sp + CPU_STACK_FRAME_L4_OFFSET]
    std     %l6, [%sp + CPU_STACK_FRAME_L6_OFFSET]
    std     %i0, [%sp + CPU_STACK_FRAME_I0_OFFSET]
    std     %i2, [%sp + CPU_STACK_FRAME_I2_OFFSET]
    std     %i4, [%sp + CPU_STACK_FRAME_I4_OFFSET]
    std     %i6, [%sp + CPU_STACK_FRAME_I6_FP_OFFSET]

    /* clear MMU no-flaut bit */
    andn    %g4, 0x2, %g4
    sta     %g4, [%g0] ASI_LEON_MMUREGS         ! clear no flaut bit
    mov     AC_M_SFAR, %g4
    lda     [%g4] ASI_LEON_MMUREGS, %g0
    mov     AC_M_SFSR, %g4
    lda     [%g4] ASI_LEON_MMUREGS, %g4         ! get the flaut information

1:

    restore
    nop

2:

    /* restore g4 and g5 */
    mov     %l6, %g4
    mov     %l7, %g5

    /* mov the trap return to the previous window */
    mov     %l4, %o0

    /* decrement the CWP to simulate a trap window on POS */
    sub     %l0, 1, %l5
    and     %l5, SPARC_PSR_CWP_MASK, %l5
    andn    %l0, SPARC_PSR_CWP_MASK, %l0
    or      %l0, %l5, %l0

#if TEST_PARTITION_PSR

    srl     %l0, 0x8, %l6
    sethi   %hi(0xFF902004), %l7
    st      %l6, [%l7 + %lo(0xFF902004)]

#endif

    /* apply the new PSR */
    mov     %l0, %psr
    nop; nop; nop                           ! WRPSR delay slots

    /* return from trap to the POS ISR handler */
    jmp     %i0
    rett    %i0 + 4


/**
 * @brief SPARC system call return without return value
 */
syscall_sparc_return:

    mov     %l0, %psr                                ! restore partition PSR
    nop; nop; nop                                    ! WRPSR delay slots
    jmp     %l1                                      ! set PC
    rett    %l2                                      ! set nPC and exit trap

/**
 * @brief SPARC system call return NO_ERROR code
 */
syscall_sparc_return_no_error:

    set     AIR_NO_ERROR, %i0                        ! set no error code
    mov     %l0, %psr                                ! restore partition PSR
    nop; nop; nop                                    ! WRPSR delay slots
    jmp     %l1                                      ! set PC
    rett    %l2                                      ! set nPC and exit trap

/**
 * @brief SPARC system call return NO_ACTION code
 */
syscall_sparc_return_no_action:

    set     AIR_NO_ACTION, %i0                       ! set no error code
    mov     %l0, %psr                                ! restore partition PSR
    nop; nop; nop                                    ! WRPSR delay slots
    jmp     %l1                                      ! set PC
    rett    %l2                                      ! set nPC and exit trap

/**
 * @brief SPARC system call return INVALID_CONFIG code
 */
syscall_sparc_return_invalid_config:

    set     AIR_INVALID_CONFIG, %i0                  ! set invalid config code
    mov     %l0, %psr                                ! restore partition PSR
    nop; nop; nop                                    ! WRPSR delay slots
    jmp     %l1                                      ! set PC
    rett    %l2                                      ! set nPC and exit trap

/**
 * @brief SPARC get instruction cache configuration register
 */
syscall_sparc_get_inst_cache_config_register:

    /* usable bits of the cache mask (leno3) */
    sethi   %hi(0x60000F), %l6
    or      %l6, %lo(0x60000F), %l6

    mov     0x8, %l7                            ! use l7 to hold addr
    lda     [%l7]2, %i0                         ! get cache register value
    ba      syscall_sparc_return                ! finish trap
    and     %i0, %l6, %i0                       ! delay slot

/**
 * * @brief SPARC get data cache configuration register
 */
syscall_sparc_get_data_cache_config_register:

    /* usable bits of the cache mask (leon3) */
    sethi   %hi(0x60000F), %l6
    or      %l6, %lo(0x60000F), %l6

    mov     0xc, %l7                            ! use l7 to hold addr
    lda     [%l7]2, %i0                         ! get cache register value
    ba      syscall_sparc_return                ! finish trap
    and     %i0, %l6, %i0                       ! delay slot

/**
 * AIR trap handler of rtems5/cpukit/score/cpu/sparc/syscall.S syscall_irqdis_fp
 *  Interrupt disable and set PSR[EF] according to caller %g1
 *  On entry:
 *    g1 = the desired PSR[EF] value (from caller)
 *  On exit:
 *    g1 = old psr (to user)
 */
syscall_sparc_set_ef:

#if PMK_FPU_SUPPORT

    /* check permissions to handle FPU */
    ld      [%l3 + CORE_CTRL_PARTITION], %l4
    ld      [%l4 + PARTITION_PERMISSIONS], %l4
    btst    AIR_PERMISSION_FPU_CONTROL, %l4
    bz      syscall_sparc_return_invalid_config
    nop

#else

    ba      syscall_sparc_return_invalid_config
    nop

#endif

    /* get virtual PSR */
    ld      [%l3 + CORE_CTRL_CONTEXT], %l3
    ld      [%l3 + CONTEXT_VCPU + SPARC_VCPU_PSR], %i0

    /* get the CWP from the real PSR (psr is stored on %l0) */
    and     %l0, SPARC_PSR_CWP_MASK, %l4
    andn    %i0, SPARC_PSR_CWP_MASK, %i0
    or      %i0, %l4, %i0

    /* get the ICC from the real PSR (psr is stored on %l0) */
    sethi   %hi(SPARC_PSR_ICC_MASK), %l4
    or      %l4, %lo(SPARC_PSR_ICC_MASK), %l4   ! ICC mask
    andn    %i0, %l4, %i0                       ! clear ICCs
    and     %l0, %l4, %l4                       ! get real ICCs
    or      %i0, %l4, %i0                       ! apply real ICCs to virtual ones

    /* Disable interrupts */
    or    %i0, 0x0f00, %l4 ! Set PIL=0xf to disable IRQ
    /*
     * We cannot use an intermediate value for operations with the PSR[EF]
     * bit since they use a 13-bit sign extension and PSR[EF] is bit 12.
     */
    sethi %hi(SPARC_PSR_EF_MASK), %l5

    andn  %l4, %l5, %l4        ! Clear PSR[EF]
    and   %g1, %l5, %g1        ! Select PSR[EF] only from %g1
    or    %l4, %g1, %l4        ! Set PSR[EF] according to %g1
    or    %i0, SPARC_PSR_ET_MASK, %g1 ! return old PSR with ET=1

    /* set virtual PSR */
    st    %l4, [%l3 + CONTEXT_VCPU + SPARC_VCPU_PSR]

    /*Apply EF value to real psr*/
    and     %l4, %l5, %l6
    andn    %l0, %l5, %l0

    ba    syscall_sparc_return_no_error   ! finish trap
    or      %l0, %l6, %l0
