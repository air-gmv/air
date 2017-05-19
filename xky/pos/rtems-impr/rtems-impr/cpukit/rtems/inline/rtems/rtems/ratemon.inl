/**
 *  @file
 *  ratemon.inl
 *
 *  @brief static inline  implementation of the inlined
 *  routines in the Rate Monotonic Manager.
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
 *  @addtogroup RTEMS_API_RATEMON Rate Monotonic Manager
 *  @{
 */

#ifndef _RTEMS_RTEMS_RATEMON_INL
#define _RTEMS_RTEMS_RATEMON_INL


/**
 *  @brief allocate rate monotonic
 *  
 *  This function allocates a period control block from
 *  the inactive chain of free period control blocks.
 *
 *  @return returns the allocated rate monotonic object
 */
RTEMS_INLINE_ROUTINE Rate_monotonic_Control *_Rate_monotonic_Allocate(void)
{
   /* return the allocated rate monotonic object with the rate
    * monotonic object information table */
   return (Rate_monotonic_Control *) _Objects_Allocate(&_Rate_monotonic_Information);
}


/**
 *  @brief free rate monotonic
 *  
 *  This routine allocates a period control block from
 *  the inactive chain of free period control blocks.
 *
 *  @param[in] the_period the period to free
 */
RTEMS_INLINE_ROUTINE void _Rate_monotonic_Free(
                                               Rate_monotonic_Control *the_period
                                               )
{
   /* free the rate monotonic object information */
   _Objects_Free(&_Rate_monotonic_Information , &the_period->Object);
}


/**
 *  @brief get rate monotonic
 *  
 *  This function maps period IDs to period control blocks.
 *  If ID corresponds to a local period, then it returns
 *  the_period control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_period is undefined.
 *
 *  @param[in] id the rate monotonic id
 *  @param[out] location the rate monotonic object location
 *
 *  @return returns the rate monotonic object or NULL if there is none
 */
RTEMS_INLINE_ROUTINE Rate_monotonic_Control *_Rate_monotonic_Get(
                                                                 Objects_Id id ,
                                                                 Objects_Locations *location
                                                                 )
{
   /* return the rate monotonic object information with:
    *   the rate monotonic object information
    *   the id of the period
    *   the address to place the location of the period */
   return (Rate_monotonic_Control *) _Objects_Get(&_Rate_monotonic_Information ,
                                                  id , location);
}

#endif
/* end of include file */

/**  
 *  @}
 */

/**
 *  @}
 */
