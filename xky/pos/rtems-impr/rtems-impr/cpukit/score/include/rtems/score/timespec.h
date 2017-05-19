/**
 *  @file
 *  timespec.h
 *
 *  @brief contains helpers for manipulating timespecs.
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6116    | 18/02/2010  | mcoutinho    | IPR 1818
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

#ifndef _RTEMS_SCORE_TIMESPEC_H
#define _RTEMS_SCORE_TIMESPEC_H

/**
 *  @defgroup HELPERS Helpers
 *
 *  @brief This handler encapsulates functionality related to manipulating
 *  struct timespecs.
 */
/**@{*/

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/score/types.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

   /**
    *  @brief Add two timespecs
    *
    *  This routine adds two timespecs.  The second argument is added
    *  to the first.
    *
    *  @param[in] time is the base time to be added to
    *  @param[in] add is the timespec to add to the first argument
    *
    *  @return This method returns the number of seconds @a time increased by.
    */
   uint32_t _Timespec_Add_to(
                             struct timespec *time ,
                             const struct timespec *add
                             );

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */
