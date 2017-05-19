/**
 *  @file
 *  rtemsapi.c
 *
 *  @brief initialize the RTEMS API
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
 *  256     | 10/10/2008  | mcoutinho    | IPR 64
 *  297     | 15/10/2008  | mcoutinho    | IPR 69
 *  4808    | 12/10/2009  | mcoutinho    | IPR 65
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  7132    | 09/04/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_API Super API
 *  @{
 */

/**
 *  @addtogroup RTEMSAPI RTEMS API
 *  @{
 */

/*
 *  RTEMS_API_INIT is defined so all of the RTEMS API
 *  data will be included in this object file.
 */

#define RTEMS_API_INIT

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/rtemsapi.h>

#include <rtems/rtems/intr.h>
#include <rtems/rtems/clock.h>
#include <rtems/rtems/tasks.h>
#include <rtems/rtems/event.h>
#include <rtems/rtems/message.h>
#include <rtems/rtems/ratemon.h>
#include <rtems/rtems/sem.h>
#include <rtems/rtems/timer.h>

/**
 * @brief RTEMS objects
 **/
Objects_Information *_RTEMS_Objects[ OBJECTS_RTEMS_CLASSES_LAST + 1 ];


void _RTEMS_API_Initialize(
                           rtems_configuration_table *configuration_table
                           )
{
    /* the configuration API */
   rtems_api_configuration_table *api_configuration;

   
   /* get the api configuration */
   api_configuration = configuration_table->RTEMS_api_configuration;

   /* initialize the classic API object information table */
   _Objects_Information_table[OBJECTS_CLASSIC_API] = _RTEMS_Objects;

   /* initialize the attributes handler */
   _Attributes_Handler_initialization();

   /* initialize the interrupt manager */
   _Interrupt_Manager_initialization();

   /* initialize the Task Manager */
   _RTEMS_tasks_Manager_initialization(api_configuration->maximum_tasks);

   /* initialize the Timer Manager */
   _Timer_Manager_initialization(api_configuration->maximum_timers);

   /* initialize the Event Manager */
   _Event_Manager_initialization();

   /* initialize the Message Queue Manager */
   _Message_queue_Manager_initialization(api_configuration->maximum_message_queues);

   /* initialize the Semaphore Manager */
   _Semaphore_Manager_initialization(api_configuration->maximum_semaphores);

   /* initialize the Rate Monotonic Manager */
   _Rate_monotonic_Manager_initialization(api_configuration->maximum_periods);

}

/* end of file */

/**  
 *  @}
 */

/**  
 *  @}
 */

