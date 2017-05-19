/**
 *  @file
 *  heap.inl
 *
 *  @brief contains the static inline implementation of the inlined
 *  routines from the heap handler.
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
 *  4546    | 28/09/2009  | mcoutinho    | IPR 778
 *  4773    | 09/10/2009  | mcoutinho    | IPR 802
 *  5247    | 30/10/2009  | mcoutinho    | IPR 828
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
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

#ifndef _RTEMS_SCORE_HEAP_INL
#define _RTEMS_SCORE_HEAP_INL

#include <rtems/score/address.h>
#include <rtems/system.h>


/**
 *  @brief get the heap head
 *  
 *  This function returns the head of the specified heap.
 *  
 *  @param[in] the_heap points to the heap being operated upon
 *  
 *  @return This method returns a pointer to the dummy head of the free
 *  block list.
 */
RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Head(
                                            Heap_Control *the_heap
                                            )
{
   /* get the heap free list (equal to the heap head) */
   return &the_heap->free_list;
}


/**
 *  @brief get the heap tail
 *  
 *  This function returns the tail of the specified heap.
 *  
 *  @param[in] the_heap points to the heap being operated upon
 *  
 *  @return This method returns a pointer to the dummy tail of the heap
 *  free list.
 */
RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Tail(
                                            Heap_Control *the_heap
                                            )
{
   /* get the heap free list (equal to the heap tail) */
   return &the_heap->free_list;
}


/**
 *  @brief get the heap first block
 *  
 *  Return the first free block of the specified heap.
 *  
 *  @param[in] the_heap points to the heap being operated upon
 *  
 *  @return This method returns a pointer to the first free block.
 */
RTEMS_INLINE_ROUTINE Heap_Block *_Heap_First(
                                             Heap_Control *the_heap
                                             )
{
   /* get the heap first block */
   return _Heap_Head(the_heap)->next;
}


/**
 *  @brief remove a block from the heap
 *  
 *  This function removes 'the_block' from doubly-linked list.
 *  
 *  @param[in] the_block is the block to remove from the heap used block
 *  list.
 */
RTEMS_INLINE_ROUTINE void _Heap_Block_remove(
                                             Heap_Block *the_block
                                             )
{
   /* block to remove */
   Heap_Block *block = the_block;

   /* next block */
   Heap_Block *next = block->next;

   /* previous block */
   Heap_Block *prev = block->prev;


   /* set the previous block next field to the next block */
   prev->next = next;

   /* set the next block previous field to the previous block */
   next->prev = prev;
}


/**
 *  @brief replace an old block by a new block of a heap
 *  
 *  This function replaces @a old_block by @a new_block in doubly-linked list.
 *  When a block is allocated, the memory is allocated from the low memory
 *  address range of the block.  This means that the upper address range of
 *  the memory block must be added to the free block list in place of the
 *  lower address portion being allocated.  This method is also used as part
 *  of resizing a block.
 *  
 *  @param[in] old_block is the block which is currently on the list.
 *  @param[in] new_block is the new block which will replace it on the list.
 */
RTEMS_INLINE_ROUTINE void _Heap_Block_replace(
                                              Heap_Block *old_block ,
                                              Heap_Block *new_block
                                              )
{
   /* auxiliary variable */
   Heap_Block *block = old_block;

   /* next block (of the old block) */
   Heap_Block *next = block->next;

   /* previous block (of the old block) */
   Heap_Block *prev = block->prev;

   /* now we have the following configuration:
    *   prev -> old_block -> next
    *   prev <- old_block <- next */

   /* update the block to the new block */
   block = new_block;

   /* initialize the block next field */
   block->next = next;

   /* initialize the block previous field */
   block->prev = prev;

   /* set the previous block next field to the new block */
   prev->next = block;

   /* set the next block previous field to the new block */
   next->prev = block;

   /* now we have the following configuration:
    *   prev -> new_block -> next
    *   prev <- new_block <- next */
}


/**
 *  @brief insert a block after another block in a heap
 *  
 *  This function inserts @a the_block after @a prev_block
 *  in the doubly-linked free block list.
 *  
 *  @param[in] prev_block is the previous block (@a the_block will be inserted after this one)
 *  @param[in] the_block is the block being inserted.
 */
RTEMS_INLINE_ROUTINE void _Heap_Block_insert_after(
                                                   Heap_Block *prev_block ,
                                                   Heap_Block *the_block
                                                   )
{
   /* previous block */
   Heap_Block *prev = prev_block;

   /* block */
   Heap_Block *block = the_block;

   /* next block */
   Heap_Block *next = prev->next;


   /* uptade the block next field */
   block->next = next;

   /* uptade the block previous field */
   block->prev = prev;

   /* update the previous block next field */
   prev->next = block;

   /* update the next block previous field */
   next->prev = block;

   /* now we have the following configuration:
    *   prev_block -> the_block -> next
    *   prev_block <- the_block <- next */
}


