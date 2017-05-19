/**
 *  @file
 *  modes.inl
 *
 *  @brief static inline implementation of the
 *  inlined routines in the Mode Handler
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
 *  568     | 17/11/2008  | mcoutinho    | IPR 69
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
 *  @addtogroup RTEMS_API_COMMON Common
 *  @{
 */

#ifndef _RTEMS_RTEMS_MODES_INL
#define _RTEMS_RTEMS_MODES_INL


/**
 *  @brief determine if the modes is preempt
 *
 *  @param[in] mode_set the mode set
 *
 *  @return returns TRUE if mode_set indicates that preemption
 *  is enabled, and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Modes_Is_preempt(
                                               Modes_Control mode_set
                                               )
{
   /* return true if the mode set has preempt mask equal to preempt value */
   return (mode_set & RTEMS_PREEMPT_MASK ) == RTEMS_PREEMPT;
}


/**
 *  @brief determine if the modes is timeslice
 *
 *  @param[in] mode_set the mode set
 *
 *  @return returns TRUE if mode_set indicates that timeslicing
 *  is enabled, and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Modes_Is_timeslice(
                                                 Modes_Control mode_set
                                                 )
{
   /* return true if the mode set has timeslice mask equal to timeslice value */
   return (mode_set & RTEMS_TIMESLICE_MASK ) == RTEMS_TIMESLICE;
}


/**
 *  @brief get the modes interrupt level
 *
 *  @param[in] mode_set the mode set
 *
 *  @return returns the interrupt level portion of the mode_set.
 */
RTEMS_INLINE_ROUTINE ISR_Level _Modes_Get_interrupt_level(
                                                          Modes_Control mode_set
                                                          )
{
   /* return he interrupt mode of the mode set */
   return ( mode_set & RTEMS_INTERRUPT_MASK );
}


/**
 *  @brief set the modes interrupt level
 *  
 *  This routine sets the current interrupt level to that specified
 *  in the mode_set.
 *
 *  @param[in] mode_set the mode set
 */
RTEMS_INLINE_ROUTINE void _Modes_Set_interrupt_level(
                                                     Modes_Control mode_set
                                                     )
{
   /* set the interrupt level defined by mode set */
   _ISR_Set_level(_Modes_Get_interrupt_level(mode_set));
}

#endif
/* end of include file */

/**  
 *  @}
 */

/**
 *  @}
 */
