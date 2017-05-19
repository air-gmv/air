/**
 *  @file
 *  userextthreadcreate.c
 *
 *  @brief the thread create extension which invokes all thread create
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


boolean _User_extensions_Thread_create(
                                       Thread_Control *the_thread
                                       )
{
    /* iterator node through the chain of extensions */
    Chain_Node *the_node;

    /* iterator extension through the chain of extensions */
    User_extensions_Control *the_extension;

    /* extension create thread decision */
    boolean status;


    /* call every user extension */
    for(the_node = _User_extensions_List.first;
        !_Chain_Is_tail(&_User_extensions_List , the_node);
        the_node = the_node->next)
    {
        /* get the extension from the node on the chain */
        the_extension = (User_extensions_Control *) the_node;

        /* if it has the thread create extension */
        if(the_extension->Callouts.thread_create != NULL)
        {
            /* then call it */
            status = ( *the_extension->Callouts.thread_create )( _Thread_Executing ,
                                                                 the_thread );

            /* if the user create thread extension does not return successful then
             * dont create the thread */
            if(!status)
            {
                /* if there any user extension that does not want the thread to be
                 * created, then dont create it */
                return FALSE;
            }
        }
    }

    /* all extensions allowed the thread to be created */
    return TRUE;
}

/**  
 *  @}
 */

/**
 *  @}
 */
