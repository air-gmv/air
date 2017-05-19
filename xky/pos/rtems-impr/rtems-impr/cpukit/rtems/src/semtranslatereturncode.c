/**
 *  @file
 *  semtranslatereturncode.c
 *
 *  @brief translate the internal semaphore code to a rtems_status_code
 *
 *  This package is the implementation of the Semaphore Manager.
 *  This manager utilizes standard Dijkstra counting semaphores to provide
 *  synchronization and mutual exclusion capabilities.
 *
 *  Directives provided are:
 *
 *     + create a semaphore\n
 *     + get an ID of a semaphore\n
 *     + delete a semaphore\n
 *     + acquire a semaphore\n
 *     + release a semaphore\n
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
 *  3255    | 22/05/2009  | mcoutinho    | IPR 463
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  7107    | 09/04/2010  | mcoutinho    | IPR 1931
 *  7253    | 12/04/2010  | mcoutinho    | IPR 2266
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_SEMAPHORE Semaphore Manager
 *  @{
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/attr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/sem.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/coresem.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#if defined(RTEMS_MULTIPROCESSING)
    #include <rtems/score/mpci.h>
#endif
#include <rtems/score/sysstate.h>

#include <rtems/score/interr.h>

RTEMS_STATIC_ATTRIBUTE rtems_status_code
_Semaphore_Translate_core_mutex_return_code_[] = {
                                                  RTEMS_SUCCESSFUL , /* CORE_MUTEX_STATUS_SUCCESSFUL */
                                                  RTEMS_UNSATISFIED , /* CORE_MUTEX_STATUS_UNSATISFIED_NOWAIT */
                                                  RTEMS_UNSATISFIED , /* CORE_MUTEX_STATUS_NESTING_NOT_ALLOWED */
                                                  RTEMS_NOT_OWNER_OF_RESOURCE , /* CORE_MUTEX_STATUS_NOT_OWNER_OF_RESOURCE */
                                                  RTEMS_OBJECT_WAS_DELETED , /* CORE_MUTEX_WAS_DELETED */
                                                  RTEMS_TIMEOUT , /* CORE_MUTEX_TIMEOUT */
                                                  RTEMS_INVALID_PRIORITY , /* CORE_MUTEX_STATUS_CEILING_VIOLATED */
};

RTEMS_STATIC_ATTRIBUTE rtems_status_code
_Semaphore_Translate_core_semaphore_return_code_[] = {
                                                      RTEMS_SUCCESSFUL , /* CORE_SEMAPHORE_STATUS_SUCCESSFUL */
                                                      RTEMS_UNSATISFIED , /* CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT */
                                                      RTEMS_OBJECT_WAS_DELETED , /* CORE_SEMAPHORE_WAS_DELETED */
                                                      RTEMS_TIMEOUT , /* CORE_SEMAPHORE_TIMEOUT  */
                                                      RTEMS_INTERNAL_ERROR , /* CORE_SEMAPHORE_MAXIMUM_COUNT_EXCEEDED */
                                                      RTEMS_INTERNAL_ERROR /* CORE_SEMAPHORE_BAD_TIMEOUT_VALUE */
};


rtems_status_code _Semaphore_Translate_core_mutex_return_code(
                                                              uint32_t status
                                                              )
{

#if defined(RTEMS_MULTIPROCESSING)

    /* if the status is proxy blocking */
    if(status == THREAD_STATUS_PROXY_BLOCKING)
    {
        /* then dont search the table and return proxy blocking */
        return RTEMS_PROXY_BLOCKING;
    }

#endif

    /* check if the error is valid */
    if(status > CORE_MUTEX_STATUS_CEILING_VIOLATED)
    {
        /* if not, return internal error */
        return RTEMS_INTERNAL_ERROR;
    }

    /* translate the mutex error to status code */
    return _Semaphore_Translate_core_mutex_return_code_[status];
}


rtems_status_code _Semaphore_Translate_core_semaphore_return_code(
                                                                  uint32_t status
                                                                  )
{

#if defined(RTEMS_MULTIPROCESSING)

    /* if the status is proxy blocking */
    if(status == THREAD_STATUS_PROXY_BLOCKING)
    {
        /* then dont search the table and return proxy blocking */
        return RTEMS_PROXY_BLOCKING;
    }

#endif

    /* check if the error is valid */
    if(status > CORE_SEMAPHORE_BAD_TIMEOUT_VALUE)
    {
        /* if not, return internal error */
        return RTEMS_INTERNAL_ERROR;
    }

    /* translate the semaphore error to status code */
    return _Semaphore_Translate_core_semaphore_return_code_[status];
}

/**  
 *  @}
 */

/**
 *  @}
 */
