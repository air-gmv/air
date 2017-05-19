/**
 *  @file
 *  status.inl
 *
 *  @brief implementations of the inlined
 *  routines for the status package.
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
 *  4546    | 28/09/2009  | mcoutinho    | IPR 778
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

#ifndef _RTEMS_RTEMS_STATUS_INL
#define _RTEMS_RTEMS_STATUS_INL


/**
 *  @brief determine if the return code is successful
 *
 *  @param[in] code the status code to determine
 *
 *  @return returns TRUE if the status code is equal to RTEMS_SUCCESSFUL,
 *  and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean rtems_is_status_successful(
                                                        rtems_status_code code
                                                        )
{
   /* return true if the code is successful */
   return (code == RTEMS_SUCCESSFUL );
}

#endif
/* end of include file */

/**  
 *  @}
 */

/**
 *  @}
 */
