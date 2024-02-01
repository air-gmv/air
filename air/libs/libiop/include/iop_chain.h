/*
 * Copyright (C) 2011  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief
 */

#ifndef __IOP_CHAIN_H__
#define __IOP_CHAIN_H__

#include <air.h>
#include <stddef.h>

typedef struct __chain_node
{

    struct __chain_node *next;
    struct __chain_node *previous;

} iop_chain_node;

typedef struct
{

    iop_chain_node *first;
    iop_chain_node *permanent_null;
    iop_chain_node *last;

} iop_chain_control;

static inline iop_chain_node *iop_chain_head(iop_chain_control *chain)
{

    return (iop_chain_node *)chain;
}

static inline iop_chain_node *iop_chain_tail(iop_chain_control *chain)
{

    return (iop_chain_node *)&chain->permanent_null;
}

static inline int iop_chain_is_empty(iop_chain_control *chain)
{

    return (chain->first == iop_chain_tail(chain));
}

static inline int iop_chain_has_only_one_node(const iop_chain_control *chain)
{

    return (chain->first == chain->last);
}

static inline int iop_chain_is_head(iop_chain_control *chain, const iop_chain_node *node)
{

    return (node == iop_chain_head(chain));
}

static inline int iop_chain_is_tail(iop_chain_control *chain, const iop_chain_node *node)
{

    return (node == iop_chain_tail(chain));
}

static inline void iop_chain_initialize_empty(iop_chain_control *chain)
{

    chain->first = iop_chain_tail(chain);
    chain->permanent_null = NULL;
    chain->last = iop_chain_head(chain);
}

static inline void iop_chain_extract_unprotected(iop_chain_node *node)
{

    iop_chain_node *next;
    iop_chain_node *previous;

    next = node->next;
    previous = node->previous;

    next->previous = previous;
    previous->next = next;
}

static inline void iop_chain_extract(iop_chain_node *node)
{

    //   ISR_Level level;
    //   _ISR_Local_disable(level);
    iop_chain_extract_unprotected(node);
    //   _ISR_Local_enable(level);
}

static inline iop_chain_node *iop_chain_get_first_unprotected(iop_chain_control *chain)
{

    iop_chain_node *return_node;
    iop_chain_node *new_first;
    return_node = chain->first;
    new_first = return_node->next;
    chain->first = new_first;
    new_first->previous = iop_chain_head(chain);
    return return_node;
}

static inline iop_chain_node *iop_chain_get_unprotected(iop_chain_control *chain)
{

    if (!iop_chain_is_empty(chain))
    {
        return iop_chain_get_first_unprotected(chain);
    }
    else
    {
        return NULL;
    }
}

static inline iop_chain_node *iop_chain_get(iop_chain_control *chain)
{

    //    ISR_Level level;
    //   _ISR_Local_disable(level);
    iop_chain_node *node = iop_chain_get_unprotected(chain);
    //   _ISR_Local_enable(level);
    return node;
}

static inline void iop_chain_insert_unprotected(iop_chain_node *after_node, iop_chain_node *node)
{

    iop_chain_node *before_node;
    node->previous = after_node;
    before_node = after_node->next;
    after_node->next = node;
    node->next = before_node;
    before_node->previous = node;
}

static inline void iop_chain_append_unprotected(iop_chain_control *chain, iop_chain_node *node)
{

    iop_chain_node *old_last_node;
    node->next = iop_chain_tail(chain);
    old_last_node = chain->last;
    chain->last = node;
    old_last_node->next = node;
    node->previous = old_last_node;
}

static inline void iop_chain_prepend_unprotected(iop_chain_control *chain, iop_chain_node *node)
{

    iop_chain_insert_unprotected(iop_chain_head(chain), node);
}

static inline void iop_chain_prepend(iop_chain_control *chain, iop_chain_node *node)
{
    //  ISR_Level level;
    //  _ISR_Local_disable(level);
    iop_chain_prepend_unprotected(chain, node);
    //   _ISR_Local_enable(level);
}

static inline void iop_chain_append(iop_chain_control *chain, iop_chain_node *node)
{

    //  ISR_Level level;
    //   _ISR_Local_disable(level);
    iop_chain_append_unprotected(chain, node);
    //   _ISR_Local_enable(level);
}

static inline void iop_chain_initialize(iop_chain_control *chain, void *starting_address, size_t number_nodes,
                                        size_t node_size)
{

    /* number of nodes left to process */
    size_t count;

    /* iterator node */
    iop_chain_node *current;

    /* next node on the chain */
    iop_chain_node *next;

    /* get the initial number of nodes */
    count = number_nodes;

    /* start at the chain head */
    current = (iop_chain_node *)iop_chain_head(chain);

    /* initialize the permanent null field of the chain */
    chain->permanent_null = NULL;

    /* initialize the next at the starting address */
    next = starting_address;

    /* run through every node */
    while (count--)
    {

        /* set the current node next field */
        current->next = next;

        /* set next node previous field */
        next->previous = current;

        /* update the current node */
        current = next;

        /* update the next node */
        next = (iop_chain_node *)((air_u32_t)next + node_size);
    }

    /* the last node next is the tail of the chain */
    current->next = (iop_chain_node *)iop_chain_tail(chain);

    /* set the last element of the chain */
    chain->last = current;
}

#endif
