/**
 *  @file
 *  userextthreadswitch.c
 *
 *  @brief the thread switch extension which invokes all thread switch
 *  extensions added
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


void _User_extensions_Thread_switch(
                                    Thread_Control *executing ,
                                    Thread_Control *heir
                                    )
{
    /* iterator node through the chain of extensions thread switches */
    Chain_Node *the_node;

    /* iterator extension through the chain of extensions thread switches */
    User_extensions_Switch_control *the_extension_switch;

    /* call every user thread context switch extension */
    for(the_node = _User_extensions_Switches_list.first;
        !_Chain_Is_tail(&_User_extensions_Switches_list , the_node);
        the_node = the_node->next)
    {
        /* get the extension from the node on the chain */
        the_extension_switch = (User_extensions_Switch_control *) the_node;

        /* call the thread context switch extension */
        ( *the_extension_switch->thread_switch )( executing , heir );
    }
}

/**  
 *  @}
 */

/**
 *  @}
 */
