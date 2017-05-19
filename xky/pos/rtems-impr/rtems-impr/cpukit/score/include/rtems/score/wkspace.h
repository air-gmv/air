/**
 *  @file
 *  wkspace.h
 *
 *  @brief contains information related to the
 *  RAM Workspace.  This Handler provides mechanisms which can be used to
 *  define, initialize and manipulate the workspace.
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
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

#ifndef _RTEMS_SCORE_WKSPACE_H
#define _RTEMS_SCORE_WKSPACE_H

/**
 *  @defgroup ScoreWorkspace Workspace Handler
 *
 *  @brief This handler encapsulates functionality related to the management of
 *  the RTEMS Executive Workspace.
 */
/**@{*/

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/score/heap.h>
#include <rtems/score/interr.h>

   /** @brief Executive Workspace Control
    *
    *  The is the heap control structure that used to manage the
    *  RTEMS Executive Workspace.
    */
   extern Heap_Control _Workspace_Area; /* executive heap header */

   /**
    *  @brief initialize the Workspace Handler
    *
    *  This routine performs the initialization necessary for this handler.
    *
    *  @param[in] starting_address is the base address of the RTEMS Executive
    *  Workspace
    *  @param[in] size is the number of bytes in the RTEMS Executive Workspace
    */
   void _Workspace_Handler_initialization(
                                          void *starting_address ,
                                          size_t size
                                          );

   /**
    *  @brief allocate memory from the Workspace Handler or raise a fatal error if
    *  it cannot
    *
    *  This routine returns the address of a block of memory of @a size
    *  bytes.  If a block of the appropriate size cannot be allocated
    *  from the workspace, then the internal error handler is invoked.
    *
    *  @param[in] size is the desired number of bytes to allocate
    *
    *  @return If successful, the starting address of the allocated memory
    */
   void *_Workspace_Allocate_or_fatal_error(
                                            size_t size
                                            );

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/wkspace.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */
