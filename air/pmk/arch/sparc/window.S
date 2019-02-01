/**
 * @file
 * @author
 * @brief
 */

#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <bsp.h>

/**
 * @brief Window overflow trap handler
 * @param %l0 psr (from trap table)
 * @param %l1 pc
 * @param %l2 npc
 */
    GLOBAL(sparc_window_overflow_handler)
SYM(sparc_window_overflow_handler):

    /* save the global registers used by this routine */
    mov     %g1, %l7                            ! save g1
    mov     %g2, %l6                            ! save g2
	mov     %g3, %l5                            ! save g3

    /* determine the new WIM */
    mov     %wim, %l3
    srl     %l3, 1, %g1
    sll     %l3, SPARC_REGISTER_WINDOWS_COUNT-1 , %l4
    or      %l4, %g1, %g1                       ! new WIM
	
    /* clear WIM to prevent error in the next save */
    mov     %g0, %wim                           ! clear WIM
    nop; nop; nop                               ! WRWIM delay

    /* go into window to be saved */
    save                                        ! go into window to be saved
    mov     %g1, %wim                           ! load new WIM
    nop; nop; nop                               ! WRWIM slot delay

    /*
     * since PMK is compiled with a flat model, this trap can only occur from
     * code in userland, thus the kernel must protected against invalid stack
     * pointers
     */

    /* 1st check if the address is aligned */
    andcc   %sp, 0x7, %g0
    bne     1f
    nop

    /* 2nd check if the address is in kernel zone */
    set     SYM(air_kernel_memory_end), %g3
    cmp     %sp, %g3
    bleu    1f
    nop

    /* 3rd disable MMU to prevent faults */
    mov     AC_M_SFSR, %g3
    lda     [%g3] ASI_LEON_MMUREGS, %g0         ! clear flauts
    lda     [%g0] ASI_LEON_MMUREGS, %g3         ! read MMU control
    or      %g3, 0x2, %g3                       ! set no_flaut bit
    sta     %g3, [%g0] ASI_LEON_MMUREGS         ! write MMU control

	/* dump the window contents */
    std     %l0, [%sp + 0x00]                   ! save local register set
    std     %l2, [%sp + 0x08]
    std     %l4, [%sp + 0x10]
    std     %l6, [%sp + 0x18]
    std     %i0, [%sp + 0x20]                   ! save input register set
    std     %i2, [%sp + 0x28]
    std     %i4, [%sp + 0x30]
    std     %i6, [%sp + 0x38]

    /* check if faults occur */
    andn    %g3, 0x2, %g3
    sta     %g3, [%g0] ASI_LEON_MMUREGS

    mov     AC_M_SFAR, %g3
    lda     [%g3] ASI_LEON_MMUREGS, %g0

    mov     AC_M_SFSR, %g3
    lda     [%g3] ASI_LEON_MMUREGS, %g3
    andcc   %g3, 0x2, %g0                       ! did a fault occur ?
    bne     1f                                  ! yes? then health-monitor
    nop

    /* return to the trap window */
    restore

    /* restore the globals used by this routine */
    mov     %l7, %g1
    mov     %l6, %g2
    mov     %l5, %g3

    /* restpre PSR */
	mov     %l0, %psr
	nop; nop; nop                               ! WRPSR delay

    /* return from trap */
    jmp  %l1                                    ! re-execute save.
    rett %l2

/**
 * @brief Call the health-monitor routine
 */
1:

    /* return to the trap window */
    restore

    /* go to health-monitor */
    sethi   %hi(sparc_isr_handler_entry), %l4
    or      %l4, %lo(sparc_isr_handler_entry), %l4      ! get point to the ISR handler

    or      %g0, 0x2B, %l3                      ! set as data-store error

    /* return from trap */
    jmp     %l4                                 ! jump to health-monitor
    jmp     %l4  + 4



/**
 * @brief Window underflow trap handler
 * @param %l0 psr (from trap table)
 * @param %l1 pc
 * @param %l2 npc
 */
    GLOBAL(sparc_window_underflow_handler)
