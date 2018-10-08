/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file isr_handler_asm.S
 * @author pfnf
 * @brief This routine provides support for the executuion of a C handler, while
 *        safely storing the required windows without relaying on the partion
 *        stack pointer
 */

/* enforce ASM definition */
#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <bsp.h>
#include <rvs.h>

    GLOBAL(sparc_isr_handler_entry)
SYM(sparc_isr_handler_entry):

    /* freeze the cache and disable instruction burst fetch (leon 3) */
    sethi   %hi(0x800035), %l4
    or      %l4, %lo(0x800035), %l4
    sta     %l4, [%g0]2

    /* get current control control and context */
    rd      %asr17, %l5
    srl     %l5, 28, %l5
    and     %l5, 0xff, %l5                          ! core index

#ifdef TEST_PMK_CRITICAL_PERF

    sll     %l5, 0x06, %l6
    sethi   %hi(0xFF902004), %l7
    st      %l6, [%l7 + %lo(0xFF902004)]

#endif

    /* get current core control structure */
    umul    %l5, CORE_CTRL_SIZE, %l5             ! core control mem. offset
    set     air_shared_area, %l4                 ! shared area pointer
    ld      [%l4 + SHAREDAREA_CORE_CTRL], %l4    ! shared area->cores
    add     %l4, %l5, %l4                        ! current core control


    /* get current core context and check if it is flagged as trashed */
    ld      [%l4 + CORE_CTRL_CONTEXT], %l5       ! core context pointer
    ld      [%l5 + CONTEXT_TRASHED], %l6
    tst     %l6                                  ! check if context is trashed
    bz      sparc_isr_handler_regular_context    ! no? then save the ISF must
    nop                                          ! be saved

/*
 * ISR from trashed context
 *    %l4 contains the current core control structure
 *    %l5 contains the current core context structure
 *
 * This sub-routine is executed when it isn't necessary to save the current
 * context, it will by-pass the window saving logic and prepare the CPU core to
 * just run the trap handling function
 */
sparc_isr_handler_trashed_context:

    /*
     * since no window is saved, it means that an invalid window is at place
     * when the ISR returns, and its context will load from the stack
     */
    add     %l0, 1, %l6                      ! increment the current CWP
    and     %l6, SPARC_PSR_CWP_MASK, %l6     ! modulo by the number of windows

    or      %g0, 1, %l7
    sll     %l7, %l6, %l7                    ! compute the new WIM
    mov     %l7, %wim                        ! Apply it

    /* reset ISR level and set stack pointer */
    mov     0x1, %l6
    st      %l6, [%l5 + CONTEXT_NESTING_LEVEL]
    ld      [%l5 + CONTEXT_INIT_STACK_POINTER], %sp
    sub     %sp, CONTEXT_CONTROL_INTERRUPT_FRAME_SIZE, %sp

    /* it isn't necessary to save ISF, jump to ISR handling function */
    ba      sparc_isr_handler_ready
    nop

/*
 * ISR from a regular context
 *    %l4 contains the current core control structure
 *    %l5 contains the current core context structure
 *
 * This sub-routine is executed when is necessary to save the current window as
 * its contents are required to resume the CPU execution
 */
sparc_isr_handler_regular_context:

    /* load ISR nesting level, and increment it */
    ld      [%l5 + CONTEXT_NESTING_LEVEL], %l6
    add     %l6, 1, %l6

    /* check if the ISR nesting level is higher than 1, and save it */
    cmp     %l6, 1
    bgu     sparc_isr_handler_from_hypervisor
    st      %l6, [%l5 + CONTEXT_NESTING_LEVEL] ! delay slot

/*
 * ISR from partition code
 *    %l4 contains the current core control structure
 *    %l5 contains the current core context structure
 *
 * This sub-routine is executed when the ISR comes from partition code, in this
 * case is necessary to check if we are at the invalid window and if soo we need
 * to save the previous window.
 *
 * We also need to change to a safe stack area and store the current ISF in
 * order to allow the ISR to be preemptable.
 */
sparc_isr_handler_from_partition:

    /* save the globals used by this routine */
    mov     %g6, %l6
    mov     %g7, %l7

    /* check if we are at the invalid window */
    mov     %wim, %g6
    srl     %g6, %l0, %g7

    cmp     %g7, 1                          ! at an invalid window?
    bne     1f                              ! no? than dont save it

    /* compute the new WIM */
    srl     %g6, 0x1, %g7
    sll     %g6, SPARC_REGISTER_WINDOWS_COUNT - 1, %g6
    or      %g6, %g7, %g6

    /* save window as if we overflowed into it */
    save
    mov     %g6, %wim                       ! apply new WIM
    nop; nop; nop                           ! WRWIM delay slots

    /* 1st check if the address is aligned */
    andcc   %sp, 0x7, %g0
    bne     2f
    nop

    /* 2nd check if the address is in kernel zone */
    set     SYM(air_kernel_memory_end), %g6
    cmp     %sp, %g6
    bleu    2f
    nop

    /* 3rd disable MMU to prevent faults */
    mov     AC_M_SFSR, %g6
    lda     [%g6] ASI_LEON_MMUREGS, %g0         ! clear flauts
    lda     [%g0] ASI_LEON_MMUREGS, %g6         ! read MMU control
    or      %g6, 0x2, %g6                       ! set no_flaut bit
    sta     %g6, [%g0] ASI_LEON_MMUREGS         ! write MMU control

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
    andn    %g6, 0x2, %g6
    sta     %g6, [%g0] ASI_LEON_MMUREGS         ! clear no flaut bit
    mov     AC_M_SFAR, %g6
    lda     [%g6] ASI_LEON_MMUREGS, %g0
    mov     AC_M_SFSR, %g6
    lda     [%g6] ASI_LEON_MMUREGS, %g6         ! get the flaut information

2:

    restore
    nop

/**
 * @brief Save the interrupt context
 */
1:

    /* load the core context stack */
    ld      [%l5 + CONTEXT_INIT_STACK_POINTER], %sp
    sub     %sp, CONTEXT_CONTROL_INTERRUPT_FRAME_SIZE, %sp

    /* make room for the ISF */
    std     %l0, [%sp + ISF_PSR_OFFSET]     ! save psr, PC
    std     %l2, [%sp + ISF_NPC_OFFSET]     ! save nPC and trap number
    st      %g1, [%sp + ISF_G1_OFFSET]      ! save g1
    std     %g2, [%sp + ISF_G2_OFFSET]      ! save g2, g3
    std     %g4, [%sp + ISF_G4_OFFSET]      ! save g4, g5
    std     %l6, [%sp + ISF_G6_OFFSET]      ! save g6, g7 -- see above
    std     %i0, [%sp + ISF_I0_OFFSET]      ! save i0, i1
    std     %i2, [%sp + ISF_I2_OFFSET]      ! save i2, i3
    std     %i4, [%sp + ISF_I4_OFFSET]      ! save i4, i5
    std     %i6, [%sp + ISF_I6_FP_OFFSET]   ! save i6/fp, i7

    rd      %y, %g1

    /* restore the globals used by this routine */
    mov     %l6, %g6
    mov     %l7, %g7

    /* call the trap handling function */
    ba      sparc_isr_handler_ready
    st      %g1, [%sp + ISF_Y_OFFSET]       ! save y, delay slot

/*
 * ISR from hypervisor code
 *    %l4 contains the current core control structure
 *    %l5 contains the current core context structure
 *
 * This sub-routine is executed when the ISR comes from hypervisor code, in this
 * case is necessary is necessary to save the current window on ISF of the
 * previous ISF in addition to the current ISF. This time the stack is already
 * set to a safe area.
 */
sparc_isr_handler_from_hypervisor:

    /* increment CWP on the current PSR */
    add     %l0, 1, %l6
    and     %l6, SPARC_PSR_CWP_MASK, %l6
    andn    %l0, SPARC_PSR_CWP_MASK, %l0
    or      %l0, %l6, %l0

    /* make room for the ISF (we cannot use %sp register here) */
    sub     %fp, CONTEXT_CONTROL_INTERRUPT_FRAME_SIZE, %l6

    /* get previous stack pointer */
    ld      [%l5 + CONTEXT_STACK_POINTER], %l7

    /* save the ISF */
    std     %l0, [%l6 + ISF_PSR_OFFSET]         ! save psr, PC
    std     %l2, [%l6 + ISF_NPC_OFFSET]         ! save nPC, trap number
    st      %l7, [%l6 + ISF_PSP_OFFSET]         ! save previous stack pointer

    st      %g1, [%l6 + ISF_G1_OFFSET]          ! save g1
    std     %g2, [%l6 + ISF_G2_OFFSET]          ! save g2, g3
    std     %g4, [%l6 + ISF_G4_OFFSET]          ! save g4, g5
    std     %g6, [%l6 + ISF_G6_OFFSET]          ! save g6, g7

    std     %i0, [%l6 + ISF_I0_OFFSET]          ! save i0, i1
    std     %i2, [%l6 + ISF_I2_OFFSET]          ! save i2, i3
    std     %i4, [%l6 + ISF_I4_OFFSET]          ! save i4, i5
    std     %i6, [%l6 + ISF_I6_FP_OFFSET]       ! save i6/fp, i7

    rd      %y, %g1
    st      %g1, [%l6 + ISF_Y_OFFSET]           ! save y

    /*
     * save the locals and stack pointer in the global registers to preserve
     * them during the return to the ISR window
     */
    mov     %l0, %g1        ! return PSR
    mov     %l1, %g2        ! return PC
    mov     %l2, %g3        ! return nPC
    mov     %l3, %g4        ! trap number
    mov     %l4, %g5        ! core control
    mov     %l5, %g6        ! core context
    mov     %l6, %g7        ! ISF pointer

    /* return the to the ISR window, and save it as if we underflowed into it */
    restore
    nop; nop; nop

    std     %l0, [%g7 + CPU_STACK_FRAME_L0_OFFSET]
    std     %l2, [%g7 + CPU_STACK_FRAME_L2_OFFSET]
    std     %l4, [%g7 + CPU_STACK_FRAME_L4_OFFSET]
    std     %l6, [%g7 + CPU_STACK_FRAME_L6_OFFSET]
    std     %i0, [%g7 + CPU_STACK_FRAME_I0_OFFSET]
    std     %i2, [%g7 + CPU_STACK_FRAME_I2_OFFSET]
    std     %i4, [%g7 + CPU_STACK_FRAME_I4_OFFSET]
    std     %i6, [%g7 + CPU_STACK_FRAME_I6_FP_OFFSET]

    /* restore this ISR state */
    mov     %g1, %l0
    mov     %g2, %l1
    mov     %g3, %l2
    mov     %g4, %l3
    mov     %g5, %l4
    mov     %g6, %l5
    mov     %g7, %sp                        ! (%sp register is available again)

/*
 * ISR handler ready
 *    %l4 contains the current core control structure
 *    %l5 contains the current core context structure
 *
 * At this point all required information is stored and we are ready to jump to
 * C code, but first we must disable interrupts and enable traps
 */
sparc_isr_handler_ready:

    /* save the current context stack pointer */
    st      %sp, [%l5 + CONTEXT_STACK_POINTER]      ! save ISF

    /* enable traps and disable interrupts */
    or      %l0, SPARC_PSR_PIL_MASK, %l5            ! disable interrupts
    mov     %l5, %psr
    wr      %l5, SPARC_PSR_ET_MASK, %psr            ! enable traps

    /* jump to C part of the ISR handler */
    mov     %sp, %o0                                ! 1st arg.: ISF pointer
    mov     %l4, %o1                                ! 2nd arg.: core control
    call    sparc_isr_handler                       ! ISR handler function
    mov     %l3, %o2                                ! 3nd arg.: trap number

    /* disable traps */
    mov     %psr, %l5
    andn    %l5, SPARC_PSR_ET_MASK, %l5
    mov     %l5, %psr                               ! disable traps
    nop; nop; nop;                                  ! WRPSR delay

    /* get current control control and context */
    rd      %asr17, %l5
    srl     %l5, 28, %l5
    and     %l5, 0xff, %l5                          ! core index

    /* get current core control structure */
    umul    %l5, CORE_CTRL_SIZE, %l5             ! core control mem. offset
    set     air_shared_area, %l4                 ! shared area pointer
    ld      [%l4 + SHAREDAREA_CORE_CTRL], %l4    ! shared area->cores
    add     %l4, %l5, %l4                        ! current core control

    /* get the core context and ISF (they may have changed in context switch) */
    ld      [%l4 + CORE_CTRL_CONTEXT], %l5          ! core context
    ld      [%l5 + CONTEXT_STACK_POINTER], %sp      ! restore ISF pointer

    /* check if the context is still trashed */
    ld      [%l5 + CONTEXT_TRASHED], %l6
    tst     %l6
    bz      sparc_isr_handler_return
    nop

/*
 * ISR restore ISR stack
 *
 * After the ISR handler, the core context continues trashed, remove the ISF
 * and restore the context from the original context
 */
    ld      [%l5 + CONTEXT_INIT_STACK_POINTER], %sp

/*
 * ISR return
 *    %l4 contains the current core control structure
 *    %l5 contains the current core context structure
 *    %o0 contains the partition ISR return address (if applicable)
 *
 * The ISR handler is returning, we must check if we are returning to hypervisor
 * code or to partition code
 */
sparc_isr_handler_return:

    /* load ISR nesting level, and decrement it */
    ld      [%l5 + CONTEXT_NESTING_LEVEL], %l6
    subcc   %l6, 1, %l6                             ! decrement ISR counter

    /* if higher than 0 then this handler is returning to hypervisor code */
    bz      sparc_isr_handler_return_to_partition
    st      %l6, [%l5 + CONTEXT_NESTING_LEVEL] ! delay slot

/*
 * ISR return hypervisor code
 *    %l4 contains the current core control structure
 *    %l5 contains the current core context structure
 *    %o0 contains the partition ISR return address (if applicable)
 *
 * This sub-routine is executed when the ISR comes from hypervisor code, in this
 * case there was a nested ISR, and we must unwind it.
 */
sparc_isr_handler_return_to_hypervisor:

    /* restore previous stack pointer */
    ld      [%sp + ISF_PSP_OFFSET], %l6     ! restore previous stack frame
    st      %l6, [%l5 + CONTEXT_STACK_POINTER]

    /* restore the previous window ISR as if we underflow into into it */
    ldd     [%sp + CPU_STACK_FRAME_L0_OFFSET], %l0
    ldd     [%sp + CPU_STACK_FRAME_L2_OFFSET], %l2
    ldd     [%sp + CPU_STACK_FRAME_L4_OFFSET], %l4
    ldd     [%sp + CPU_STACK_FRAME_L6_OFFSET], %l6
    ldd     [%sp + CPU_STACK_FRAME_I0_OFFSET], %i0
    ldd     [%sp + CPU_STACK_FRAME_I2_OFFSET], %i2
    ldd     [%sp + CPU_STACK_FRAME_I4_OFFSET], %i4
    ldd     [%sp + CPU_STACK_FRAME_I6_FP_OFFSET], %i6

    /* load the current ISR PSR */
    ld      [%sp + ISF_PSR_OFFSET], %g1

    /* fix the PSR with the new CWP */
    mov     %psr, %g2                       ! grab the new CWP
    sub     %g2, 1, %g2                     ! jump to the next window
    and     %g2, SPARC_PSR_CWP_MASK, %g2    ! modulo by number of windows
    andn    %g1, SPARC_PSR_CWP_MASK, %g1    ! clear the old CWP
    or      %g1, %g2, %g1                   ! apply the the new CWP

    /* apply the modified PSR (without risking an overflow trap) */
    mov     %g1, %psr
    nop; nop; nop                           ! WRPSR delay slots

    /* restore y register */
    ld      [%fp + ISF_Y_OFFSET], %g1       ! restore y
    wr      %g1, 0, %y

    /* restore the PC and nPC */
    ld      [%fp + ISF_PC_OFFSET], %l1      ! restore PC
    ldd     [%fp + ISF_NPC_OFFSET], %l2     ! restore nPC, trap number

    /* restore the global registers */
    ld      [%fp + ISF_G1_OFFSET], %g1      ! restore g1
    ldd     [%fp + ISF_G2_OFFSET], %g2      ! restore g2, g3
    ldd     [%fp + ISF_G4_OFFSET], %g4      ! restore g4, g5
    ldd     [%fp + ISF_G6_OFFSET], %g6      ! restore g6, g7

    /* restore the output registers from the previous ISR */
    ldd     [%fp + ISF_I0_OFFSET], %i0      ! restore o0, o1
    ldd     [%fp + ISF_I2_OFFSET], %i2      ! restore o2, o3
    ldd     [%fp + ISF_I4_OFFSET], %i4      ! restore o4, o5
    ldd     [%fp + ISF_I6_FP_OFFSET], %i6   ! restore o6/sp, o7

    /* return to the previous ISR */
    jmp     %l1
    rett    %l2

/*
 * ISR return partition code
 *    %l4 contains the current core control structure
 *    %l5 contains the current core context structure
 *    %o0 contains the partition ISR return address (if applicable)
 *
 * This sub-routine is executed when the ISR comes from partition code, in this
 * case is necessary to restore the previous partition state, and simulation an
 * virtual trap if the partition have an handler defined for the current trap
 * number
 */
sparc_isr_handler_return_to_partition:

    /* save the locals required for the return */
    mov     %l5, %g1                        ! current core context

    /* restore y register */
    ld      [%sp + ISF_Y_OFFSET], %l5       ! restore y
    wr      %l5, 0, %y

    /* restore the PSR, PC and nPC */
    ldd     [%sp + ISF_PSR_OFFSET], %l0     ! restore psr, PC
    ldd     [%sp + ISF_NPC_OFFSET], %l2     ! restore nPC, trap number

    /* fix the PSR with the new CWP */
    mov     %psr, %l6
    and     %l6, SPARC_PSR_CWP_MASK, %l6    ! grab the new CWP
    andn    %l0, SPARC_PSR_CWP_MASK, %l0    ! clear the old CWP
    or      %l0, %l6, %l0                   ! apply the the new CWP

    /* restore the previous window output registers */
    ldd     [%sp + ISF_I0_OFFSET], %i0      ! restore o0, o1
    ldd     [%sp + ISF_I2_OFFSET], %i2      ! restore o2, o3
    ldd     [%sp + ISF_I4_OFFSET], %i4      ! restore o4, o5
    ldd     [%sp + ISF_I6_FP_OFFSET], %i6   ! restore o6/sp, o7

    /*
     * check if the trap return will cause an window underflow,
     * this happens if a partition context switch occurred
     */
    rd      %wim, %g4
    add     %l0, 1, %g5
    and     %g5, SPARC_PSR_CWP_MASK, %g5    ! g5 = cwp + 1
    srl     %g4, %g5, %g5                   ! g5 = win >> cwp + 1 ; shift count

    cmp     %g5, 1
    bne     1f

    /* rotate WIM to the left */
    sll     %g4, 1, %g5
    srl     %g4, SPARC_REGISTER_WINDOWS_COUNT -1 , %g4
    or      %g4, %g5, %g4
    /* restore the window just as if we underflowed into it */
    mov     %g4,  %wim                      ! WIM = new WIM
    nop; nop; nop                           ! WRWIM delay slots

    restore

    /* 1st check if the address is aligned */
    andcc   %sp, 0x7, %g0
    bne     2f
    nop

    /* 2nd check if the address is in kernel zone */
    set     SYM(air_kernel_memory_end), %g4
    cmp     %sp, %g4
    bleu    2f
    nop

    /* 3rd disable MMU to prevent faults */
    mov     AC_M_SFSR, %g4
    lda     [%g4] ASI_LEON_MMUREGS, %g0         ! clear flauts
    lda     [%g0] ASI_LEON_MMUREGS, %g4         ! read MMU control
    or      %g4, 0x2, %g4                       ! set no_flaut bit
    sta     %g4, [%g0] ASI_LEON_MMUREGS         ! write MMU control

    /* restore the window contents */
    ldd     [%sp + CPU_STACK_FRAME_L0_OFFSET], %l0
    ldd     [%sp + CPU_STACK_FRAME_L2_OFFSET], %l2
    ldd     [%sp + CPU_STACK_FRAME_L4_OFFSET], %l4
    ldd     [%sp + CPU_STACK_FRAME_L6_OFFSET], %l6
    ldd     [%sp + CPU_STACK_FRAME_I0_OFFSET], %i0
    ldd     [%sp + CPU_STACK_FRAME_I2_OFFSET], %i2
    ldd     [%sp + CPU_STACK_FRAME_I4_OFFSET], %i4
    ldd     [%sp + CPU_STACK_FRAME_I6_FP_OFFSET], %i6

    /* clear MMU no-flaut bit */
    andn    %g4, 0x2, %g4
    sta     %g4, [%g0] ASI_LEON_MMUREGS         ! clear no flaut bit
    mov     AC_M_SFAR, %g4
    lda     [%g4] ASI_LEON_MMUREGS, %g0
    mov     AC_M_SFSR, %g4
    lda     [%g4] ASI_LEON_MMUREGS, %g4         ! get the flaut information

2:
    /* return to the current window */
    save
    nop

1:
    /* resotre current core context */
    mov     %g1, %l5

    /* restore the global registers */
    ld      [%sp + ISF_G1_OFFSET], %g1     ! restore g1
    ldd     [%sp + ISF_G2_OFFSET], %g2     ! restore g2, g3
    ldd     [%sp + ISF_G4_OFFSET], %g4     ! restore g4, g5
    ldd     [%sp + ISF_G6_OFFSET], %g6     ! restore g6, g7

    /* check if we are returning to the partition or virtual partition trap */
    tst     %o0
    bz      1f
    nop

/*
 * ISR return partition code (virtual trap inside partition)
 *    %l0 contains the current PSR
 *    %l1 contains the partition return PC
 *    %l2 contains the partition return nPC
 *    %l3 contains the virtual trap number for the partition
 *    %l5 contains the current core context
 *    %o0 contains the partition virtual trap entry point
 */
    /* decrement the CWP to simulate a trap window on POS */
    sub     %l0, 1, %l5
    and     %l5, SPARC_PSR_CWP_MASK, %l5
    andn    %l0, SPARC_PSR_CWP_MASK, %l0
    or      %l0, %l5, %l0

    /* apply the new PSR */
    mov     %l0, %psr
    nop; nop; nop                           ! WRPSR delay slots

    /* return from trap to the POS ISR handler */
    jmp     %i0
    rett    %i0 + 4

/*
 * ISR return partition code
 *    %l0 contains the current PSR
 *    %l1 contains the ISR return PC
 *    %l2 contains the ISR return nPC
 *    %l5 contains the current core context
 */
1:

    /* restore cache state */
    ld      [%l5 + CONTEXT_VCPU + SPARC_VCPU_CCTRL], %l4
    sethi   %hi(0x810000), %l6
    or      %l6, %l4, %l5
    sta     %l5, [%g0]2

    /* restore the PSR */
    mov     %l0, %psr
    nop; nop; nop                          ! WRPSR delay slots

    /* return to the previous ISR */
    jmp     %l1
    rett    %l2

/**
 * @brief Disable preemption, by appling a given PIL mask
 */
    GLOBAL(sparc_disable_preemption)
SYM(sparc_disable_preemption):

    mov     %o0, %o1
    ta      0x7F
    retl
    nop

/**
 * @brief Enable preemption, by allowing all interrupts via PIL
 */
    GLOBAL(sparc_enable_preemption)
SYM(sparc_enable_preemption):

    mov     %g0, %o1
    ta      0x7F
    mov     %o2, %o0
    retl
    nop
