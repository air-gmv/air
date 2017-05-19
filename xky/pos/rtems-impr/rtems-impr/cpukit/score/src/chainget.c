/**
 *  @file
 *  chainget.c
 *
 *  @brief get a node from a chain
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
 *  @addtogroup ScoreChain Chain Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/address.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>


Chain_Node *_Chain_Get(
                       Chain_Control *the_chain
                       )
{
    /* interrupt level */
    ISR_Level level;

    /* function result */
    Chain_Node *return_node;

    
    /* reset the function result to NULL */
    return_node = NULL;

    /* enter critical section */
    _ISR_Disable(level);

    /* check if chain is empty */
    if(!_Chain_Is_empty(the_chain))
    {
        /* if not empty, then get the first node */
        return_node = _Chain_Get_first_unprotected(the_chain);
    }

    /* leave critical section */
    _ISR_Enable(level);

    /* return the function result */
    return return_node;
}

/**  
 *  @}
 */

/**
 *  @}
 */
