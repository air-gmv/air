/**
 *  @file
 *  isr.inl
 *
 *  @brief contains the static implementation of all
 *  inlined routines in the Interrupt Handler.
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
 *  6116    | 18/02/2010  | mcoutinho    | IPR 1818
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreISR ISR Handler
 *  @{
 */

#ifndef _RTEMS_SCORE_ISR_INL
#define _RTEMS_SCORE_ISR_INL

#include <rtems/system.h>
#include <stddef.h>
#include <stdint.h>

/**
 *  @brief determine if the interrupt vector number is valid
 *  
 *  This function returns TRUE if the vector is a valid vector number
 *  for this processor and FALSE otherwise.
 *
 *  @param[in] vector the interrupt vector number
 *
 *  @return returns TRUE if the interrupt vector number is valid and FALSE
 *  otherwise
 */
RTEMS_INLINE_ROUTINE boolean _ISR_Is_vector_number_valid(
                                                         uint32_t vector
                                                         )
{
   /* vector is valid if it is lower than the maximum vector number */
   return ( vector <= CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER );
}


/**
 *  @brief determine if the user handler is valid
 *  
 *  This function returns TRUE if handler is the entry point of a valid
 *  use interrupt service routine and FALSE otherwise.
 *
 *  @param[in] handler the user handler for the interrupt
 *
 *  @return returns TRUE if the handler is valdid and FALSE otherwise
 */
RTEMS_INLINE_ROUTINE boolean _ISR_Is_valid_user_handler(
                                                        void *handler
                                                        )
{
   /* only NULL handlers are invalid */
   return ( handler != NULL );
}

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */
