/**
 *  @file
 *  priority.inl
 *
 *  @brief the static inline implementation of all inlined
 *  routines in the Priority Handler.
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
 *  4546    | 28/09/2009  | mcoutinho    | IPR 778
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScorePriority Priority Handler
 *  @{
 */

#ifndef _RTEMS_SCORE_PRIORITY_INL
#define _RTEMS_SCORE_PRIORITY_INL

#include <rtems/score/bitfield.h>
#include <rtems/system.h>


/**
 *  @brief initialize the Priority Handler
 *  
 *  This routine performs the initialization necessary for this handler.
 */
RTEMS_INLINE_ROUTINE void _Priority_Handler_initialization(void)
{
   /* iterator */
   size_t index;

   /* initialize the major priority bit at 0*/
   _Priority_Major_bit_map = 0;

   /* loop through the 16 priority entries */
   for(index = 0; index < 16; index++)
   {
      /* and reset each entry */
      _Priority_Bit_map[ index ] = 0;
   }
}


/**
 *  @brief determine if a priority value is valid
 *  
 *  This function returns TRUE if the_priority if valid for a
 *  user task, and FALSE otherwise.
 *
 *  @param[in] the_priority the priority
 *
 *  @return returns TRUE if the priority is valid and FALSE otherwise
 */
RTEMS_INLINE_ROUTINE boolean _Priority_Is_valid(
                                                Priority_Control the_priority
                                                )
{
   /* since PRIORITY_MINIMUM is 0 and priorities are stored unsigned,
    * then checking for less than 0 is unnecessary */

   /* priority is valid iff its is lower than the maxim priority */
   return ( the_priority <= PRIORITY_MAXIMUM );
}


/**
 *  @brief determine the major portion of a priority value
 *  
 *  This function returns the major portion of the_priority.
 *
 *  @param[in] the_priority the priority
 *
 *  @return returns the major portion of the priority value
 */
RTEMS_INLINE_ROUTINE uint32_t _Priority_Major(
                                              Priority_Control the_priority
                                              )
{
   /* return the major part of the priority */
   return ( the_priority / 16 );
}


/**
 *  @brief determine the minor portion of a priority value
 *  
 *  This function returns the minor portion of the_priority.
 *
 *  @param[in] the_priority the priority
 *
 *  @return returns the minor portion of the priority value
 */
RTEMS_INLINE_ROUTINE uint32_t _Priority_Minor(
                                              Priority_Control the_priority
                                              )
{
   /* return the minor part of the priority */
   return ( the_priority % 16 );
}

#if ( CPU_USE_GENERIC_BITFIELD_CODE == TRUE )


/**
 *  @brief gets the mask associated with the major or minor number passed to it
 *  
 *  This function returns the mask associated with the major or minor
 *  number passed to it.
 *
 *  @param[in] bit_number the bit number
 *
 *  @return returns the mask associated with the bit number
 */
RTEMS_INLINE_ROUTINE uint32_t _Priority_Mask(
                                             uint32_t bit_number
                                             )
{
   /* returns the mask associated with the bit number */
   return (0x8000 >> bit_number );
}


/**
 *  @brief translate the bit number returned by the bit scan
 *  
 *  This function translates the bit numbers returned by the bit scan
 *  of a priority bit field into something suitable for use as
 *  a major or minor component of a priority.
 *
 *  @param[in] bit_number the bit number
 *
 *  @return returns the bit number
 */
RTEMS_INLINE_ROUTINE uint32_t _Priority_Bits_index(
                                                   uint32_t bit_number
                                                   )
{
   /* return the specified bit_number */
   return bit_number;
}

#endif


/**
 *  @brief add a priority to a priority bit map
 *  
 *  This routine uses the_priority_map to update the priority
 *  bit maps to indicate that a thread has been readied.
 *
 *  @param[in] the_priority_map the priority map
 */
RTEMS_INLINE_ROUTINE void _Priority_Add_to_bit_map(
                                                   Priority_Information *the_priority_map
                                                   )
{
   /* set the priority minor number with the ready minor numbers */
   *the_priority_map->minor |= the_priority_map->ready_minor;

   /* add to the major bit map the ready major priority bits */
   _Priority_Major_bit_map |= the_priority_map->ready_major;
}


/**
 *  @brief remove a priority from a priority bit map
 *  
 *  This routine uses the_priority_map to update the priority
 *  bit maps to indicate that a thread has been removed from the
 *  ready state.
 *
 *  @param[in] the_priority_map the priority map
 */
RTEMS_INLINE_ROUTINE void _Priority_Remove_from_bit_map(
                                                        Priority_Information *the_priority_map
                                                        )
{
   /* remove from the priorirty minor the bloked minor numbers*/
   *the_priority_map->minor &= the_priority_map->block_minor;

   /* if there are no minor */
   if(*the_priority_map->minor == 0)
   {
      /* then reset the major */
      _Priority_Major_bit_map &= the_priority_map->block_major;
   }
}


/**
 *  @brief get highest priority from the priority bit map
 *  
 *  This function returns the priority of the highest priority
 *  ready thread.
 *
 *  @return returns the highest priority from the bit map
 */
RTEMS_INLINE_ROUTINE Priority_Control _Priority_Get_highest(void)
{
   /* priority minor number */
   Priority_Bit_map_control minor;

   /* priority major number */
   Priority_Bit_map_control major;


   /* find the first bit in the major priority bit map */
   _Bitfield_Find_first_bit(_Priority_Major_bit_map , major);

   /* find the first bit in the priority bit map of the major found */
   _Bitfield_Find_first_bit(_Priority_Bit_map[major] , minor);

   /* return the highest priority (sum of the major * 16 with the minor) */
   return (_Priority_Bits_index(major) << 4 ) +
       _Priority_Bits_index(minor);
}


/**
 *  @brief initialize the priority map with a new priority
 *  
 *  This routine initializes the_priority_map so that it
 *  contains the information necessary to manage a thread
 *  at new_priority.
 *
 *  @param[in] the_priority_map the priority map
 *  @param[in] new_priority the new priority
 */
RTEMS_INLINE_ROUTINE void _Priority_Initialize_information(
                                                           Priority_Information *the_priority_map ,
                                                           Priority_Control new_priority
                                                           )
{
   /* priority major number */
   Priority_Bit_map_control major;

   /* priority minor number */
   Priority_Bit_map_control minor;

   /* priority mask */
   Priority_Bit_map_control mask;


   /* get the major number of the priority */
   major = _Priority_Major(new_priority);

   /* get the minor number of the priority */
   minor = _Priority_Minor(new_priority);

   /* set the bit map minor number to the address of the priority bit map
    * of the major number */
   the_priority_map->minor = &_Priority_Bit_map[ _Priority_Bits_index(major) ];

   /* get the major number mask */
   mask = _Priority_Mask(major);

   /* set the ready major */
   the_priority_map->ready_major = mask;

   /* reset the block major */
   the_priority_map->block_major = ~mask;

   /* get the minor number mask */
   mask = _Priority_Mask(minor);

    /* set the ready minor */
   the_priority_map->ready_minor = mask;

   /* reset the block minor */
   the_priority_map->block_minor = ~mask;
}

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */
