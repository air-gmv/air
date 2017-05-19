/**
 *  @file
 *  chain.h
 *
 *  @brief contains all the constants and structures associated
 *  with the Doubly-Linked Chain Handler.
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

#ifndef _RTEMS_SCORE_CHAIN_H
#define _RTEMS_SCORE_CHAIN_H

/**
 *  @defgroup ScoreChain Chain Handler
 *
 *   @brief The Chain Handler is used to manage sets of entities.
 *
 *  This handler
 *  provides two data structures.  The Chain Node data structure is included
 *  as the first part of every data structure that will be placed on
 *  a chain.  The second data structure is Chain Control which is used
 *  to manage a set of Chain Nodes.
 */
/**@{*/

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/score/address.h>
#include <stddef.h>

   /**
    *  @brief node of a chain
    *
    *  This type definition promotes the name for the Chain Node used by
    *  all RTEMS code.  It is a separate type definition because a forward
    *  reference is required to define it.  See @ref Chain_Node_struct for
    *  detailed information.
    */
   typedef struct Chain_Node_struct Chain_Node;

   /**
    *  @brief node of a chain declaration
    *
    *  This is used to manage each element (node) which is placed
    *  on a chain.
    *
    *  @note Typically, a more complicated structure will use the
    *        chain package.  The more complicated structure will
    *        include a chain node as the first element in its
    *        control structure.  It will then call the chain package
    *        with a pointer to that node element.  The node pointer
    *        and the higher level structure start at the same address
    *        so the user can cast the pointers back and forth.
    *
    */
   struct Chain_Node_struct
   {
      /**
       * @brief point to the node after to this one on this chain.
       */
      Chain_Node *next;
      /**
       * @brief point to the node immediate prior to this one on this chain.
       */
      Chain_Node *previous;
   };

   /**
    *  @brief Chain declaration
    *
    * manage a chain.  A chain consists of a doubly
    * linked list of zero or more nodes.
    *
    * @note This implementation does not require special checks for
    *   manipulating the first and last elements on the chain.
    *   To accomplish this the @a Chain_Control structure is
    *   treated as two overlapping @ref Chain_Node structures.
    *   The permanent head of the chain overlays a node structure on the
    *   @a first and @a permanent_null fields.  The permanent tail
    *   of the chain overlays a node structure on the
    *   @a permanent_null and @a last elements of the structure.
    *
    */
   typedef struct
   {
      /**
       * @brief point to the first node on this chain.
       */
      Chain_Node *first;
      /**
       * @brief This field is always 0.
       */
      Chain_Node *permanent_null;
      /**
       * @brief point to the last node on this chain.
       */
      Chain_Node *last;
   } Chain_Control;

   /**
    *  @brief initialize a chain
    *
    *  This routine initializes @a the_chain structure to manage the
    *  contiguous array of @a number_nodes nodes which starts at
    *  @a starting_address.  Each node is of @a node_size bytes.
    *
    *  @param[in] the_chain specifies the chain to initialize
    *  @param[in] starting_address is the starting address of the array
    *  of elements
    *  @param[in] number_nodes is the numebr of nodes that will be in the chain
    *  @param[in] node_size is the size of each node
    */
   void _Chain_Initialize(
                          Chain_Control *the_chain ,
                          void *starting_address ,
                          size_t number_nodes ,
                          size_t node_size
                          );

   /**
    *  @brief extract a chain node
    *
    *  This kernel routine deletes the given node from a chain.
    *  INTERRUPT LATENCY:
    *  only case
    *
    *  @param[in] the_node pointer to node in chain to be deleted
    */
   void _Chain_Extract(
                       Chain_Node *the_node
                       );

   /**
    *  @brief get a chain node
    *
    *  This kernel routine returns a pointer to a node taken from the
    *  given chain.
    *  INTERRUPT LATENCY:
    *  only case
    *
    *  @param[in] the_chain pointer to chain header
    *
    *  @return return_node pointer to node in chain allocated
    *  CHAIN_END if no nodes available
    */
   Chain_Node *_Chain_Get(
                          Chain_Control *the_chain
                          );

   /**
    *  @brief append a node on a chain
    *
    *  This kernel routine puts a node on the end of the specified chain.
    *  INTERRUPT LATENCY:
    *  only case
    *
    *  @param[in] the_chain pointer to chain header
    *  @param[in] the_node address of node to put at rear of chain
    */
   void _Chain_Append(
                      Chain_Control *the_chain ,
                      Chain_Node *the_node
                      );

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/chain.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */
