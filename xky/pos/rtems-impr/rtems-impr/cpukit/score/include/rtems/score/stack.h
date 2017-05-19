/**
 *  @file
 *  stack.h
 *
 *  @brief contains all information about the thread
 *  Stack Handler.  This Handler provides mechanisms which can be used to
 *  initialize and utilize stacks.
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
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

#ifndef _RTEMS_SCORE_STACK_H
#define _RTEMS_SCORE_STACK_H

/**
 *  @defgroup ScoreStack Stack Handler
 *
 *   @brief This handler encapsulates functionality which is used in the management
 *  of thread stacks.
 */
/**@{*/

#ifdef __cplusplus
extern "C"
{
#endif

   /**
    *  @brief minimum stack size which every
    *  thread must exceed.
    */
#define STACK_MINIMUM_SIZE  CPU_STACK_MINIMUM_SIZE

   /**
    *  @brief control block used to manage each stack.
    */
   typedef struct
   {
      /** This is the stack size. */
      size_t size;
      /** This is the low memory address of stack. */
      void *area;
   } Stack_Control;

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/stack.inl>
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
