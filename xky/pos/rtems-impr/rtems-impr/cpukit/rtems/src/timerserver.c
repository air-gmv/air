/**
 *  @file
 *  timerserver.c
 *
 *  @brief initialize the timer server task
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2002.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  4482    | 23/09/2009  | mcoutinho    | IPR 559
 *  4771    | 09/10/2009  | mcoutinho    | IPR 793
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  7124    | 09/04/2010  | mcoutinho    | IPR 1931
 *  7253    | 12/04/2010  | mcoutinho    | IPR 2266
 *  9293    | 03/01/2011  | mcoutinho    | IPR 451
 *  9293    | 03/01/2011  | mcoutinho    | IPR 2817
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_TIMER Timer Manager
 *  @{
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/timer.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>
#include <rtems/rtems/intr.h>
#include <rtems/rtems/tasks.h>
#include <rtems/rtems/support.h>
#include <rtems/score/thread.h>


Watchdog_Interval _Timer_Server_seconds_last_time;


Watchdog_Interval _Timer_Server_ticks_last_time;


Watchdog_Control _Timer_Seconds_timer;


/**
 *  @brief entry point of the timer server
 *
 *  This is the server for task based timers.  This task executes whenever
 *  a task-based timer should fire.  It services both "after" and "when"
 *  timers.  It is not created automatically but must be created explicitly
 *  by the application before task-based timers may be initiated.
 *
 *  @param[in] ignored the task argument is ignored
 *
 *  @return does not return (the timer server runs in a loop)
 */
RTEMS_STATIC_ATTRIBUTE Thread _Timer_Server_body(
                                                 uint32_t ignored
                                                 )
{
    /* interrupt level */
    rtems_interrupt_level level;

    /* Initialize the "last time" markers to indicate the timer that
     * the server was initiated. */

    /* enter critical section to protect _Watchdog_Ticks_since_boot variable */
    rtems_interrupt_disable(level);

    /* initialize the last clock tick marker */
    _Timer_Server_ticks_last_time = _Watchdog_Ticks_since_boot;

    /* leave critical section */
    rtems_interrupt_enable(level);

    /* initialize the last second marker */
    _Timer_Server_seconds_last_time = _TOD_Seconds_since_epoch;

    /* disable thred dispatch */
    _Thread_Disable_dispatch();

    /* timer server is on an infinite loop */
    while(1)
    {
        /* Block the timer server until there is something to do */
        _Thread_Set_state(_Timer_Server , STATES_DELAYING);

        /* reset the clock ticks timer */
        _Timer_Server_reset_ticks_timer();

        /* reset the seconds timer */
        _Timer_Server_reset_seconds_timer();

        /* re-enable thread dispatch (this will effectly change the running task) */
        _Thread_Enable_dispatch();

        /* at this point, at least one of the timers this task relies
         * upon has fired.  Stop them both while we process any outstanding
         * timers.  Before we block, we will restart them */
        _Timer_Server_stop_ticks_timer();
        _Timer_Server_stop_seconds_timer();

        /* disable dispatching while processing the timers since we want
         * to mimic the environment that non-task-based TSRs execute in.
         * This ensures that the primary difference is that _ISR_Nest_level
         * is 0 for task-based timers and non-zero for the others */
        _Thread_Disable_dispatch();

        /* process the timers which were invoked with
         * rtems_timer_server_fire_after */
        _Timer_Server_process_ticks_chain();

        /* process the timers which were invoked with
         * rtems_timer_server_fire_when */
        _Timer_Server_process_seconds_chain();
    }

    /* unreached - only to remove warnings */
    return 0;
}


