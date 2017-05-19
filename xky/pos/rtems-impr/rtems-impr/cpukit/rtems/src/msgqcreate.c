/**
 *  @file
 *  msgqcreate.c
 *
 *  @brief create a message queue
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
 *  7082    | 09/04/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_MESSAGE Message Queue Manager
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/coremsg.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#if defined(RTEMS_MULTIPROCESSING)
    #include <rtems/score/mpci.h>
#endif
#include <rtems/rtems/status.h>
#include <rtems/rtems/attr.h>
#include <rtems/rtems/message.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/support.h>


rtems_status_code rtems_message_queue_create(
                                             rtems_name name ,
                                             uint32_t count ,
                                             uint32_t max_message_size ,
                                             rtems_attribute attribute_set ,
                                             Objects_Id *id
                                             )
{
    /* created message queue */
    register Message_queue_Control *the_message_queue;

    /* CORE message queue attributes */
    CORE_message_queue_Attributes the_msgq_attributes;
#if defined(RTEMS_MULTIPROCESSING)
    /* flag indicating if the message queue is global */
    boolean is_global;
#endif


    /* check if name is valid */
    if(!rtems_is_name_valid(name))
    {
        /* return invalid name */
        return RTEMS_INVALID_NAME;
    }

    /* check if id address is valid */
    if(!id)
    {
        /* the id address is not a valid */

        /* return an invalid address */
        return RTEMS_INVALID_ADDRESS;
    }

#if defined(RTEMS_MULTIPROCESSING)

    /* check if the message queue is global and there is no multiprocessing */
    if(( is_global = _Attributes_Is_global(attribute_set) ) &&
       !_System_state_Is_multiprocessing)
    {
        /* return multiprocessing is not configured */
        return RTEMS_MP_NOT_CONFIGURED;
    }
#endif

    /* check if count if valid */
    if(count == 0)
    {
        /* count is not valid (cannot create a message queue with space for 0
         * messages */
        return RTEMS_INVALID_NUMBER;
    }

    /* check if maximum message size if valid */
    if(max_message_size == 0)
    {
        /* maximum message is invalid (cannot be zero) */
        return RTEMS_INVALID_SIZE;
    }

#if defined(RTEMS_MULTIPROCESSING)

    /* I am not 100% sure this should be an error.
     * It seems reasonable to create a queue with a large max size,
     * and then just send smaller msgs from remote (or all) nodes. */
    if(is_global && ( _MPCI_table->maximum_packet_size < max_message_size ))
    {
        /* return invalid message size */
        return RTEMS_INVALID_SIZE;
    }

#endif

    /* parameters are all OK, continue creating the message queue */

    /* disable dispatch to protect object pointer */
    _Thread_Disable_dispatch();

    /* allocate message queue */
    the_message_queue = _Message_queue_Allocate(count , max_message_size);

    /* check that message queue was allocated */
    if(!the_message_queue)
    {
        /* enable dispatch */
        _Thread_Enable_dispatch();

        /* and return error */
        return RTEMS_TOO_MANY;
    }

#if defined(RTEMS_MULTIPROCESSING)

    /* if the message queue is global and allocate it and open it */
    if(is_global &&
       !( _Objects_MP_Allocate_and_open(&_Message_queue_Information ,
                                        name , the_message_queue->Object.id , FALSE) ))
    {
        /* could not allocate or open */

        /* undo previous step */

        /* free the message queue */
        _Message_queue_Free(the_message_queue);

        /* re-enable thread dispatch */
        _Thread_Enable_dispatch();

        /* and return that too many message queues were created */
        return RTEMS_TOO_MANY;
    }

#endif

    /* set the message queue attribute set */
    the_message_queue->attribute_set = attribute_set;

    /* check the queueing discipline */
    if(_Attributes_Is_priority(attribute_set))
    {
        /* discipline is priority queueing */
        the_msgq_attributes.discipline = CORE_MESSAGE_QUEUE_DISCIPLINES_PRIORITY;
    }
    else
    {
        /* discipline is FIFO queueing */
        the_msgq_attributes.discipline = CORE_MESSAGE_QUEUE_DISCIPLINES_FIFO;
    }

    /* check the result of the initialziation */
    if(!_CORE_message_queue_Initialize(&the_message_queue->message_queue ,
                                       &the_msgq_attributes ,
                                       count ,
                                       max_message_size))
    {
        /* if it wasnt correctly initialized then undo previous steps */

#if defined(RTEMS_MULTIPROCESSING)

        /* if message queue is global */
        if(is_global)
        {
            /* close it */
            _Objects_MP_Close(&_Message_queue_Information ,
                              the_message_queue->Object.id);
        }

#endif

        /* free the message queue */
        _Message_queue_Free(the_message_queue);

        /* and enable dispatch */
        _Thread_Enable_dispatch();

        /* return not satisfied */
        return RTEMS_UNSATISFIED;
    }

    /* otherwise, open the a new object */
    _Objects_Open(&_Message_queue_Information ,
                  &the_message_queue->Object ,
                  (Objects_Name) name);

    /* set the id */
    *id = the_message_queue->Object.id;

#if defined(RTEMS_MULTIPROCESSING)

    /* if the queue is global */
    if(is_global)
    {
        /* send a create message queue packet to the remote node */
        _Message_queue_MP_Send_process_packet(MESSAGE_QUEUE_MP_ANNOUNCE_CREATE ,
                                              the_message_queue->Object.id ,
                                              name ,
                                              0);
    }
#endif

    /* enable dispatch */
    _Thread_Enable_dispatch();

    /* and return success */
    return RTEMS_SUCCESSFUL;
}

/**  
 *  @}
 */

/**
 *  @}
 */
