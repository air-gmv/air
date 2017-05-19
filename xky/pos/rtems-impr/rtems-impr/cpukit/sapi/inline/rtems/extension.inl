/**
 *  @file
 *  extension.inl
 *
 *  @brief static inline implementation of the inlined routines for the
 *  Extension Manager.
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
 *  4546    | 28/09/2009  | mcoutinho    | IPR 778
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_API Super API
 *  @{
 */

/**
 *  @addtogroup SUPER_API_EXTENSION Extension Manager
 *  @{
 */

#ifndef __EXTENSION_MANAGER_inl
#define __EXTENSION_MANAGER_inl

#include <rtems/system.h>


/**
 *  @brief allocate an extension
 *  
 *  This function allocates a extension control block from
 *  the inactive chain of free extension control blocks.
 *
 *  @return returns the allocated extension
 */
RTEMS_INLINE_ROUTINE Extension_Control *_Extension_Allocate(void)
{
   /* allocate the user extension object information */
   return (Extension_Control *) _Objects_Allocate(&_Extension_Information);
}


/**
 *  @brief free an extension
 *  
 *  This routine frees a extension control block to the
 *  inactive chain of free extension control blocks.
 *
 *  @param[in] the_extension the extension to free
 */
RTEMS_INLINE_ROUTINE void _Extension_Free(
                                          Extension_Control *the_extension
                                          )
{
   /* free the user extension object information */
   _Objects_Free(&_Extension_Information , &the_extension->Object);
}


/**
 *  @brief get an extension
 *  
 *  This function maps extension IDs to extension control blocks.
 *  If ID corresponds to a local extension, then it returns
 *  the extension control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  Otherwise, location is set
 *  to OBJECTS_ERROR and the returned value is undefined.
 *
 *  @param[in] id the extension id
 *  @param[out] location the location where is the extension
 *
 *  @return returns the extension (or NULL if none exists)
 */
RTEMS_INLINE_ROUTINE Extension_Control *_Extension_Get(
                                                       Objects_Id id ,
                                                       Objects_Locations *location
                                                       )
{
   /* return the extension object information with:
    *   the extension object information
    *   the id of the extension
    *   the address to place the location of the extension */
   return (Extension_Control *) _Objects_Get(&_Extension_Information ,
                                             id ,
                                             location);
}

#endif
/* end of include file */

/**  
 *  @}
 */

/**  
 *  @}
 */