rtems_status_code rtems_timer_initiate_server(
                                              uint32_t priority ,
                                              uint32_t stack_size ,
                                              rtems_attribute attribute_set
                                              )
{
    /* id of the timer server task */
    rtems_id id;

    /* return value of this function (if an error occurs) */
    rtems_status_code status;

    /* priority of the timer server task */
    rtems_task_priority _priority;


    /* make sure the requested priority is valid */

    /* initialize the priority */
    _priority = priority;

    /* check if the priority is the default timer server priority */
    if(priority == RTEMS_TIMER_SERVER_DEFAULT_PRIORITY)
    {
        /* if so, then the priority is 0 */
        _priority = 0;
    }
        /* else, check if the priority is valid */
    else if(!_RTEMS_tasks_Priority_is_valid(priority))
    {
        /* is not, return invalid priority error */
        return RTEMS_INVALID_PRIORITY;
    }

    /* disable thread dispatch
     * (make sure the test versus create/start operation are atomic) */
    _Thread_Disable_dispatch();

    /* check if timer server has already been created */
    if(_Timer_Server)
    {
        /* enable thread dispatch */
        _Thread_Enable_dispatch();

        /* return incorrect state (timer server already initiated) */
        return RTEMS_INCORRECT_STATE;
    }

    /*  Create the Timer Server with the name the name of "TIME".  The attribute
     *  RTEMS_SYSTEM_TASK allows us to set a priority to 0 which will makes it
     *  higher than any other task in the system.  It can be viewed as a low
     *  priority interrupt.  It is also always NO_PREEMPT so it looks like
     *  an interrupt to other tasks.
     *
     *  We allow the user to override the default priority because the Timer
     *  Server can invoke TSRs which must adhere to language run-time or
     *  other library rules.  For example, if using a TSR written in Ada the
     *  Server should run at the same priority as the priority Ada task.
     *  Otherwise, the priority ceiling for the mutex used to protect the
     *  GNAT run-time is violated. */
    status = rtems_task_create(rtems_build_name('T' , 'I' , 'M' , 'E') , /* "TIME" */
                               _priority , /* create with priority 1 since 0 is illegal */
                               stack_size , /* let user specify stack size */
                               RTEMS_NO_PREEMPT , /* no preempt is like an interrupt */
                               /* user may want floating point but we need */
                               /*   system task specified for 0 priority */
                               attribute_set | RTEMS_SYSTEM_TASK ,
                               &id); /* get the id back */

    /* if could not create the timer server */
    if(status)
    {
        /* enable thread dispatch */
        _Thread_Enable_dispatch();

        /* return the creation error */
        return status;
    }

    /* start the timer server */
    status = rtems_task_start(id , /* the id from create */
                              (rtems_task_entry) _Timer_Server_body , /* the timer server entry point */
                              0 /* there is no argument */);

    /* if could not start the timer server */
    if(status)
    {
        /*  One would expect a call to rtems_task_delete() here to clean up
         *  but there is actually no way (in normal circumstances) that the
         *  start can fail.  The id and starting address are known to be
         *  be good.  If this service fails, something is weirdly wrong on the
         *  target such as a stray write in an ISR or incorrect memory layout */
        _Thread_Enable_dispatch();

        /* return the creation error */
        return status;
    }

    /*  We work with the TCB pointer, not the ID, so we need to convert
     *  to a TCB pointer from here out.
     *  NOTE: Setting the pointer to the Timer Server TCB to a value other than
     *        NULL indicates that task-based timer support is initialized. */
    _Timer_Server = (Thread_Control *) _Objects_Get_local_object(&_RTEMS_tasks_Information ,
                                                                 _Objects_Get_index(id));

    /* check for a possible error in getting the timer server */
    if(_Timer_Server == NULL)
    {
        /* return internal error */
        return RTEMS_INTERNAL_ERROR;
    }

    /* Initialize the timer lists that the server will manage */

    /* initialize the timer clock ticks chain */
    _Chain_Initialize_empty(&_Timer_Ticks_chain);

    /* initialize the timer seconds chain */
    _Chain_Initialize_empty(&_Timer_Seconds_chain);

    /* Initialize the timers that will be used to control when the
     * Timer Server wakes up and services the task-based timers */

    /* initialize the timer server clock ticks watchdog */
    _Watchdog_Initialize(&_Timer_Server->Timer , _Thread_Delay_ended , id , NULL);

    /* initialize the timer server seconds watchdog */
    _Watchdog_Initialize(&_Timer_Seconds_timer , _Thread_Delay_ended , id , NULL);

    /* enable thread dispatch */
    _Thread_Enable_dispatch();

    /* return success */
    return RTEMS_SUCCESSFUL;
}


void _Timer_Server_process_ticks_chain(void)
{
    /* current time (in clock ticks) */
    Watchdog_Interval snapshot;

    /* number of clock ticks that have elapsed */
    Watchdog_Interval ticks;


    /* get number of clock ticks elapsed */
    snapshot = _Watchdog_Ticks_since_boot;

    /* calculate elapsed clock ticks since last the last time the timer was called */

    /* if the snapshot is larger than the last processed clock tick */
    if(snapshot >= _Timer_Server_ticks_last_time)
    {
        /* get the difference */
        ticks = snapshot - _Timer_Server_ticks_last_time;
    }
    else
    {
        /* get the difference (there was an overrun) */
        ticks = (0xFFFFFFFF - _Timer_Server_ticks_last_time) + snapshot;
    }

    /* set the new snapshot */
    _Timer_Server_ticks_last_time = snapshot;

    /* adjust the watchdog timer ticks chain by <ticks> units*/
    _Watchdog_Adjust(&_Timer_Ticks_chain , WATCHDOG_FORWARD , ticks);
}


void _Timer_Server_process_seconds_chain(void)
{
    /* current time (in clock ticks) */
    Watchdog_Interval snapshot;

    /* number of clock ticks that have elapsed */
    Watchdog_Interval ticks;

    /* Process the seconds chain.  Start by checking that the Time
     * of Day (TOD) has not been set backwards.  If it has then
     * we want to adjust the _Timer_Seconds_chain to indicate this */
    snapshot = _TOD_Seconds_since_epoch;
    if(snapshot > _Timer_Server_seconds_last_time)
    {
        /* This path is for normal forward movement and cases where the
         * TOD has been set forward */
        ticks = snapshot - _Timer_Server_seconds_last_time;

        /* adjust the watchdog timer seconds chain by <ticks> units */
        _Watchdog_Adjust(&_Timer_Seconds_chain , WATCHDOG_FORWARD , ticks);

    }
    else if(snapshot < _Timer_Server_seconds_last_time)
    {
        /* The current TOD is before the last TOD which indicates that
         * TOD has been set backwards */
        ticks = _Timer_Server_seconds_last_time - snapshot;

        /* adjust the watchdog timer seconds chain by <ticks> units */
        _Watchdog_Adjust(&_Timer_Seconds_chain , WATCHDOG_BACKWARD , ticks);
    }

    /* update the last time the timer server seconds chain was updated */
    _Timer_Server_seconds_last_time = snapshot;
}

/**  
 *  @}
 */

/**
 *  @}
 */
