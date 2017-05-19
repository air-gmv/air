/**
 *  @file
 *  stddef.h
 *
 *  @brief header file to define the NULL macro and standard types
 *
 *  COPYRIGHT (c) 2010.
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  9621    | 23/02/2011  | mcoutinho    | IPR 67
 *  9622    | 23/02/2011  | mcoutinho    | IPR 451
 *  9877    | 18/03/2011  | aconstantino | SPR 2819
 *  $Rev: 9880 $ | $Date: 2011-03-18 19:58:36 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2853
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

/* if using the newlib, then */
#ifdef RTEMS_USES_NEWLIB
/* include the newlib header file */
#include_next <stddef.h>
#else

#ifndef _RTEMS_STDDEF_H
#define _RTEMS_STDDEF_H

/* otherwise, include our definitions */

/**
 * @brief null pointer definition
 */
#define NULL ((void *)0)

/**
 * @brief standard size definition
 */
typedef long unsigned int size_t;

#endif /* _RTEMS_STDDEF_H */

#endif /* RTEMS_USES_NEWLIB */

/**
 *  @}
 */

/**
 *  @}
 */

