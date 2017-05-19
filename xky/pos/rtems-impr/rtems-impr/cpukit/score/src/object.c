/**
 *  @file
 *  object.c
 *
 *  @brief initialize the object handler
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
 *  615     | 17/11/2008  | mcoutinho    | IPR 64
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6325    | 01/03/2010  | mcoutinho    | IPR 1931
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreObject Object Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/address.h>
#include <rtems/score/chain.h>
#include <rtems/score/object.h>
#if defined(RTEMS_MULTIPROCESSING)
    #include <rtems/score/objectmp.h>
#endif
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/isr.h>


#if defined(RTEMS_MULTIPROCESSING)

uint16_t _Objects_Local_node;

uint16_t _Objects_Maximum_nodes;

#endif


Objects_Information **_Objects_Information_table[OBJECTS_APIS_LAST + 1];


void _Objects_Handler_initialization(
#if defined(RTEMS_MULTIPROCESSING)
    uint32_t node ,
                                     uint32_t maximum_nodes ,
                                     uint32_t maximum_global_objects
#endif
    )
{
    /* if multiprocessing is enabled */
#if defined(RTEMS_MULTIPROCESSING)

    /* if the number of nodes is invalid */
    if(node < 1 || node > maximum_nodes)
    {
        /* raise internal error */
        _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                 TRUE ,
                                 INTERNAL_ERROR_INVALID_NODE);
    }

    /* initialize the local node number */
    _Objects_Local_node = node;

    /* initialize the maximum number of nodes */
    _Objects_Maximum_nodes = maximum_nodes;

    /* initialize the Object information */
    _Objects_MP_Handler_initialization(node ,
                                       maximum_nodes ,
                                       maximum_global_objects);
#endif
}

/**  
 *  @}
 */

/**
 *  @}
 */
