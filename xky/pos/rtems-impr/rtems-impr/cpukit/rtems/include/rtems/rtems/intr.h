/**
 *  @file
 *  intr.h
 *
 *  @brief contains all the constants and structures associated
 *  with the Interrupt Manager.
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
 *  4844    | 15/10/2009  | mcoutinho    | IPR 2517
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
 *  @defgroup RTEMS_API_INTERRUPT Interrupt Manager
 *
 *  @brief The Interrupt Manager allows the user to manage interrupts, by catching
 *  specific interrupts, disabling and enabling interrupts, etc.
 *  @{
 */

#ifndef _RTEMS_RTEMS_INTR_H
#define _RTEMS_RTEMS_INTR_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/score/isr.h>

   /**
    *  @brief interrupt level type
    */
   typedef ISR_Level rtems_interrupt_level;

   /**
    *  @brief define the control block used to manage
    *  the vectors.
    */
   typedef ISR_Vector_number rtems_vector_number;

   /**
    *  @brief return type for ISR Handler
    */
   typedef void rtems_isr;

   /**
    *  @brief pointer to an ISR Handler
    */
   typedef rtems_isr(*rtems_isr_entry)(
                                        rtems_vector_number
                                        );

   /**
    *  @brief initialize the Interrupt Manager
    *
    *  This routine initializes the interrupt manager.
    */
   void _Interrupt_Manager_initialization(void);

   /**
    *  @brief catch an interrupt
    *
    *  This directive allows a thread to specify what action to take when
    *  catching signals.
    *
    *  @param[in] new_isr_handler address of interrupt service routine (isr)
    *  @param[in] vector interrupt vector number
    *  @param[out] old_isr_handler address at which to store previous ISR address
    *  previous ISR address
    *
    *  @return RTEMS_SUCCESSFUL always succeeds
    */
   rtems_status_code rtems_interrupt_catch(
                                           rtems_isr_entry new_isr_handler ,
                                           rtems_vector_number vector ,
                                           rtems_isr_entry *old_isr_handler
                                           );

   /**
    *  @brief disable interrupts
    *
    *  This routine disables all maskable interrupts and returns the
    *  previous level in _isr_cookie.
    */
#define rtems_interrupt_disable( _isr_cookie ) \
    _ISR_Disable(_isr_cookie)

   /**
    *  @brief enable interrupts
    *
    *  This routine enables maskable interrupts to the level indicated
    *  _isr_cookie.
    */
#define rtems_interrupt_enable( _isr_cookie ) \
    _ISR_Enable(_isr_cookie)

   /**
    *  @brief flash interrupts
    *
    *  This routine temporarily enables maskable interrupts to the
    *  level in _isr_cookie before redisabling them.
    */
#define rtems_interrupt_flash( _isr_cookie ) \
    _ISR_Flash(_isr_cookie)

   /**
    *  @brief rtems_interrupt_is_in_progress
    *
    *  This function returns TRUE if the processor is currently servicing
    *  an interrupt and FALSE otherwise.   A return value of TRUE indicates
    *  that the caller is an interrupt service routine, NOT a thread.  The
    *  directives available to an interrupt service routine are restricted.
    */
#define rtems_interrupt_is_in_progress() \
    _ISR_Is_in_progress()

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

/**  
 *  @}
 */

/**
 *  @}
 */
