/**
 *  @file
 *  modes.h
 *
 *  @brief contains all constants and structures associated
 *  with the RTEMS thread and RTEMS_ASR modes.
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
 *  7450    | 20/04/2010  | mcoutinho    | IPR 69
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_COMMON Common
 *  @{
 */

#ifndef _RTEMS_RTEMS_MODES_H
#define _RTEMS_RTEMS_MODES_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/score/isr.h>

   /**
    *  @brief define the control block used to manage
    *  each a mode set.
    */
   typedef uint32_t Modes_Control;

   /*
    *  The following constants define the individual modes and masks
    *  which may be used to compose a mode set and to alter modes.
    */

#define RTEMS_ALL_MODE_MASKS     0x0000ffff

#define RTEMS_DEFAULT_MODES     0x00000000
#define RTEMS_CURRENT_MODE      0

   /**
    * @brief determine the RTEMS mode preempt mask
    **/
#define RTEMS_PREEMPT_MASK    0x00000100  /* preemption bit           */

   /**
    * @brief determine the RTEMS mode timeslice mask
    **/
#define RTEMS_TIMESLICE_MASK  0x00000200  /* timeslice bit            */

   /**
    * @brief determine the RTEMS mode interrupt mask
    **/
#define RTEMS_INTERRUPT_MASK  CPU_MODES_INTERRUPT_MASK

   /**
    * @brief determine the RTEMS preempt mode value
    **/
#define RTEMS_PREEMPT      0x00000000     /* enable preemption        */

   /**
    * @brief determine the RTEMS no preempt mode value
    **/
#define RTEMS_NO_PREEMPT   0x00000100     /* disable preemption       */

   /**
    * @brief determine the RTEMS no timeslice mode value
    **/
#define RTEMS_NO_TIMESLICE 0x00000000     /* disable timeslicing      */

   /**
    * @brief determine the RTEMS timeslice mode value
    **/
#define RTEMS_TIMESLICE    0x00000200     /* enable timeslicing       */

   /*
    *  The number of bits for interrupt levels is CPU dependent.
    *  RTEMS supports 0 to 256 levels in bits 0-7 of the mode.
    */

   /**
    *
    *  @brief get the processor dependent interrupt
    *  level which corresponds to the requested interrupt level.
    *
    *  @param[in] _mode_set the interrupt mode set
    *
    *  @note: RTEMS supports 256 interrupt levels using the least
    *       significant eight bits of MODES.CONTROL.  On any
    *       particular CPU, fewer than 256 levels may be supported.
    */

#define RTEMS_INTERRUPT_LEVEL( _mode_set ) \
  ( (_mode_set) & RTEMS_INTERRUPT_MASK )


#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/modes.inl>
#endif

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
