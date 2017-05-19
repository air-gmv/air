/**
 *  @file
 *  string.h
 *
 *  @brief Struct and function declarations for dealing with strings.
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
 *  9872    | 16/03/2011  | aconstantino | SPR 2819
 *  9877    | 18/03/2011  | aconstantino | SPR 2846
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
#include_next <string.h>
#else

#ifndef _RTEMS_STRING_H
#define _RTEMS_STRING_H

/* otherwise, include our definitions */

#include <stddef.h>

/**
 * @brief copy n bytes from source memory area to destination memory area
 *
 * @param[out] dst0 destination address
 * @param[in] src0 source address
 * @param[in] len0 number of bytes to copy
 *
 * @return returns a pointer to dst0
 **/
void *memcpy(void *dst0 , const void *src0 , size_t len0);


/**
 * @brief fill the first n bytes of a memory area with a constant byte
 *
 * @param[out] m destination address
 * @param[in] c byte to set
 * @param[in] n number of bytes to set
 *
 * @return returns a pointer to m
 **/
void *memset(void *m , int c , size_t n);


/**
 *  @brief compare the first n bytes of two strings
 *
 *  character string compare
 *
 *  This function compares up to n characters
 *  from the string s1 to the string s2.
 *
 *  @param[in] s1 first string to compare
 *  @param[in] s2 second string to compare
 *  @param[in] n number of bytes to compare
 *
 *  @return If s1 sorts lexicographically after, strncmp returns a number
 *  greater than zero. If the two strings are equivalent, strncmp returns zero.
 *  If s1 sorts lexicographically before s2, strncmp returns a
 *  number less than zero.
 */
int strncmp(const char *s1 , const char *s2 , size_t n);

#endif /* RTEMS_USES_NEWLIB */

#endif /* _STRING_H */

/**
 *  @}
 */

/**
 *  @}
 */
