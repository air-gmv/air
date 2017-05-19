/**
 *  @file
 *  interr.h
 *
 *  @brief contains constants and prototypes related
 *  to the Internal Error Handler.
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
 *  602     | 17/11/2008  | mcoutinho    | IPR 65
 *  3509    | 19/06/2009  | mcoutinho    | IPR 528
 *  3706    | 09/07/2009  | jsousa       | IPR 153
 *  4406    | 21/09/2009  | mcoutinho    | IPR 605
 *  4443    | 21/09/2009  | mcoutinho    | IPR 768
 *  4944    | 23/10/2009  | mcoutinho    | IPR 819
 *  5099    | 28/10/2009  | mcoutinho    | IPR 553
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

#ifndef _RTEMS_SCORE_INTERR_H
#define _RTEMS_SCORE_INTERR_H

/**
 *  @defgroup ScoreIntErr Internal Error Handler
 *
 *   @brief This handler encapsulates functionality which provides the foundation
 *  Semaphore services used in all of the APIs supported by RTEMS.
 */
/**@{*/

#include <rtems/score/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

   /**
    *  @brief lists the possible sources from which an error
    *  can be reported.
    */
   typedef enum
   {
      INTERNAL_ERROR_CORE ,
      INTERNAL_ERROR_RTEMS_API ,
      INTERNAL_ERROR_INTERRUPTION_NOT_HANDLED
   } Internal_errors_Source;

   /**
    *  @brief list of errors which are generated internally by the executive core.
    */
   typedef enum
   {
      INTERNAL_ERROR_NO_CONFIGURATION_TABLE = 0 ,
      INTERNAL_ERROR_NO_CPU_TABLE = 1 ,
      INTERNAL_ERROR_INVALID_WORKSPACE_ADDRESS = 2 ,
      INTERNAL_ERROR_TOO_LITTLE_WORKSPACE = 3 ,
      INTERNAL_ERROR_WORKSPACE_ALLOCATION = 4 ,
      INTERNAL_ERROR_INTERRUPT_STACK_TOO_SMALL = 5 ,
      INTERNAL_ERROR_THREAD_EXITTED = 6 ,
      INTERNAL_ERROR_INCONSISTENT_MP_INFORMATION = 7 ,
      INTERNAL_ERROR_INVALID_NODE = 8 ,
      INTERNAL_ERROR_NO_MPCI = 9 ,
      INTERNAL_ERROR_BAD_PACKET = 10 ,
      INTERNAL_ERROR_OUT_OF_PACKETS = 11 ,
      INTERNAL_ERROR_OUT_OF_GLOBAL_OBJECTS = 12 ,
      INTERNAL_ERROR_OUT_OF_PROXIES = 13 ,
      INTERNAL_ERROR_INVALID_GLOBAL_ID = 14 ,
      INTERNAL_ERROR_BAD_STACK_HOOK = 15 ,
      INTERNAL_ERROR_BAD_ATTRIBUTES = 16 ,
      INTERNAL_ERROR_INVALID_MESSAGE_SUBMIT_TYPE = 17 ,
      INTERNAL_ERROR_TOO_MANY_INITIAL_EXTENSIONS = 18 ,
      INTERNAL_ERROR_INVALID_MICROSECONDS_PER_TICK = 19 ,
      INTERNAL_ERROR_INVALID_TICKS_PER_TIMESLICE = 20 ,
      INTERNAL_ERROR_TOO_MANY_DEVICE_DRIVERS = 21 ,
      INTERNAL_ERROR_NO_DEVICE_DRIVER_TABLE = 22 ,
      INTERNAL_ERROR_NO_USER_EXTENSION_TABLE = 23 ,
      INTERNAL_ERROR_NO_API_CONFIGURATION_TABLE = 24 ,
      INTERNAL_ERROR_INVALID_NUMBER_TASKS = 25 ,
      INTERNAL_ERROR_INVALID_NUMBER_INITIALIZATION_TASKS = 26 ,
      INTERNAL_ERROR_NO_USER_INITIALIZATION_TASKS_TABLE = 27 ,
      INTERNAL_ERROR_NOT_ENOUGH_MEMORY = 28 ,
      INTERNAL_ERROR_WRONG_ENVIRONMENT = 29 ,
      INTERNAL_ERROR_INVALID_EVENT_SYNC_STATE = 30 ,
      INTERNAL_ERROR_INVALID_PENDING_MESSAGE = 31 ,
      INTERNAL_ERROR_INVALID_OBJECT_INDEX = 32 ,
      INTERNAL_ERROR_INVALID_OBJECT_LOCATION = 33 ,
      INTERNAL_ERROR_INVALID_LOCK_NESTING_BEHAVIOR = 34 ,
      INTERNAL_ERROR_INVALID_CORE_MUTEX_ATTRIBUTE = 35 ,
      INTERNAL_ERROR_INVALID_THREAD_QUEUE_STATE = 36 ,
      INTERNAL_ERROR_INVALID_THREAD_QUEUE_DISCIPLINE = 37 ,
      INTERNAL_ERROR_INVALID_THREAD_BUDGET_ALGORITHM = 38 ,
      INTERNAL_ERROR_INVALID_WATCHDOG_DIRECTION = 39 ,
      INTERNAL_ERROR_INVALID_WATCHDOG_STATE = 40 ,
      INTERNAL_ERROR_INVALID_MUTEX_NEST_COUNT = 41 ,
      INTERNAL_ERROR_INVALID_HEAP_ALIGNMENT = 42
   } Internal_errors_Core_list;

   /**
    *  @brief holds the fatal error information.
    */
   typedef struct
   {
      /**
       * @brief This is the source of the error.
       */
      Internal_errors_Source the_source;
      /**
       * @brief This indicates if the error is internal of external.
       */
      boolean is_internal;
      /**
       * @brief This is the error code.
       */
      uint32_t the_error;
   } Internal_errors_Information;

   /**
    *  @brief store the error information when a fatal error occurs
    */
   extern Internal_errors_Information Internal_errors_What_happened;

   /**
    *  @brief announce that an internal error occurred
    *
    *  This routine will invoke the fatal error handler supplied by the user,
    *  followed by the the default one provided by the executive.  The default
    *  error handler assumes no hardware is present to help inform the user
    *  of the problem.  Halt stores the error code in a known register,
    *  disables interrupts, and halts the CPU.  If the CPU does not have a
    *  halt instruction, it will loop to itself.
    *
    *  @param[in] the_source what subsystem the error originated in
    *  @param[in] is_internal if the error was internally generated
    *  @param[in] the_error fatal error status code
    *
    *  @note The the_error is not necessarily a directive status code.
    */
   void _Internal_error_Occurred(
                                 Internal_errors_Source the_source ,
                                 boolean is_internal ,
                                 uint32_t the_error
                                 );

   /**
    *  @brief handles the application safe state
    *
    *  This routine is invoked when a error occurs.
    *  Care must be taken when invoking RTEMS directives inside this function.
    *  note that an RTEMS error could have occurred and RTEMS might be corrupted.
    *  For example, the system could not have been properly initalized because not
    *  enough memory was allocated.
    *
    *  @param[in] the_source what subsystem the error originated in
    *  @param[in] is_internal if the error was internally generated
    *  @param[in] the_error fatal error status code
    *
    *  @return  returns TRUE if the error was corrected and FALSE otherwise
    *
    *  @note The the_error is not necessarily a directive status code.
    *  @note This function does not necessarily have to return. It can restart
    *    the system, delete/restart the calling thread, enter a polling mode, etc
    */
   boolean app_safe_state_handler(
                                  Internal_errors_Source the_source ,
                                  boolean is_internal ,
                                  uint32_t the_error
                                  );

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */
