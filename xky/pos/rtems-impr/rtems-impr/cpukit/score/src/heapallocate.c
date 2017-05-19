/**
 *  @file
 *  heapallocate.c
 *
 *  @brief allocate a block from a heap
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
 *  613     | 17/11/2008  | mcoutinho    | IPR 71
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6325    | 01/03/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreHeap Heap Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/heap.h>


void *_Heap_Allocate(
                     Heap_Control *the_heap ,
                     size_t size
                     )
{
    /* size of the block to allocate */
    uint32_t the_size;

    /* iterator throught the heap */
    Heap_Block *the_block;

    /* initialize the user area pointer to NULL */
    void *ptr = NULL;

    /* last block on the heap */
    Heap_Block * const tail = _Heap_Tail(the_heap);


    /* calculate the block size */
    the_size = _Heap_Calc_block_size(size , the_heap->page_size ,
                                     the_heap->min_block_size);

    /* check if the block size is invalid */
    if(the_size == 0)
    {
        /* return null if invalid */
        return NULL;
    }

    /* Find large enough free block. */
    for(the_block = _Heap_First(the_heap);
        the_block != tail;
        the_block = the_block->next)
    {
        /* As we always coalesce free blocks, prev block must have been used. */
        _HAssert(_Heap_Is_prev_used(the_block));

        /* Don't bother to mask out the HEAP_PREV_USED bit as it won't change the
           result of the comparison. */
        if(the_block->size >= the_size)
        {
            /* allocate the block from the heap */
            (void) _Heap_Block_allocate(the_heap , the_block , the_size);

            /* get the user area block address */
            ptr = _Heap_User_area(the_block);

            /* check if the user area is aligned to the page size */
            _HAssert(_Heap_Is_aligned_ptr(ptr , the_heap->page_size));

            /* leave the loop */
            break;
        }
    }

    /* return the user area pointer */
    return ptr;
}

/**  
 *  @}
 */

/**
 *  @}
 */
