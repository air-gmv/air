/**
 *  @file
 *  options.h
 *
 *  @brief contains information which defines the
 *  options available on many directives.
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
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_COMMON Common
 *  @{
 */

#ifndef _RTEMS_RTEMS_OPTIONS_H
#define _RTEMS_RTEMS_OPTIONS_H

#ifdef __cplusplus
extern "C"
{
#endif

   /**
    *  @brief define the control block used to manage
    *  option sets.
    */
   typedef uint32_t rtems_option;

   /*
    *  The following constants define the individual options which may
    *  be used to compose an option set.
    */


   /**
    * @brief define the default options (wait or events all)
    **/
#define RTEMS_DEFAULT_OPTIONS   0x00000000

   /**
    * @brief define the wait on resource
    **/
#define RTEMS_WAIT      0x00000000

   /**
    * @brief define the no wait on resource
    **/
#define RTEMS_NO_WAIT   0x00000001

   /**
    * @brief define the wait for all events
    **/
#define RTEMS_EVENT_ALL 0x00000000

   /**
    * @brief define the wait on any event
    **/
#define RTEMS_EVENT_ANY 0x00000002

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/options.inl>
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
