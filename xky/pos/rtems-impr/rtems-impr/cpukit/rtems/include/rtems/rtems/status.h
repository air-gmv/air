/**
 *  @file
 *  status.h
 *
 *  @brief contains the status codes returned from the
 *  executive directives.
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
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_COMMON Common
 *  @{
 */

#ifndef _RTEMS_RTEMS_STATUS_H
#define _RTEMS_RTEMS_STATUS_H

#ifdef __cplusplus
extern "C"
{
#endif

   /* types */

   /* enumerated constants */

   /**
    * @brief RTEMS status code
    **/
   typedef enum
   {
      RTEMS_SUCCESSFUL = 0 , /* successful completion */
      RTEMS_TASK_EXITTED = 1 , /* returned from a thread */
      RTEMS_MP_NOT_CONFIGURED = 2 , /* multiprocessing not configured */
      RTEMS_INVALID_NAME = 3 , /* invalid object name */
      RTEMS_INVALID_ID = 4 , /* invalid object id */
      RTEMS_TOO_MANY = 5 , /* too many */
      RTEMS_TIMEOUT = 6 , /* timed out waiting */
      RTEMS_OBJECT_WAS_DELETED = 7 , /* object deleted while waiting */
      RTEMS_INVALID_SIZE = 8 , /* specified size was invalid */
      RTEMS_INVALID_ADDRESS = 9 , /* address specified is invalid */
      RTEMS_INVALID_NUMBER = 10 , /* number was invalid */
      RTEMS_NOT_DEFINED = 11 , /* item has not been initialized */
      RTEMS_RESOURCE_IN_USE = 12 , /* resources still outstanding */
      RTEMS_UNSATISFIED = 13 , /* request not satisfied */
      RTEMS_INCORRECT_STATE = 14 , /* thread is in wrong state */
      RTEMS_ALREADY_SUSPENDED = 15 , /* thread already in state */
      RTEMS_ILLEGAL_ON_SELF = 16 , /* illegal on calling thread */
      RTEMS_ILLEGAL_ON_REMOTE_OBJECT = 17 , /* illegal for remote object */
      RTEMS_CALLED_FROM_ISR = 18 , /* called from wrong environment */
      RTEMS_INVALID_PRIORITY = 19 , /* invalid thread priority */
      RTEMS_INVALID_CLOCK = 20 , /* invalid date/time */
      RTEMS_INVALID_NODE = 21 , /* invalid node id */
      RTEMS_NOT_CONFIGURED = 22 , /* directive not configured */
      RTEMS_NOT_OWNER_OF_RESOURCE = 23 , /* not owner of resource */
      RTEMS_NOT_IMPLEMENTED = 24 , /* directive not implemented */
      RTEMS_INTERNAL_ERROR = 25 , /* RTEMS inconsistency detected */
      RTEMS_INTERNAL_INVALID_OBJECT_LOCATION = 26 , /* RTEMS cannot find correct object location */
      RTEMS_INTERNAL_INVALID_PERIOD_STATE = 27 , /* RTEMS has an invalid period state */
      RTEMS_INTERNAL_INVALID_TIMER_CLASS = 28 , /* RTEMS has an invalid timer class */
      RTEMS_NO_MEMORY = 29 , /* could not get enough memory */
      RTEMS_IO_ERROR = 30 , /* driver IO error */
      RTEMS_PROXY_BLOCKING = 31 /* internal error only */
   } rtems_status_code;

#define RTEMS_STATUS_CODES_FIRST RTEMS_SUCCESSFUL
#define RTEMS_STATUS_CODES_LAST  RTEMS_PROXY_BLOCKING

   /**
    * @brief lookup table to translate between internal error and rtems_status_code
    **/
   extern rtems_status_code _Status_Object_name_errors_to_status[];

#ifdef RTEMS_API_INIT
   rtems_status_code _Status_Object_name_errors_to_status[] = {
                                                               RTEMS_SUCCESSFUL , /* OBJECTS_SUCCESSFUL */
                                                               RTEMS_INVALID_NAME , /* OBJECTS_INVALID_NAME */
                                                               RTEMS_INVALID_ADDRESS , /* OBJECTS_INVALID_ADDRESS */
                                                               RTEMS_INVALID_ID , /* OBJECTS_INVALID_ID */
                                                               RTEMS_INVALID_NODE /* OBJECTS_INVALID_NODE */
   };
#endif

   /*
    *  Applications are allowed to use the macros to compare status codes.
    */

#include <rtems/rtems/status.inl>

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
