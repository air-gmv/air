/**
 *  @file
 *  sem.c
 *
 *  @brief initialize the Semaphore Manager
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
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  7100    | 09/04/2010  | mcoutinho    | IPR 1931
 *  8312    | 21/06/2010  | mcoutinho    | IPR 451
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


Objects_Information _Semaphore_Information;


void _Semaphore_Manager_initialization(
                                       uint32_t maximum_semaphores
                                       )
{
    /* initialize the information for the semaphore manager */
    _Objects_Initialize_information(&_Semaphore_Information , /* object information table */
                                    OBJECTS_CLASSIC_API , /* object API */
                                    OBJECTS_RTEMS_SEMAPHORES , /* object class */
                                    maximum_semaphores , /* maximum objects of this class */
                                    sizeof ( Semaphore_Control ) , /* size of this object's control block */
                                    FALSE , /* TRUE if the name is a string */
                                    RTEMS_MAXIMUM_NAME_LENGTH /* maximum length of an object name */
#if defined(RTEMS_MULTIPROCESSING)
        ,
                                    TRUE , /* TRUE if this is a global object class */
                                    _Semaphore_MP_Send_extract_proxy /* Proxy extraction support callout */
#endif
        );


    /* if multiprocessing is selected */
#if defined(RTEMS_MULTIPROCESSING)

    /* register the MP Process Packet routine */
    _MPCI_Register_packet_processor(MP_PACKET_SEMAPHORE ,
                                    _Semaphore_MP_Process_packet);
#endif

}

/**  
 *  @}
 */

/**
 *  @}
 */
