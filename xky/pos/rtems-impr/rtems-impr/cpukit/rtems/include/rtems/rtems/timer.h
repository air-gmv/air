/**
 *  @file
 *  timer.h
 *
 *  @brief all the constants, structures, and
 *  prototypes associated with the Timer Manager.  This manager provides
 *  facilities to configure, initiate, cancel, and delete timers which will
 *  fire at specified intervals of time.
 *
 *  Directives provided are:
 *
 *     + create a timer\n
 *     + get an ID of a timer\n
 *     + delete a timer\n
 *     + set timer to fire in context of clock tick\n
 *        - after a number of ticks have passed\n
 *        - when a specified date and time has been reached\n
 *     + initiate the timer server task\n
 *     + set timer to fire in context of the timer server task\n
 *        - after a number of ticks have passed\n
 *        - when a specified date and time has been reached\n
 *     + reset a timer\n
 *     + cancel a time\n
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
 *  4481    | 23/09/2009  | mcoutinho    | IPR 559
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5330    | 02/11/2009  | mcoutinho    | IPR 854
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
 *  @defgroup RTEMS_API_TIMER Timer Manager
 *
 *  @brief The Timer Manager component allows the user to specify timers, which can be
 *  fired in the context of an ISR or Thread (with the timer server).
 *  @{
 */

#ifndef _RTEMS_RTEMS_TIMER_H
#define _RTEMS_RTEMS_TIMER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/score/object.h>
#include <rtems/score/watchdog.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/clock.h>
#include <rtems/rtems/attr.h>

   /**
    *  @brief details the classes to which a timer
    *  may belong.
    */
   typedef enum
   {
      TIMER_INTERVAL ,
      TIMER_INTERVAL_ON_TASK ,
      TIMER_TIME_OF_DAY ,
      TIMER_TIME_OF_DAY_ON_TASK ,
      TIMER_DORMANT
   } Timer_Classes;

   /**
    *  @brief define a pointer to a timer service routine.
    */
   typedef void rtems_timer_service_routine;

   typedef rtems_timer_service_routine(*rtems_timer_service_routine_entry)(
                                                                            rtems_id ,
                                                                            void *
                                                                            );

   /**
    *  @brief defines the information control block used to manage
    *  this class of objects.
    */
   extern Objects_Information _Timer_Information;

   /**
    *  @brief TCB of the timer server
    *
    *  The timer server NULL before the
    *  server is executing and task-based timers are not allowed to be
    *  initiated until the server is started.
    *
    *  The timer server thread is used to execute timers in the context of a thread
    *  instead of in the context of an ISR.
    */
   extern Thread_Control *_Timer_Server;

   /**
    * @brief timer ticks chain
    *
    * The timer ticks chain contains the list of interval timers based on clock
    * ticks that are executed in the context of the Timer Server.
    *
    * This chain is used when a timer is fired after a specified number of clock
    * ticks have elasped
    **/
   extern Chain_Control _Timer_Ticks_chain;

   /**
    * @brief timer seconds chain
    *
    * The timer seconds chain contains the list of interval timers based on seconds
    * that are executed in the context of the Timer Server.
    *
    * This chain is used when a timer is selected to fire when a time wall reaches.
    * The time wall has a minimum resolution of a second, so that is why it is not
    * on the timer ticks chain.
    **/
   extern Chain_Control _Timer_Seconds_chain;

   /**
    *  @brief control block used to manage
    *  each timer
    */
   typedef struct
   {
      Objects_Control Object;
      Watchdog_Control Ticker;
      Timer_Classes the_class;
   } Timer_Control;

   /**
    *  @brief initialize the Timer Manager component
    *
    *  This routine initializes all timer manager related data structures.
    *
    *  @param[in] maximum_timers number of timers to initialize
    */
   void _Timer_Manager_initialization(
                                      uint32_t maximum_timers
                                      );

   
   /**
    *  @brief create a timer
    *
    *  This directive creates a timer and performs some initialization.
    *
    *  @param[in] name timer name
    *  @param[in] id pointer to timer id
    *  @param[out] id timer id
    *
    *  @return RTEMS_SUCCESSFUL if successful.\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_timer_create(
                                        rtems_name name ,
                                        Objects_Id *id
                                        );

   /**
    *  @brief ident the timer
    *
    *  This directive returns the system ID associated with
    *  the timer name.
    *
    *  @param[in] name user defined message queue name
    *  @param[out] id pointer to timer id
    *
    *  @return RTEMS_SUCCESSFUL if successful.\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_timer_ident(
                                       rtems_name name ,
                                       Objects_Id *id
                                       );

   /**
    *  @brief cancel a timer
    *
    *  This directive allows a thread to cancel a timer.
    *
    *  @param[in] id timer id
    *
    *  @return RTEMS_SUCCESSFUL if successful.\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_timer_cancel(
                                        Objects_Id id
                                        );

   /**
    *  @brief delete a timer
    *
    *  This directive allows a thread to delete a timer.
    *
    *  @param[in] id timer id
    *
    *  @return RTEMS_SUCCESSFUL if successful.\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_timer_delete(
                                        Objects_Id id
                                        );

   /**
    *  @brief wake a timer (in the context of an ISR) after a specified number of
    *  clock ticks have elapsed
    *
    *
    *  This directive allows a thread to start a timer.
    *
    *  @param[in] id timer id
    *  @param[in] ticks interval until routine is fired
    *  @param[in] routine routine to schedule
    *  @param[in] user_data passed as argument to routine when it is fired
    *
    *  @return RTEMS_SUCCESSFUL if successful.\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_timer_fire_after(
                                            Objects_Id id ,
                                            rtems_interval ticks ,
                                            rtems_timer_service_routine_entry routine ,
                                            void *user_data
                                            );

   /**
    *  @brief wake a timer (in the context of an task) after a specified number of
    *  clock ticks have elapsed
    *
    *  This directive allows a thread to start a timer which will by
    *  executed by the Timer Server when it fires.
    *
    *  @param[in] id timer id
    *  @param[in] ticks interval until routine is fired
    *  @param[in] routine routine to schedule
    *  @param[in] user_data passed as argument to routine when it is fired
    *
    *  @return RTEMS_SUCCESSFUL if successful.\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_timer_server_fire_after(
                                                   Objects_Id id ,
                                                   rtems_interval ticks ,
                                                   rtems_timer_service_routine_entry routine ,
                                                   void *user_data
                                                   );

   /**
    *  @brief wake a timer (in the context of an ISR) when a specified time wall as
    *  reached
    *
    *  This directive allows a thread to start a timer.
    *
    *  @param[in] id timer id
    *  @param[in] wall_time time of day to fire timer
    *  @param[in] routine routine to schedule
    *  @param[in] user_data passed as argument to routine when it is fired
    *
    *  @return RTEMS_SUCCESSFUL if successful.\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_timer_fire_when(
                                           Objects_Id id ,
                                           rtems_time_of_day *wall_time ,
                                           rtems_timer_service_routine_entry routine ,
                                           void *user_data
                                           );

   /**
    *  @brief wake a timer (in the context of an task) when a specified time wall
    *  as reached
    *
    *  This directive allows a thread to start a timer which will by
    *  executed by the Timer Server when it fires.
    *
    *  @param[in] id timer id
    *  @param[in] wall_time time of day to fire timer
    *  @param[in] routine routine to schedule
    *  @param[in] user_data passed as argument to routine when it is fired
    *
    *  @return RTEMS_SUCCESSFUL if successful.\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_timer_server_fire_when(
                                                  Objects_Id id ,
                                                  rtems_time_of_day *wall_time ,
                                                  rtems_timer_service_routine_entry routine ,
                                                  void *user_data
                                                  );

   /**
    *  @brief reset a timer
    *
    *  This routine implements the rtems_timer_reset directive.  It is used
    *  to reinitialize the interval timer associated with ID just as if
    *  rtems_timer_fire_after were re-invoked with the same arguments that
    *  were used to initiate this timer.
    *
    *  @param[in] id the timer id
    *
    *  @return RTEMS_SUCCESSFUL if successful.\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_timer_reset(
                                       Objects_Id id
                                       );


   /**
    * @brief tiemr server default priority
    **/
#define RTEMS_TIMER_SERVER_DEFAULT_PRIORITY -1

   /**
    *  @brief initiate the timer server
    *
    *  This directive creates and starts the server for task-based timers.
    *  It must be invoked before any task-based timers can be initiated.
    *
    *  @param[in] priority timer server priority
    *  @param[in] stack_size stack size in bytes
    *  @param[in] attribute_set timer server attributes
    *
    *  @return RTEMS_SUCCESSFUL if successful.\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_timer_initiate_server(
                                                 uint32_t priority ,
                                                 uint32_t stack_size ,
                                                 rtems_attribute attribute_set
                                                 );

   /**
    * @brief timer information
    **/
   typedef struct
   {
      Timer_Classes the_class;
      Watchdog_Interval initial;
      Watchdog_Interval start_time;
      Watchdog_Interval stop_time;
   } rtems_timer_information;

   /**
    *  @brief timer used to control when the Timer Server wakes up to service
    *  "when" timers.
    */
   extern Watchdog_Control _Timer_Seconds_timer;

   /*
    *  Macros and routines that expose the mechanisms required to service
    *  the Timer Server timer.  These stop the timer, synchronize it with
    *  the current time, and restart it.
    */

#define _Timer_Server_stop_ticks_timer() \
      _Watchdog_Remove( &_Timer_Server->Timer )

#define _Timer_Server_stop_seconds_timer() \
      _Watchdog_Remove( &_Timer_Seconds_timer );

   /**
    *  @brief process timer server clock ticks chain
    *
    *  This routine is responsible for adjusting the list of task-based
    *  interval timers to reflect the passage of time.
    */
   void _Timer_Server_process_ticks_chain(void);

   /**
    *  @brief process timer server seconds chain
    *
    *  This routine is responsible for adjusting the list of task-based
    *  time of day timers to reflect the passage of time.
    */
   void _Timer_Server_process_seconds_chain(void);

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/timer.inl>
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
