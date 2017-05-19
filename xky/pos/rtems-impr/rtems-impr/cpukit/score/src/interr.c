/**
 *  @file
 *  interr.c
 *
 *  @brief announce that an internal error occurred
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
 *  3693    | 08/07/2009  | mcoutinho    | IPR 153
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6325    | 01/03/2010  | mcoutinho    | IPR 1931
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreIntErr Internal Error Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/interr.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/userext.h>
#include <rtems.h>
#include <rtems/rtems/intr.h>


Internal_errors_Information Internal_errors_What_happened;


void _Internal_error_Occurred(
                              Internal_errors_Source the_source ,
                              boolean is_internal ,
                              uint32_t the_error
                              )
{
    /* interrupt level */
   rtems_interrupt_level level;

   /* old system state */
   System_state_Codes old_state;


   /* initialize the internal error fields */

   /* initialize the source field */
   Internal_errors_What_happened.the_source = the_source;

   /* initialize the internal field */
   Internal_errors_What_happened.is_internal = is_internal;

   /* initialize the error field */
   Internal_errors_What_happened.the_error = the_error;

   /* save the old state */
   old_state = _System_state_Get();

   /* call the user fatal extension only if the user extension manager
    * has been initialized */

   /* check if the system is UP */
   if(_System_state_Is_up(old_state))
   {
      /* call user fatal extensions */
      _User_extensions_Fatal(the_source , is_internal , the_error);
   }

   /* enter critical section  */
   rtems_interrupt_disable(level);

   /* if the user fatal extension could not handle the error
    * set the system to the safe state mode */
   _System_state_Set(SYSTEM_STATE_APP_SAFE);

   /* call the application safe state handler
    * call the handler until it decides to restart the system,
    * delete the calling thread, correct the error, etc */
   while(!app_safe_state_handler(the_source , is_internal , the_error));

   /* restore the old state */
   _System_state_Set(old_state);

   /* leave critical section */
   rtems_interrupt_enable(level);

}

/**  
 *  @}
 */

/**
 *  @}
 */
