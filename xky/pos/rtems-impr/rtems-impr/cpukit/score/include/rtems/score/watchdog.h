/**
 *  @file
 *  watchdog.h
 *
 *  @brief contains all the constants and structures associated
 *  with watchdog timers.   This Handler provides mechanisms which can be
 *  used to initialize and manipulate watchdog timers.
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
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

#ifndef _RTEMS_SCORE_WATCHDOG_H
#define _RTEMS_SCORE_WATCHDOG_H

/**
 *  @defgroup ScoreWatchdog Watchdog Handler
 *
 *  @brief This handler encapsulates functionality related to the scheduling of
 *  watchdog functions to be called at specific times in the future.
 *
 *  @note This handler does not have anything to do with hardware watchdog
 *        timers.
 */
/**@{*/

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/score/object.h>

   /**
    *  @brief Maximum Interval Length
    *
    *  The following type defines the control block used to manage
    *  intervals.
    */
#define WATCHDOG_MAXIMUM_INTERVAL ((Watchdog_Interval) 0xffffffff)

   /**
    *  @brief Watchdog Interval Type
    *
    *  This type is used to specify the length of intervals.
    */
   typedef uint32_t Watchdog_Interval;

   /**
    *  @brief Watchdog Nanoseconds Since Last Tick Extension
    *
    *  This type defines a pointer to the BSP plugin to obtain the number
    *  of nanoseconds since the last clock tick.
    */
   typedef uint32_t(*Watchdog_Nanoseconds_since_last_tick_routine)(void);

   /**
    *  @brief Watchdog Service Routine Return Type
    *
    *  This type defines the return type from a Watchdog Service Routine.
    */
   typedef void Watchdog_Service_routine;

   /**
    *  @brief Watchdog Service Routine Pointer Type
    *
    *  This type define a pointer to a watchdog service routine.
    */
   typedef Watchdog_Service_routine(*Watchdog_Service_routine_entry)(
                                                                      Objects_Id ,
                                                                      void *
                                                                      );

   /**
    *  @brief No timeout constant
    *
    *  This is the constant for indefinite wait.  It is actually an
    *  illegal interval.
    */
#define WATCHDOG_NO_TIMEOUT  0

   /**
    *  @brief Watchdog States Type
    *
    *  This enumerated type is the set of the states in which a
    *  watchdog timer may be at any given time.
    */
   typedef enum
   {
      /**
       * @brief This is the state when the watchdog is off all chains
       */
      WATCHDOG_INACTIVE ,

      /**
       * @brief This is the state when the watchdog is off all chains, but we are
       * currently searching for the insertion point.
       */
      WATCHDOG_BEING_INSERTED ,

      /**
       * @brief This is the state when the watchdog is on a chain, and allowed to fire.
       */
      WATCHDOG_ACTIVE ,

      /**
       * @brief This is the state when the watchdog is on a chain, but we should
       * remove without firing if it expires.
       */
      WATCHDOG_REMOVE_IT
   } Watchdog_States;

   /**
    *  @brief Watchdog Adjustment Directions Type
    *
    *  The following enumerated type details the manner in which
    *  a watchdog chain may be adjusted by the @ref _Watchdog_Adjust
    *  routine.  The direction indicates a movement FORWARD
    *  or BACKWARD in time.
    */
   typedef enum
   {
      /**
       * @brief adjust delta value forward
       */
      WATCHDOG_FORWARD ,

      /**
       * @brief adjust delta value backward
       */
      WATCHDOG_BACKWARD
   } Watchdog_Adjust_directions;

   /**
    *  @brief Watchdog Control Structure
    *
    *  defines the control block used to manage each watchdog timer.
    */
   typedef struct
   {
      /**
       * @brief This field is a Chain Node structure and allows this to be placed on
       * chains for set management.
       */
      Chain_Node Node;

      /**
       * @brief This field is the state of the watchdog.
       */
      Watchdog_States state;

      /**
       * @brief This field is the initially requested interval.
       */
      Watchdog_Interval initial;

      /**
       * @brief This field is the remaining portion of the interval.
       */
      Watchdog_Interval delta_interval;

      /**
       * @brief This field is the number of system clock ticks when this was scheduled.
       */
      Watchdog_Interval start_time;

      /**
       * @brief This field is the number of system clock ticks when this was suspended.
       */
      Watchdog_Interval stop_time;

      /**
       * @brief This field is the function to invoke.
       */
      Watchdog_Service_routine_entry routine;

      /**
       * @brief This field is the Id to pass as an argument to the routine.
       */
      Objects_Id id;

      /**
       * @brief This field is an untyped pointer to user data that is passed to the
       *  watchdog handler routine.
       */
      void *user_data;
   } Watchdog_Control;

   /**
    *  @brief Watchdog Synchronization Level
    *
    *  This used for synchronization purposes
    *  during an insert on a watchdog delta chain.
    */
   extern volatile uint32_t _Watchdog_Sync_level;

   /**
    *  @brief Watchdog Synchronization Count
    *
    *  This used for synchronization purposes
    *  during an insert on a watchdog delta chain.
    */
   extern volatile uint32_t _Watchdog_Sync_count;

   /**
    *  @brief number of clock Ticks Since System Boot
    *
    *  This contains the number of ticks since the system was booted.
    */
   extern volatile Watchdog_Interval _Watchdog_Ticks_since_boot;

   /**
    *  @brief Watchdog Nanoseconds Since Last Tick Handler
    *
    *  This is a pointer to the optional BSP plugin to obtain the number
    *  of nanoseconds since the last clock tick.
    */
   extern Watchdog_Nanoseconds_since_last_tick_routine
   _Watchdog_Nanoseconds_since_tick_handler;

   /**
    *  @brief Per Ticks Watchdog List
    *
    *  This is the watchdog chain which is managed at ticks.
    */
   extern Chain_Control _Watchdog_Ticks_chain;

   /**
    *  @brief Per Seconds Watchdog List
    *
    *  This is the watchdog chain which is managed at second boundaries.
    */
   extern Chain_Control _Watchdog_Seconds_chain;

   /**
    *  @brief initialize the Watchdog Handler
    *
    *  This routine initializes the watchdog handler.  The watchdog
    *  synchronization flag is initialized and the watchdog chains are
    *  initialized and emptied.
    */
   void _Watchdog_Handler_initialization(void);

   /**
    *  @brief remove Watchdog from the watchdog chain
    *
    *  This routine removes @a the_watchdog from the watchdog chain on which
    *  it resides and returns the state @a the_watchdog timer was in.
    *
    *  @param[in] the_watchdog will be removed
    *
    *  @return the state in which @a the_watchdog was in when removed
    */
   Watchdog_States _Watchdog_Remove(
                                    Watchdog_Control *the_watchdog
                                    );

   /**
    *  @brief adjust the watchdog chain by a number of units forward or backwards
    *
    *  This routine adjusts the delta chain backward or forward in response
    *  to a time change.
    *
    *  @param[in] header pointer to the delta chain to be adjusted
    *  @param[in] direction forward or backward adjustment to delta chain
    *  @param[in] units units to adjust
    */
   void _Watchdog_Adjust(
                         Chain_Control *header ,
                         Watchdog_Adjust_directions direction ,
                         Watchdog_Interval units
                         );

   /**
    *  @brief insert a Watchdog on a watchdog chain
    *
    *  This routine inserts @a the_watchdog into the @a header watchdog chain
    *  for a time of @a units.
    *
    *  @param[in] header is @a the_watchdog list to insert @a the_watchdog on
    *  @param[in] the_watchdog is the watchdog to insert
    */
   void _Watchdog_Insert(
                         Chain_Control *header ,
                         Watchdog_Control *the_watchdog
                         );

   /**
    *  @brief announce the passage of a time unit to the Watchdog Handler
    *
    *  This routine decrements the delta counter in response to a tick.  The
    *  delta chain is updated accordingly.
    *
    *  @param[in] header pointer to the delta chain to be tickled
    */
   void _Watchdog_Tickle(
                         Chain_Control *header
                         );

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/watchdog.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */
