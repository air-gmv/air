/**
 *  @file
 *  userextaddset.c
 *
 *  @brief add a set to a user extension
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
 *  @addtogroup ScoreUserExt User Extension Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/userext.h>


void _User_extensions_Add_set(
                              User_extensions_Control *the_extension ,
                              User_extensions_Table *extension_table
                              )
{
    /* set the extension callouts */
    the_extension->Callouts = *extension_table;

    /* append to the user extension list the new extension */
    _Chain_Append(&_User_extensions_List , &the_extension->Node);

    /* if a switch handler is present, append it to the switch chain */

    /* if there is a thread switch extension */
    if(extension_table->thread_switch != NULL)
    {
        /* set the user switch handler */
        the_extension->Switch.thread_switch = extension_table->thread_switch;

        /* append it to the user extension switch list */
        _Chain_Append(&_User_extensions_Switches_list ,
                      &the_extension->Switch.Node);
    }
}

/**  
 *  @}
 */

/**
 *  @}
 */
