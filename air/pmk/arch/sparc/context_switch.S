/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file
 * @author pfnf
 * @brief Contains the routines to peform a partition core context save
 *        and partition core context restore on a SPARC v8
 * @note This impelementation assumes on -mflat compilation
 */

#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <bsp.h>
#include <air.h>
#include <sparc.h>
#include <partition.h>

/**
 * @brief Partition context save
 * @param %o0 current core control pointer
 */
    GLOBAL(sparc_core_context_save)
SYM(sparc_core_context_save):

    /* load the current core context */
    ld      [%o0 + CORE_CTRL_CONTEXT], %o2

    /* load partition control */
    ld      [%o0 + CORE_CTRL_PARTITION], %o1

    /* save MMU control and fault status register
     *
     * this value allows us to determine if give memory read or write from the
     * partition space was invalid, even if the operation was preempted
     */
    mov     AC_M_SFSR, %o3
    lda     [%o3] ASI_LEON_MMUREGS, %o3
    lda     [%g0] ASI_LEON_MMUREGS, %o4
    st      %o3, [%o2 + CONTEXT_VCPU + SPARC_VCPU_MMU_FSR]
    st      %o4, [%o2 + CONTEXT_VCPU + SPARC_VCPU_MMU_CTRL]

#if PMK_FPU_SUPPORT

    /* check if the context belongs to a partition */
    tst     %o1
    bz      1f
    nop

    /* check the permissions to use the FPU */
    ld      [%o1 + PARTITION_PERMISSIONS], %o3
    btst    AIR_PERMISSION_FPU_CONTROL, %o3
    bz      1f

    /* enable EF */
    mov     %psr, %o3
    set     SPARC_PSR_EF_MASK, %o4              ! EF mask
    or      %o3, %o4, %o3                       ! set EF bit
    mov     %o3, %psr                           ! enable EF
    nop; nop; nop;                              ! WRPSR delay

    /* get the FP context memory area */
    ld      [%o2 + PMK_CORE_CTX_FPU], %o3

    /* save FP registers */
    std     %f0,  [%o3 + FO_F1_OFFSET]          ! save f0  and f1
    std     %f2,  [%o3 + F2_F3_OFFSET]          ! save f2  and f3
    std     %f4,  [%o3 + F4_F5_OFFSET]          ! save f4  and f5
    std     %f6,  [%o3 + F6_F7_OFFSET]          ! save f6  and f7
    std     %f8,  [%o3 + F8_F9_OFFSET]          ! save f8  and f9
    std     %f10, [%o3 + F1O_F11_OFFSET]        ! save f10 and f11
    std     %f12, [%o3 + F12_F13_OFFSET]        ! save f12 and f13
    std     %f14, [%o3 + F14_F15_OFFSET]        ! save f14 and f15
    std     %f16, [%o3 + F16_F17_OFFSET]        ! save f16 and f17
    std     %f18, [%o3 + F18_F19_OFFSET]        ! save f18 and f19
    std     %f20, [%o3 + F2O_F21_OFFSET]        ! save f20 and f21
    std     %f22, [%o3 + F22_F23_OFFSET]        ! save f22 and f23
    std     %f24, [%o3 + F24_F25_OFFSET]        ! save f24 and f25
    std     %f26, [%o3 + F26_F27_OFFSET]        ! save f26 and f27
    std     %f28, [%o3 + F28_F29_OFFSET]        ! save f28 and f29
    std     %f30, [%o3 + F3O_F31_OFFSET]        ! save f30 and f31

    /* save FPU FSR register */
    st      %fsr, [%o3 + FSR_OFFSET]            ! save FSR

1:
#endif

    /* save the global registers used by the routine */
    mov     %g1, %o1
    mov     %g2, %o2
    mov     %g3, %o3
    mov     %g4, %o4
    mov     %g5, %o5
/*
 * Flush all partition windows
 *  - Disable MMU faults
 *  - Flush all windows
 */
pmk_partition_context_save_window_flush:

    /* disable MMU to prevent faults */
    mov     AC_M_SFSR, %g5
    lda     [%g5] ASI_LEON_MMUREGS, %g0         ! clear flauts
    lda     [%g0] ASI_LEON_MMUREGS, %g5         ! read MMU control
    or      %g5, 0x2, %g5                       ! set no_flaut bit
    sta     %g5, [%g0] ASI_LEON_MMUREGS         ! write MMU control

    /* get the current PSR and WIM required to flush the windows */
    mov     %psr, %g1                           ! get current PSR
    mov     %wim, %g2                           ! get current WIM
    and     %g1, SPARC_PSR_CWP_MASK, %g3        ! get current CWP

    or      %g0, 1, %g4
    sll     %g4, %g3, %g4                       ! window bitmask for CWP invalid


pmk_partition_context_save_window_flush_loop:

    /* get current window bitmask, as if we had done a restore */
    sll     %g4, 1, %g3
    srl     %g4, SPARC_REGISTER_WINDOWS_COUNT - 1, %g4
    or      %g4, %g3, %g4

    /* check if a restore would cause an underflow */
    andcc   %g4, %g2, %g0
    bnz     pmk_partition_context_save_finish
    nop

    restore
    nop

pmk_partition_context_save_window_flushing:     ! label for debugging

    /* check if the stack address is aligned */
    andcc   %sp, 0x7, %g0
    bne     pmk_partition_context_save_window_flush_loop
    nop

    /* check if the address is in kernel zone */
    set     SYM(air_kernel_memory_end), %g3
    cmp     %sp, %g3
    bgu pmk_partition_context_save_window_dump
    nop

    /* If ISR nesting level > 1 then a preemptible ISR was interrupted*/
    /* Get current control control and context */
    rd      %asr17, %g3
    srl     %g3, 28, %g3
    and     %g3, 0xff, %g3                       ! core index

    /* get current core control structure */
    umul    %g3, CORE_CTRL_SIZE, %g3             ! core control mem. offset
    set     air_shared_area, %g5                 ! shared area pointer
    ld      [%g5 + SHAREDAREA_CORE_CTRL], %g5    ! shared area->cores
    add     %g5, %g3, %g5                        ! current core control
    ld      [%g5 + CORE_CTRL_CONTEXT], %g3       ! core context pointer

    ld      [%g3 + CONTEXT_NESTING_LEVEL], %g5   ! get ISR nesting level
    cmp     %g5, 0x1
    bleu    pmk_partition_context_save_window_flush_loop
    nop

pmk_partition_context_save_window_dump:
    /* dump the window content */
    std     %l0, [%sp + CPU_STACK_FRAME_L0_OFFSET]
    std     %l2, [%sp + CPU_STACK_FRAME_L2_OFFSET]
    std     %l4, [%sp + CPU_STACK_FRAME_L4_OFFSET]
    std     %l6, [%sp + CPU_STACK_FRAME_L6_OFFSET]
    std     %i0, [%sp + CPU_STACK_FRAME_I0_OFFSET]
    std     %i2, [%sp + CPU_STACK_FRAME_I2_OFFSET]
    std     %i4, [%sp + CPU_STACK_FRAME_I4_OFFSET]
    std     %i6, [%sp + CPU_STACK_FRAME_I6_FP_OFFSET]

    ba      pmk_partition_context_save_window_flush_loop
    nop

/*
 * Finish the partition context save
 *  - Re-enable MMU
 *  - Restore PSR
 *  - Compute the new WIM
 *  - Restore the global registers
 */
pmk_partition_context_save_finish:

    /* enable MMU and clear fault register */
    lda     [%g0] ASI_LEON_MMUREGS, %g5     ! read MMU control
    andn    %g5, 0x2, %g5
    sta     %g5, [%g0] ASI_LEON_MMUREGS     ! clear no flaut bit
    mov     AC_M_SFAR, %g5
    lda     [%g5] ASI_LEON_MMUREGS, %g0
    mov     AC_M_SFSR, %g5
    lda     [%g5] ASI_LEON_MMUREGS, %g5     ! clear flaut information

    /* compute the new WIM */
    add     %g1, 1, %g2                      ! increment the current CWP
    and     %g2, SPARC_PSR_CWP_MASK, %g2     ! modulo by the number of windows

    or      %g0, 1, %g3
    sll     %g3, %g2, %g3                    ! compute the new WIM
    mov     %g3, %wim                        ! apply it

    /* restore the PSR, returning to the initial window */
    mov     %g1, %psr
    nop; nop; nop                           ! WRPSR delay slots

    /* restore the global registers */
    mov     %o1, %g1
    mov     %o2, %g2
    mov     %o3, %g3
    mov     %o4, %g4
    retl                                    ! return to caller
    mov     %o5, %g5                        ! optimized delay slot
/**
 * @brief Partition context restore
 * @param %o0 current core control pointer
 */
    GLOBAL(sparc_core_context_restore)
SYM(sparc_core_context_restore):

    /* load the current core context */
    ld      [%o0 + CORE_CTRL_CONTEXT], %o2

    /* load partition control */
    ld      [%o0 + CORE_CTRL_PARTITION], %o1

    /* restore MMU control register */
    ld      [%o2 + CONTEXT_VCPU + SPARC_VCPU_MMU_CTRL], %o3
    sta     %o3, [%g0] ASI_LEON_MMUREGS

    /* set the partition MMU context */
    tst     %o1
    bz      1f
    nop

    /* MMU context switch */
    mov     0x200, %o4
    ld      [%o1 + PARTITION_IDX], %o3
    sta     %o3, [%o4]0x19             ! 0x19 is ASI of the MMU register

    /* flush cache */
    flush
1:


#if PMK_FPU_SUPPORT

    /* check if the context belongs to a partition */
    tst     %o1
    bz      1f
    nop

    /* check the permissions to use the FPU */
    ld      [%o1 + PARTITION_PERMISSIONS], %o3
    btst    AIR_PERMISSION_FPU_CONTROL, %o3
    bz      1f

    /* enable EF */
    mov     %psr, %o3
    set     SPARC_PSR_EF_MASK, %o4              ! EF mask
    or      %o3, %o4, %o3                       ! set EF bit
    mov     %o3, %psr                           ! enable EF
    nop; nop; nop;                              ! WRPSR delay

    /* get the FP context memory area */
    ld      [%o2 + PMK_CORE_CTX_FPU], %o3

    /* restore FP registers */
    ldd     [%o3 + FO_F1_OFFSET], %f0           ! restore f0  and f1
    ldd     [%o3 + F2_F3_OFFSET], %f2           ! restore f2  and f3
    ldd     [%o3 + F4_F5_OFFSET], %f4           ! restore f4  and f5
    ldd     [%o3 + F6_F7_OFFSET], %f6           ! restore f5  and f7
    ldd     [%o3 + F8_F9_OFFSET], %f8           ! restore f8  and f9
    ldd     [%o3 + F1O_F11_OFFSET], %f10        ! restore f10  and f11
    ldd     [%o3 + F12_F13_OFFSET], %f12        ! restore f12  and f13
    ldd     [%o3 + F14_F15_OFFSET], %f14        ! restore f14  and f15
    ldd     [%o3 + F16_F17_OFFSET], %f16        ! restore f16  and f17
    ldd     [%o3 + F18_F19_OFFSET], %f18        ! restore f18  and f19
    ldd     [%o3 + F2O_F21_OFFSET], %f20        ! restore f20  and f21
    ldd     [%o3 + F22_F23_OFFSET], %f22        ! restore f22  and f23
    ldd     [%o3 + F24_F25_OFFSET], %f24        ! restore f24  and f25
    ldd     [%o3 + F26_F27_OFFSET], %f26        ! restore f26  and f27
    ldd     [%o3 + F28_F29_OFFSET], %f28        ! restore f28  and f29
    ldd     [%o3 + F3O_F31_OFFSET], %f30        ! restore f30  and f31

    /* restore FPU FSR register */
    ld      [%o3 + FSR_OFFSET], %fsr            ! restore FSR

1:
#endif

    ld      [%o0 + CORE_CTRL_IDX], %o0              ! current core id
    umul    %o0, 0x04, %o0                          ! core register offset

    /* get context interrupt mask */
    ld      [%o2 + CONTEXT_VCPU + SPARC_VCPU_IMASK], %o1

    /* merge partition IRQ mask with the PMK IRQ mask */
    sethi   %hi(IRQ_mask_protected_interrupts), %o2
    ld      [%o2 + %lo(IRQ_mask_protected_interrupts)], %o2
    andn    %o1, %o2, %o1                           ! remove PMK interrupts from mask

    sethi   %hi(pmk_irq_mask), %o2
    or      %o2, %lo(pmk_irq_mask), %o2
    ld      [%o2 + %o0], %o2                        ! load core PMK mask
    or      %o1, %o2, %o1                           ! add it to the partition mask

    /* apply mask */
    sethi   %hi(IRQ_mask_register), %o2
    ld      [%o2 + %lo(IRQ_mask_register)], %o2     ! IRQ base mask register
    st      %o1, [%o2 + %o0]                        ! restore core's mask

    retl                                            ! return to the caller
    nop
