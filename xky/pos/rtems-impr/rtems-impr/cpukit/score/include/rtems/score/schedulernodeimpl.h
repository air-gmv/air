/*
 * Copyright (c) 2014, 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULERNODEIMPL_H
#define _RTEMS_SCORE_SCHEDULERNODEIMPL_H

#include <rtems/score/schedulernode.h>
#include <rtems/score/priorityimpl.h>

struct _Scheduler_Control;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SCHEDULER_NODE_OF_WAIT_PRIORITY_NODE( node ) \
  RTEMS_CONTAINER_OF( node, Scheduler_Node, Wait.Priority.Node.Node.Chain )

#define SCHEDULER_NODE_OF_WAIT_PRIORITY( node ) \
  RTEMS_CONTAINER_OF( node, Scheduler_Node, Wait.Priority )

RTEMS_INLINE_ROUTINE void _Scheduler_Node_do_initialize(
  const struct _Scheduler_Control *scheduler,
  Scheduler_Node                  *node,
  Thread_Control                  *the_thread,
  Priority_Control                 priority
)
{
  node->owner = the_thread;

  node->Priority.value = priority;
  node->Priority.prepend_it = false;

#if defined(RTEMS_SMP)
  _Chain_Initialize_node( &node->Thread.Wait_node );
  node->Wait.Priority.scheduler = scheduler;
  node->user = the_thread;
  node->idle = NULL;
  _SMP_sequence_lock_Initialize( &node->Priority.Lock );
#else
  (void) scheduler;
  (void) the_thread;
#endif
}

RTEMS_INLINE_ROUTINE const Scheduler_Control *_Scheduler_Node_get_scheduler(
  const Scheduler_Node *node
)
{
  return _Priority_Get_scheduler( &node->Wait.Priority );
}

RTEMS_INLINE_ROUTINE Thread_Control *_Scheduler_Node_get_owner(
  const Scheduler_Node *node
)
{
  return node->owner;
}

RTEMS_INLINE_ROUTINE Priority_Control _Scheduler_Node_get_priority(
  Scheduler_Node *node,
  bool           *prepend_it_p
)
{
  Priority_Control priority;
  bool             prepend_it;

#if defined(RTEMS_SMP)
  unsigned int     seq;

  do {
    seq = _SMP_sequence_lock_Read_begin( &node->Priority.Lock );
#endif

    priority = node->Priority.value;
    prepend_it = node->Priority.prepend_it;

#if defined(RTEMS_SMP)
  } while ( _SMP_sequence_lock_Read_retry( &node->Priority.Lock, seq ) );
#endif

  *prepend_it_p = prepend_it;

  return priority;
}

RTEMS_INLINE_ROUTINE void _Scheduler_Node_set_priority(
  Scheduler_Node   *node,
  Priority_Control  new_priority,
  bool              prepend_it
)
{
#if defined(RTEMS_SMP)
  unsigned int seq;

  seq = _SMP_sequence_lock_Write_begin( &node->Priority.Lock );
#endif

  node->Priority.value = new_priority;
  node->Priority.prepend_it = prepend_it;

#if defined(RTEMS_SMP)
  _SMP_sequence_lock_Write_end( &node->Priority.Lock, seq );
#endif
}

#if defined(RTEMS_SMP)
RTEMS_INLINE_ROUTINE Thread_Control *_Scheduler_Node_get_user(
  const Scheduler_Node *node
)
{
  return node->user;
}

RTEMS_INLINE_ROUTINE void _Scheduler_Node_set_user(
  Scheduler_Node *node,
  Thread_Control *user
)
{
  node->user = user;
}

RTEMS_INLINE_ROUTINE Thread_Control *_Scheduler_Node_get_idle(
  const Scheduler_Node *node
)
{
  return node->idle;
}
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERNODEIMPL_H */
