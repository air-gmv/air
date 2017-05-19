/**
 *  @file
 *  exshutdown.c
 *
 *  @brief shutdown RTEMS and the system
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  234     | 09/10/2008  | mcoutinho    | IPR 67
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6116    | 18/02/2010  | mcoutinho    | IPR 1818
 *  7129    | 09/04/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_API Super API
 *  @{
 */

/**
 *  @addtogroup SUPER_API_INITIALIZATION Initialization Manager
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/thread.h>


void rtems_shutdown_executive(
                              uint32_t result
                              )
{
    /* check the system state */
    if(_System_state_Current != SYSTEM_STATE_SHUTDOWN)
    {
        /* if it is not shutdown then set the system state to shutdown */
        _System_state_Set(SYSTEM_STATE_SHUTDOWN);

        /* and stop multitasking */
        _Thread_Stop_multitasking();

        /* program will not return from here */
    }
}

/**  
 *  @}
 */

/**  
 *  @}
 */

