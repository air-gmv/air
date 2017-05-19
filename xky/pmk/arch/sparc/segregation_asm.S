/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file segregation_asm.S
 * @author pfnf
 * @brief Provides low level functions to contro the state of a SPARC MMU
 */

#ifndef ASM
#define ASM
#endif

#include <bsp.h>
#include <asm.h>

/**
 *  @brief Initialize the SPARC MMU - Total Store Ordering
 */
    GLOBAL(sparc_mmu_initialize)
SYM(sparc_mmu_initialize):

    /* Setup tables */
    srl    %o0,6, %o0
    sll    %o0,2, %o0
    mov    0x100, %o1
    sta    %o0, [%o1]#ASI_AIUS_L

    /* Set context */
    mov     0x200, %o1
    sta     %g0, [%o1]#ASI_AIUS_L

    /* Activate MMU - Total Store Ordering */
    lda     [%g0]#ASI_AIUS_L, %o0
    bset    1, %o0                      ! activate MMU */
    andn    %o0, 0x80, %o0              ! activate TSO - Total Store Ordering
    retl
    sta     %o0, [%g0]#ASI_AIUS_L       ! delay slot

/*
 *  @brief Change the SPARC MMU - Context
 */
    GLOBAL(sparc_mmu_set_context)
SYM(sparc_mmu_set_context):

    mov     0x200, %o1
    sta     %o0, [%o1]#ASI_AIUS_L

/**
 *  @brief Enables the SPARC MMU
 */
    GLOBAL(sparc_mmu_enable)
SYM(sparc_mmu_enable):
    lda     [%g0]#ASI_AIUS_L, %o1
    bset    1, %o1
    sta     %o1, [%g0]#ASI_AIUS_L
    set     0x400, %o1
    retl
    sta     %g0, [%o1]#ASI_AIUP_L           ! optimized delay slot


/**
 *  @brief Disables the SPARC MMU
 */
    GLOBAL(sparc_mmu_disable)
SYM(sparc_mmu_disable):
    lda     [%g0]#ASI_AIUS_L, %o1
    bclr    1, %o1
    sta     %o1, [%g0]#ASI_AIUS_L
    set     0x400, %o1
    retl
    sta     %g0, [%o1]#ASI_AIUP_L           ! optimized delay slot


/**
 * @brief Peform a protected copy to the paritition user space
 * @param %o0 destination address
 * @param %o1 source address
 * @param %o2 size of memory block
 * @return
 */
    GLOBAL(sparc_mmu_disable_faults)
 SYM(sparc_mmu_disable_faults):

    /* disable MMU faults */
    mov     AC_M_SFSR, %o1
    lda     [%o1] ASI_LEON_MMUREGS, %g0         ! clear flauts
    lda     [%g0] ASI_LEON_MMUREGS, %o1         ! read MMU control
    or      %o1, 0x2, %o1                       ! set no_flaut bit
    retl
    sta     %o1, [%g0] ASI_LEON_MMUREGS         ! write MMU control


    GLOBAL(sparc_mmu_enable_faults)
 SYM(sparc_mmu_enable_faults):

    /* load previous FSR from the context control (must be atomic) */
    mov     0, %o2
    swap    [%o0 + CONTEXT_VCPU + SPARC_VCPU_MMU_FSR], %o2

    /* enable MMU faults and clear its registers */
    lda     [%g0] ASI_LEON_MMUREGS, %o1
    andn    %o1, 0x2, %o1
    sta     %o1, [%g0] ASI_LEON_MMUREGS         ! enable MMU faults
    mov     AC_M_SFAR, %o1
    lda     [%o1] ASI_LEON_MMUREGS, %g0         ! clear Fault Address register
    mov     AC_M_SFSR, %o1
    lda     [%o1] ASI_LEON_MMUREGS, %o1         ! clear Fault Status register

    /* check if faults */
    or      %o1, %o2, %o0                       ! merge previous and current FSR
    retl                                        ! return to caller
    and     %o0, 0x2, %o0                       ! did fault occur ? (delay slot)

