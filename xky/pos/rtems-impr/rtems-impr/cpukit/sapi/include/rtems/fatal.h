/**
 *  @file
 *  fatal.h
 *
 *  @brief constants and prototypes related to the Fatal Error Manager.
 *  This manager processes all fatal or irrecoverable errors.
 *
 *  This manager provides directives to:
 *\n
 *     + announce a fatal error has occurred\n
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
 *  @addtogroup SUPER_API Super API
 *  @{
 */

/**
 *  @defgroup SUPER_API_FATAL Fatal Error Manager
 *
 *  @brief The Fatal Error Manager consists on an API that allows the user to announce
 *  that an application fatal error occurred.
 *  @{
 */

#ifndef _RTEMS_FATAL_H
#define _RTEMS_FATAL_H

#ifdef __cplusplus
extern "C"
{
#endif

   /**
    *  @brief announce that a fatal error occurred
    *
    *  This is the routine which implements the rtems_fatal_error_occurred
    *  directive.  It is invoked when the application or RTEMS
    *  determines that a fatal error has occurred.
    *
    *  @param[in] the_error the error
    */
   void rtems_fatal_error_occurred(
                                   uint32_t the_error
                                   );

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

