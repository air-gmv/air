/**
 *  @file
 *  intrcatch.c
 *
 *  @brief catch an interrupt
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
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  7078    | 09/04/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_INTERRUPT Interrupt Manager
 *  @{
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/score/isr.h>
#include <rtems/rtems/intr.h>


rtems_status_code rtems_interrupt_catch(
                                        rtems_isr_entry new_isr_handler ,
                                        rtems_vector_number vector ,
                                        rtems_isr_entry *old_isr_handler
                                        )
{
   /* check if ISR vector number is valid */
   if(!_ISR_Is_vector_number_valid(vector))
   {
      /* invalid number error */
      return RTEMS_INVALID_NUMBER;
   }

   /* check if ISR handler is valid */
   if(!_ISR_Is_valid_user_handler((void *) new_isr_handler))
   {
      /* invalid address error */
      return RTEMS_INVALID_ADDRESS;
   }

   /* check if old isr is valid */
   if(!_ISR_Is_valid_user_handler((void *) old_isr_handler))
   {
      /* invalid address error */
      return RTEMS_INVALID_ADDRESS;
   }

   /* install the vector */
   _ISR_Install_vector(vector ,
                       (proc_ptr) new_isr_handler ,
                       (proc_ptr *) old_isr_handler);

   /* return success */
   return RTEMS_SUCCESSFUL;
}

/**  
 *  @}
 */

/**
 *  @}
 */
