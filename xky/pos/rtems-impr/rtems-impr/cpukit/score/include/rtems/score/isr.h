/**
 *  @file
 *  isr.h
 *
 *  @brief all the constants and structures associated
 *  with the management of processor interrupt levels.  This handler
 *  supports interrupt critical sections, vectoring of user interrupt
 *  handlers, nesting of interrupts, and manipulating interrupt levels.
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
 *  4433    | 21/09/2009  | mcoutinho    | IPR 751
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

#ifndef _RTEMS_SCORE_ISR_H
#define _RTEMS_SCORE_ISR_H

/**
 *  @defgroup ScoreISR ISR Handler
 *
 *   @brief This handler encapsulates functionality which provides the foundation
 *  ISR services used in all of the APIs supported by RTEMS.
 */
/**@{*/

#ifdef __cplusplus
extern "C"
{
#endif

   /**
    *  @brief defines the control block used to manage
    *  the interrupt level portion of the status register.
    */
   typedef uint32_t ISR_Level;

   /**
    *  @brief defines the type used to manage the vectors.
    */
   typedef uint32_t ISR_Vector_number;

   /**
    *  @brief Return type for ISR Handler
    */
   typedef void ISR_Handler;

#if (CPU_ISR_PASSES_FRAME_POINTER == 1)
   /**
    *  @brief pointer to an ISR Handler
    */
   typedef ISR_Handler(*ISR_Handler_entry)(
                                            ISR_Vector_number ,
                                            CPU_Interrupt_frame *
                                            );
#else
   /**
    *  @brief pointer to an ISR Handler
    */
   typedef ISR_Handler(*ISR_Handler_entry)(
                                            ISR_Vector_number
                                            );
#endif

   /**
    *  @brief promotes out the number of vectors truly supported by
    *  the current CPU being used.  This is usually the number of distinct vectors
    *  the cpu can vector.
    */
#define ISR_NUMBER_OF_VECTORS                CPU_INTERRUPT_NUMBER_OF_VECTORS

   /**
    *  @brief promotes out the highest valid interrupt vector number.
    */
#define ISR_INTERRUPT_MAXIMUM_VECTOR_NUMBER  CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER

   /**
    *  @brief contains the interrupt service routine nest level.
    *  When this variable is zero, a thread is executing.
    */
   extern volatile uint32_t _ISR_Nest_level;

   /**
    *  @brief declares the Vector Table.  Application
    *  interrupt service routines are vectored by the ISR Handler via this table.
    */
   extern ISR_Handler_entry *_ISR_Vector_table;

   /**
    *  @brief initialize the ISR Handler component
    *
    *  Initializes the CPU interrupt registers (if any) and the places the
    *  default interrupt ISR handlers.
    */
   void _ISR_Handler_initialization(void);

   /**
    *  @brief disable interrupts
    *
    *  This routine disables all interrupts so that a critical section
    *  of code can be executing without being interrupted.  Upon return,
    *  the argument _level will contain the previous interrupt mask level.
    */
#define _ISR_Disable( _level ) \
  do { \
    _CPU_ISR_Disable( _level ); \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
  } while (0)

   /**
    *  @brief enable interrupts
    *
    *  This routine enables interrupts to the previous interrupt mask
    *  LEVEL.  It is used at the end of a critical section of code to
    *  enable interrupts so they can be processed again.
    */
#define _ISR_Enable( _level ) \
  do { \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
    _CPU_ISR_Enable( _level ); \
  } while (0)

   /**
    *  @brief flash interrupts (enable and disable interrupts)
    *
    *  This routine temporarily enables interrupts to the previous
    *  interrupt mask level and then disables all interrupts so that
    *  the caller can continue into the second part of a critical
    *  section.  This routine is used to temporarily enable interrupts
    *  during a long critical section.  It is used in long sections of
    *  critical code when a point is reached at which interrupts can
    *  be temporarily enabled.  Deciding where to flash interrupts
    *  in a long critical section is often difficult and the point
    *  must be selected with care to ensure that the critical section
    *  properly protects itself.
    */
#define _ISR_Flash( _level ) \
  do { \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
    _CPU_ISR_Flash( _level ); \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
  } while (0)

   /**
    *  @brief install a new interrupt handler
    *
    *  This routine installs new_handler as the interrupt service routine
    *  for the specified vector.  The previous interrupt service routine is
    *  returned as old_handler.
    */
#define _ISR_Install_vector( _vector, _new_handler, _old_handler ) \
  _CPU_ISR_install_vector( _vector, _new_handler, _old_handler )

   /**
    *  @brief get the current interrupt level
    */
#define _ISR_Get_level() \
        _CPU_ISR_Get_level()

   /**
    *  @brief set the interrupt level
    *
    *  This routine sets the current interrupt level to that specified
    *  by new_level.  The new interrupt level is effective when the
    *  routine exits.
    */
#define _ISR_Set_level( _new_level ) \
        _CPU_ISR_Set_level( _new_level )

   /**
    *  @brief determine if an ISR is in progress
    *
    *  This function returns TRUE if the processor is currently servicing
    *  and interrupt and FALSE otherwise.   A return value of TRUE indicates
    *  that the caller is an interrupt service routine, NOT a thread.  The
    */
#if (CPU_PROVIDES_ISR_IS_IN_PROGRESS == TRUE)
   boolean _ISR_Is_in_progress(void);
#else
#define _ISR_Is_in_progress() \
        (_ISR_Nest_level != 0)
#endif

#include <rtems/score/isr.inl>

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */
