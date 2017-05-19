/**
 *  @file
 *  eventtimeout.c
 *
 *  @brief announce that a thread has timeout waiting for an event
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
 *  571     | 17/11/2008  | mcoutinho    | IPR 64
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  7076    | 09/04/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_EVENT Event Manager
 *  @{
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/event.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/tasks.h>


void _Event_Timeout(
                    Objects_Id id ,
                    void *ignored
                    )
{
    /* the task to awake */
    Thread_Control *the_thread;

    /* the task to awake location*/
    Objects_Locations location;

    /* the interrupt level */
    ISR_Level level;


    /* get the thread */
    the_thread = _Thread_Get(id , &location);

    /* check the task location */
    switch(location)
    {

#if defined(RTEMS_MULTIPROCESSING)
            /* task is remote */
        case OBJECTS_REMOTE: /* impossible */
#endif

            /* error situation */
        case OBJECTS_ERROR:
            /* just leave */
            break;

            /* task is local (only possible situation) */
        case OBJECTS_LOCAL:

            /*  If the event manager is not synchronized, then it is either
             *  "nothing happened", "timeout", or "satisfied".   If the_thread
             *  is the executing thread, then it is in the process of blocking
             *  and it is the thread which is responsible for the synchronization
             *  process.
             *
             *  If it is not satisfied, then it is "nothing happened" and
             *  this is the "timeout" transition.  After a request is satisfied,
             *  a timeout is not allowed to occur. */

            /* enter critical section */
            _ISR_Disable(level);

            /* verify thread is waiting */
            if(the_thread->Wait.count)
            {
                /* thread is no longer waiting */
                the_thread->Wait.count = 0;

                /* check if the state is not synchronized and the executing thread */
                if(_Event_Sync_state != EVENT_SYNC_SYNCHRONIZED &&
                   _Thread_Is_executing(the_thread))
                {
                    /* if not satisfied */
                    if(_Event_Sync_state != EVENT_SYNC_SATISFIED)
                    {
                        /* then a timeout occurred */
                        _Event_Sync_state = EVENT_SYNC_TIMEOUT;
                    }
                    /* leave critical section */
                    _ISR_Enable(level);
                }
                else
                {
                    /* then a timeout occurred */
                    the_thread->Wait.return_code = RTEMS_TIMEOUT;

                    /* leave critical section */
                    _ISR_Enable(level);

                    /* and unblock the thread */
                    _Thread_Unblock(the_thread);
                }
            }
            else
            {
                /* leave critical section */
                _ISR_Enable(level);
            }

            /* unnest dispatch (disabled by _Thread_Get) */
            _Thread_Unnest_dispatch();

            /* and leave */
            break;

            /* default clause, invalid object location */
        default:
            
            /* raise internal error */
            _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                     TRUE ,
                                     INTERNAL_ERROR_INVALID_OBJECT_LOCATION);
    }
}

/**  
 *  @}
 */

/**
 *  @}
 */