/**
 *  @brief determine if the heap is aligned
 *  
 *  Return TRUE if @a value is a multiple of @a alignment,  FALSE otherwise
 *  
 *  @param[in] value is the address to verify alignment of.
 *  @param[in] alignment is the alignment factor to verify.
 *  
 *  @return This method returns TRUE if the address is aligned and false
 *  otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Heap_Is_aligned(
                                              uint32_t value ,
                                              uint32_t alignment
                                              )
{
   /* if the remainder of the value by the aligment is 0
    * then the address is aligned */
   return (value % alignment ) == 0;
}


/**
 *  @brief align the heap up
 *  
 *  Align @a *value up to the nearest multiple of @a alignment.
 *  
 *  @param[out] value is a pointer to be aligned.
 *  @param[in] alignment is the alignment value.
 */
RTEMS_INLINE_ROUTINE void _Heap_Align_up(
                                         uint32_t *value ,
                                         uint32_t alignment
                                         )
{
   /* copy of the pointed by the value */
   uint32_t v = *value;

   /* copy of the alignment */
   uint32_t a = alignment;

   /* remainder of the value by the aligment */
   uint32_t r = v % a;


   /* if the value is not aligned */
   if(r)
   {
      /* then align it */
      *value = v - r + a;
   }
   else
   {
      /* else, dont need to align it */
      *value = v;
   }

}


/**
 *  @brief align the heap down
 *  
 *  Align @a *value down to the nearest multiple of @a alignment.
 *  
 *  @param[out] value is a pointer to be aligned.
 *  @param[in] alignment is the alignment value.
 */
RTEMS_INLINE_ROUTINE void _Heap_Align_down(
                                           uint32_t *value ,
                                           uint32_t alignment
                                           )
{
   /* copy of the pointed by the value */
   uint32_t v = *value;

   /* first, calculate the amount is not aligned ( v % align )
    * then decrease that value and store it */
   *value = v - ( v % alignment );
}


/**
 *  @brief determine if the heap (given a pointer) is aligned
 *  
 *  Return TRUE if @a ptr is aligned at @a alignment boundary,
 *  FALSE otherwise.
 *  
 *  @param[in] ptr is the pointer to verify alignment of.
 *  @param[in] alignment is the alignment factor.
 *  
 *  @return This method returns TRUE if @a ptr is aligned at @a alignment
 *  boundary, and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Heap_Is_aligned_ptr(
                                                  void *ptr ,
                                                  uint32_t alignment
                                                  )
{
   /* if the remainder of the value by the aligment is 0
    * then the address is aligned */
   return (_H_p2u(ptr) % alignment ) == 0;
}


/**
 *  @brief align the heap up by a specified value
 *  
 *  Align @a *value up to the nearest multiple of @a alignment.
 *  
 *  @param[out] value is a pointer to be aligned.
 *  @param[in] alignment is the alignment value.
 *  
 */
RTEMS_INLINE_ROUTINE void _Heap_Align_up_uptr(
                                              _H_uptr_t *value ,
                                              uint32_t alignment
                                              )
{
   /* copy of the pointed by the value */
   _H_uptr_t v = *value;

   /* copy of the alignment */
   uint32_t a = alignment;

   /* remainder of the value by the aligment */
   _H_uptr_t r = v % a;

   /* if the value is not aligned */
   if(r)
   {
      /* then align it */
      *value = v - r + a;
   }
   else
   {
      /* else, dont need to align it */
      *value = v;
   }
}


/**
 *  @brief align the heap down by a specified value
 *  
 *  Align @a *value down to the nearest multiple of @a alignment.
 *  
 *  @param[out] value is a pointer to be aligned.
 *  @param[in] alignment is the alignment value.
 * 
 */
RTEMS_INLINE_ROUTINE void _Heap_Align_down_uptr(
                                                _H_uptr_t *value ,
                                                uint32_t alignment
                                                )
{
   /* copy of the pointed by the value */
   _H_uptr_t v = *value;

   /* first, calculate the amount is not aligned ( v % align )
    * then decrease that value and store it */
   *value = v - ( v % alignment );
}


/**
 *  @brief determine the block location (starting address)
 *  
 *  This function calculates and returns a block's location (address)
 *  in the heap based upon a base address @a base and an @a offset.
 *  
 *  @param[in] base is the base address of the memory area.
 *  @param[in] offset is the byte offset into @a base.
 *  
 *  @return returns a pointer to the block's address.
 */
RTEMS_INLINE_ROUTINE Heap_Block *_Heap_Block_at(
                                                void *base ,
                                                uint32_t offset
                                                )
{
   /* determine the address where the block is at */
   return (Heap_Block *) _Addresses_Add_offset(base , offset);
}


/**
 *  @brief determine the user memory area of a block
 *  
 *  This function returns the starting address of the portion of @a the_block
 *  which the user may access.
 *  
 *  @param[in] the_block is the heap block to find the user area of.
 *  
 *  @return returns a pointer to the start of the user area in
 *  the block.
 */
RTEMS_INLINE_ROUTINE void *_Heap_User_area(
                                           Heap_Block *the_block
                                           )
{
   /* determine the address where the user area of the block is */
   return (void *) _Addresses_Add_offset(the_block , HEAP_BLOCK_USER_OFFSET);
}


/**
 *  @brief determine the heap start of a block
 *  
 *  Fill @a *the_block with the address of the beginning of the block given
 *  pointer to the user accessible area @a base.
 *  
 *  @param[in] the_heap points to the heap being operated upon
 *  @param[in] base points to the user area in the block.
 *  @param[out] the_block will be the address of the heap block.
 */
RTEMS_INLINE_ROUTINE void _Heap_Start_of_block(
                                               Heap_Control *the_heap ,
                                               void *base ,
                                               Heap_Block **the_block
                                               )
{
   /* address of the base */
   _H_uptr_t addr = _H_p2u(base);


   /* The address passed could be greater than the block address plus
    * HEAP_BLOCK_USER_OFFSET as _Heap_Allocate_aligned() may produce such user
    * pointers. To get rid of this offset we need to align the address down
    * to the nearest 'page_size' boundary. */
   _Heap_Align_down_uptr(&addr , the_heap->page_size);
   *the_block = (Heap_Block *) ( addr - HEAP_BLOCK_USER_OFFSET );
}


/**
 *  @brief determine if the previous block of a specified block of a heap is
 *  used
 *  
 *  This function returns TRUE if the previous block of @a the_block
 *  is in use, and FALSE otherwise.
 *  
 *  @param[in] the_block is the block to operate upon.
 *  
 *  @return returns different than 0 if the previous block is used
 */
RTEMS_INLINE_ROUTINE boolean _Heap_Is_prev_used(
                                                Heap_Block *the_block
                                                )
{
   /* returns different than 0 if the size has the flag HEAP_PREV_USED */
   return (the_block->size & HEAP_PREV_USED );
}


/**
 *  @brief determine the block size
 *  
 *  This function returns the size of @a the_block in bytes.
 *  
 *  @param[in] the_block is the block to operate upon.
 *  
 *  @return returns the size of the specified heap block in bytes.
 */
RTEMS_INLINE_ROUTINE uint32_t _Heap_Block_size(
                                               Heap_Block *the_block
                                               )
{
   /* return the size of the block */
   return (the_block->size & ~HEAP_PREV_USED );
}


/**
 *  @brief determine if a block is inside a heap
 *  
 *  This function returns TRUE if @a the_block is within the memory area
 *  managed by @a the_heap, and FALSE otherwise.
 *  
 *  @param[in] the_heap points to the heap being operated upon
 *  @param[in] the_block is the block address to check.
 *  
 *  @return returns TRUE if @a the_block appears to have been
 *  allocated from @a the_heap, and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Heap_Is_block_in(
                                               Heap_Control *the_heap ,
                                               Heap_Block *the_block
                                               )
{
   /* determine if the block address is withing the heap addresses */
   return _Addresses_Is_in_range(the_block , the_heap->start , the_heap->final);
}


/**
 *  @brief assert if the condition is true, giving an internal error if it is
 *  false
 *
 *  This function checks if the @a cond is true or false. If it is true, then
 *  nothing will be done. If it is false, then an internal error will be raised
 *  with the following parameters:
 *    INTERNAL_ERROR_CORE
 *    TRUE
 *    INTERNAL_ERROR_INVALID_HEAP_ALIGNMENT
 *
 *  @param[in] cond the condition check to raise or not an internal error
 */
RTEMS_INLINE_ROUTINE void _HAssert(boolean cond)
{
   /* if the condition is not true */
   if(!cond)
   {
      /* raise an internal error
       *   internal error core
       *   internal
       *   invalid heap alignment */
      _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                               TRUE ,
                               INTERNAL_ERROR_INVALID_HEAP_ALIGNMENT);
   }
}
/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */
