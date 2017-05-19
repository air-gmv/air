/**
 *  @file
 *  sysstate.inl
 *
 *  @brief contains the inline implementation of routines regarding the
 *  system state.
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  609     | 17/11/2008  | mcoutinho    | IPR 64
 *  3614    | 02/07/2009  | mcoutinho    | IPR 153
 *  4546    | 28/09/2009  | mcoutinho    | IPR 778
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
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

#ifndef _RTEMS_SCORE_SYSSTATE_INL
#define _RTEMS_SCORE_SYSSTATE_INL

#include <rtems/system.h>


/**
 *  @brief initialize the System State Handler
 *  
 *  This routine initializes the system state handler.
 *
 *  @param[in] is_multiprocessing determine if the system is multiprocessing
 */
RTEMS_INLINE_ROUTINE void _System_state_Handler_initialization(
#if defined(RTEMS_MULTIPROCESSING)
    boolean is_multiprocessing
#endif
    )
{
   /* set the system state to before initialization */
   _System_state_Current = SYSTEM_STATE_BEFORE_INITIALIZATION;

   /* if has multiprocessing */
#if defined(RTEMS_MULTIPROCESSING)

   /* then set the system multiprocessing state */
   _System_state_Is_multiprocessing = is_multiprocessing;
#endif
}


/**
 *  @brief set the system state
 *  
 *  This routine sets the current system state to that specified by
 *  the called.
 *
 *  @param[in] state the system state to set
 */
RTEMS_INLINE_ROUTINE void _System_state_Set(
                                            System_state_Codes state
                                            )
{
   /* set the system state */
   _System_state_Current = state;
}


/**
 *  @brief get the system state
 *  
 *  This function returns the current system state.
 *
 *  @return returns the current system state
 */
RTEMS_INLINE_ROUTINE System_state_Codes _System_state_Get(void)
{
   /* return the system state */
   return _System_state_Current;
}


/**
 *  @brief determine if the system state is up
 *  
 *  This function returns TRUE if the state is equal to the
 *  "up" state, and FALSE otherwise.
 *
 *  @param[in] state the state to check
 *
 *  @return returns TRUE if the system state is "UP" and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _System_state_Is_up(
                                                 System_state_Codes state
                                                 )
{
   /* system is up if is equal to SYSTEM_STATE_UP */
   return (state == SYSTEM_STATE_UP );
}

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */
