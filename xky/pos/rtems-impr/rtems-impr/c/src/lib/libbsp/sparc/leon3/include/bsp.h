/**
 *  @file
 *  bsp.h
 *
 *  @brief This include file contains all SPARC simulator definitions.
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995.
 *  European Space Agency.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  555     | 17/11/2008  | mcoutinho    | IPR 66
 *  4341    | 15/09/2009  | mcoutinho    | IPR 606
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5475    | 09/11/2009  | mcoutinho    | IPR 862
 *  6524    | 09/03/2010  | mcoutinho    | IPR 1944
 *  8183    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SPARC_LEON3_BSP SPARC LEON3 BSP
 *  @{
 */

#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems.h>
#include <leon.h>
#include <rtems/score/cpu.h>
#include <sharedBSPs.h>
#include <sharedSPARC.h>
   
   extern int _leon3_clkirq;

   /**
    * SPARC CPU variant: LEON3
    */
#define LEON3 1

   /**
    *  @brief scan in amba buses for interrupt controller and clock hardware
    */
   void bsp_leon3_predriver_hook(void);


#if (CPU_PROVIDES_IDLE_THREAD_BODY == TRUE)

   /**
    *  @brief entry point to the BSP idle thread
    *
    *  Some SPARC implementations have low power, sleep, or idle modes.  This
    *  tries to take advantage of those models.
    */
   void _CPU_Thread_Idle_body(void);

#endif /* CPU_PROVIDES_IDLE_THREAD_BODY */

   /**
    * @brief determine if this BSP has clock driver
    */
#define BSP_HAS_CLOCK_DRIVER                         TRUE

   /**
    * @brief determine if this BSP needs support at clock tick
    */
#define BSP_CLOCK_DRIVER_NEEDS_SUPPORT_AT_CLOCK_TICK FALSE

   /**
    * @brief determine if this BSP needs to find the timer first
    */
#define BSP_CLOCK_DRIVER_NEEDS_FIND_TIMER_SUPPORT    TRUE

   /**
    * @brief Leon3 has instruction cache
    */
#define HAS_INSTRUCTION_CACHE                        TRUE

   /**
    * @brief clock driver support to find timer
    **/
#if defined(RTEMS_MULTIPROCESSING)


#define Clock_driver_support_find_timer()\
{\
   int cnt;\
   amba_apb_device dev;\
   /* Find LEON3 GP Timer */\
   cnt = amba_find_apbslv(&amba_conf , VENDOR_GAISLER , GAISLER_GPTIMER , &dev);\
   if(cnt > 0)\
   {\
      /* Found APB GPTIMER Timer */\
      LEON3_Timer_Regs = ( volatile LEON3_Timer_Regs_Map * ) dev.start;\
      _leon3_clkirq = ( LEON3_Timer_Regs->status & 0xfc ) >> 3;\
   \
      if(Configuration.User_multiprocessing_table != NULL)\
      {\
         _leon3_clkirq += LEON3_Cpu_Index;\\
      }\
   }\
}
#else


#define Clock_driver_support_find_timer() \
{\
   int cnt;  \
   amba_apb_device dev;  \
   \
   /* Find LEON3 GP Timer */\
   cnt = amba_find_apbslv(&amba_conf , VENDOR_GAISLER , GAISLER_GPTIMER , &dev);  \
   if(cnt > 0)\
   {\
      /* Found APB GPTIMER Timer */\
      LEON3_Timer_Regs = ( volatile LEON3_Timer_Regs_Map * ) dev.start;  \
      _leon3_clkirq = ( LEON3_Timer_Regs->status & 0xfc ) >> 3;  \
   }   \
}

#endif


#ifdef __cplusplus
}
#endif

#endif

/**  
 *  @}
 */
