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
#include <air.h>


    GLOBAL(air_sparc_disable_interrupts)
SYM(air_sparc_disable_interrupts):

    set     AIR_SYSCALL_SPARC_DISABLE_INTERRUPTS, %o5
    ta      AIR_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(air_sparc_enable_interrupts)
SYM(air_sparc_enable_interrupts):

    set     AIR_SYSCALL_SPARC_ENABLE_INTERRUPTS, %o5
    ta      AIR_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(air_sparc_disable_traps)
SYM(air_sparc_disable_traps):

    set     AIR_SYSCALL_SPARC_DISABLE_TRAPS, %o5
    ta      AIR_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(air_sparc_enable_traps)
SYM(air_sparc_enable_traps):

    set     AIR_SYSCALL_SPARC_ENABLE_TRAPS, %o5
    ta      AIR_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(air_sparc_disable_fpu)
SYM(air_sparc_disable_fpu):

    set     AIR_SYSCALL_SPARC_DISABLE_FPU, %o5
    ta      AIR_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(air_sparc_enable_fpu)
SYM(air_sparc_enable_fpu):

    set     AIR_SYSCALL_SPARC_ENABLE_FPU, %o5
    ta      AIR_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(air_sparc_get_tbr)
SYM(air_sparc_get_tbr):

    set     AIR_SYSCALL_SPARC_GET_TBR, %o5
    ta      AIR_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(air_sparc_set_tbr)
SYM(air_sparc_set_tbr):

    set     AIR_SYSCALL_SPARC_SET_TBR, %o5
    ta      AIR_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(air_sparc_get_psr)
SYM(air_sparc_get_psr):

    set     AIR_SYSCALL_SPARC_GET_PSR, %o5
    ta      AIR_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(air_sparc_set_psr)
SYM(air_sparc_set_psr):

    set     AIR_SYSCALL_SPARC_SET_PSR, %o5
    ta      AIR_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(air_sparc_get_cache_register)
SYM(air_sparc_get_cache_register):

    set     AIR_SYSCALL_SPARC_GET_CACHE_REGISTER, %o5
    ta      AIR_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(air_sparc_set_cache_register)
SYM(air_sparc_set_cache_register):

    set     AIR_SYSCALL_SPARC_SET_CACHE_REGISTER, %o5
    ta      AIR_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(air_sparc_get_irq_mask_register)
SYM(air_sparc_get_irq_mask_register):

    set     AIR_SYSCALL_SPARC_GET_IRQ_MASK_REGISTER, %o5
    ta      AIR_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(air_sparc_set_irq_mask_register)
SYM(air_sparc_set_irq_mask_register):

    set     AIR_SYSCALL_SPARC_SET_IRQ_MASK_REGISTER, %o5
    ta      AIR_SYSCALL_SPARC_TRAP
    retl
    nop


    GLOBAL(air_sparc_set_irq_force_register)
SYM(air_sparc_set_irq_force_register):

    set     AIR_SYSCALL_SPARC_SET_IRQ_FORCE_REGISTER, %o5
    ta      AIR_SYSCALL_SPARC_TRAP
    retl
    nop

    GLOBAL(air_sparc_get_inst_cache_config_register)
SYM(air_sparc_get_inst_cache_config_register):

    set     AIR_SYSCALL_SPARC_GET_INST_CACHE_CFG_REGISTER, %o5
    ta      AIR_SYSCALL_SPARC_TRAP
    retl
    nop

    GLOBAL(air_sparc_get_data_cache_config_register)
SYM(air_sparc_get_data_cache_config_register):

    set     AIR_SYSCALL_SPARC_GET_DATA_CACHE_CFG_REGISTER, %o5
    ta      AIR_SYSCALL_SPARC_TRAP
    retl
    nop
