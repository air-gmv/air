/**
 *  @file
 *  objectallocate.c
 *
 *  @brief allocate an object from the object control blocks
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
 *  3520    | 29/06/2009  | mcoutinho    | IPR 531
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6325    | 01/03/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
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


Objects_Control *_Objects_Allocate(
                                   Objects_Information *information
                                   )
{
    /* object to allocate */
   Objects_Control *the_object;


   /* if the application is using the optional manager stubs and
    * still attempts to create the object, the information block
    * should be all zeroed out because it is in the BSS.  So let's
    * check that code for this manager is even present */
   if(information->size == 0)
   {
      /* could not allocate */
      return NULL;
   }

   /* OK. The manager should be initialized and configured to have objects.
    * With any luck, it is safe to attempt to allocate an object. */
   the_object = (Objects_Control *) _Chain_Get(&information->Inactive);

   /* return the allocated object */
   return the_object;
}

/**  
 *  @}
 */

/**
 *  @}
 */
