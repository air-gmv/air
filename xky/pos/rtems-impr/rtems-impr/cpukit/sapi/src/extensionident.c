/**
 *  @file
 *  extensionident.c
 *
 *  @brief ident a user extension from the extension name
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


rtems_status_code rtems_extension_ident(
                                        rtems_name name ,
                                        Objects_Id *id
                                        )
{
    /* status of the object to name to id translation */
    Objects_Name_or_id_lookup_errors status;

    /* get the extension identifer from:
     *   the user extension object information
     *   the user extension name
     *   search only local node (if multiprocessing is enabled)
     *   pass the id address (to place the id) */
    status = _Objects_Name_to_id(&_Extension_Information ,
                                 (Objects_Name) name ,
#if defined(RTEMS_MULTIPROCESSING)
        OBJECTS_SEARCH_LOCAL_NODE ,
#endif
        id);

    /* return the status of the object get translated to rtems_status_code */
    return _Status_Object_name_errors_to_status[ status ];
}

/**  
 *  @}
 */

/**  
 *  @}
 */

