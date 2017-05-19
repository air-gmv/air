/**
 *  @file
 *  userext.c
 *
 *  @brief initialize the User Extension Handler
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2007.
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
 *  6325    | 01/03/2010  | mcoutinho    | IPR 1931
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreUserExt User Extension Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <string.h>


Chain_Control _User_extensions_List;


Chain_Control _User_extensions_Switches_list;


void _User_extensions_Handler_initialization(
                                             uint32_t number_of_extensions ,
                                             User_extensions_Table *initial_extensions
                                             )
{
    /* iterator extension through the initial extensions */
    User_extensions_Control *extension;

    /* iterator index through the initial extensions */
    uint32_t i;


    /* initialize the user extension chain */
    _Chain_Initialize_empty(&_User_extensions_List);

    /* initialize the user extension switch chain */
    _Chain_Initialize_empty(&_User_extensions_Switches_list);

    /* if there are initial extensions */
    if(initial_extensions)
    {
        /* allocate a extension */
        extension = (User_extensions_Control *)
            _Workspace_Allocate_or_fatal_error(number_of_extensions *
                                               sizeof ( User_extensions_Control ));
        /* clear the user extensions */
        memset(extension , 0 ,
               number_of_extensions * sizeof ( User_extensions_Control ));

        /* for every extension */
        for(i = 0; i < number_of_extensions; i++)
        {
            /* add it */
            _User_extensions_Add_set(extension , &initial_extensions[i]);

            /* increment the number of extensions */
            extension++;
        }
    }
}


/**  
 *  @}
 */

/**
 *  @}
 */
