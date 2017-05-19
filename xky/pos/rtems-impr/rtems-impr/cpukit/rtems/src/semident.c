/**
 *  @file
 *  semident.c
 *
 *  @brief identify a semaphore based on its name
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
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  7104    | 09/04/2010  | mcoutinho    | IPR 1931
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


rtems_status_code rtems_semaphore_ident(
                                        rtems_name name ,
                                        uint32_t node ,
                                        rtems_id *id
                                        )
{
    /* status of the object indentification */
    Objects_Name_or_id_lookup_errors status;

    /* get object id from the:
     *   semaphore object table
     *   with the specified name
     *   if multiprocessing, then with the node
     *   and place the result in the pointed by id */
    status = _Objects_Name_to_id(&_Semaphore_Information ,
                                 (Objects_Name) name ,
#if defined(RTEMS_MULTIPROCESSING)
        node ,
#endif
        id);

    /* translate return code */
    return _Status_Object_name_errors_to_status[ status ];
}

/**  
 *  @}
 */

/**
 *  @}
 */
