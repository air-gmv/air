/**
 *  @file
 *  bsppost.c
 *
 *  @brief This function does nothing. It is a relique of when RTEMS used this
 *  to: open /dev/console for stdin, stdout, and stderr if it exists.
 *
 *  Newlib will automatically associate the file descriptors with the first
 *  thress files opened
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
 *  233     | 09/10/2008  | mcoutinho    | IPR 66
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6116    | 18/02/2010  | mcoutinho    | IPR 1818
 *  8183    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SHARED_BETWEEN_BSPS Shared between BSPs
 *  @{
 */

#include <rtems.h>
#include <sharedBSPs.h>


void bsp_postdriver_hook(void)
{
    /* dont need do to anything */
}

/**  
 *  @}
 */
