/**
 *  @file
 *  rtems.h
 *
 *  @brief information about RTEMS executive that
 *  is required by the application and is CPU independent.  It includes
 *  two (2) CPU dependent files to tailor its data structures for a
 *  particular processor.
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
 *  297     | 15/10/2008  | mcoutinho    | IPR 69
 *  565     | 17/11/2008  | mcoutinho    | IPR 64
 *  4844    | 15/10/2009  | mcoutinho    | IPR 2517
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @defgroup RTEMS_API RTEMS API
 *
 *  @brief RTEMS API is the standard API provided by RTEMS
 *
 *  RTEMS interfaces with the application through the RTEMS API component. The
 *  application should use this interface (as well as possibly others) to
 *  communicate with RTEMS.
 *
 *  @{
 */

/**
 *  @defgroup RTEMS_API_COMMON Common
 *
 *  @brief The Common component contains the code common for the RTEMS API
 *  @{
 */

#ifndef _RTEMS_H
#define _RTEMS_H

#ifdef __cplusplus
extern "C"
{
#endif

   /*
    *  Unless told otherwise, the RTEMS include files will hide some stuff
    *  from normal application code.  Defining this crosses a boundary which
    *  is undesirable since it means your application is using RTEMS features
    *  which are not included in the formally defined and supported API.
    *  Define this at your own risk.
    */

#if (!defined(__RTEMS_VIOLATE_KERNEL_VISIBILITY__)) && (!defined(__RTEMS_INSIDE__))
#define __RTEMS_APPLICATION__
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>

#include <rtems/config.h>
#include <rtems/init.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/tasks.h>
#include <rtems/rtems/intr.h>
#include <rtems/rtems/cache.h>
#include <rtems/rtems/clock.h>
#include <rtems/extension.h>
#include <rtems/rtems/timer.h>
#include <rtems/rtems/sem.h>
#include <rtems/rtems/message.h>
#include <rtems/rtems/event.h>
#include <rtems/rtems/event.h>
#include <rtems/io.h>
#include <rtems/fatal.h>
#include <rtems/rtems/ratemon.h>

#include <rtems/rtems/support.h>
#include <rtems/score/sysstate.h>

#define RTEMS_HAS_HARDWARE_FP CPU_HARDWARE_FP

   /*
    *  The following define the constants which may be used in name searches.
    */

#define RTEMS_SEARCH_ALL_NODES   OBJECTS_SEARCH_ALL_NODES
#define RTEMS_SEARCH_OTHER_NODES OBJECTS_SEARCH_OTHER_NODES
#define RTEMS_SEARCH_LOCAL_NODE  OBJECTS_SEARCH_LOCAL_NODE
#define RTEMS_WHO_AM_I           OBJECTS_WHO_AM_I

   /*
    * Parameters and return id's for _Objects_Get_next
    */

#define RTEMS_OBJECT_ID_INITIAL_INDEX        OBJECTS_ID_INITIAL_INDEX
#define RTEMS_OBJECT_ID_FINAL_INDEX          OBJECTS_ID_FINAL_INDEX
#define RTEMS_OBJECT_ID_INITIAL(api, class, node) OBJECTS_ID_INITIAL(api, class, node)

#define RTEMS_OBJECT_ID_FINAL                OBJECTS_ID_FINAL

   /**
    *  @brief determine the minimum stack size which every
    *  thread must exceed.
    */
#define RTEMS_MINIMUM_STACK_SIZE  STACK_MINIMUM_SIZE

   /**
    *  @brief Constant for indefinite wait.  (actually an illegal interval)
    */
#define RTEMS_NO_TIMEOUT  WATCHDOG_NO_TIMEOUT

#if defined(RTEMS_MULTIPROCESSING)
   /*
    *  An MPCI must support packets of at least this size.
    */

#define RTEMS_MINIMUM_PACKET_SIZE  MP_PACKET_MINIMUM_PACKET_SIZE

   /*
    *  The following constant defines the number of uint32_t  's
    *  in a packet which must be converted to native format in a
    *  heterogeneous system.  In packets longer than
    *  MP_PACKET_MINIMUN_HETERO_CONVERSION uint32_t  's, some of the "extra" data
    *  may a user message buffer which is not automatically endian swapped.
    */

#define RTEMS_MINIMUN_HETERO_CONVERSION  MP_PACKET_MINIMUN_HETERO_CONVERSION
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
