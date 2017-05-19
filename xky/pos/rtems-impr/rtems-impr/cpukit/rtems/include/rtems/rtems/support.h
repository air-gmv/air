/**
 *  @file
 *  support.h
 *
 *  @brief contains information about support functions for
 *  the RTEMS API.
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
 *  4844    | 15/10/2009  | mcoutinho    | IPR 2517
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

#ifndef _RTEMS_RTEMS_SUPPORT_H
#define _RTEMS_RTEMS_SUPPORT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/rtems/types.h>

   /**
    *  @brief build a rtems name
    *
    *  This function returns an object name composed of the four characters
    *  C1, C2, C3, and C4.
    *
    *  NOTE:
    *  This must be implemented as a macro for use in Configuration Tables.
    *
    */
#define rtems_build_name( _C1, _C2, _C3, _C4 ) \
  ( (uint32_t  )(_C1) << 24 | (uint32_t  )(_C2) << 16 | (uint32_t  )(_C3) << 8 | (uint32_t  )(_C4) )

   /**
    *  @brief get object identifier index
    *
    *  This function returns the index portion of the ID.
    */
#define rtems_get_index( _id ) \
  _Objects_Get_index( _id )

   /*
    *  Time related
    */

#define RTEMS_MILLISECONDS_TO_MICROSECONDS(_ms) \
        TOD_MILLISECONDS_TO_MICROSECONDS(_ms)
#define RTEMS_MILLISECONDS_TO_TICKS(_ms) \
        TOD_MILLISECONDS_TO_TICKS(_ms)
#define RTEMS_MICROSECONDS_TO_TICKS(_ms) \
        TOD_MICROSECONDS_TO_TICKS(_ms)

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/support.inl>
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
