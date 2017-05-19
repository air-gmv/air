/**
 *  @file
 *  extensiondelete.c
 *
 *  @brief delete a user extension
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
 *  596     | 17/11/2008  | mcoutinho    | IPR 64
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  7132    | 09/04/2010  | mcoutinho    | IPR 1931
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

#include <rtems/system.h>
#include <rtems/rtems/support.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/extension.h>


rtems_status_code rtems_extension_delete(
                                         Objects_Id id
                                         )
{
    /* user extension to delete */
    Extension_Control *the_extension;

    /* user extension to delete location */
    Objects_Locations location;


    /* get the extension to be deleted */
    the_extension = _Extension_Get(id , &location);

    /* check the extension location */
    switch(location)
    {
            /* user extension id is invalid */
        case OBJECTS_ERROR:
#if defined(RTEMS_MULTIPROCESSING)
            /* user extension id is invalid */
        case OBJECTS_REMOTE:
            /* should never return this */
#endif
            /* return invalid id */
            return RTEMS_INVALID_ID;

            /* user extension is local (nominal case) */
        case OBJECTS_LOCAL:

            /* remove the user extension set */
            _User_extensions_Remove_set(&the_extension->Extension);

            /* close the extension object information */
            _Objects_Close(&_Extension_Information , &the_extension->Object);

            /* deallocate the memory of the extension */
            _Extension_Free(the_extension);

            /* re-enable thread dispatch (disabled by _Extension_Get) */
            _Thread_Enable_dispatch();

            /* and return success */
            return RTEMS_SUCCESSFUL;

            /* default clause: the object location is invalid */
        default:

            /* an internal error occured and the object location is invalid */
            return RTEMS_INTERNAL_INVALID_OBJECT_LOCATION;
    }
}

/**  
 *  @}
 */

/**  
 *  @}
 */

