/**
 *  @file
 *  bitfield.h
 *
 *  @brief contains all bit field manipulation routines.
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

#ifndef _RTEMS_SCORE_BITFIELD_H
#define _RTEMS_SCORE_BITFIELD_H

/**
 *  @defgroup ScoreBitfield Bitfield Handler
 *
 *  @brief This handler encapsulates functionality that is used to manipulate the
 *  priority bitfields used to lookup which priority has the highest
 *  priority ready to execute thread.
 */
/**@{*/

#ifdef __cplusplus
extern "C"
{
#endif

#if ( CPU_USE_GENERIC_BITFIELD_DATA == TRUE )

   /**
    *  @brief this table helps to perform a highly optimized bit scan without the
    *  use of special CPU instructions.
    */
   extern const unsigned char __log2table[256];

#endif

   /**
    *  @brief find the first bit set of a 32-bit variable
    *
    *  This routine returns the @a _bit_number of the first bit set
    *  in the specified value.  The correspondence between @a _bit_number
    *  and actual bit position is processor dependent.  The search for
    *  the first bit set may run from most to least significant bit
    *  or vice-versa.
    *
    *  @param[in] _value is the value to bit scan.
    *  @param[in] _bit_number is the position of the first bit set.
    *
    *  @note This routine is used when the executing thread is removed
    *  from the ready state and, as a result, its performance has a
    *  significant impact on the performance of the executive as a whole.
    *
    *  @note This routine must be a macro because if a CPU specific version
    *  is used it will most likely use inline assembly.
    */
#if ( CPU_USE_GENERIC_BITFIELD_CODE == FALSE )
#define _Bitfield_Find_first_bit( _value, _bit_number ) \
        _CPU_Bitfield_Find_first_bit( _value, _bit_number )
#else
#define _Bitfield_Find_first_bit( _value, _bit_number )  \
  {                                                      \
    register uint32_t   __value = (uint32_t  ) (_value); \
    register const unsigned char *__p = __log2table;     \
                                                         \
    if ( __value < 0x100 )                               \
    {                                                    \
      (_bit_number) = __p[ __value ] + 8;                \
    }                                                    \
    else                                                 \
    {                                                    \
      (_bit_number) = __p[ __value >> 8 ];               \
    }                                                    \
  }
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */

/**
 *  @}
 */
