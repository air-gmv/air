/*
 * Copyright (C) 2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file isr_asm.S
 * @author pfnf
 * @brief Bare Partition ISR handler
 * @note This is a very simplified handler, it doesn't not support nesting
 */

/* enforce ASM definition */
#ifndef ASM
#define ASM
#endif

#include <air.h>
#include <asm.h>
#include <sparc.h>

/**
 * @brief ISR handler
 */
    GLOBAL(sparc_isr_handler)
SYM(sparc_isr_handler):

    set     AIR_SYSCALL_SPARC_GET_PSR, %o5
    ta      AIR_SYSCALL_SPARC_TRAP
    mov     %o0, %l0

save_isf:

    sub     %fp, CONTEXT_CONTROL_INTERRUPT_FRAME_SIZE, %sp

    ! make space for ISF
    std     %l0, [%sp + ISF_PSR_OFFSET]         ! save psr, PC
    st      %l2, [%sp + ISF_NPC_OFFSET]         ! save nPC
    st      %g1, [%sp + ISF_G1_OFFSET]          ! save g1
    std     %g2, [%sp + ISF_G2_OFFSET]          ! save g2, g3
    std     %g4, [%sp + ISF_G4_OFFSET]          ! save g4, g5
    std     %g6, [%sp + ISF_G6_OFFSET]          ! save g6, g7

    std     %i0, [%sp + ISF_I0_OFFSET]          ! save i0, i1
    std     %i2, [%sp + ISF_I2_OFFSET]          ! save i2, i3
    std     %i4, [%sp + ISF_I4_OFFSET]          ! save i4, i5
    std     %i6, [%sp + ISF_I6_FP_OFFSET]       ! save i6/fp, i7

    rd      %y, %g1
    st      %g1, [%sp + ISF_Y_OFFSET]           ! save y
    st      %l6, [%sp + ISF_TPC_OFFSET]         ! save real trapped pc


    mov     %sp, %g6                            ! 2nd arg to ISR Handler

    /* get ISR handler */
    set     AIR_SYSCALL_GET_CORE_ID, %o5
    ta      AIR_SYSCALL_OS_TRAP

    set     isr_table, %g4
    umul    %o0, 0x04, %o0
    ld      [%g4 + %o0], %g4

    mov     %g6, %o1
    and     %l3, 0xFF, %g5         ! remove synchronous trap indicator
    sll     %g5, 2, %g5            ! g5 = offset into table
    ld      [%g4 + %g5], %g4       ! g4 = _ISR_Vector_table[ vector ]
    tst     %g4                    ! check if the pointer is valid
    bz      skip_user_handler      ! no? then skip user handler

    ! o1 = 2nd arg = address of the ISF, WAS LOADED WHEN ISF WAS SAVED!!!
    mov       %l3, %o0             ! o0 = 1st arg = vector number
    call      %g4
    nop

skip_user_handler:

    /* disable traps */
    mov     %l0, %o0
    set     AIR_SYSCALL_SPARC_SET_PSR, %o5
    ta      AIR_SYSCALL_SPARC_TRAP         ! **** DISABLE TRAPS ****

    /* return to user-land */
    ld      [%sp + ISF_Y_OFFSET], %l5      ! restore y
    wr      %l5, 0, %y

    ldd     [%sp + ISF_PSR_OFFSET], %l0    ! restore psr, PC
    ld      [%sp + ISF_NPC_OFFSET], %l2    ! restore nPC

    andn    %l0, SPARC_PSR_ET_MASK, %l0
    mov     %sp, %o2

    ld      [%sp + ISF_G1_OFFSET], %g1    ! restore g1
    ldd     [%sp + ISF_G2_OFFSET], %g2    ! restore g2, g3
    ldd     [%sp + ISF_G4_OFFSET], %g4    ! restore g4, g5
    ldd     [%sp + ISF_G6_OFFSET], %g6    ! restore g6, g7

    ldd     [%sp + ISF_I0_OFFSET], %i0    ! restore i0, i1
    ldd     [%sp + ISF_I2_OFFSET], %i2    ! restore i2, i3
    ldd     [%sp + ISF_I4_OFFSET], %i4    ! restore i4, i5
    ldd     [%sp + ISF_I6_FP_OFFSET], %i6 ! restore i6/fp, i7

    mov     %l0, %o0
    set     AIR_SYSCALL_SPARC_SET_PSR, %o5
    ta      AIR_SYSCALL_SPARC_TRAP        ! **** DISABLE TRAPS ****

    /* restore the cache state  */
    set     AIR_SYSCALL_SPARC_RESTORE_CACHE_REGISTER, %o5
    ta      AIR_SYSCALL_SPARC_TRAP

    /* Return from trap */
    mov     %l1, %o0                    ! transfer control and
    mov     %l2, %o1                    ! go back to tasks window
    set     AIR_SYSCALL_SPARC_RETT, %o5
    ta      AIR_SYSCALL_SPARC_TRAP