SYM(sparc_window_underflow_handler):

    /* save the globals used by this handler */
    mov     %g1, %l6
    mov     %g2, %l7

    /* determine the new WIM */
    mov     %wim, %l3
    sll     %l3, 1, %l4
    srl     %l3, SPARC_REGISTER_WINDOWS_COUNT - 1, %l5
    or      %l5, %l4, %l5

    /* apply the new WIM */
    mov     %l5, %wim
    nop; nop; nop                               ! WRWIM delay

    /* jump into the window that we wish to restore */
    restore
    restore

    /*
     * since PMK is compiled with a flat model, this trap can only occur from
     * code in userland, thus the kernel must protected against invalid stack
     * pointers, except syscalls where we explicitly enabled pre-emption.
     */

    /* 1st check if the address is aligned */
    andcc   %sp, 0x7, %g0
    bne     1f
    nop

    /* 2nd check if the address is in kernel zone */
    set     SYM(air_kernel_memory_end), %g1
    cmp     %sp, %g1
    bgu sparc_window_underflow_handler_restore
    nop

    /* Only restore an address in kernel zone while we are executing a ISR*/
    /* Get current control control and context */
    rd      %asr17, %g1
    srl     %g1, 28, %g1
    and     %g1, 0xff, %g1                       ! core index

    /* get current core control structure */
    umul    %g1, CORE_CTRL_SIZE, %g1             ! core control mem. offset
    set     air_shared_area, %g2                 ! shared area pointer
    ld      [%g2 + SHAREDAREA_CORE_CTRL], %g2    ! shared area->cores
    add     %g2, %g1, %g2                        ! current core control
    ld      [%g2 + CORE_CTRL_CONTEXT], %g1       ! core context pointer

    ld      [%g1 + CONTEXT_NESTING_LEVEL], %g2   ! get ISR nesting level
    tst     %g2
    bz      1f
    nop

sparc_window_underflow_handler_restore:
    /* 3rd disable MMU to prevent faults */
    mov     AC_M_SFSR, %g1
    lda     [%g1] ASI_LEON_MMUREGS, %g0         ! clear flauts
    lda     [%g0] ASI_LEON_MMUREGS, %g1         ! read MMU control
    or      %g1, 0x2, %g1                       ! set no_flaut bit
    sta     %g1, [%g0] ASI_LEON_MMUREGS         ! write MMU control

    /* restore the window contents */
    ldd     [%sp + 0x00], %l0                   ! first the local register set
    ldd     [%sp + 0x08], %l2
    ldd     [%sp + 0x10], %l4
    ldd     [%sp + 0x18], %l6
    ldd     [%sp + 0x20], %i0                   ! then the input registers
    ldd     [%sp + 0x28], %i2
    ldd     [%sp + 0x30], %i4
    ldd     [%sp + 0x38], %i6

    /* check if faults occur */
    andn    %g1, 0x2, %g1
    sta     %g1, [%g0] ASI_LEON_MMUREGS         ! clear no flaut bit

    mov     AC_M_SFAR, %g1
    lda     [%g1] ASI_LEON_MMUREGS, %g0

    mov     AC_M_SFSR, %g1
    lda     [%g1] ASI_LEON_MMUREGS, %g1
    andcc   %g1, 0x2, %g0                       ! did a fault occur ?
    bne     1f                                  ! yes? then health-monitor
    nop

    /* return to the trap window */
    save
    save

    /* restore the used global */
    mov     %l6, %g1
    mov     %l7, %g2

    /* return from trap */
    jmp   %l1                        ! Re-execute restore.
    rett  %l2

/**
 * @brief Call the health-monitor routine
 */
1:

    /* return to the trap window */
    save
    save

    /* go to health-monitor */
    sethi   %hi(sparc_isr_handler_entry), %l4
    or      %l4, %lo(sparc_isr_handler_entry), %l4      ! get point to the ISR handler

    or      %g0, 0x2B, %l3                      ! set as data-store error

    /* return from trap */
    jmp     %l4                                 ! jump to health-monitor
    jmp     %l4 + 4

/**
 * @brief Flush all windows trap handler
 * @param %l0 psr (from trap table)
 * @param %l1 pc
 * @param %l2 npc
 */
    GLOBAL(sparc_window_flush_handler)
SYM(sparc_window_flush_handler):

    mov %g6, %l6
    mov %g7, %l7

    /* disable MMU to prevent faults */
    mov     AC_M_SFSR, %g6
    lda     [%g6] ASI_LEON_MMUREGS, %g0         ! clear flauts
    lda     [%g0] ASI_LEON_MMUREGS, %g6         ! read MMU control
    or      %g6, 0x2, %g6                       ! set no_flaut bit
    sta     %g6, [%g0] ASI_LEON_MMUREGS         ! write MMU control
		
    /* get PSR and WIM */
    mov     %l0, %g1                            ! get PSR
    mov     %wim, %g2                           ! get WIM

    /* get CWP + 1 */
    and     %l0, SPARC_PSR_CWP_MASK, %g3
    add     %g3, 1, %g5
    and     %g5, SPARC_REGISTER_WINDOWS_COUNT - 1, %g5

    /* get WIM for CWP + 1 invalid */
    mov     1, %g4
    sll     %g4, %g5, %g4

    /* go back the trap window */
    restore

