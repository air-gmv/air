/**
 *  @file
 *  objectcomparenamestring.c
 *
 *  @brief compare two objects name string
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2002.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6116    | 18/02/2010  | mcoutinho    | IPR 1818
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreObject Object Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/address.h>
#include <rtems/score/chain.h>
#include <rtems/score/object.h>
#if defined(RTEMS_MULTIPROCESSING)
   #include <rtems/score/objectmp.h>
#endif
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/isr.h>
#include <string.h>


boolean _Objects_Compare_name_string(
                                     void *name_1 ,
                                     void *name_2 ,
                                     uint16_t length
                                     )
{
   /* check if the names are equal */
   if(!strncmp(name_1 , name_2 , length))
   {
      /* they are equal */
      return TRUE;
   }

   /* the names are not equal */
   return FALSE;
}

/**  
 *  @}
 */

/**
 *  @}
 */
