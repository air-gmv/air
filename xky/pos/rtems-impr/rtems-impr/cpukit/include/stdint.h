/**
 *  @file
 *  stdint.h
 *
 *  @brief handles the standard integer types
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
 *  9622    | 23/02/2011  | mcoutinho    | IPR 67
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
#include_next <stdint.h>
#else

#ifndef _RTEMS_STDINT_H
#define _RTEMS_STDINT_H

/* otherwise, include our definitions */

/* definition of the basic types for a 32-bit processor */
#if (__INT_MAX__ == 0x7fffffff)

/**
 * @brief signed 8 bit integer
 **/
typedef signed char int8_t;

/**
 * @brief unsigned 8 bit integer
 **/
typedef unsigned char uint8_t;

/**
 * @brief signed 16 bit integer
 **/
typedef signed short int16_t;

/**
 * @brief unsigned 16 bit integer
 **/
typedef unsigned short uint16_t;

/**
 * @brief signed 32 bit integer
 **/
typedef signed long int32_t;

/**
 * @brief unsigned 32 bit integer
 **/
typedef unsigned long uint32_t;

/**
 * @brief signed 64 bit integer
 **/
typedef signed long long int64_t;

/**
 * @brief unsigned 64 bit integer
 **/
typedef unsigned long long uint64_t;

/**
 * @brief signed pointer
 **/
typedef signed long intptr_t;

/**
 * @brief unsigned pointer
 **/
typedef unsigned long uintptr_t;

/**
 * @brief definition for the maximum signed integer
 *
 * @note the compiler defines __INT_MAX__
 **/
#define INT_MAX __INT_MAX__

/**
 * @brief definition for the minimum signed integer
 **/
#define INT_MIN (-INT_MAX - 1)

/**
 * @brief definition for the maximum unsigned integer
 **/
#define UINT_MAX (INT_MAX * 2U + 1U)

#else

#error "Edilib only supported for 32-bit machines"

#endif

#endif /* RTEMS_USES_NEWLIB */

#endif /* _RTEMS_STDINT_H */

/**
 *  @}
 */

/**
 *  @}
 */
