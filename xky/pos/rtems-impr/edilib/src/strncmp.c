/**
 *  @file
 *
 *  @brief Implementation of the function strncmp
 *
 *  COPYRIGHT (c) 2010.
 *  Edisoft S.A.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: strncmp.c 8184 2010-06-15 10:55:59Z mcoutinho $
 *
 *  $Change description: created file to contain the strncmp function
 *  Author: mcoutinho$
 *
 */

/**
 *  @addtogroup LIBC LibC
 *  @{
 */

#include <string.h>
#include <stdint.h>


int strncmp(const char *s1 , const char *s2 , size_t n)
{
    /* return value */
    int32_t res = 0;

    /* iterator index */
    uint32_t index = 0U;


    /* iterate through the strings while they are equal or the end
     * of a string has been reached */
    while(( index < n ) && ( s1[index] == s2[index] ) && ( s1[index] != 0 ))
    {
        /* increment the pointer in each string */
        index++;
    }

    /* if the index was the last position */
    if(index == n)
    {
        /* then all vector entries are equal */
        res = 0;
    }
    else
    {
        /* else, return the comparasion between the last char on the
         * string analyzed */
        res = (int32_t) s1[index] - (int32_t) s2[index];
    }

    /* return the result */
    return res;
}

/**
 *  @}
 */
