/**
 *  @file
 *  sysstate.c
 *
 *  @brief Handle the system state. Contains a global variable with the
 *  system state
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 2008
 *  EDISOFT - Empresa de Serviços e Desenvolvimento de Software, S.A.
 *
 *  http://rtemscentre.edisoft.pt
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE at:
 *  http://rtemscentre.edisoft.pt/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  3616    | 02/07/2009  | mcoutinho    | IPR 153
 *  7329    | 14/04/2010  | mcoutinho    | IPR 64
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9877 $ | $Date: 2011-03-18 18:39:36 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreSysState System State Handler
 *  @{
 */

#include <rtems/score/sysstate.h>

#ifdef RTEMS_MULTIPROCESSING

/* by default, the system is not multiprocessing */
boolean _System_state_Is_multiprocessing = FALSE;
#endif

/* initialize the system state to before initialization */
System_state_Codes _System_state_Current = SYSTEM_STATE_BEFORE_INITIALIZATION;

/**
 * @}
 */

/**
 *  @}
 */
