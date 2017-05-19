/**
 *  @file
 *  start.S
 *
 *  @brief Common start code for SPARC.
 *
 *  This is based on the file srt0.s provided with the binary
 *  distribution of the SPARC Instruction Simulator (SIS) found
 *  at ftp://ftp.estec.esa.nl/pub/ws/wsd/erc32.
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  3511    | 19/06/2009  | mcoutinho    | IPR 42
 *  4865    | 19/10/2009  | mcoutinho    | IPR 814
 *  5167    | 29/10/2009  | mcoutinho    | IPR 828
 *  6521    | 09/03/2010  | mcoutinho    | IPR 866
 *  6524    | 09/03/2010  | mcoutinho    | IPR 1944
 *  8702    | 18/08/2010  | mcoutinho    | IPR 451
 *  9872    | 16/03/2011  | aconstantino | SPR 2819
 *  $Rev: 9877 $ | $Date: 2011-03-18 18:39:36 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2846
 *
 **/

/**
 *  @addtogroup SHARED_BETWEEN_SPARC_BSPS Shared between SPARC BSPs
 *  @{
 */

#include <rtems/asm.h>
#include <libcpu/syscall.h>

/**
 * \cond
 */

  .seg    "text"
  PUBLIC(start)
  .global start, __bsp_mem_init

SYM(start):
start:
    .global SYM(_ERC32_MEC_Timer_Control_Mirror), SYM(CLOCK_SPEED)
    .global SYM(Configuration)

SYM(_ERC32_MEC_Timer_Control_Mirror):

  /**
   *  This is the hard reset code.
   **/
        PUBLIC(hard_reset)
SYM(hard_reset):

  set     (SYM(rdb_start)), %g6               ! End of work-space area
  st      %sp, [%g6]                          ! save sp to rdb_start

  /**
   * clear the bss
   **/
1:

  sethi   %hi(_edata),%g2
  or      %g2,%lo(_edata),%g2                 ! g2 = start of bss
  sethi   %hi(_end),%g3
  or      %g3,%lo(_end),%g3                   ! g3 = end of bss
  mov     %g0,%g1                             ! so std has two zeros

zerobss:
  std     %g0,[%g2]
  add     %g2,8,%g2
  cmp     %g2,%g3
  bleu,a  zerobss
  nop

  mov     %0, %o2                             ! environ
  mov     %0, %o1                             ! argv
  mov     %0, %o0                             ! argc
  call    SYM(boot_card)
  sub     %sp, 0x60, %sp                      ! room for boot_card to save args

  /**
   * Set partition to idle
   **/
  PUBLIC(bsp_cleanup)
SYM(bsp_cleanup):
  set     XKY_SYSCALL_SET_PARTITION_MODE, %o5 ! set partition mode trap id
  set     XKY_MODE_IDLE, %o1                  ! mode idle
  set     0xFFFFFFFF, %o0                     ! current partition
  ta      XKY_SYSCALL_OS_TRAP                 ! call XKY OS trap

  .global SYM(rdb_start)                      ! place to store the end of systen                             ! memory
SYM(rdb_start):
  nop
/* end of file */

/**
 * \endcond
 */

/**
 *  @}
 */

