/**
 *  @file
 *  extensioncreate.c
 *
 *  @brief create an user extension
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


rtems_status_code rtems_extension_create(
                                         rtems_name name ,
                                         rtems_extensions_table *extension_table ,
                                         Objects_Id *id
                                         )
{
    /* user extension to create */
    Extension_Control *the_extension;


    /* check if the name is valid */
    if(!rtems_is_name_valid(name))
    {
        /* return invalid user extension name */
        return RTEMS_INVALID_NAME;
    }

    /* disable thread dispatch to prevent deletion */
    _Thread_Disable_dispatch();

    /* allocate the extension */
    the_extension = _Extension_Allocate();

    /* if the extension could not be allocated */
    if(!the_extension)
    {
        /* re-enable thread dispatch */
        _Thread_Enable_dispatch();

        /* return too many user extensions created */
        return RTEMS_TOO_MANY;
    }

    /* add the extension to the system extension set */
    _User_extensions_Add_set(&the_extension->Extension , extension_table);

    /* open an object extension information */
    _Objects_Open(&_Extension_Information ,
                  &the_extension->Object ,
                  (Objects_Name) name);

    /* return the identifier */
    *id = the_extension->Object.id;

    /* re-enable thread dispatch */
    _Thread_Enable_dispatch();

    /* and return success */
    return RTEMS_SUCCESSFUL;
}

/**  
 *  @}
 */

/**  
 *  @}
 */

