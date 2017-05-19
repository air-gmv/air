/**
 *  @file
 *  sem.h
 *
 *  @brief contains all the constants and structures associated
 *  with the Semaphore Manager.  This manager utilizes standard Dijkstra
 *  counting semaphores to provide synchronization and mutual exclusion
 *  capabilities.
 *
 *  Directives provided are:
 *
 *     + create a semaphore
 *     + get an ID of a semaphore
 *     + delete a semaphore
 *     + acquire a semaphore
 *     + release a semaphore
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
 *  561     | 17/11/2008  | mcoutinho    | IPR 64
 *  561     | 17/11/2008  | mcoutinho    | IPR 65
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @defgroup RTEMS_API_SEMAPHORE Semaphore Manager
 *  
 *  @brief The Semaphore Manager allows the user to implement semaphores.
 *
 *  This allows
 *  thread synchronization. Semaphores with priority inheritance (simple
 *  priority inheritance and ceiling) are available.
 *  @{
 */

#ifndef _RTEMS_RTEMS_SEM_H
#define _RTEMS_RTEMS_SEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/rtems/types.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/tasks.h>
#include <rtems/rtems/attr.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/object.h>
#include <rtems/score/coresem.h>

    /**
     *  @brief define the control block used to manage each semaphore
     */
    typedef struct {
        Objects_Control Object;
        rtems_attribute attribute_set;

        union {
            CORE_mutex_Control mutex;
            CORE_semaphore_Control semaphore;
        } Core_control;
    } Semaphore_Control;

    /**
     *  @brief define the information control block used to manage
     *  this class of objects.
     */
    extern Objects_Information _Semaphore_Information;

    /**
     *  @brief initialize the Semaphore Manager
     *
     *  This routine initializes all semaphore manager related data structures.
     *
     *  @param[in] maximum_semaphores maximum configured semaphores
     */
    void _Semaphore_Manager_initialization(
            uint32_t maximum_semaphores
            );

    /**
     *  @brief create a semaphore
     *
     *  This directive creates a semaphore and sets the initial value based
     *  on the given count.  A semaphore id is returned.
     *
     *  @param[in] name user defined semaphore name
     *  @param[in] count initial count of semaphore
     *  @param[in] attribute_set semaphore attributes
     *  @param[in] priority_ceiling semaphore's ceiling priority
     *  @param[out] id pointer to semaphore id
     *
     *  @return RTEMS_SUCCESSFUL if successful
     *  error code if unsuccessful
     */
    rtems_status_code rtems_semaphore_create(
            rtems_name name,
            uint32_t count,
            rtems_attribute attribute_set,
            rtems_task_priority priority_ceiling,
            rtems_id *id
            );

    /**
     *  @brief ident a semaphore
     *
     *  This routine implements the rtems_semaphore_ident directive.
     *  This directive returns the semaphore ID associated with name.
     *  If more than one semaphore is named name, then the semaphore
     *  to which the ID belongs is arbitrary.  node indicates the
     *  extent of the search for the ID of the semaphore named name.
     *  The search can be limited to a particular node or allowed to
     *  encompass all nodes.
     *
     *  @param[in] name semaphore name
     *  @param[in] node node on a multiprocessor system
     *  @param[out] id identifier of the semaphore
     *
     *  @return returns RTEMS_SUCCESSFUL if successful.
     *  the error_code if unsuccessful
     */
    rtems_status_code rtems_semaphore_ident(
            rtems_name name,
            uint32_t node,
            rtems_id *id
            );

    /**
     *  @brief delete a semaphore
     *
     *  This routine implements the rtems_semaphore_delete directive.
     *  This directive allows a thread to delete a semaphore specified by
     *  the semaphore id.  The semaphore is freed back to the inactive
     *  semaphore chain.
     *
     *  @param[in] id semaphore id
     *
     *  @return returns RTEMS_SUCCESSFUL if successful.
     *  the error_code if unsuccessful
     */
    rtems_status_code rtems_semaphore_delete(
            rtems_id id
            );

    /**
     *  @brief obtain a semaphore
     *
     *  This routine implements the rtems_semaphore_obtain directive.  It
     *  attempts to obtain a unit from the semaphore associated with ID.
     *  If a unit can be allocated, the calling task will return immediately.
     *  If no unit is available, then the task may return immediately or
     *  block waiting for a unit with an optional timeout of timeout
     *  clock ticks.  Whether the task blocks or returns immediately
     *  is based on the RTEMS_NO_WAIT option in the option_set.
     *
     *  @param[in] id semaphore id
     *  @param[in] option_set the option (wait, no-wait)
     *  @param[in] timeout the timeout (if selected wait option) in number of clock
     *  ticks
     *
     *  @return returns RTEMS_SUCCESSFUL if successful.
     *  the error_code if unsuccessful
     */
    rtems_status_code rtems_semaphore_obtain(
            rtems_id id,
            uint32_t option_set,
            rtems_interval timeout
            );

    /**
     *  @brief release a semaphore
     *
     *  This routine implements the rtems_semaphore_release directive.  It
     *  frees a unit to the semaphore associated with ID.  If a task was
     *  blocked waiting for a unit from this semaphore, then that task will
     *  be readied and the unit given to that task.  Otherwise, the unit
     *  will be returned to the semaphore.
     *
     *  @param[in] id semaphore id
     *
     *  @return returns RTEMS_SUCCESSFUL if successful.
     *  the error_code if unsuccessful
     */
    rtems_status_code rtems_semaphore_release(
            rtems_id id
            );

    /**
     *  @brief flush a semaphore
     *
     *  This directive allows a thread to flush the threads
     *  pending on the semaphore.
     *
     *  @param[in] id semaphore id
     *
     *  @return returns RTEMS_SUCCESSFUL if successful.
     *  the error_code if unsuccessful
     */
    rtems_status_code rtems_semaphore_flush(
            rtems_id id
            );

    /**
     *  @brief seize a semaphore
     *
     *  This routine attempts to receive a unit from the_semaphore.
     *  If a unit is available or if the RTEMS_NO_WAIT option is enabled in
     *  option_set, then the routine returns.  Otherwise, the calling task
     *  is blocked until a unit becomes available.
     *
     *  @param[in] the_semaphore the sempahore to seize
     *  @param[in] option_set the option
     *
     *  @return returns TRUE of the semaphore was obtained and FALSE otherwise
     */
    boolean _Semaphore_Seize(
            Semaphore_Control *the_semaphore,
            uint32_t option_set
            );

    /**
     *  @brief translate internal return code of the core Mutex to
     *  rtems_status_code
     *
     *  This function returns a RTEMS status code based on the mutex
     *  status code specified.
     *
     *  @param[in] the_mutex_status mutex status code to translate
     *
     *  @return translated RTEMS status code
     */
    rtems_status_code _Semaphore_Translate_core_mutex_return_code(
            uint32_t the_mutex_status
            );

    /**
     *  @brief translate internal return code of the core semaphore to
     *  rtems_status_code
     *
     *  This function returns a RTEMS status code based on the semaphore
     *  status code specified.
     *
     *  @param[in] the_mutex_status semaphore status code to translate
     *
     *  @return translated RTEMS status code
     */
    rtems_status_code _Semaphore_Translate_core_semaphore_return_code(
            uint32_t the_mutex_status
            );

#if defined(RTEMS_MULTIPROCESSING)
    /**
     *  @brief support of core mutex on multiprocessing systems
     *
     *  This function processes the global actions necessary for remote
     *  accesses to a global semaphore based on a core mutex.  This function
     *  is called by the core.
     *
     *  @param[in] the_thread the thread to block
     *  @param[in] id the mutex id
     */
    void _Semaphore_Core_mutex_mp_support(
            Thread_Control *the_thread,
            rtems_id id
            );
#endif



#if defined(RTEMS_MULTIPROCESSING)
    /**
     *  @brief support of core semaphore on multiprocessing systems
     *
     *  _Semaphore_Core_mp_support
     *  This function processes the global actions necessary for remote
     *  accesses to a global semaphore based on a core semaphore.  This function
     *  is called by the core.
     *
     *  @param[in] the_thread thread to block
     *  @param[in] id the mutex id
     */
    void _Semaphore_Core_semaphore_mp_support(
            Thread_Control *the_thread,
            rtems_id id
            );
#endif


#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/sem.inl>
#endif
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/semmp.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */

/**  
 *  @}
 */

/**
 *  @}
 */
