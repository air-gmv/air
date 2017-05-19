/**
 *  @file
 *  tasks.h
 *
 *  @brief  all constants and structures associated
 *  with RTEMS tasks.  This manager provides a comprehensive set of directives
 *  to create, delete, and administer tasks.
 *
 *  Directives provided are:
 *\n
 *     + create a task\n
 *     + get an ID of a task\n
 *     + start a task\n
 *     + restart a task\n
 *     + delete a task\n
 *     + suspend a task\n
 *     + resume a task\n
 *     + set a task's priority\n
 *     + change the current task's mode\n
 *     + get a task notepad entry\n
 *     + set a task notepad entry\n
 *     + wake up after interval\n
 *     + wake up when specified\n
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
 *  563     | 17/11/2008  | mcoutinho    | IPR 69
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
 *  @defgroup RTEMS_API_TASK Task Manager
 *  
 *  @brief The Task Manager component allows the user to manage threads.
 *
 *  It allows
 *  the creation/deletion/suspension/resuming/etc of threads, among several
 *  other features.
 *  @{
 */

#ifndef _RTEMS_RTEMS_TASKS_H
#define _RTEMS_RTEMS_TASKS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/types.h>
#include <rtems/rtems/eventset.h>
#include <rtems/rtems/attr.h>
#include <rtems/rtems/status.h>

   /**
    *  @brief constant to be used as the ID of current task
    */
#define RTEMS_SELF                OBJECTS_ID_OF_SELF

   /**
    *  @brief constant passed to the rtems_task_wake_after directive as the
    *  interval when a task wishes to yield the CPU.
    */
#define RTEMS_YIELD_PROCESSOR WATCHDOG_NO_TIMEOUT

   /**
    *  @brief define the type for an RTEMS API task priority.
    */
   typedef Priority_Control rtems_task_priority;

#define RTEMS_NO_PRIORITY           RTEMS_CURRENT_PRIORITY

#define RTEMS_MINIMUM_PRIORITY      (PRIORITY_MINIMUM + 1)
#define RTEMS_MAXIMUM_PRIORITY      PRIORITY_MAXIMUM

   /**
    *  @brief constant passed to rtems_task_set_priority when the
    *  caller wants to obtain the current priority.
    */
#define RTEMS_CURRENT_PRIORITY      PRIORITY_MINIMUM

   /*
    *  Notepads constants (indices into notepad array)
    */

#define RTEMS_NOTEPAD_FIRST 0             /* lowest numbered notepad */
#define RTEMS_NOTEPAD_0    0              /* notepad location 0  */
#define RTEMS_NOTEPAD_1    1              /* notepad location 1  */
#define RTEMS_NOTEPAD_2    2              /* notepad location 2  */
#define RTEMS_NOTEPAD_3    3              /* notepad location 3  */
#define RTEMS_NOTEPAD_4    4              /* notepad location 4  */
#define RTEMS_NOTEPAD_5    5              /* notepad location 5  */
#define RTEMS_NOTEPAD_6    6              /* notepad location 6  */
#define RTEMS_NOTEPAD_7    7              /* notepad location 7  */
#define RTEMS_NOTEPAD_8    8              /* notepad location 8  */
#define RTEMS_NOTEPAD_9    9              /* notepad location 9  */
#define RTEMS_NOTEPAD_10   10             /* notepad location 10 */
#define RTEMS_NOTEPAD_11   11             /* notepad location 11 */
#define RTEMS_NOTEPAD_12   12             /* notepad location 12 */
#define RTEMS_NOTEPAD_13   13             /* notepad location 13 */
#define RTEMS_NOTEPAD_14   14             /* notepad location 14 */
#define RTEMS_NOTEPAD_15   15             /* notepad location 15 */
#define RTEMS_NOTEPAD_LAST RTEMS_NOTEPAD_15     /* highest numbered notepad */

#define RTEMS_NUMBER_NOTEPADS  (RTEMS_NOTEPAD_LAST+1)

   /**
    * @brief prototype of the function that creates the initial user tasks
    */
    extern void (*_RTEMS_tasks_Initialize_user_tasks_p )(void);

   /**
    *  @brief external API name for Thread_Control
    */
   typedef Thread_Control rtems_tcb;

   /**
    *  @brief define the "return type" of an RTEMS task.
    */
   typedef void rtems_task;

   /**
    *  @brief define the argument to an RTEMS task.
    */
   typedef uintptr_t rtems_task_argument;

   /**
    *  @brief define the type for the entry point of an RTEMS task.
    */
   typedef rtems_task(*rtems_task_entry)(
                                          rtems_task_argument
                                          );

   /**
    *  @brief define the Initialization Tasks Table
    *
    *  Each entry contains the information required by RTEMS to
    *  create and start a user task automatically at executive
    *  initialization time.
    */
   typedef struct
   {
      rtems_name name; /* task name */
      size_t stack_size; /* task stack size */
      rtems_task_priority initial_priority; /* task priority */
      rtems_attribute attribute_set; /* task attributes */
      rtems_task_entry entry_point; /* task entry point */
      rtems_mode mode_set; /* task initial mode */
      rtems_task_argument argument; /* task argument */
   } rtems_initialization_tasks_table;

   /**
    *  @brief API specific information required by each thread for
    *  the RTEMS API to function correctly
    */
   typedef struct
   {
      uint32_t Notepads[ RTEMS_NUMBER_NOTEPADS ];
      rtems_event_set pending_events;
      rtems_event_set event_condition;
   } RTEMS_API_Control;

   /**
    *  @brief define the information control block used to
    *  manage this class of objects.
    */
   extern Objects_Information _RTEMS_tasks_Information;

   /**
    *  @brief initialize the Task Manager component
    *
    *  This routine initializes all Task Manager related data structures.
    *
    *  @param[in] maximum_tasks number of tasks to initialize
    */
   void _RTEMS_tasks_Manager_initialization(
                                            uint32_t maximum_tasks
                                            );

   /**
    *  @brief create a task
    *
    *  This directive creates a thread by allocating and initializing a
    *  thread control block and a stack.  The newly created thread is
    *  placed in the dormant state.
    *
    *  @param[in] name user defined thread name
    *  @param[in] initial_priority thread priority
    *  @param[in] stack_size stack size in bytes
    *  @param[in] initial_modes initial thread mode
    *  @param[in] attribute_set thread attributes
    *  @param[out] id pointer to thread id
    *
    *  @return RTEMS_SUCCESSFUL if successful\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_task_create(
                                       rtems_name name ,
                                       rtems_task_priority initial_priority ,
                                       size_t stack_size ,
                                       rtems_mode initial_modes ,
                                       rtems_attribute attribute_set ,
                                       Objects_Id *id
                                       );

   /**
    *  @brief ident a task
    *
    *  This routine implements the rtems_task_ident directive.
    *  This directive returns the task ID associated with name.
    *  If more than one task is named name, then the task to
    *  which the ID belongs is arbitrary.  node indicates the
    *  extent of the search for the ID of the task named name.
    *  The search can be limited to a particular node or allowed to
    *  encompass all nodes.
    *
    *  @param[in] name the task name
    *  @param[in] node the task node
    *  @param[out] id the task id
    *
    *  @return RTEMS_SUCCESSFUL if successful\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_task_ident(
                                      rtems_name name ,
                                      uint32_t node ,
                                      Objects_Id *id
                                      );

   /**
    *  @brief delete a task
    *
    *  This directive allows a thread to delete itself or the thread
    *  identified in the id field.  The executive halts execution
    *  of the thread and frees the thread control block.
    *
    *  @param[in] id thread id
    *
    *  @return nothing if id is the requesting thread (always succeeds)\n
    *  RTEMS_SUCCESSFUL if successful and id is not the requesting thread\n
    *  error code if unsuccessful\n
    */
   rtems_status_code rtems_task_delete(
                                       Objects_Id id
                                       );

   /**
    *  @brief get a note from a task
    *
    *  This directive obtains the note from the specified notepad
    *  of the specified thread.
    *
    *  @param[in] id thread id
    *  @param[in] notepad notepad number
    *  @param[out] note pointer to note
    *
    *  @return RTEMS_SUCCESSFUL if successful\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_task_get_note(
                                         Objects_Id id ,
                                         uint32_t notepad ,
                                         uint32_t *note
                                         );

   /**
    *  @brief set a note to a task
    *
    *  This directive sets the specified notepad contents to the given
    *  note.
    *
    *  @param[in] id thread id
    *  @param[in] notepad notepad number
    *  @param[in] note note value
    *
    *  @return RTEMS_SUCCESSFUL if successful\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_task_set_note(
                                         Objects_Id id ,
                                         uint32_t notepad ,
                                         uint32_t note
                                         );

   /**
    *  @brief set/get a task mode
    *
    *  This directive enables and disables several modes of
    *  execution for the requesting thread.
    *  Output:
    *  previous_mode_set - previous mode set
    *  always return RTEMS_SUCCESSFUL;
    *
    *  @param[in] mode_set new mode
    *  @param[in] mask mask
    *  @param[in] previous_mode_set address of previous mode set
    *
    *  @return RTEMS_SUCCESSFUL if successful\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_task_mode(
                                     rtems_mode mode_set ,
                                     rtems_mode mask ,
                                     rtems_mode *previous_mode_set
                                     );

   /**
    *  @brief restart a task
    *
    *  This directive readies the specified thread.  It restores
    *  the thread environment to the original values established
    *  at thread creation and start time.  A thread can be restarted
    *  from any state except the dormant state.
    *
    *  @param[in] id thread id
    *  @param[in] arg thread argument
    *
    *  @return RTEMS_SUCCESSFUL if successful\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_task_restart(
                                        Objects_Id id ,
                                        uint32_t arg
                                        );

   /**
    *  @brief suspend a task
    *
    *  This directive will place the specified thread in the "suspended"
    *  state.  Note that the suspended state can be in addition to
    *  other waiting states.
    *
    *  @param[in] id thread id
    *
    *  @return RTEMS_SUCCESSFUL if successful\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_task_suspend(
                                        Objects_Id id
                                        );

   /**
    *  @brief resume a task
    *
    *  This directive will remove the specified thread
    *  from the suspended state.
    *
    *  @param[in] id thread id
    *
    *  @return RTEMS_SUCCESSFUL if successful\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_task_resume(
                                       Objects_Id id
                                       );

   /**
    *  @brief set a task priority
    *
    *  This directive changes the priority of the specified thread.
    *  The specified thread can be any thread in the system including
    *  the requesting thread.
    *
    *  @param[in] id thread id (0 indicates requesting thread)
    *  @param[in] new_priority thread priority (0 indicates current priority)
    *  @param[out] old_priority pointer to previous priority
    *
    *  @return RTEMS_SUCCESSFUL if successful\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_task_set_priority(
                                             Objects_Id id ,
                                             rtems_task_priority new_priority ,
                                             rtems_task_priority *old_priority
                                             );

   /**
    *  @brief sleep a task until a specified time wall
    *
    *  This directive blocks the requesting thread until the given date and
    *  time is reached.
    *
    *  @param[in] time_buffer pointer to the time and date structure
    *
    *  @return RTEMS_SUCCESSFUL if successful\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_task_wake_when(
                                          rtems_time_of_day *time_buffer
                                          );

   /**
    *  @brief start a task
    *
    *  This directive readies the thread identified by the id
    *  based on its current priorty, to await execution.  A thread
    *  can be started only from the dormant state.
    *
    *  @param[in] id thread id
    *  @param[in] entry_point start execution address of thread
    *  @param[in] argument thread argument
    *
    *  @return RTEMS_SUCCESSFUL if successful\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_task_start(
                                      rtems_id id ,
                                      rtems_task_entry entry_point ,
                                      rtems_task_argument argument
                                      );

   /**
    *  @brief sleep a task for a specified number of clock ticks
    *
    *  This directive suspends the requesting thread for the given amount
    *  of ticks.
    *
    *  @param[in] ticks number of ticks to wait
    *
    *  @return RTEMS_SUCCESSFUL if successful\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_task_wake_after(
                                           rtems_interval ticks
                                           );

   /**
    *  @brief determine if a task is suspended
    *
    *  This directive returns a status indicating whether or not
    *  the specified task is suspended.
    *
    *  @param[in] id thread id
    *
    *  @return RTEMS_SUCCESSFUL if successful and not suspended\n
    *  RTEMS_ALREADY_SUSPENDED if successful and suspended\n
    *  error code if unsuccessful\n
    */
   rtems_status_code rtems_task_is_suspended(
                                             Objects_Id id
                                             );

   /**
    *  @brief add a variable to a task
    *
    *  This directive registers a task variable.
    *
    *  @param[in] tid the task id
    *  @param[in] ptr the pointer to the variable
    *  @param[in] dtor the destructor (if any)
    *
    *  @return RTEMS_SUCCESSFUL if successful\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_task_variable_add(
                                             rtems_id tid ,
                                             void **ptr ,
                                             void (*dtor )(void *)
                                             );

   /**
    *  @brief get a variable from a task
    *
    *  This directive gets the value of a task variable.
    *
    *  @param[in] tid the task id
    *  @param[in] ptr the pointer to the variable
    *  @param[out] result the result
    *
    *  @return RTEMS_SUCCESSFUL if successful\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_task_variable_get(
                                             rtems_id tid ,
                                             void **ptr ,
                                             void **result
                                             );

   /**
    *  @brief delete a variable of a task
    *
    *  This directive removes a task variable.
    *
    *  @param[in] tid the task id
    *  @param[in] ptr the variable to delete
    *
    *  @return RTEMS_SUCCESSFUL if successful\n
    *  error code if unsuccessful
    */
   rtems_status_code rtems_task_variable_delete(
                                                rtems_id tid ,
                                                void **ptr
                                                );

   /**
    *  @brief initialize the user tasks
    *
    *  This routine creates and starts all configured user
    *  initialzation threads.
    */
   void _RTEMS_tasks_Initialize_user_tasks(void);

   /**
    *  @brief create and start the initialization tasks
    *
    *  If the user has configured a set of Classic API Initialization Tasks,
    *  then we need to install the code that runs that loop.
    */
   void _RTEMS_tasks_Initialize_user_tasks_body(void);

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/tasks.inl>
#endif
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/taskmp.h>
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
