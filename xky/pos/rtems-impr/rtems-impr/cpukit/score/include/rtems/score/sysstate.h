/**
 *  @file
 *  sysstate.h
 *
 *  @brief contains information regarding the system state.
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
 *  3610    | 02/07/2009  | mcoutinho    | IPR 64
 *  3610    | 02/07/2009  | mcoutinho    | IPR 153
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

#ifndef _RTEMS_SCORE_SYSSTATE_H
#define _RTEMS_SCORE_SYSSTATE_H

/**
 *  @defgroup ScoreSysState System State Handler
 *
 *  @brief This handler encapsulates functionality related to the management of the
 *  internal system state of RTEMS.
 */
/**@{*/

#include <rtems/system.h>
#include <rtems/score/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

   /**
    *  @brief possible system states.
    */
   typedef enum
   {
      /**
       * @brief This indicates that the system state is between the start
       *  of rtems_initialize_executive_early and the end of the first
       *  phase of initialization.
       */
      SYSTEM_STATE_BEFORE_INITIALIZATION ,
      /**
       * @brief This indicates that the system state is between end of the first
       *  phase of initializatin but before  multitasking is started.
       */
      SYSTEM_STATE_BEFORE_MULTITASKING ,
      /**
       * @brief This indicates that the system state is attempting to initiate
       *  multitasking.
       */
      SYSTEM_STATE_BEGIN_MULTITASKING ,
      /**
       * @brief This indicates that the system is up and operating normally.
       */
      SYSTEM_STATE_UP ,
      /**
       * @brief This indicates that an error was found and the system is now controled by the application
       */
      SYSTEM_STATE_APP_SAFE ,
      /**
       * @brief This indicates that the system is in the midst of a shutdown.
       */
      SYSTEM_STATE_SHUTDOWN ,
      /**
       * @brief This indicates that a fatal error has occurred.
       */
      SYSTEM_STATE_FAILED
   } System_state_Codes;

   /**
    * @brief This defines the first system state.
    */
#define SYSTEM_STATE_CODES_FIRST SYSTEM_STATE_BEFORE_INITIALIZATION

   /**
    * @brief This defines the highest value system state.
    */
#define SYSTEM_STATE_CODES_LAST  SYSTEM_STATE_FAILED

   /**
    *  @brief indicates whether or not this is
    *  an multiprocessing system.
    */
#ifdef RTEMS_MULTIPROCESSING
   extern boolean _System_state_Is_multiprocessing;
#endif

   /**
    *  @brief current system state
    */
   extern System_state_Codes _System_state_Current;

   /*
    *  Make it possible for the application to get the system state information.
    */

#include <rtems/score/sysstate.inl>

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */

/**
 *  @}
 */
