/*
 * Copyright (C) 2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file smp_asm.S
 * @author pfnf
 * @brief lock definitions
 */

/* enforce ASM definition */
#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <sparc.h>

    GLOBAL(sparc_lock_init)
SYM(sparc_lock_init):

    retl
    clr     [%o0]                           ! Clear the lock variable


    GLOBAL(sparc_lock_acquire)
SYM(sparc_lock_acquire):

    mov     1, %o1
    swap    [%o0], %o1                      ! Atomic swap
    tst     %o1
    be      2f
    nop
1:                                          ! Busy-Wait
    ld      [%o0], %o1
    tst      %o1
    bne     1b
    nop
    ba      smp_lock_acquire
    nop

2:                                          ! Lock acquired
    retl
    nop

    GLOBAL(sparc_lock_try_acquire)
SYM(sparc_lock_try_acquire):

    mov     1, %o1
    swap    [%o0], %o1                      ! Atomic swap
    cmp     %g0, %o1
    retl
    subx    %g0, -1, %o0

    GLOBAL(sparc_lock_release)
SYM(sparc_lock_release):

    stbar
    retl
    st      %g0, [%o0]                      ! Lock released