pmk_window_flush_loop:

    sll     %g4, 1, %g5                   ! rotate the "wim" left 1
    srl     %g4, SPARC_REGISTER_WINDOWS_COUNT - 1, %g4
    or      %g4, %g5, %g4                 ! g4 = wim if we do one restore
 
    /* check if a restore would cause an underflow */
    andcc   %g4, %g2, %g0                 ! Any windows to flush?
    bnz     pmk_window_flush_done         ! No, then continue
    nop
 
    /* go back one window */
    restore

    /* check if the stack address is aligned */
    andcc   %sp, 0x7, %g0
    bne     1f
    nop

    /* check if the stack address is in kernel zone */
    set     SYM(air_kernel_memory_end), %g7
    cmp     %sp, %g7
    bleu    1f
    nop

    /* dump the window contentns */
    std     %l0, [%sp + CPU_STACK_FRAME_L0_OFFSET]
    std     %l2, [%sp + CPU_STACK_FRAME_L2_OFFSET]
    std     %l4, [%sp + CPU_STACK_FRAME_L4_OFFSET]
    std     %l6, [%sp + CPU_STACK_FRAME_L6_OFFSET]
    std     %i0, [%sp + CPU_STACK_FRAME_I0_OFFSET]
    std     %i2, [%sp + CPU_STACK_FRAME_I2_OFFSET]
    std     %i4, [%sp + CPU_STACK_FRAME_I4_OFFSET]
    std     %i6, [%sp + CPU_STACK_FRAME_I6_FP_OFFSET]
 
    /* go for the next window */
    ba      pmk_window_flush_loop
    nop

pmk_window_flush_done:

    /* check if any MMU faults occurred */
    andn    %g6, 0x2, %g6
    sta     %g6, [%g0] ASI_LEON_MMUREGS         ! clear no flaut bit
    mov     AC_M_SFAR, %g6
    lda     [%g6] ASI_LEON_MMUREGS, %g0
    mov     AC_M_SFSR, %g6
    lda     [%g6] ASI_LEON_MMUREGS, %g6         ! get the flaut information

    andcc   %g6, 0x2, %g0                       ! did a fault occur ?
    bne     1f                                  ! yes? then health-monitor
    nop

    /*restore PSR */
    mov     %g1, %psr
    /* compute the new WIM */
    add     %g3, 2, %g2                   ! calculate desired WIM
    and     %g2, SPARC_REGISTER_WINDOWS_COUNT - 1, %g2
    mov     1, %g4
    sll     %g4, %g2, %g4                 ! g4 = new WIM

    /* apply the new WIM*/
    mov     %g4, %wim
    nop
    /* restore globals */
    mov %l6, %g6
    mov %l7, %g7

    /* return from trap */
    jmp     %l2
    rett    %l2 + 4

/**
 * @brief Call the health-monitor routine
 */
1:

    /* restore the WIM and PSR */
    mov     %g2, %wim
    mov     %g1, %psr
    nop; nop; nop;                              ! WRPSR, WRWIM delay slots

    /* clear MMU no-flaut bit */
    andn    %g6, 0x2, %g6
    sta     %g6, [%g0] ASI_LEON_MMUREGS         ! clear no flaut bit
    mov     AC_M_SFAR, %g6
    lda     [%g6] ASI_LEON_MMUREGS, %g0
    mov     AC_M_SFSR, %g6
    lda     [%g6] ASI_LEON_MMUREGS, %g6         ! get the flaut information

    /* go to health-monitor */
    sethi   %hi(sparc_isr_handler_entry), %l4
    or      %l4, %lo(sparc_isr_handler_entry), %l4      ! get point to the ISR handler

    or      %g0, 0x2B, %l3                      ! set as data-store error

    /* restore globals */
    mov %l6, %g6
    mov %l7, %g7

    /* return from trap */
    jmp     %l4                                 ! jump to health-monitor
    jmp     %l4 + 4
