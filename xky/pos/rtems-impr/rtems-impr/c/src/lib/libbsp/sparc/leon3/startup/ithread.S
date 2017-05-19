/**
 *  @file
 *  ithread.S
 *
 *  @brief Idle Thread Body
 *
 *  This routine puts LEON3 in power-down mode.
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  4413    | 21/09/2009  | mcoutinho    | IPR 607
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  8698    | 18/08/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SPARC_LEON3_BSP SPARC LEON3 BSP
 *  @{
 */

#include <rtems/asm.h>
#include <rtems/score/cpu.h>

#if (CPU_PROVIDES_IDLE_THREAD_BODY == TRUE)

/**
 * \cond
 */

/* LEON specific power-down function */

        .align 4
        PUBLIC(_CPU_Thread_Idle_body)
SYM(_CPU_Thread_Idle_body):

pwdloop:               ! start of the cycle
  mov     %g0, %asr19  ! move 0 to the ASR 19 register
  ba      pwdloop      ! and jump to the start of the function again
  nop                  ! delay slot
  retl                 ! return of the function (should never reach here)
  nop                  ! delay slot

/**
 * \endcond
 */

#endif

/**
 *  @}
 */

