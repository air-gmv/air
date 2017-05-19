/**
 *  @file
 *  wkspace.inl
 *
 *  @brief contains the bodies of the routines which contains
 *  information related to the RAM Workspace.
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreWorkspace Workspace Handler
 *  @{
 */

#ifndef _RTEMS_SCORE_WKSPACE_INL
#define _RTEMS_SCORE_WKSPACE_INL

#include <rtems/system.h>


/**
 *  @brief allocate a block of memory from the workspace
 *
 *  @param[in] size the size of the workspace
 *
 *  @return returns the address of a block of memory of size
 *  bytes.  If a block of the appropriate size cannot be allocated
 *  from the workspace, then NULL is returned.
 */
RTEMS_INLINE_ROUTINE void *_Workspace_Allocate(
                                               size_t size
                                               )
{
   /* allocate in the heap the block of memory */
   return _Heap_Allocate(&_Workspace_Area , size);
}


/**
 *  @brief free a block of memory of the workspace
 *  
 *  This function frees the specified block of memory.  
 *
 *  @param[in] block the block to be free 
 *
 *  @return If the block belongs to the Workspace and can be successfully freed,
 *  then TRUE is returned.  Otherwise FALSE is returned.
 */
RTEMS_INLINE_ROUTINE boolean _Workspace_Free(
                                             void *block
                                             )
{
   /* deallocate in the heap the block of memory */
   return _Heap_Free(&_Workspace_Area , block);
}

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */
