/**
 *  @file
 *  chain.inl
 *
 *  @brief contains the bodies of the routines which are
 *  associated with doubly linked chains and inlined.
 *
 *  @note  The routines in this file are ordered from simple
 *         to complex.  No other Chain Handler routine is referenced
 *         unless it has already been defined.
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  3550    | 29/06/2009  | mcoutinho    | IPR 539
 *  4546    | 28/09/2009  | mcoutinho    | IPR 778
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
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

#ifndef _RTEMS_SCORE_CHAIN_INL
#define _RTEMS_SCORE_CHAIN_INL

#include <rtems/system.h>

/**
 *  @brief get Chain Head
 *  
 *  This function returns a pointer to the first node on the chain.
 *  
 *  @param[in] the_chain is the chain to be operated upon.
 *  
 *  @return This method returns the permanent head node of the chain.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Head(
                                             Chain_Control *the_chain
                                             )
{
   /* returns the chain itself (is interpreted as the first two fields) */
   return (Chain_Node *) the_chain;
}


/**
 *  @brief get Chain Tail
 *  
 *  This function returns a pointer to the last node on the chain.
 *  
 *  @param[in] the_chain is the chain to be operated upon.
 *  
 *  @return This method returns the permanent tail node of the chain.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Tail(
                                             Chain_Control *the_chain
                                             )
{
   /* the tail is chain itself with an offset of 4 bytes (start at permanent null) */
   return (Chain_Node * ) &the_chain->permanent_null;
}


/**
 *  @brief determine if the Chain is empty
 *  
 *  This function returns TRUE if there a no nodes on @a the_chain and
 *  FALSE otherwise.
 *  
 *  @param[in] the_chain is the chain to be operated upon.
 *  
 *  @return This function returns TRUE if there a no nodes on @a the_chain and
 *  FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Chain_Is_empty(
                                             Chain_Control *the_chain
                                             )
{
   /* check if the first field is equal to the tail of the chain (no elements) */
   return (the_chain->first == _Chain_Tail(the_chain) );
}


/**
 *  @brief determine if the chain has only one node
 *  
 *  This function returns TRUE if there is only one node on @a the_chain and
 *  FALSE otherwise.
 *  
 *  @param[in] the_chain is the chain to be operated upon.
 *  
 *  @return This function returns TRUE if there is only one node on
 *  @a the_chain and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Chain_Has_only_one_node(
                                                      const Chain_Control *the_chain
                                                      )
{
   /* check if the first is equal to the last (has one node) */
   return (the_chain->first == the_chain->last );
}


/**
 *  @brief determine if the node is the chain head
 *  
 *  This function returns TRUE if @a the_node is the head of the_chain and
 *  FALSE otherwise.
 *  
 *  @param[in] the_chain is the chain to be operated upon.
 *  @param[in] the_node is the node to check for being the Chain Head.
 *  
 *  @return This function returns TRUE if @a the_node is the head of
 *  @a the_chain and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Chain_Is_head(
                                            Chain_Control *the_chain ,
                                            const Chain_Node *the_node
                                            )
{
   /* check if the node is the head */
   return (the_node == _Chain_Head(the_chain) );
}


/**
 *  @brief determine if the node is the chain tail
 *  
 *  This function returns TRUE if the_node is the tail of the_chain and
 *  FALSE otherwise.
 *  
 *  @param[in] the_chain is the chain to be operated upon.
 *  @param[in] the_node is the node to check for being the Chain Tail.
 *
 *  @return returns TRUE if the node is the chain tail and FALSE otherwise
 */
RTEMS_INLINE_ROUTINE boolean _Chain_Is_tail(
                                            Chain_Control *the_chain ,
                                            const Chain_Node *the_node
                                            )
{
   /* check if the node is the tail */
   return (the_node == _Chain_Tail(the_chain) );
}


/**
 *  @brief initialize an empty chain
 *  
 *  This routine initializes the specified chain to contain zero nodes.
 *  
 *  @param[in] the_chain is the chain to be initialized.
 */
RTEMS_INLINE_ROUTINE void _Chain_Initialize_empty(
                                                  Chain_Control *the_chain
                                                  )
{
   /* initialize the first field (equal to permanent null address) */
   the_chain->first = _Chain_Tail(the_chain);

   /* the permanent null is null */
   the_chain->permanent_null = NULL;

   /* the last is the address of the first field (no elements are in the chain) */
   the_chain->last = _Chain_Head(the_chain);
}


/**
 *  @brief extract a Node (unprotected)
 *  
 *  This routine extracts the_node from the chain on which it resides.
 *  It does NOT disable interrupts to ensure the atomicity of the
 *  extract operation.
 *  
 *  @param[in] the_node is the node to be extracted.
 */
RTEMS_INLINE_ROUTINE void _Chain_Extract_unprotected(
                                                     Chain_Node *the_node
                                                     )
{
   /* next node */
   Chain_Node *next;

   /* previous node */
   Chain_Node *previous;


   /* extract the node from the chain */

   /* save the next node */
   next = the_node->next;

   /* save the previous node */
   previous = the_node->previous;

   /* set the next previous to the previous */
   next->previous = previous;

   /* set the previous next to the next */
   previous->next = next;

   /* the previous node and the next node have been changed in order to remove
    * the node */
}


