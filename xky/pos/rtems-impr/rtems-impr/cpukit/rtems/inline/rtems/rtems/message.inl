/**
 *  @file
 *  message.inl
 *
 *  @brief static inline implementation of all
 *  inlined routines in the Message Manager.
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
 *  3594    | 02/07/2009  | mcoutinho    | IPR 97
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
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

#ifndef _RTEMS_RTEMS_MESSAGE_INL
#define _RTEMS_RTEMS_MESSAGE_INL

#include <rtems/score/wkspace.h>


/**
 *  @brief free a message queue
 *  
 *  This routine deallocates a message queue control block into
 *  the inactive chain of free message queue control blocks.
 *
 *  @param[in] the_message_queue the message queue to free
 *
 */
RTEMS_INLINE_ROUTINE void _Message_queue_Free(
                                              Message_queue_Control *the_message_queue
                                              )
{
   /* free the message queue object information */
   _Objects_Free(&_Message_queue_Information , &the_message_queue->Object);
}


/**
 *  @brief get a message queue
 *  
 *  This function maps message queue IDs to message queue control
 *  blocks.  If ID corresponds to a local message queue, then it
 *  returns the_message_queue control pointer which maps to ID
 *  and location is set to OBJECTS_LOCAL.  If the message queue ID is
 *  global and resides on a remote node, then location is set
 *  to OBJECTS_REMOTE, and the_message_queue is undefined.
 *  Otherwise, location is set to OBJECTS_ERROR and
 *  the_message_queue is undefined.
 *
 *  @param[in] id the message queue id
 *  @param[out] location the location of the message queue
 *
 *  @return returns the message control structure
 */
RTEMS_INLINE_ROUTINE Message_queue_Control *_Message_queue_Get(
                                                               Objects_Id id ,
                                                               Objects_Locations *location
                                                               )
{
   /* return the message queue object information with:
    *   the message queue object information
    *   the id of the message queue
    *   the address to place the location of the message queue */
   return (Message_queue_Control *) _Objects_Get(&_Message_queue_Information ,
                                                 id , location);
}

#endif
/* end of include file */

/**  
 *  @}
 */

/**
 *  @}
 */
