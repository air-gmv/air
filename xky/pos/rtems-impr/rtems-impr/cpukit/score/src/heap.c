/**
 *  @file
 *  heap.c
 *
 *  @brief initialize the Heap Handler
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  612     | 17/11/2008  | mcoutinho    | IPR 71
 *  3617    | 02/07/2009  | mcoutinho    | IPR 97
 *  5059    | 27/10/2009  | mcoutinho    | IPR 828
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


uint32_t _Heap_Initialize(
                          Heap_Control *the_heap ,
                          void *starting_address ,
                          size_t size ,
                          uint32_t page_size
                          )
{
    /* first block of the heap */
    Heap_Block *the_block;

    /* block size */
    uint32_t the_size;

    /* start of the heap */
    _H_uptr_t start;

    /* aligned start of the heap */
    _H_uptr_t aligned_start;

    /* overhead need at the end of the first block */
    uint32_t overhead;

    /* returns the maximum available memory for this heap */
    uint32_t result;


    /* check if the page size is 0 */
    if(page_size == 0)
    {
        /* set the page size to the minimum */
        page_size = CPU_ALIGNMENT;
    }
    else
    {
        /* align the page size up to be multiple of CPU_ALIGNMENT */
        _Heap_Align_up(&page_size , CPU_ALIGNMENT);
    }

    /* calculate aligned_start so that aligned_start + HEAP_BLOCK_USER_OFFSET
     * (value of user pointer) is aligned on 'page_size' boundary. Make sure
     * resulting 'aligned_start' is not below 'starting_address'. */
    start = _H_p2u(starting_address);
    aligned_start = start + HEAP_BLOCK_USER_OFFSET;
    _Heap_Align_up_uptr(&aligned_start , page_size);
    aligned_start -= HEAP_BLOCK_USER_OFFSET;

    /* calculate 'min_block_size'. It's HEAP_MIN_BLOCK_SIZE aligned up to the
     * nearest multiple of 'page_size' */
    the_heap->min_block_size = HEAP_MIN_BLOCK_SIZE;
    _Heap_Align_up(&the_heap->min_block_size , page_size);

    /* calculate 'the_size' -- size of the first block so that there is enough
     * space at the end for the permanent last block. It is equal to 'size'
     * minus total overhead aligned down to the nearest multiple of
     * 'page_size' */
    overhead = HEAP_OVERHEAD + ( aligned_start - start );

    /* check if the size is small */
    if(size < overhead)
    {
        /* too small area for the heap */
        result = 0;
    }
    else
    {
        the_size = size - overhead;
        _Heap_Align_down(&the_size , page_size);

        /* check the size */
        if(the_size == 0)
        {
            /* too small area for the heap */
            result = 0;
        }
        else
        {
            /* initialize the heap page size */
            the_heap->page_size = page_size;

            /* initialize the heap start address */
            the_heap->begin = starting_address;

            /* initialize the heap end address */
            the_heap->end = starting_address + size;

            /* initialize the heap first (and only) block */
            the_block = (Heap_Block *) aligned_start;

            the_block->prev_size = page_size;
            the_block->size = the_size | HEAP_PREV_USED;
            the_block->next = _Heap_Tail(the_heap);
            the_block->prev = _Heap_Head(the_heap);

            /* the heap head next pointer points to the block */
            _Heap_Head(the_heap)->next = the_block;

            /* the heap head previous pointer points to the block */
            _Heap_Tail(the_heap)->prev = the_block;

            /* set the start of the heap to the block */
            the_heap->start = the_block;

            /* assert that the heap page size is aligned to CPU_ALIGNMENT */
            _HAssert(_Heap_Is_aligned(the_heap->page_size , CPU_ALIGNMENT));

            /* assert that the heap minimum block size is aligned to the page size */
            _HAssert(_Heap_Is_aligned(the_heap->min_block_size , page_size));

            /* assert that the the user area of the block is aligned to the page size */
            _HAssert(_Heap_Is_aligned_ptr(_Heap_User_area(the_block) , page_size));

            the_block = _Heap_Block_at(the_block , the_size);

            /* permanent final block of the heap */
            the_heap->final = the_block;

            /* previous block is free */
            the_block->prev_size = the_size;

            the_block->size = page_size;

            result = ( the_size - HEAP_BLOCK_USED_OVERHEAD );
        }
    }

    /* return maximum memory available */
    return result;
}


size_t _Heap_Calc_block_size(
                             size_t size ,
                             uint32_t page_size ,
                             uint32_t min_size)
{
    /* block size [in bytes] */
    uint32_t block_size = size + HEAP_BLOCK_USED_OVERHEAD;

    
    /* align the block size by the page size */
    _Heap_Align_up(&block_size , page_size);

    /* check if the block size is too little */
    if(block_size < min_size)
    {
        block_size = min_size;
    }

    /* check if an overflow occurred */
    if(block_size <= size)
    {
        /* reset the block size */
        block_size = 0;
    }

    /* return the block size */
    return block_size;
}


uint32_t _Heap_Block_allocate(
                              Heap_Control* the_heap ,
                              Heap_Block* the_block ,
                              uint32_t alloc_size
                              )
{
    /* block size [in bytes] */
    uint32_t const block_size = _Heap_Block_size(the_block);

    /* free space */
    uint32_t const the_rest = block_size - alloc_size;


    /* assert if the block is aligned to the page_size */
    _HAssert(_Heap_Is_aligned(block_size , the_heap->page_size));

    /* assert if the allocated size is aligned to the page_size */
    _HAssert(_Heap_Is_aligned(alloc_size , the_heap->page_size));

    /* assert if the allocated size is smaller than the block size */
    _HAssert(alloc_size <= block_size);

    /* assert if the heap previous block is being used */
    _HAssert(_Heap_Is_prev_used(the_block));

    /* if the free space of the block is larger than the minimum block size */
    if(the_rest >= the_heap->min_block_size)
    {
        /* then split the block so that upper part is still free, and
         * lower part becomes used. This is slightly less optimal than leaving
         * lower part free as it requires replacing block in the free blocks list,
         * but it makes it possible to reuse this code in the _Heap_Resize_block(). */
        Heap_Block *next_block = _Heap_Block_at(the_block , alloc_size);
        _Heap_Block_replace(the_block , next_block);
        the_block->size = alloc_size | HEAP_PREV_USED;
        next_block->size = the_rest | HEAP_PREV_USED;
        _Heap_Block_at(next_block , the_rest)->prev_size = the_rest;
    }
    else
    {
        /* otherwise, don't split the block as remainder is either zero or too
         * small to be used as a separate free block. Change 'alloc_size' to the
         * size of the block and remove the block from the list of free blocks */
        _Heap_Block_remove(the_block);
        alloc_size = block_size;
        _Heap_Block_at(the_block , alloc_size)->size |= HEAP_PREV_USED;
    }

    /* return the allocated size */
    return alloc_size;
}

/**  
 *  @}
 */

/**
 *  @}
 */
