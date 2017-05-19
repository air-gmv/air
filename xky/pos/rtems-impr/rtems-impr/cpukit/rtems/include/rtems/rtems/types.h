/**
 *  @file
 *  types.h
 *
 *  @brief definition of the types used by the RTEMS API.
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
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
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

#ifndef _RTEMS_RTEMS_TYPES_H
#define _RTEMS_RTEMS_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

   /*
    *  RTEMS basic type definitions
    */

#include <stdint.h>
#include <rtems/score/heap.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>
#include <rtems/rtems/modes.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#include <rtems/score/mppkt.h>
#endif

   /**
    * @brief RTEMS name
    **/
   typedef uint32_t rtems_name;

   /**
    * @brief RTEMS object identifer
    **/
   typedef Objects_Id rtems_id;

   /**
    * @brief RTEMS context
    **/
   typedef Context_Control rtems_context;

   /**
    * @brief RTEMS floating point context
    **/
   typedef Context_Control_fp rtems_context_fp;

   /**
    * @brief RTEMS interrupt frame
    **/
   typedef CPU_Interrupt_frame rtems_interrupt_frame;

   /**
    *  @brief RTEMS interval
    */
   typedef Watchdog_Interval rtems_interval;

   /**
    * @brief RTEMS boolean
    */
   typedef boolean rtems_boolean;

   /**
    *  @brief define the time of control block.  This
    *  control block is used to maintain the current time of day.
    *
    *  @note This is an RTEID style time/date.
    */
   typedef struct
   {
      /** This field is the year, A.D. */
      uint32_t year;
      /** This field is the month, 1 -> 12 */
      uint32_t month;
      /** This field is the day, 1 -> 31 */
      uint32_t day;
      /** This field is the hour, 0 -> 23 */
      uint32_t hour;
      /** This field is the minute, 0 -> 59 */
      uint32_t minute;
      /** This field is the second, 0 -> 59 */
      uint32_t second;
      /** This field is the elapsed ticks between secs */
      uint32_t ticks;
   } rtems_time_of_day;

   /*
    *  @brief define the type for an RTEMS API task mode.
    */
   typedef Modes_Control rtems_mode;

   /*
    *  MPCI related entries
    */

#if defined(RTEMS_MULTIPROCESSING)
   typedef MP_packet_Classes rtems_mp_packet_classes;
   typedef MP_packet_Prefix rtems_packet_prefix;

   typedef MPCI_initialization_entry rtems_mpci_initialization_entry;
   typedef MPCI_get_packet_entry rtems_mpci_get_packet_entry;
   typedef MPCI_return_packet_entry rtems_mpci_return_packet_entry;
   typedef MPCI_send_entry rtems_mpci_send_packet_entry;
   typedef MPCI_receive_entry rtems_mpci_receive_packet_entry;

   typedef MPCI_Entry rtems_mpci_entry;

   typedef MPCI_Control rtems_mpci_table;
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
