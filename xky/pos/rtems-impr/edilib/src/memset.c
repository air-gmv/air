/**
 *  @file
 *
 *  @brief Implementation of the function memset
 *
 *  COPYRIGHT (c) 2010.
 *  Edisoft S.A.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: memset.c 8184 2010-06-15 10:55:59Z mcoutinho $
 *
 *  $Change description: created file to contain the memset function
 *  Author: mcoutinho$
 *
 */

/**
 *  @addtogroup LIBC LibC
 *  @{
 */

#include <stdint.h>
#include <string.h>


/**
 * @brief size of a block
 */
#define LBLOCKSIZE ((uint32_t) sizeof(uintptr_t))

/**
 * @brief check if the address is alligned
 */
#define UNALIGNED(X) ((uintptr_t)X & (LBLOCKSIZE - 1))

/**
 * @brief check if the size is too small
 */
#define TOO_SMALL(LEN) ((LEN) < LBLOCKSIZE)


void *memset(void *m , int c , size_t n)
{

#if defined(PREFER_SIZE_OVER_SPEED) || defined(__OPTIMIZE_SIZE__)

    /* copy the destination address to another variable */
    int8_t *s = (int8_t *) m;


    /* while there are bytes left to copy */
    while(n != 0)
    {
        /* copy the byte */
        *s = (int8_t) c;

        /* increment index on array */
        s++;

        /* decrement number of remaining bytes to copy */
        n--;
    }

    /* return the destination address */
    return m;

#else
    
    /* pointer to address where to write */
    int8_t *s = (int8_t *) m;

    /* iterator */
    uint32_t i;

    /* buffer where to place small array */
    uint32_t buffer;

    /* pointer to address aligned */
    uintptr_t *aligned_addr;

    /* To avoid sign extension, copy c to an unsigned variable.  */
    uint32_t d = (uint32_t) c & 0xffU;


    /* check if the size is too small or the destination address in not aligned */
    if(( TOO_SMALL(n) == 0U ) && ( UNALIGNED(m) == 0U ))
    {
        /* If we get this far, we know that n is large and m is word-aligned. */
        aligned_addr = (uintptr_t *) m;

        /* Store D into each char sized location in BUFFER so that
                we can set large blocks quickly.  */
        if(LBLOCKSIZE == 4U)
        {
            /* copy to the buffer */
            buffer = (uint32_t) ( ( d << 8U ) | d );
            buffer = buffer | ( buffer << 16U );
        }
        else
        {
            /* reset the buffer */
            buffer = 0U;

            /* copy to the buffer the char */
            for(i = 0U; i < LBLOCKSIZE; i++)
            {
                buffer = ( buffer << 8U ) | d;
            }
        }

        /* copy the aligned bytes */
        while(n >= LBLOCKSIZE * 4U)
        {
            /* copy the first four byte word */
            *aligned_addr = buffer;
            aligned_addr++;

            /* copy the second four byte word */
            *aligned_addr = buffer;
            aligned_addr++;

            /* copy the third four byte word */
            *aligned_addr = buffer;
            aligned_addr++;

            /* copy the fourth four byte word */
            *aligned_addr = buffer;
            aligned_addr++;

            /* decrement the number of bytes copied */
            n -= (size_t) ( 4U * LBLOCKSIZE );
        }

        /* copy the remaining bytes */
        while(n >= LBLOCKSIZE)
        {
            /* copy the first four byte word */
            *aligned_addr = buffer;
            aligned_addr++;

            /* decrement the number of bytes copied */
            n -= LBLOCKSIZE;
        }
        /* Pick up the remainder with a bytewise loop.  */
        s = (int8_t*) aligned_addr;
    }

    /* copy the remaining bytes */
    while(n > 0U)
    {
        /* copy the byte */
        *s = (int8_t) d;
        s++;
        /* decrement size */
        n--;
    }

    /* return the destination address */
    return m;

#endif /* not PREFER_SIZE_OVER_SPEED */
}

/**
 *  @}
 */
