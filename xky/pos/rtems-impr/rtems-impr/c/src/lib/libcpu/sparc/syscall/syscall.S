/**
 *  @file
 *  syscall.S
 *
 *  @brief contains emulated system calls using software trap 0.
 *
 *  The following calls are supported:
 *
 *    + syscall
 *
 *  Also contains the enable and disable interrupt functions
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1995. European Space Agency.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  8702    | 18/08/2010  | mcoutinho    | IPR 451
 *  9872    | 16/03/2011  | aconstantino | SPR 2819
 *  9877    | 18/03/2011  | aconstantino | SPR 2846
 *  $Rev: 9929 $ | $Date: 2011-03-23 12:02:26 +0000 (Wed, 23 Mar 2011) $| $Author: sfaustino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SPARC_CPUS SPARC CPUs
 *  @{
 */

#include <libcpu/syscall.h>
#include <rtems/asm.h>

/**
 * \cond
 */

        .seg    "text"

        /**
         *  system call
         *
         *  On entry:
         *
         *    l0 = psr (from trap table)
         *    l1 = pc
         *    l2 = npc
         *    g1 = system call id
         **/
        PUBLIC(syscall)

SYM(syscall):

  subcc     %g1, SYS_irqdis, %g0          ! check for syscall 2 (disable interrupts)
  bne       3f                            ! if not, continue checking
  or        %l0, 0x0f00, %l4              ! set PIL=15
  mov       %l4, %psr                     ! move the register with PIL=15 to PSR
  or        %l0, SPARC_PSR_ET_MASK, %i0   ! return old psr with ET=1
  ba,a      9f                            ! jump to leave
3:
    subcc     %g1, SYS_irqen, %g0           ! check for syscall 3 (disable interrupts)
    bne       pil_level                     ! if not, jump to halt the CPU
    and       %i0, SPARC_PSR_PIL_MASK, %l4  ! set the level (argument) bits to l4
    andn      %l0, SPARC_PSR_PIL_MASK, %l5  ! set the PSR[PIL] bits to l5
    or        %l5, %l4, %l4                 ! enable the interrupts set on the level
    mov       %l4, %psr                     ! and move it to the PSR register
    ba,a      9f                            ! jump to leave

pil_level:
	subcc	%g1,SYS_getintlevel,%g0
	bne 	1f
	rd	%psr, %i0
	and	%i0,SPARC_PSR_PIL_MASK,%i0
	srl	%i0,SPARC_PSR_PIL_BIT_POSITION,%i0
	ba,a	9f    
1:
    ta        0                             ! halt
9:                                          ! leave
    mov       0, %g1                        ! clear %g1
    jmpl      %l2, %g0                      ! return
    rett      %l2 + 4

    PUBLIC(sparc_disable_interrupts)
SYM(sparc_disable_interrupts):

    mov       SYS_irqdis, %g1               ! move the interrupt disable value to g1
    ta        0                             ! generate software trap that disables interrupts
    retl                                    ! return
    nop                                     ! delay slot

    PUBLIC(sparc_enable_interrupts)
SYM(sparc_enable_interrupts):

    mov       SYS_irqen, %g1                ! move the interrupt enable value to g1
    ta        0                             ! generate software trap that enables interrupts
    retl                                    ! return
    nop                                     ! delay slot

/**
 * @brief GMV PMK - Get the processor interrupt level
 */
        PUBLIC(sparc_get_int_level)
SYM(sparc_get_int_level):

	mov	SYS_getintlevel, %g1
	ta	0
	retl
	nop	

/**
 * @brief GMV PMK - Get the processor status register (PSR) content
 */
	
	PUBLIC(sparc_get_psr)
SYM(sparc_get_psr):
	ta	0x7e
	nop 
	andn	%o0,0x80,%o0
	add	%o0,1,%l7
	and	%l7,SPARC_PSR_CWP_MASK,%l7
	andn	%o0,SPARC_PSR_CWP_MASK,%o0
	retl
	or	%o0,%l7,%o0

/**
 * @brief GMV PMK - Get the trap base register (TBR) content
 */	
	PUBLIC(sparc_get_tbr)
SYM(sparc_get_tbr):
	ta	0x7d
	nop 
	retl
	nop	
	
    
/* end of file */

/**
 * \endcond
 */

/**
 *  @}
 */
