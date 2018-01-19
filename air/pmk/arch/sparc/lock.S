/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file lock.S
 * @author pfnf
 * @brief Assembly basic spinlock based on swap instruction
 */

/* enforce ASM definition */
#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <sparc.h>


/**
 * @brief Initialize the lock
 * @param lock lock object
 */
    GLOBAL(pmk_lock_init)
SYM(pmk_lock_init):
    retl
    clr     [%o0]                           ! Clear the lock variable

/**
 * @brief Acquires the lock
 * @param lock lock object
 *
 * Acquires the lock, if the lock isn't available, the current core busy-waits
 * until it becomes available
 */
    GLOBAL(pmk_lock_acquire)
SYM(pmk_lock_acquire):
    mov     1, %o1
    swap    [%o0], %o1                      ! Atomic swap
    tst     %o1
    be      sparc_lock_acquire_out
    nop
sparc_lock_acquire_loop:                    ! Busy-Wait
    ld      [%o0], %o1
    tst      %o1
    bne     sparc_lock_acquire_loop
    nop
    ba,a    pmk_lock_acquire
    nop
sparc_lock_acquire_out:                     ! Lock acquired
    retl
    nop


/**
 * @brief SPARC non-preemptable spin-lock (lock)
 * @param %o0 lock variable pointer
 * @return previous PIL value
 */
    GLOBAL(sparc_lock_irqsave)
SYM(sparc_lock_irqsave):

    set     SPARC_PSR_PIL_MASK, %o1         ! new PIL mask
    ta      0x7F                            ! disable interrupts, and get PIL

1:                                          ! spin-lock loop
    mov     0x01, %o1
    swap    [%o0], %o1                      ! try aquire lock
    tst     %o1                             ! if zero, the lock was free
    bne     1b                              ! if not, spin it!
    nop

    retl
    mov     %o2, %o0                        ! return previous PIL

/**
 * @brief SPARC non-preemptable spin-lock (unlock)
 * @param %o0 lock variable pointer
 * @param %o1 PIL value to restore
 */
    GLOBAL(sparc_unlock_irqrestore)
SYM(sparc_unlock_irqrestore):

    clr     [%o0]                           ! releasing control
    ta      0x7F                            ! enable interrupts
    retl
    nop

/**
 * @brief Try to acquire the lock
 * @param lock lock object
 * @returns true if the lock was acquired, false otherwise
 *
 * Try to acquire the lock, if the lock isn't available, this function returns
 * immediately
 */
    GLOBAL(pmk_lock_try_acquire)
SYM(pmk_lock_try_acquire):
    mov     1, %o1
    swap    [%o0], %o1                      ! Atomic swap
    cmp     %g0, %o1
    retl
    subx    %g0, -1, %o0

/**
 * @brief Releases the lock
 * @param lock lock object
 *
 * Releases the lock allowing other cores to acquire it
 */
    GLOBAL(pmk_lock_release)
SYM(pmk_lock_release):
    stbar
    retl
    st      %g0, [%o0]                      ! Lock released

/**
 * @brief Swap PIL trap
 *
 * This trap is required in order to safely disable interrupts when traps are
 * enabled. This trap doesn't respect the SPARC ABI, it is designed for
 * sparc_lock_irqsave and sparc_unlock_irqrestore. This trap is only accessible
 * from supervisor code.
 */
    GLOBAL(sparc_supervisor_swap_pil)
SYM(sparc_supervisor_swap_pil):

    andcc   %l0, SPARC_PSR_PS_MASK, %g0     ! check if call is from
    bz      1f                              ! supervisor
    nop

    and     %l0, SPARC_PSR_PIL_MASK, %i2    ! get current PIL
    andn    %l0, SPARC_PSR_PIL_MASK, %l0    ! clear current PIL
    or      %l0, %i1, %l0                   ! apply new PIL

1:
    mov     %l0, %psr                       ! restore PSR
    nop; nop; nop;                          ! WRPSR delay slots

    jmp     %l2                             ! set PC
    rett    %l2 + 0x4                       ! set nPC and exit trap
