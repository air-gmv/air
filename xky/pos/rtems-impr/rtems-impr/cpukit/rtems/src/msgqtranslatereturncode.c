/**
 *  @file
 *  msgqtranslatereturncode.c
 *
 *  @brief translate the message queue internal code to a rtems_status_code
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
 *  7089    | 09/04/2010  | mcoutinho    | IPR 1931
 *  7253    | 12/04/2010  | mcoutinho    | IPR 2266
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

RTEMS_STATIC_ATTRIBUTE rtems_status_code
_Message_queue_Translate_core_return_code_[] = {
                                                /* CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL */
                                                RTEMS_SUCCESSFUL ,

                                                /* CORE_MESSAGE_QUEUE_STATUS_INVALID_SIZE */
                                                RTEMS_INVALID_SIZE ,

                                                /* CORE_MESSAGE_QUEUE_STATUS_TOO_MANY */
                                                RTEMS_TOO_MANY ,

                                                /* CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED */
                                                RTEMS_UNSATISFIED ,

                                                /* CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_NOWAIT */
                                                RTEMS_UNSATISFIED ,

                                                /* CORE_MESSAGE_QUEUE_STATUS_WAS_DELETED */
                                                RTEMS_OBJECT_WAS_DELETED ,

                                                /* CORE_MESSAGE_QUEUE_STATUS_TIMEOUT */
                                                RTEMS_TIMEOUT
};


rtems_status_code _Message_queue_Translate_core_message_queue_return_code(
                                                                          uint32_t status
                                                                          )
{
    /* if multiprocessing is enabled */
#if defined(RTEMS_MULTIPROCESSING)

    /* check if the status is proxy blocking */
    if(status == THREAD_STATUS_PROXY_BLOCKING)
    {
        /* then return proxy blocking status */
        return RTEMS_PROXY_BLOCKING;
    }
    else
    {

#endif
        /* check the if the status is not on the table */
        if(status > CORE_MESSAGE_QUEUE_STATUS_TIMEOUT)
        {
            /* internal error occurred, error should be on the table */
            return RTEMS_INTERNAL_ERROR;
        }
        else
        {
            /* otherwise, translate the code according to the table */
            return _Message_queue_Translate_core_return_code_[status];
        }

        /* if multiprocessing is enabled */
#if defined(RTEMS_MULTIPROCESSING)

        /* end the bracket of multiprocessing */
    }
    
#endif
    
}

/**  
 *  @}
 */

/**
 *  @}
 */
