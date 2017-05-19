/**
 *  @file
 *  heapfree.c
 *
 *  @brief free a block from a heap
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
 *  614     | 17/11/2008  | mcoutinho    | IPR 71
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


boolean _Heap_Free(
                   Heap_Control *the_heap ,
                   void *starting_address
                   )
{
    /* block to free */
    Heap_Block *the_block;

    /* next block of the block to free */
    Heap_Block *next_block;

    /* size of the block to free */
    uint32_t the_size;

    /* size of the next block */
    uint32_t next_size;

    /* flag indicating if the next block is free */
    boolean next_is_free;
    

    /* check if the starting address is valid */
    if(!_Addresses_Is_in_range(starting_address ,
                               (void *) the_heap->start ,
                               (void *) the_heap->final))
    {
        /* check if the starting address is NULL */
        _HAssert(starting_address != NULL);

        /* return FALSE (starting address not valid) */
        return ( FALSE );
    }

    /* calculate the start of block of the starting address */
    _Heap_Start_of_block(the_heap , starting_address , &the_block);

    /* check if the block is not inside the heap */
    if(!_Heap_Is_block_in(the_heap , the_block))
    {
        /* always assert (should ALWAYS be inside the heap) */
        _HAssert(FALSE);

        /* return false */
        return ( FALSE );
    }

    /* determine the next block address */
    the_size = _Heap_Block_size(the_block);
    next_block = _Heap_Block_at(the_block , the_size);

    /* check if the next block is inside the heap */
    if(!_Heap_Is_block_in(the_heap , next_block))
    {
        /* always assert (should ALWAYS be inside the heap) */
        _HAssert(FALSE);

        /* return false */
        return ( FALSE );
    }

    /* check if the previous block of the next block (should be the block) is used */
    if(!_Heap_Is_prev_used(next_block))
    {
        /* always assert (previous block should ALWAYS be used) */
        _HAssert(FALSE);

        /* return false */
        return ( FALSE );
    }

    /* determine the size of the next block */
    next_size = _Heap_Block_size(next_block);

    /* determine if the next block is being used */
    next_is_free = next_block < the_heap->final &&
        !_Heap_Is_prev_used(_Heap_Block_at(next_block , next_size));

    if(!_Heap_Is_prev_used(the_block))
    {
        /* get the previous block size */
        uint32_t const prev_size = the_block->prev_size;

        /* get the previous block */
        Heap_Block * const prev_block = _Heap_Block_at(the_block , -prev_size);

        /* check if the previous block is inside the heap */
        if(!_Heap_Is_block_in(the_heap , prev_block))
        {
            /* always assert (should ALWAYS be inside the heap) */
            _HAssert(FALSE);

            /* return false */
            return ( FALSE );
        }

        /* As we always coalesce free blocks, the block that preceedes prev_block
           must have been used. */
        if(!_Heap_Is_prev_used(prev_block))
        {
            /* always assert (should ALWAYS be used) */
            _HAssert(FALSE);

            /* return false */
            return ( FALSE );
        }

        /* if next block is used */
        if(next_is_free)
        {
            /* coalesce both */
            uint32_t const size = the_size + prev_size + next_size;

            /* remnove the next block */
            _Heap_Block_remove(next_block);

            /* set the previous block size */
            prev_block->size = size | HEAP_PREV_USED;

            /* determine the next block */
            next_block = _Heap_Block_at(prev_block , size);

            /* check if the previous block of the next block (should be the block)
             * is still being used */
            _HAssert(!_Heap_Is_prev_used(next_block));

            /* set the next block size */
            next_block->prev_size = size;
        }
        else
        {
            /* coalesce prev */
            uint32_t const size = the_size + prev_size;
            prev_block->size = size | HEAP_PREV_USED;
            next_block->size &= ~HEAP_PREV_USED;
            next_block->prev_size = size;
        }
    }
    else if(next_is_free)
    {
        /* coalesce next */
        uint32_t const size = the_size + next_size;
        _Heap_Block_replace(next_block , the_block);
        the_block->size = size | HEAP_PREV_USED;
        next_block = _Heap_Block_at(the_block , size);
        next_block->prev_size = size;
    }
    else
    {
        /* no coalesce */

        /* add 'the_block' to the head of the free blocks list as it tends to
         * produce less fragmentation than adding to the tail */
        _Heap_Block_insert_after(_Heap_Head(the_heap) , the_block);
        the_block->size = the_size | HEAP_PREV_USED;
        next_block->size &= ~HEAP_PREV_USED;
        next_block->prev_size = the_size;
    }

    /* return true, could free the block */
    return ( TRUE );
}

/**  
 *  @}
 */

/**
 *  @}
 */
