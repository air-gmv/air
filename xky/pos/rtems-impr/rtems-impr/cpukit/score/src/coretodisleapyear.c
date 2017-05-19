/**
 *  @file
 *  coretodisleapyear.c
 *
 *  @brief determine if the year is a leap year
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 2008
 *  EDISOFT - Empresa de Serviços e Desenvolvimento de Software, S.A.
 *
 *  http://rtemscentre.edisoft.pt
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  234     | 09/10/2008  | mcoutinho    | IPR 67
 *  9621    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9877 $ | $Date: 2011-03-18 18:39:36 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/


/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreTOD Time Of Day (TOD) Handler
 *  @{
 */

#include <stdint.h>


int32_t _Is_Leap_Year(int32_t y)
{
    /* return 1 if the year is divisible by 4 and not by 100 or
     * is divisible by 400 */
    return (((y % 4) == 0) && ((y % 100) != 0)) || ((y % 400) == 0);
}

/**
 *  @}
 */

/**
 *  @}
 */
