/**
 *  @file
 *  sem.inl
 *
 *  @brief static inline implementation of the inlined
 *  routines from the Semaphore Manager.
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
 *  4546    | 28/09/2009  | mcoutinho    | IPR 778
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_SEMAPHORE Semaphore Manager
 *  @{
 */

#ifndef _RTEMS_RTEMS_SEM_INL
#define _RTEMS_RTEMS_SEM_INL

#include <rtems/system.h>

/**
 *  @brief allocate semaphore
 *  
 *  This function allocates a semaphore control block from
 *  the inactive chain of free semaphore control blocks.
 *
 *  @return returns the allocated semaphore
 */
RTEMS_INLINE_ROUTINE Semaphore_Control *_Semaphore_Allocate(void)
{
   /* allocate the semaphore object information */
   return (Semaphore_Control *) _Objects_Allocate(&_Semaphore_Information);
}


/**
 *  @brief free semaphore
 *  
 *  This routine frees a semaphore control block to the
 *  inactive chain of free semaphore control blocks.
 *
 *  @param[in] the_semaphore the semaphore to free
 */
RTEMS_INLINE_ROUTINE void _Semaphore_Free(
                                          Semaphore_Control *the_semaphore
                                          )
{
   /* free the semaphore object information */
   _Objects_Free(&_Semaphore_Information , &the_semaphore->Object);
}


/**
 *  @brief get semaphore
 *  
 *  This function maps semaphore IDs to semaphore control blocks.
 *  If ID corresponds to a local semaphore, then it returns
 *  the_semaphore control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the semaphore ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_semaphore is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_semaphore is undefined.
 *
 *  @param[in] id the semaphore id
 *  @param[out] location the location of the semaphore
 *
 *  @return returns the semaphore or NULL if there exists none
 */
RTEMS_INLINE_ROUTINE Semaphore_Control *_Semaphore_Get(
                                                       Objects_Id id ,
                                                       Objects_Locations *location
                                                       )
{
   /* return the semaphore object information with:
    *   the semaphore object information
    *   the id of the sempahore
    *   the address to place the location of the semaphore */
   return (Semaphore_Control *) _Objects_Get(&_Semaphore_Information ,
                                             id , location);
}


/**
 *  @brief get semaphore (interrupts disabled)
 *
 *  @param[in] id the semaphore id
 *  @param[out] location the location of the semaphore
 *  @param[out] level the interrupt level that is set by the function
 *
 *  @return returns the sempahore
 */
RTEMS_INLINE_ROUTINE Semaphore_Control *_Semaphore_Get_interrupt_disable(
                                                                         Objects_Id id ,
                                                                         Objects_Locations *location ,
                                                                         ISR_Level *level
                                                                         )
{
   /* get the semaphore object information with interrupts disabled
    *   and the semaphore object information
    *   the semaphore id
    *   the semaphore location */
   return (Semaphore_Control *) _Objects_Get_isr_disable(&_Semaphore_Information ,
                                                         id ,
                                                         location ,
                                                         level);
}

#endif
/*  end of include file */

/**  
 *  @}
 */

/**
 *  @}
 */
