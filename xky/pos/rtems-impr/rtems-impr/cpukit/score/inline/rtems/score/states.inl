/**
 *  @file
 *  states.inl
 *
 *  @brief contains the macro implementation of the inlined
 *  routines associated with thread state information.
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
 *  @addtogroup ScoreStates Thread States Handler
 *  @{
 */

#ifndef _RTEMS_SCORE_STATES_INL
#define _RTEMS_SCORE_STATES_INL

#include <rtems/system.h>


/**
 *  @brief set a state
 *  
 *  This function sets the given states_to_set into the current_state
 *  passed in.  The result is returned to the user in current_state.
 *
 *  @param[in] states_to_set the specific state to set
 *  @param[in] current_state the state to change
 *
 *  @return returns the new state set
 */
RTEMS_INLINE_ROUTINE States_Control _States_Set(
                                                States_Control states_to_set ,
                                                States_Control current_state
                                                )
{
   /* return the current state with the new state */
   return (current_state | states_to_set );
}


/**
 *  @brief clear a state
 *  
 *  This function clears the given states_to_clear into the current_state
 *  passed in.  The result is returned to the user in current_state.
 *
 *  @param[in] states_to_clear the specific state(s) to clear
 *  @param[in] current_state the state state to change
 *
 *  @return returns the new state cleared
 */
RTEMS_INLINE_ROUTINE States_Control _States_Clear(
                                                  States_Control states_to_clear ,
                                                  States_Control current_state
                                                  )
{
   /* return the current state without the specified state */
   return (current_state & ~states_to_clear );
}


/**
 *  @brief determine if the state is ready
 *  
 *  This function returns TRUE if the_states indicates that the
 *  state is READY, and FALSE otherwise.
 *
 *  @param[in] the_states the state to check
 *
 *  @return returns TRUE if the state is ready and FALSE otherwise
 */
RTEMS_INLINE_ROUTINE boolean _States_Is_ready(
                                              States_Control the_states
                                              )
{
   /* state is ready if is equal to STATES_READY */
   return (the_states == STATES_READY );
}


/**
 *  @brief determine if the state is dormant
 *
 *  @param[in] the_states the state to check
 *
 *  @return returns different than 0 if the state is dormant
 */
RTEMS_INLINE_ROUTINE boolean _States_Is_dormant(
                                                States_Control the_states
                                                )
{
   /* state is dormant if has the STATES_DORMANT bit set */
   return (the_states & STATES_DORMANT );
}


/**
 *  @brief determine if the state is suspended
 *
 *  @param[in] the_states the state to check
 *
 *  @return returns different than 0 is the state is suspended
 */
RTEMS_INLINE_ROUTINE boolean _States_Is_suspended(
                                                  States_Control the_states
                                                  )
{
   /* state is suspended if has the STATES_SUSPENDED bit set */
   return (the_states & STATES_SUSPENDED );
}


/**
 *  @brief determine if the state is waiting for event
 *
 *  @param[in] the_states the state to check
 *
 *  @return returns different than 0 if is state is waiting for event
 */
RTEMS_INLINE_ROUTINE boolean _States_Is_waiting_for_event(
                                                          States_Control the_states
                                                          )
{
   /* state is waiting for event if has the STATES_WAITING_FOR_EVENT bit set */
   return (the_states & STATES_WAITING_FOR_EVENT );
}

#if defined(RTEMS_MULTIPROCESSING)


/**
 *  @brief determine if the state is waiting for RPC reply
 *
 *  @param[in] the_states the state to check
 *
 *  @return returns different than 0 if the state is RPC reply
 */
RTEMS_INLINE_ROUTINE boolean _States_Is_waiting_for_rpc_reply(
                                                              States_Control the_states
                                                              )
{
   /* state is waiting for RPC reply if has the STATES_WAITING_FOR_RPC_REPLY bit set */
   return (the_states & STATES_WAITING_FOR_RPC_REPLY );
}
#endif


/**
 *  @brief determine if the state is waiting for period
 *
 *  @param[in] the_states the state to check
 *
 *  @return returns different than 0 if the state is waiting for period
 */
RTEMS_INLINE_ROUTINE boolean _States_Is_waiting_for_period(
                                                           States_Control the_states
                                                           )
{
   /* state is waiting for period if has the STATES_WAITING_FOR_PERIOD bit set */
   return (the_states & STATES_WAITING_FOR_PERIOD );
}


/**
 *  @brief determine if the state is wainting on thread queue
 *
 *  @param[in] the_states the state to check
 *
 *  @return returns different than 0 if the state is waiting on thread queue
 */
RTEMS_INLINE_ROUTINE boolean _States_Is_waiting_on_thread_queue(
                                                                States_Control the_states
                                                                )
{
   /* state is waiting on thread queue if has the STATES_WAITING_ON_THREAD_QUEUE
    * bit set */
   return (the_states & STATES_WAITING_ON_THREAD_QUEUE );
}


/**
 *  @brief determine if a set of states are set
 *
 *  @param[in] the_states the states to check if they are set
 *  @param[in] mask the mask
 *
 *  @return returns different than 0 if the states on the mask are set and FALSE otherwise
 */
RTEMS_INLINE_ROUTINE boolean _States_Are_set(
                                             States_Control the_states ,
                                             States_Control mask
                                             )
{
   /* check if the any of the states is set */
   return ( ( the_states & mask ) != STATES_READY );
}

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */
