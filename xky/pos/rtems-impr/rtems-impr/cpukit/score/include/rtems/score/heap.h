/**
 *  @file
 *  heap.h
 *
 *  @brief contains the information pertaining to the Heap
 *  Handler.
 *
 *  A heap is a doubly linked list of variable size
 *  blocks which are allocated using the first fit method.  Garbage
 *  collection is performed each time a block is returned to the heap by
 *  coalescing neighbor blocks.  Control information for both allocated
 *  and unallocated blocks is contained in the heap space.  A heap control
 *  structure contains control information for the heap.
 *
 *  FIXME: the alignment routines could be made faster should we require only
 *         powers of two to be supported both for 'page_size' and for
 *         'alignment' arguments. However, both workspace and malloc heaps are
 *         initialized with CPU_HEAP_ALIGNMENT as 'page_size', and while all
 *         the BSPs seem to use CPU_ALIGNMENT (that is power of two) as
 *         CPU_HEAP_ALIGNMENT, for whatever reason CPU_HEAP_ALIGNMENT is only
 *         required to be multiple of CPU_ALIGNMENT and explicitly not
 *         required to be a power of two.
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
 *  601     | 17/11/2008  | mcoutinho    | IPR 71
 *  4773    | 09/10/2009  | mcoutinho    | IPR 802
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

#ifndef _RTEMS_SCORE_HEAP_H
#define _RTEMS_SCORE_HEAP_H

#include <rtems/score/interr.h>

/**
 *  @defgroup ScoreHeap Heap Handler
 *
 *   @brief This handler encapsulates functionality which provides the foundation
 *  Heap services used in all of the APIs supported by RTEMS.
 */
/**@{*/

#ifdef __cplusplus
extern "C"
{
#endif

   /**
    * @brief define unsigned integer type to store 'void *'.
    *
    *  Analog of C99 'uintptr_t'. This should work on 16/32/64 bit architectures.
    *
    * FIXME: Something like this should better be defined by
    *        'rtems/score/types.h' and used here.
    */
   typedef uintptr_t _H_uptr_t;

   /**
    *  @brief Heap block
    */
   typedef struct Heap_Block_struct Heap_Block;

   /**
    *  @brief Heap block declaration
    *
    *  The following defines the data structure used to manage individual blocks
    *  in a heap.  When the block is allocated, the 'next' and 'prev' fields, as
    *  well as 'prev_size' field of the next block, are not used by the heap
    *  manager and thus the address returned for the block starts at the address
    *  of the 'next' field and the size of the user accessible area includes the
    *  size of the 'prev_size' field.
    *
    *  @note The 'next' and 'prev' pointers are only valid when the block is free.
    *     Caution must be taken to ensure that every block is large enough to
    *     hold them and that they are not accessed while the block is actually
    *     allocated (i.e., not free).
    *
    *  @note The 'prev_size' field is only valid when HEAP_PREV_USED bit is clear
    *     in the 'size' field indicating that previous block is not allocated.
    *     If the bit is set, the 'prev_size' field is part of user-accessible
    *     space of the previous allocated block and thus shouldn't be accessed
    *     by the heap manager code. This trick allows to further decrease
    *     overhead in the used blocks to the size of 'size' field (4 bytes).
    *
    */
   struct Heap_Block_struct
   {
      /** size of prev block (if prev block is free) */
      uint32_t prev_size;
      /** size of block in bytes and status of prev block */
      uint32_t size;
      /** pointer to the next free block */
      Heap_Block *next;
      /** pointer to the previous free block */
      Heap_Block *prev;
   };

   /**
    *  @brief This flag used in the 'size' field of each heap block to indicate
    *  if previous block is free or in use. As sizes are always multiples of
    *  4, the 2 least significant bits would always be 0, and we use one of them
    *  to store the flag.
    */
#define HEAP_PREV_USED    1     /* indicates previous block is in use */

   /*
    * The following constants reflect various requirements of the
    *  heap data structures which impact the management of a heap.
    */

   /**
    *  @brief heap minimum block size
    *
    */
#define HEAP_MIN_BLOCK_SIZE (sizeof(Heap_Block))

   /**
    *  @brief Offset of the block header from the block pointer. Equal to the
    *  offsetof(Heap_Block.size).
    */
#define HEAP_BLOCK_HEADER_OFFSET (sizeof(uint32_t))

   /**
    *  @brief Offset of user data pointer from the block pointer. Equal to the
    *  offsetof(Heap_Block.next).
    */
#define HEAP_BLOCK_USER_OFFSET (sizeof(uint32_t) * 2)

   /**
    *  @brief Number of bytes of overhead in used block. Equal to the
    *  sizeof(Heap_Block.size).
    */
#define HEAP_BLOCK_USED_OVERHEAD \
  (HEAP_BLOCK_USER_OFFSET - HEAP_BLOCK_HEADER_OFFSET)

   /**
    * @brief Size of the permanent dummy last block.
    */
#define HEAP_OVERHEAD HEAP_BLOCK_USER_OFFSET

   /**
    *  @brief Control block used to manage each heap.
    */
   typedef struct
   {
      /**
       * @brief head and tail of circular list of free blocks
       */
      Heap_Block free_list;
      /**
       * @brief allocation unit and alignment
       */
      uint32_t page_size;
      /**
       * @brief minimum block size aligned on page_size
       */
      uint32_t min_block_size;
      /**
       * @brief first address of memory for the heap
       */
      void *begin;
      /**
       * @brief first address past end of memory for the heap
       */
      void *end;
      /**
       * @brief first valid block address in the heap
       */
      Heap_Block *start;
      /**
       * @brief last valid block address in the heap
       */
      Heap_Block *final;
   } Heap_Control;

   /**
    *  @brief Status codes for _Heap_Extend
    */
   typedef enum
   {
      HEAP_EXTEND_SUCCESSFUL ,
      HEAP_EXTEND_ERROR ,
      HEAP_EXTEND_NOT_IMPLEMENTED
   } Heap_Extend_status;

   /**
    *  @brief Status codes for _Heap_Resize_block
    */
   typedef enum
   {
      HEAP_RESIZE_SUCCESSFUL ,
      HEAP_RESIZE_UNSATISFIED ,
      HEAP_RESIZE_FATAL_ERROR
   } Heap_Resize_status;

   /**
    *  @brief Information block returned by the Heap routines used to
    *  obtain heap information.  This information is returned about
    *  either free or used blocks.
    */
   typedef struct
   {
      /**
       * @brief Number of blocks of this type.
       */
      uint32_t number;
      /**
       * @brief Largest blocks of this type.
       */
      uint32_t largest;
      /**
       * @brief Total size of the blocks of this type.
       */
      uint32_t total;
   } Heap_Information;

   /**
    *  @brief Information block returned by _Heap_Get_information
    */
   typedef struct
   {
      /**
       * @brief This field is information on the used blocks in the heap.
       */
      Heap_Information Free;
      /**
       * @brief This field is information on the used blocks in the heap.
       */
      Heap_Information Used;
   } Heap_Information_block;

   /**
    *  @brief initialize the heap
    *
    *  This kernel routine initializes a heap.
    *  This is what a heap looks like in memory immediately after initialization:\n\n
    *  +--------------------------------+ <- begin = starting_address\n
    *  |  unused space due to alignment |\n
    *  |       size < page_size         |\n
    *  0  +--------------------------------+ <- first block\n
    *  |  prev_size = page_size         |\n
    *  4  +--------------------------------+\n
    *  |  size = size0              | 1 |\n
    *  8  +---------------------+----------+ <- aligned on page_size\n
    *  |  next = HEAP_TAIL   |          |\n
    *  12  +---------------------+          |\n
    *  |  prev = HEAP_HEAD   |  memory  |\n
    *  +---------------------+          |\n
    *  |                     available  |\n
    *  |                                |\n
    *  |                for allocation  |\n
    *  |                                |\n
    *  size0  +--------------------------------+ <- last dummy block\n
    *  |  prev_size = size0             |\n
    *  +4  +--------------------------------+\n
    *  |  size = page_size          | 0 | <- prev block is free\n
    *  +8  +--------------------------------+ <- aligned on page_size\n
    *  |  unused space due to alignment |\n
    *  |       size < page_size         |\n
    *  +--------------------------------+ <- end = begin + size\n
    *  Below is what a heap looks like after first allocation of SIZE bytes using\n
    *  _Heap_allocate(). BSIZE stands for SIZE + 4 aligned up on 'page_size'\n
    *  boundary.\n
    *  +--------------------------------+ <- begin = starting_address\n
    *  |  unused space due to alignment |\n
    *  |       size < page_size         |\n
    *  0  +--------------------------------+ <- used block\n
    *  |  prev_size = page_size         |\n
    *  4  +--------------------------------+\n
    *  |  size = BSIZE              | 1 | <- prev block is used\n
    *  8  +--------------------------------+ <- aligned on page_size\n
    *  |              .                 | Pointer returned to the user\n
    *  |              .                 | is 8 for _Heap_Allocate()\n
    *  |              .                 | and is in range\n
    *  8 +        |         user-accessible        | [8,8+page_size) for\n
    *  page_size +- - -                      - - -+ _Heap_Allocate_aligned()\n
    *  |             area               |\n
    *  |              .                 |\n
    *  BSIZE  +- - - - -     .        - - - - -+ <- free block\n
    *  |              .                 |\n
    *  BSIZE  +4  +--------------------------------+\n
    *  |  size = S = size0 - BSIZE  | 1 | <- prev block is used\n
    *  BSIZE  +8  +-------------------+------------+ <- aligned on page_size\n
    *  |  next = HEAP_TAIL |            |\n
    *  BSIZE +12  +-------------------+            |\n
    *  |  prev = HEAP_HEAD |     memory |\n
    *  +-------------------+            |\n
    *  |                   .  available |\n
    *  |                   .            |\n
    *  |                   .        for |\n
    *  |                   .            |\n
    *  BSIZE +S+0 +-------------------+ allocation + <- last dummy block\n
    *  |  prev_size = S    |            |\n
    *  +S+4 +-------------------+------------+\n
    *  |  size = page_size          | 0 | <- prev block is free\n
    *  +S+8 +--------------------------------+ <- aligned on page_size\n
    *  |  unused space due to alignment |\n
    *  |       size < page_size         |\n
    *  +--------------------------------+ <- end = begin + size\n
    *\n
    *  @param[in] the_heap pointer to heap header
    *  @param[in] starting_address starting address of heap
    *  @param[in] size size of heap
    *  @param[in] page_size allocatable unit of memory
    *
    *  @return returns maximum memory available if RTEMS_SUCCESSFUL
    *  otherwise
    *
    *  @note If allocation were performed by _Heap_Allocate_aligned(), the
    *  block size BSIZE is defined differently, and previously free block will
    *  be split so that upper part of it will become used block
    */
   uint32_t _Heap_Initialize(
                             Heap_Control *the_heap ,
                             void *starting_address ,
                             size_t size ,
                             uint32_t page_size
                             );

   /**
    *  @brief extend the heap
    *
    *  This routine grows @a the_heap memory area using the size bytes which
    *  begin at @a starting_address.
    *
    *  @param[in] the_heap is the heap to operate upon
    *  @param[in] starting_address is the starting address of the memory
    *  to add to the heap
    *  @param[in] size is the size in bytes of the memory area to add
    *  @param[in] amount_extended points to a user area to return the
    *
    *  @return a status indicating success or the reason for failure
    *  size filled in with the amount of memory added to the heap
    */
   Heap_Extend_status _Heap_Extend(
                                   Heap_Control *the_heap ,
                                   void *starting_address ,
                                   size_t size ,
                                   uint32_t *amount_extended
                                   );

   /**
    *  @brief allocate a block from the heap
    *
    *  This function attempts to allocate a block of @a size bytes from
    *  @a the_heap.  If insufficient memory is free in @a the_heap to allocate
    *  a block of the requested size, then NULL is returned.
    *
    *  @param[in] the_heap is the heap to operate upon
    *  @param[in] size is the amount of memory to allocate in bytes
    *
    *  @return NULL if unsuccessful and a pointer to the block if successful
    */
   void *_Heap_Allocate(
                        Heap_Control *the_heap ,
                        size_t size
                        );

   /**
    *  @brief allocate an alligned block from a heap
    *
    *  This function attempts to allocate a memory block of @a size bytes from
    *  @a the_heap so that the start of the user memory is aligned on the
    *  @a alignment boundary. If @a alignment is 0, it is set to CPU_ALIGNMENT.
    *  Any other value of @a alignment is taken "as is", i.e., even odd
    *  alignments are possible.
    *  Returns pointer to the start of the memory block if success, NULL if
    *  failure.
    *
    *  @param[in] the_heap is the heap to operate upon
    *  @param[in] size is the amount of memory to allocate in bytes
    *  @param[in] alignment the required alignment
    *
    *  @return NULL if unsuccessful and a pointer to the block if successful
    */
   void *_Heap_Allocate_aligned(
                                Heap_Control *the_heap ,
                                size_t size ,
                                uint32_t alignment
                                );

   /**
    *  @brief determine the size of the user block of memory
    *
    *  This function sets @a *size to the size of the block of user memory
    *  which begins at @a starting_address. The size returned in @a *size could
    *  be greater than the size requested for allocation.
    *  Returns TRUE if the @a starting_address is in the heap, and FALSE
    *  otherwise.
    *
    *  @param[in] the_heap is the heap to operate upon
    *  @param[in] starting_address is the starting address of the user block
    *  to obtain the size of
    *  @param[in] size points to a user area to return the size in
    *
    *  @return TRUE if successfully able to determine the size, FALSE otherwise
    *  *size filled in with the size of the user area for this block
    */
   boolean _Heap_Size_of_user_area(
                                   Heap_Control *the_heap ,
                                   void *starting_address ,
                                   size_t *size
                                   );

   /**
    *  @brief resize a block inside a heap
    *
    *  This function tries to resize in place the block that is pointed to by the
    *  @a starting_address to the new @a size.
    *
    *  @param[in] the_heap is the heap to operate upon
    *  @param[in] starting_address is the starting address of the user block
    *  to be resized
    *  @param[in] size is the new size
    *  @param[in] old_mem_size points to a user area to return the size of the
    *  user memory area of the block before resizing.
    *  @param[in] avail_mem_size points to a user area to return the size of
    *  the user memory area of the free block that has been enlarged or
    *  created due to resizing, 0 if none.
    *
    *  @return HEAP_RESIZE_SUCCESSFUL if successfully able to resize the block,
    *  HEAP_RESIZE_UNSATISFIED if the block can't be resized in place,
    *  HEAP_RESIZE_FATAL_ERROR if failure
    *   - *old_mem_size filled in with the size of the user memory area of
    *     the block before resizing.
    *   - *avail_mem_size filled in with the size of the user memory area
    *     of the free block that has been enlarged or created due to
    *     resizing, 0 if none.
    */
   Heap_Resize_status _Heap_Resize_block(
                                         Heap_Control *the_heap ,
                                         void *starting_address ,
                                         size_t size ,
                                         uint32_t *old_mem_size ,
                                         uint32_t *avail_mem_size
                                         );

   /**
    *  @brief free a block from the heap
    *
    *  This routine returns the block of memory which begins
    *  at @a starting_address to @a the_heap.  Any coalescing which is
    *  possible with the freeing of this routine is performed.
    *
    *  @param[in] the_heap is the heap to operate upon
    *  @param[in] starting_address is the starting address of the user block
    *  to free
    *
    *  @return TRUE if successfully freed, FALSE otherwise
    */
   boolean _Heap_Free(
                      Heap_Control *the_heap ,
                      void *starting_address
                      );

#if !defined(__RTEMS_APPLICATION__)

   /**
    *  @brief pointer to unsigned integer conversion.
    */
#define _H_p2u(_p) ((_H_uptr_t)(_p))

#include <rtems/score/heap.inl>

   /**
    *  @brief convert user requested 'size' of memory block to the block size.
    *
    *  @param[in] size is the size of the block requested
    *  @param[in] page_size is the page size of this heap instance
    *  @param[in] min_size is minimum size block that should be allocated
    *  from this heap instance
    *
    *  @return This method returns block size on success, 0 if overflow occured.
    *
    *  @note This is an internal routine used by _Heap_Allocate() and
    *  _Heap_Allocate_aligned().  Refer to 'heap.c' for details.
    */
   extern size_t _Heap_Calc_block_size(
                                       size_t size ,
                                       uint32_t page_size ,
                                       uint32_t min_size
                                       );

   /**
    *  @brief allocate a block of memory from a heap
    *
    *  This method allocates a block of size @a alloc_size from @a the_block
    *  belonging to @a the_heap. Split @a the_block if possible, otherwise
    *  allocate it entirely.  When split, make the lower part used, and leave
    *  the upper part free.
    *  This is an internal routines used by _Heap_Allocate() and
    *  _Heap_Allocate_aligned().  Refer to 'heap.c' for details.
    *
    *  @param[in] the_heap is the heap to operate upon
    *  @param[in] the_block is the block to allocates the requested size from
    *  @param[in] alloc_size is the requested number of bytes to take out of
    *  the block
    *
    *  @return This methods returns the size of the allocated block.
    */
   extern uint32_t _Heap_Block_allocate(
                                        Heap_Control* the_heap ,
                                        Heap_Block* the_block ,
                                        uint32_t alloc_size
                                        );

#endif /* !defined(__RTEMS_APPLICATION__) */

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */
