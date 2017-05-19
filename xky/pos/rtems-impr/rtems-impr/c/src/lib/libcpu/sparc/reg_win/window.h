/**
 *  @file
 *  window.h
 *
 *  @brief SPARC window management
 *
 *  COPYRIGHT (c) 2010.
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  5501    | 10/11/2009  | mcoutinho    | IPR 866
 *  8183    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9877 $ | $Date: 2011-03-18 18:39:36 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SPARC_CPUS SPARC CPUs
 *  @{
 */

#ifndef __SPARC_WINDOW_h
#define __SPARC_WINDOW_h

/**
 * @brief window overflow trap handler
 * This function corresponds to the ISR of the window overflow trap
 *
 *  Calculate new WIM by "rotating" the valid bits in the WIM right
 *  by one position.  The following shows how the bits move for a SPARC
 *  cpu implementation where SPARC_NUMBER_OF_REGISTER_WINDOWS is 8.
 *
 *    OLD WIM = 76543210\n
 *    NEW WIM = 07654321\n
 *
 *  Inputs:
 *    CPU register %l0 contains the psr\n
 *    CPU register %l1 contains the pc\n
 *    CPU register %l2 contains the npc\n
 *
 **/
void window_overflow_trap_handler(void);


/**
 * @brief window underflow trap handler
 * This function corresponds to the ISR of the window underflow trap
 *
 *  Calculate new WIM by "rotating" the valid bits in the WIM left
 *  by one position.  The following shows how the bits move for a SPARC
 *  cpu implementation where SPARC_NUMBER_OF_REGISTER_WINDOWS is 8.
 *
 *    OLD WIM = 76543210\n
 *    NEW WIM = 07654321\n
 *
 *  Inputs:
 *    CPU register %l0 contains the psr\n
 *    CPU register %l1 contains the pc\n
 *    CPU register %l2 contains the npc\n
 *
 **/
void window_underflow_trap_handler(void);

#endif
/* end of include file */

/**
 *  @}
 */
