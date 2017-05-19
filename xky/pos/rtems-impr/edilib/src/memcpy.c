/**
 *  @file
 *
 *  @brief implementation of the function memcpy
 *
 *  COPYRIGHT (c) 2010.
 *  Edisoft S.A.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: memcpy.c 8184 2010-06-15 10:55:59Z mcoutinho $
 *
 *  $Change description: created file to contain the memcpy function
 *  Author: mcoutinho$
 *
 */

/**
 *  @addtogroup LIBC LibC
 *  @{
 */

#include <stddef.h>
#include <stdint.h>

/**
 * @brief check if either of the addresses are aligned
 *
 * Nonzero if either X or Y is not aligned on a "long" boundary.
 */
#define UNALIGNED(X, Y) \
  (((uint32_t)X & (sizeof (uint32_t) - 1U)) | ((uint32_t)Y & (sizeof (uint32_t) - 1U)))

/**
 * @brief determine how many bytes are copied each iteration of the 4X unrolled
 * loop
 */
#define BIGBLOCKSIZE    ((uint32_t)sizeof (int32_t) << 2U)

/**
 * @brief determine how many bytes are copied each iteration of the word copy
 * loop
 */
#define LITTLEBLOCKSIZE ((uint32_t)sizeof (int32_t))

/**
 * @brief Threshhold for punting to the byte copier
 */
#define TOO_SMALL(LEN)  ((LEN) < BIGBLOCKSIZE)



void *memcpy(void *dst0 , const void *src0 , size_t len0)
{

#if defined(PREFER_SIZE_OVER_SPEED) || defined(__OPTIMIZE_SIZE__)

    /* get destination address as a byte pointer */
    int8_t *dst = (int8_t *) dst0;

    /* get source address as a byte pointer */
    int8_t *src = (const int8_t *) src0;

    
    /* save the destination address */
    void * save = dst0;

    /* copy the source vector to destination for len0 bytes*/
    while(len0--)
    {
        /* copy the source to the destination */
        *dst++ = *src++;
    }

    /* return the saved destination address */
    return save;

#else

    /* get destination address as a byte pointer */
    int8_t *dst = (int8_t *) dst0;

    /* get source address as a byte pointer */
    const int8_t *src = src0;

    /* alligned destination pointer */
    int32_t *aligned_dst;

    /* aligned source pointer */
    const int32_t *aligned_src;

    /* lenght in integer */
    uint32_t len = (uint32_t) len0;


    /* If the size is small, or either SRC or DST is unaligned,
     * then punt into the byte copy loop.  This should be rare.  */
    if(( ( TOO_SMALL(len) ) == 0U ) &&
       ( ( UNALIGNED((uint32_t) src0 , (uint32_t) dst0) ) == 0 ))
    {
        /* get the destination */
        aligned_dst = (int32_t*) dst;

        /* get the source */
        aligned_src = (int32_t*) src;

        /* Copy 4X long words at a time if possible.  */
        while(len >= BIGBLOCKSIZE)
        {
            /* copy the first 4 bytes */
            *aligned_dst = *aligned_src;
            aligned_dst++;
            aligned_src++;

            /* copy the second 4 bytes */
            *aligned_dst = *aligned_src;
            aligned_dst++;
            aligned_src++;

            /* copy the third 4 bytes */
            *aligned_dst = *aligned_src;
            aligned_dst++;
            aligned_src++;

            /* copy the fourth 4 bytes */
            *aligned_dst = *aligned_src;
            aligned_dst++;
            aligned_src++;

            /* decrement the number of bytes left to copy */
            len -= BIGBLOCKSIZE;
        }

        /* Copy one long word at a time if possible.  */
        while(len >= LITTLEBLOCKSIZE)
        {
            /* copy 4 bytes */
            *aligned_dst = *aligned_src;
            aligned_dst++;
            aligned_src++;

            /* decrement the number of bytes left to copy */
            len -= LITTLEBLOCKSIZE;
        }

        /* Pick up any residual with a byte copier.  */
        dst = (int8_t*) aligned_dst;
        src = (int8_t*) aligned_src;
    }

    /* copy residual bytes (maximum of 3) */
    while(len > 0U)
    {
        /* copy each byte */
        *dst = *src;

        /* increment the pointer */
        dst++;
        src++;

        /* decrement number of bytes */
        len--;
    }

    /* return the destination address */
    return dst0;
    
#endif /* not PREFER_SIZE_OVER_SPEED */
}

/**
 *  @}
 */
