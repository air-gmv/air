/**
 *  @file
 *  userextthreadbegin.c
 *
 *  @brief several extensions (thread begin, exitted and fatal) invoked at
 *  key points on the system lifetime
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


void _User_extensions_Thread_begin(
                                   Thread_Control *executing
                                   )
{
    /* iterator node through the chain of extensions */
    Chain_Node *the_node;

    /* iterator extension through the chain of extensions */
    User_extensions_Control *the_extension;


    /* call every user extension */
    for(the_node = _User_extensions_List.first;
        !_Chain_Is_tail(&_User_extensions_List , the_node);
        the_node = the_node->next)
    {
        /* get the extension from the node on the chain */
        the_extension = (User_extensions_Control *) the_node;

        /* if it has the thread begin extension */
        if(the_extension->Callouts.thread_begin != NULL)
        {
            /* call the thread begin extension */
            ( *the_extension->Callouts.thread_begin )( executing );
        }
    }
}


void _User_extensions_Thread_exitted(
                                     Thread_Control *executing
                                     )
{
    /* iterator node through the chain of extensions */
    Chain_Node *the_node;

    /* iterator extension through the chain of extensions */
    User_extensions_Control *the_extension;


    /* call every user extension */
    for(the_node = _User_extensions_List.last;
        !_Chain_Is_head(&_User_extensions_List , the_node);
        the_node = the_node->previous)
    {
        /* get the extension from the node on the chain */
        the_extension = (User_extensions_Control *) the_node;

        /* if it has the thread exitted extension */
        if(the_extension->Callouts.thread_exitted != NULL)
        {
            /* call the thread exitted extension */
            ( *the_extension->Callouts.thread_exitted )( executing );
        }
    }
}


void _User_extensions_Fatal(
                            Internal_errors_Source the_source ,
                            boolean is_internal ,
                            uint32_t the_error
                            )
{
    /* iterator node through the chain of extensions */
    Chain_Node *the_node;

    /* iterator extension through the chain of extensions */
    User_extensions_Control *the_extension;


    /* call every user extension */
    for(the_node = _User_extensions_List.last;
        !_Chain_Is_head(&_User_extensions_List , the_node);
        the_node = the_node->previous)
    {
        /* get the extension from the node on the chain */
        the_extension = (User_extensions_Control *) the_node;

        /* if it has the fatal extension */
        if(the_extension->Callouts.fatal != NULL)
        {
            /* call the fatal extension */
            ( *the_extension->Callouts.fatal )( the_source , is_internal , the_error );
        }
    }
}

/**  
 *  @}
 */

/**
 *  @}
 */
