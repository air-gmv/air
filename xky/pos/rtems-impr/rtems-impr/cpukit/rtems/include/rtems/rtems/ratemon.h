/**
 *  @file
 *  ratemon.h
 *
 *  @brief contains all the constants, structures, and
 *  prototypes associated with the Rate Monotonic Manager.  This manager
 *  provides facilities to implement tasks which execute in a periodic fashion.
 *
 *  Directives provided are:
 *\n
 *     + create a rate monotonic timer\n
 *     + cancel a period\n
 *     + delete a rate monotonic timer\n
 *     + conclude current and start the next period\n
 *     + obtain status information on a period\n
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  560     | 17/11/2008  | mcoutinho    | IPR 70
 *  2479    | 15/04/2009  | mcoutinho    | IPR 452
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6116    | 18/02/2010  | mcoutinho    | IPR 1818
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @defgroup RTEMS_API_RATEMON Rate Monotonic Manager
 *
 *  @brief The Rate Monotonic Manager allows the user to establish periodic threads.
 *  @{
 */

#ifndef _RTEMS_RTEMS_RATEMON_H
#define _RTEMS_RTEMS_RATEMON_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <string.h>

   /**
    *  @brief define the states in which a
    *  period may be.
    */
   typedef enum
   {
      /**
       * @brief  off chain, never initialized
       **/
      RATE_MONOTONIC_INACTIVE ,

      /**
       * @brief  on chain, owner is blocking on it
       **/
      RATE_MONOTONIC_OWNER_IS_BLOCKING ,

      /**
       * @brief  on chain, running continuously
       **/
      RATE_MONOTONIC_ACTIVE ,

      /**
       * @brief on chain, expired while owner was blocking on it
       **/
      RATE_MONOTONIC_EXPIRED_WHILE_BLOCKING ,

      /**
       * @brief  off chain, will be reset by next rtems_rate_monotonic_period
       **/
      RATE_MONOTONIC_EXPIRED
   } rtems_rate_monotonic_period_states;

   /**
    *  @brief interval passed to the rate_monontonic_period
    *  directive to obtain status information.
    */
#define RTEMS_PERIOD_STATUS       WATCHDOG_NO_TIMEOUT

   /**
    *  @brief define the period status structure.
    */
   typedef struct
   {
      Objects_Id owner;
      rtems_rate_monotonic_period_states state;
   } rtems_rate_monotonic_period_status;

   /**
    *  @brief define the control block used to manage
    *  each period.
    */
   typedef struct
   {
      Objects_Control Object;
      Watchdog_Control Timer;
      rtems_rate_monotonic_period_states state;
      uint32_t next_length;
      Thread_Control *owner;
   } Rate_monotonic_Control;

   /**
    * @brief rate monotonic manager object information
    **/
   extern Objects_Information _Rate_monotonic_Information;

   /**
    *  @brief initialize  Rate Monotonic Manager
    *
    *  @param[in] maximum_periods number of periods timers to initialize
    *
    *  @note The Rate Monotonic Manager is built on top of the Watchdog
    *  Handler.
    */
   void _Rate_monotonic_Manager_initialization(
                                               uint32_t maximum_periods
                                               );

   /**
    *  @brief create a rate monotonic
    *
    *  This directive creates a rate monotonic timer and performs
    *  some initialization.
    *
    *  @param[in] name name of period
    *  @param[out] id pointer to rate monotonic id
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_rate_monotonic_create(
                                                 rtems_name name ,
                                                 Objects_Id *id
                                                 );

   /**
    *  @brief ident a rate monotonic
    *
    *  This directive returns the system ID associated with
    *  the rate monotonic period name.
    *
    *  @param[in] name user defined period name
    *  @param[out] id pointer to period id
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_rate_monotonic_ident(
                                                rtems_name name ,
                                                Objects_Id *id
                                                );

   /**
    *  @brief cancel a rate monotonic
    *
    *  This routine implements the rtems_rate_monotonic_cancel directive.  This
    *  directive stops the period associated with ID from continuing to
    *  run.
    *
    *  @param[in] id the rate monotonic id
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_rate_monotonic_cancel(
                                                 Objects_Id id
                                                 );

   /**
    *  @brief delete a rate monotonic
    *
    *  This directive allows a thread to delete a rate monotonic timer.
    *
    *  @param[in] id rate monotonic id
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_rate_monotonic_delete(
                                                 Objects_Id id
                                                 );

   /**
    *  @brief get status of a rate monotonic
    *
    *  This directive allows a thread to obtain status information on a
    *  period.
    *
    *  @param[in] id rate monotonic id
    *  @param[out] status pointer to status control block
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_rate_monotonic_get_status(
                                                     Objects_Id id ,
                                                     rtems_rate_monotonic_period_status *status
                                                     );

   /**
    *  @brief periodicalize a rate monotonic
    *
    *  This routine implements the rtems_rate_monotonic_period directive.  When
    *  length is non-zero, this directive initiates the period associated with
    *  ID from continuing for a period of length.  If length is zero, then
    *  result is set to indicate the current state of the period.
    *
    *  @param[in] id rate monotonic id
    *  @param[in] length the time internal to wait
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_rate_monotonic_period(
                                                 Objects_Id id ,
                                                 rtems_interval length
                                                 );

   /**
    *  @brief announce that a timeout has occurred to a rate monotonic
    *
    *  This routine is invoked when the period represented
    *  by ID expires.  If the task which owns this period is blocked
    *  waiting for the period to expire, then it is readied and the
    *  period is restarted.  If the owning task is not waiting for the
    *  period to expire, then the period is placed in the EXPIRED
    *  state and not restarted.
    *
    *  @param[in] id the id of the thread to unblock
    *  @param[in] ignored ignored
    */
   void _Rate_monotonic_Timeout(
                                Objects_Id id ,
                                void *ignored
                                );


#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/ratemon.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

/**  
 *  @}
 */

/**
 *  @}
 */
