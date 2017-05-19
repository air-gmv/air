/**
 *  @file
 *  address.inl
 *
 *  @brief contains the bodies of the routines
 *  about addresses which are inlined.
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
 *  @addtogroup ScoreAddress Address Handler
 *  @{
 */

#ifndef _RTEMS_SCORE_ADDRESS_INL
#define _RTEMS_SCORE_ADDRESS_INL

#include <rtems/system.h>

/**
 *  @brief add Offset to Address
 *  
 *  This function is used to add an @a offset to a @a base address.
 *  It returns the resulting address.  This address is typically
 *  converted to an access type before being used further.
 *  
 *  @param[in] base is the base address.
 *  @param[in] offset is the offset to add to @a base.
 *  
 *  @return returns the resulting address.
 */
RTEMS_INLINE_ROUTINE void *_Addresses_Add_offset(
                                                 void *base ,
                                                 uint32_t offset
                                                 )
{
   /* return the sum of the base (cast to a byte address) plus offset */
   return (void *) ( (char *) base + offset );
}


/**
 *  @brief determine if address is aligned
 *  
 *  This function returns TRUE if the given address is correctly
 *  aligned for this processor and FALSE otherwise.  Proper alignment
 *  is based on correctness and efficiency.
 *  
 *  @param[in] address is the address being checked for alignment.
 *  
 *  @return returns different than 0 if the address is aligned and
 *  0 otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Addresses_Is_aligned(
                                                   void *address
                                                   )
{
   /* check if there is no alignment */
#if (CPU_ALIGNMENT == 0)
   /* exception case when CPU_ALIGNMENT is 0, is always aligned */
   return TRUE;
#else
   /* return true if the address is divisible by CPU_ALIGNMENT */
   return (( (uintptr_t) address % CPU_ALIGNMENT ) == 0 );
#endif
}


/**
 *  @brief determine if address is in range
 *  
 *  This function returns TRUE if the given address is within the
 *  memory range specified and FALSE otherwise.  base is the address
 *  of the first byte in the memory range and limit is the address
 *  of the last byte in the memory range.  The base address is
 *  assumed to be lower than the limit address.
 *  
 *  @param[in] address is the address to check.
 *  @param[in] base is the lowest address of the range to check against.
 *  @param[in] limit is the highest address of the range to check against.
 *  
 *  @return returns different than 0 if the given @a address is within the
 *  memory range specified and 0 otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Addresses_Is_in_range(
                                                    void *address ,
                                                    void *base ,
                                                    void *limit
                                                    )
{
   /* return true if the address is between the base and limit */
   return (address >= base && address <= limit );
}

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */
