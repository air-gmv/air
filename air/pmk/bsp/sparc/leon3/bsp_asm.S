/*
 * Copyright (C) 2008-2012  GMVIS Skysoft S.A.
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

/* enforce ASM definition */
#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <sparc.h>


    GLOBAL(leon_shutdown_core)
SYM(leon_shutdown_core):

    mov     %psr, %o0
    andn    %o0, SPARC_PSR_ET_MASK, %o0         ! disable traps
    or      %o0, SPARC_PSR_PIL_MASK, %o0        ! disable interrupts
    mov     %o0, %psr                           ! apply new psr
    nop; nop; nop;                              ! WRPSR delay
    ta      0x0


    GLOBAL(leon_idle_loop)
SYM(leon_idle_loop):

    mov     %g0, %asr19                         ! power down core
    lda     [%sp] 1, %g0                        ! fix some processors
    ba      leon_idle_loop                      ! stay in idle loop after IRQ
    nop

    GLOBAL(leon_restart_core)
SYM(leon_restart_core):

    mov     %psr, %o0
    andn    %o0, SPARC_PSR_ET_MASK, %o0         ! disable traps
    or      %o0, SPARC_PSR_PIL_MASK, %o0        ! disable interrupts
    mov     %o0, %psr                           ! apply new psr
    nop; nop; nop                               ! WRPSR delay

    set     pmk_trap_table, %o0                 ! jump to trap table
    jmp     %o0
    jmp     %o0 + 0x04
