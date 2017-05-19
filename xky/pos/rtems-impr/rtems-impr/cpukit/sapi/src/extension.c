/**
 *  @file
 *  extension.c
 *
 *  @brief initialize the extension manager
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
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  7130    | 09/04/2010  | mcoutinho    | IPR 1931
 *  8315    | 21/06/2010  | mcoutinho    | IPR 451
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


Objects_Information _Extension_Information;


void _Extension_Manager_initialization(
                                       uint32_t maximum_extensions
                                       )
{
    /* initialize the object extension information with:
     *   the extension table information
     *   object API
     *   extension class
     *   number of maximum extensions
     *   size of each extension
     *   name is not a string
     *   maximum name for the string
     *   if multiprocessing, the extension is not global
     *   if multiprocessing, no proxy extraction support callout function */
    _Objects_Initialize_information(&_Extension_Information ,
                                    OBJECTS_CLASSIC_API ,
                                    OBJECTS_RTEMS_EXTENSIONS ,
                                    maximum_extensions ,
                                    sizeof ( Extension_Control ) ,
                                    FALSE , 
                                    RTEMS_MAXIMUM_NAME_LENGTH 
#if defined(RTEMS_MULTIPROCESSING)
        ,
                                    FALSE , 
                                    NULL 
#endif
        );
}

/**  
 *  @}
 */

/**  
 *  @}
 */

