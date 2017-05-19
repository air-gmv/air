/**
 *  @file
 *  chain.c
 *
 *  @brief Initialize a chain
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
 *  8316    | 21/06/2010  | mcoutinho    | IPR 451
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


void _Chain_Initialize(
                       Chain_Control *the_chain ,
                       void *starting_address ,
                       size_t number_nodes ,
                       size_t node_size
                       )
{
    /* number of nodes left to process */
    size_t count;

    /* iterator node */
    Chain_Node *current;

    /* next node on the chain */
    Chain_Node *next;

    /* get the initial number of nodes */
    count = number_nodes;

    /* start at the chain head */
    current = _Chain_Head(the_chain);

    /* initialize the permanent null field of the chain */
    the_chain->permanent_null = NULL;

    /* initialize the next at the starting address */
    next = starting_address;

    /* run through every node */
    while(count--)
    {
        /* set the current node next field */
        current->next = next;

        /* set next node previous field */
        next->previous = current;

        /* update the current node */
        current = next;

        /* update the next node */
        next = (Chain_Node *)
            _Addresses_Add_offset((void *) next , node_size);
    }

    /* the last node next is the tail of the chain */
    current->next = _Chain_Tail(the_chain);

    /* set the last element of the chain */
    the_chain->last = current;
}

/**  
 *  @}
 */

/**
 *  @}
 */
