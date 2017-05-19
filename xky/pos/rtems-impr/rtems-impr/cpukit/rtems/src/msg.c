/**
 *  @file
 *  msg.c
 *
 *  @brief initialize the Message Queue Manager
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
 *  7079    | 09/04/2010  | mcoutinho    | IPR 1931
 *  8310    | 21/06/2010  | mcoutinho    | IPR 451
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


Objects_Information _Message_queue_Information;


void _Message_queue_Manager_initialization(
                                           uint32_t maximum_message_queues
                                           )
{
    /* initialize the information for the message queue manager */
    _Objects_Initialize_information(&_Message_queue_Information , /* object information table */
                                    OBJECTS_CLASSIC_API , /* object API */
                                    OBJECTS_RTEMS_MESSAGE_QUEUES , /* object class */
                                    maximum_message_queues , /* maximum objects of this class */
                                    sizeof ( Message_queue_Control ) ,
                                    /* size of this object's control block */
                                    FALSE , /* TRUE if names of this object are strings */
                                    RTEMS_MAXIMUM_NAME_LENGTH /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
        ,
                                    TRUE , /* TRUE if this is a global object class */
                                    _Message_queue_MP_Send_extract_proxy /* Proxy extraction support callout */
#endif
        );

#if defined(RTEMS_MULTIPROCESSING)
    /* Register the MP Process Packet routine */
    _MPCI_Register_packet_processor(MP_PACKET_MESSAGE_QUEUE ,
                                    _Message_queue_MP_Process_packet);
#endif

}

/**  
 *  @}
 */

/**
 *  @}
 */