/**
 *  @brief get the First Node of a chain (unprotected)
 *  
 *  This function removes the first node from the_chain and returns
 *  a pointer to that node.  It does NOT disable interrupts to ensure
 *  the atomicity of the get operation.
 *  
 *  @param[in] the_chain is the chain to attempt to get the first node from.
 *  
 *  @return This method returns the first node on the chain even if it is
 *  the Chain Tail.
 *  
 *  @note This routine assumes that there is at least one node on the chain
 *  and always returns a node even if it is the Chain Tail.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Get_first_unprotected(
                                                              Chain_Control *the_chain
                                                              )
{
   /* auxiliary variable for the return value of this function */
   Chain_Node *return_node;

   /* pointer to the new first node */
   Chain_Node *new_first;


   /* get the first node */
   return_node = the_chain->first;

   /* get the second node (will be the new first) */
   new_first = return_node->next;

   /* set the second node to be the new first */
   the_chain->first = new_first;

   /* set the new first node previous field to the chain head */
   new_first->previous = _Chain_Head(the_chain);

   /* first node has been removed, return it */
   return return_node;
}


/**
 *  @brief get the First Node of a chain (unprotected)
 *  
 *  This function removes the first node from the_chain and returns
 *  a pointer to that node.  If the_chain is empty, then NULL is returned.
 *  
 *  @param[in] the_chain is the chain to attempt to get the first node from.
 *  
 *  @return This method returns the first node on the chain or NULL if the
 *  chain is empty.
 *  
 *  @note It does NOT disable interrupts to ensure the atomicity of the
 *  get operation.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Get_unprotected(
                                                        Chain_Control *the_chain
                                                        )
{
   /* check if the chain is empty */
   if(!_Chain_Is_empty(the_chain))
   {
      /* chain not empty, get the first node */
      return _Chain_Get_first_unprotected(the_chain);
   }
   else
   {
      /* chain is empty, nothing to get */
      return NULL;
   }
}


/**
 *  @brief insert a Node to a chain (unprotected)
 *  
 *  This routine inserts the_node on a chain immediately following
 *  after_node.
 *  
 *  @param[in] after_node is the node which will precede @a the_node on the
 *  chain.
 *  @param[in] the_node is the node to be inserted.
 *  
 *  @note It does NOT disable interrupts to ensure the atomicity
 *  of the extract operation.
 */
RTEMS_INLINE_ROUTINE void _Chain_Insert_unprotected(
                                                    Chain_Node *after_node ,
                                                    Chain_Node *the_node
                                                    )
{
   /* auxiliary variable for the node before */
   Chain_Node *before_node;

   /* insert the node */

   /* set the node previous field */
   the_node->previous = after_node;

   /* save the after node */
   before_node = after_node->next;

   /* the after node next is now the node */
   after_node->next = the_node;

   /* the node next is the before node */
   the_node->next = before_node;

   /* set the before node previous to the node */
   before_node->previous = the_node;

   /* node has been inserted in the chain after the after_node */
}


/**
 *  @brief append a Node to a chain (unprotected)
 *  
 *  This routine appends the_node onto the end of the_chain.
 *  
 *  @param[in] the_chain is the chain to be operated upon.
 *  @param[in] the_node is the node to be appended.
 *  
 *  @note It does NOT disable interrupts to ensure the atomicity of the
 *  append operation.
 */
RTEMS_INLINE_ROUTINE void _Chain_Append_unprotected(
                                                    Chain_Control *the_chain ,
                                                    Chain_Node *the_node
                                                    )
{
   /* auxiliary variable for the old last node */
   Chain_Node *old_last_node;

   /* append a node to the end of the chain */

   /* set the node next field to the end of the chain */
   the_node->next = _Chain_Tail(the_chain);

   /* save the current chain last node */
   old_last_node = the_chain->last;

   /* update the last node of the chain */
   the_chain->last = the_node;

   /* the old last node next field poonts to the node */
   old_last_node->next = the_node;

   /* the node previous field points to the old last node */
   the_node->previous = old_last_node;

   /* node has been appended to the end of the chain */
}


/**
 *  @brief prepend a Node to a chain (unprotected)
 *  
 *  This routine prepends the_node onto the front of the_chain.
 *  
 *  @param[in] the_chain is the chain to be operated upon.
 *  @param[in] the_node is the node to be prepended.
 *  
 *  @note It does NOT disable interrupts to ensure the atomicity of the
 *  prepend operation.
 */
RTEMS_INLINE_ROUTINE void _Chain_Prepend_unprotected(
                                                     Chain_Control *the_chain ,
                                                     Chain_Node *the_node
                                                     )
{
   /* prepend is the same thing as inserting at the head of the chain */
   _Chain_Insert_unprotected(_Chain_Head(the_chain) , the_node);
}

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */
