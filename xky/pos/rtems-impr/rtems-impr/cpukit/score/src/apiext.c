/**
 *  @file
 *  apiext.c
 *
 *  @brief routines to implement the API Extension Handler
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
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6325    | 01/03/2010  | mcoutinho    | IPR 1931
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreAPIExtension API Extension Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/apiext.h>


Chain_Control _API_extensions_List;


void _API_extensions_Initialization(void)
{
    /* initialize the API extension chain */
    _Chain_Initialize_empty(&_API_extensions_List);
}


void _API_extensions_Add(
                         API_extensions_Control *the_extension
                         )
{
    /* append the extension to the API extension list */
    _Chain_Append(&_API_extensions_List , &the_extension->Node);
}


void _API_extensions_Run_predriver(void)
{
    /* iterator node on the chain */
    Chain_Node *the_node;

    /* iterator extension */
    API_extensions_Control *the_extension;


    /* run through every node on the list */
    for(the_node = _API_extensions_List.first;
        !_Chain_Is_tail(&_API_extensions_List , the_node);
        the_node = the_node->next)
    {
        /* get the extension from the node on the chain */
        the_extension = (API_extensions_Control *) the_node;

        /* check if there is a pre-driver hook */
        if(the_extension->predriver_hook)
        {
            /* if there is, then call it */
            ( *the_extension->predriver_hook )( );
        }
    }
}


void _API_extensions_Run_postdriver(void)
{
    /* iterator node on the chain */
    Chain_Node *the_node;

    /* iterator extension */
    API_extensions_Control *the_extension;


    /* run through every node on the list */
    for(the_node = _API_extensions_List.first;
        !_Chain_Is_tail(&_API_extensions_List , the_node);
        the_node = the_node->next)
    {
        /* get the extension from the node on the chain */
        the_extension = (API_extensions_Control *) the_node;

        /* check if there is a post-driver hook */
        if(the_extension->postdriver_hook)
        {
            /* if there is, then call it */
            ( *the_extension->postdriver_hook )( );
        }
    }
}

/* end of file */

/**  
 *  @}
 */

/**
 *  @}
 */
