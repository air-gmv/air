/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013 - 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file
 * @author pfnf
 * @brief SPARC virtualization system calls (user side)
 */

/* enforce ASM definition */
#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <xky.h>


    GLOBAL(xky_sparc_disable_interrupts)
SYM(xky_sparc_disable_interrupts):

    set     XKY_SYSCALL_SPARC_DISABLE_INTERRUPTS, %o5
    ta      XKY_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(xky_sparc_enable_interrupts)
SYM(xky_sparc_enable_interrupts):

    set     XKY_SYSCALL_SPARC_ENABLE_INTERRUPTS, %o5
    ta      XKY_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(xky_sparc_disable_traps)
SYM(xky_sparc_disable_traps):

    set     XKY_SYSCALL_SPARC_DISABLE_TRAPS, %o5
    ta      XKY_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(xky_sparc_enable_traps)
SYM(xky_sparc_enable_traps):

    set     XKY_SYSCALL_SPARC_ENABLE_TRAPS, %o5
    ta      XKY_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(xky_sparc_disable_fpu)
SYM(xky_sparc_disable_fpu):

    set     XKY_SYSCALL_SPARC_DISABLE_FPU, %o5
    ta      XKY_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(xky_sparc_enable_fpu)
SYM(xky_sparc_enable_fpu):

    set     XKY_SYSCALL_SPARC_ENABLE_FPU, %o5
    ta      XKY_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(xky_sparc_get_tbr)
SYM(xky_sparc_get_tbr):

    set     XKY_SYSCALL_SPARC_GET_TBR, %o5
    ta      XKY_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(xky_sparc_set_tbr)
SYM(xky_sparc_set_tbr):

    set     XKY_SYSCALL_SPARC_SET_TBR, %o5
    ta      XKY_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(xky_sparc_get_psr)
SYM(xky_sparc_get_psr):

    set     XKY_SYSCALL_SPARC_GET_PSR, %o5
    ta      XKY_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(xky_sparc_set_psr)
SYM(xky_sparc_set_psr):

    set     XKY_SYSCALL_SPARC_SET_PSR, %o5
    ta      XKY_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(xky_sparc_get_cache_register)
SYM(xky_sparc_get_cache_register):

    set     XKY_SYSCALL_SPARC_GET_CACHE_REGISTER, %o5
    ta      XKY_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(xky_sparc_set_cache_register)
SYM(xky_sparc_set_cache_register):

    set     XKY_SYSCALL_SPARC_SET_CACHE_REGISTER, %o5
    ta      XKY_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(xky_sparc_get_irq_mask_register)
SYM(xky_sparc_get_irq_mask_register):

    set     XKY_SYSCALL_SPARC_GET_IRQ_MASK_REGISTER, %o5
    ta      XKY_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(xky_sparc_set_irq_mask_register)
SYM(xky_sparc_set_irq_mask_register):

    set     XKY_SYSCALL_SPARC_SET_IRQ_MASK_REGISTER, %o5
    ta      XKY_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(xky_sparc_set_irq_force_register)
SYM(xky_sparc_set_irq_force_register):

    set     XKY_SYSCALL_SPARC_SET_IRQ_FORCE_REGISTER, %o5
    ta      XKY_SYSCALL_SPARC_TRAP
    retl
    nop
