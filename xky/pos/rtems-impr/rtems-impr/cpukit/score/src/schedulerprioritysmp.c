/**
 * @file
 *
 * @ingroup ScoreSchedulerPrioritySMP
 *
 * @brief Deterministic Priority SMP Scheduler Implementation
 */

/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/schedulerprioritysmpimpl.h>

static Scheduler_priority_SMP_Context *
_Scheduler_priority_SMP_Get_context( const Scheduler_Control *scheduler )
{
  return (Scheduler_priority_SMP_Context *) _Scheduler_Get_context( scheduler );
}

void _Scheduler_priority_SMP_Initialize( const Scheduler_Control *scheduler )
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Get_context( scheduler );

  _Scheduler_SMP_Initialize( &self->Base );
  _Priority_bit_map_Initialize( &self->Bit_map );
  _Scheduler_priority_Ready_queue_initialize(
    &self->Ready[ 0 ],
    scheduler->maximum_priority
  );
}

void _Scheduler_priority_SMP_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
)
{
  Scheduler_Context              *context;
  Scheduler_priority_SMP_Context *self;
  Scheduler_priority_SMP_Node    *the_node;

  the_node = _Scheduler_priority_SMP_Node_downcast( node );
  _Scheduler_SMP_Node_initialize(
    scheduler,
    &the_node->Base,
    the_thread,
    priority
  );

  context = _Scheduler_Get_context( scheduler );
  self = _Scheduler_priority_SMP_Get_self( context );
  _Scheduler_priority_Ready_queue_update(
    &the_node->Ready_queue,
    priority,
    &self->Bit_map,
    &self->Ready[ 0 ]
  );
}

static Scheduler_Node *_Scheduler_priority_SMP_Get_highest_ready(
  Scheduler_Context *context,
  Scheduler_Node    *node
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Get_self( context );

  (void) node;

  return (Scheduler_Node *) _Scheduler_priority_Ready_queue_first(
    &self->Bit_map,
    &self->Ready[ 0 ]
  );
}

void _Scheduler_priority_SMP_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Block(
    context,
    thread,
    node,
    _Scheduler_priority_SMP_Extract_from_ready,
    _Scheduler_priority_SMP_Get_highest_ready,
    _Scheduler_priority_SMP_Move_from_ready_to_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy
  );
}

static bool _Scheduler_priority_SMP_Enqueue_ordered(
  Scheduler_Context    *context,
  Scheduler_Node       *node,
  Chain_Node_order      order,
  Scheduler_SMP_Insert  insert_ready,
  Scheduler_SMP_Insert  insert_scheduled
)
{
  return _Scheduler_SMP_Enqueue_ordered(
    context,
    node,
    order,
    insert_ready,
    insert_scheduled,
    _Scheduler_priority_SMP_Move_from_scheduled_to_ready,
    _Scheduler_SMP_Get_lowest_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy
  );
}

static bool _Scheduler_priority_SMP_Enqueue_lifo(
  Scheduler_Context *context,
  Scheduler_Node    *node
)
{
  return _Scheduler_priority_SMP_Enqueue_ordered(
    context,
    node,
    _Scheduler_SMP_Insert_priority_lifo_order,
    _Scheduler_priority_SMP_Insert_ready_lifo,
    _Scheduler_SMP_Insert_scheduled_lifo
  );
}

static bool _Scheduler_priority_SMP_Enqueue_fifo(
  Scheduler_Context *context,
  Scheduler_Node    *node
)
{
  return _Scheduler_priority_SMP_Enqueue_ordered(
    context,
    node,
    _Scheduler_SMP_Insert_priority_fifo_order,
    _Scheduler_priority_SMP_Insert_ready_fifo,
    _Scheduler_SMP_Insert_scheduled_fifo
  );
}

static bool _Scheduler_priority_SMP_Enqueue_scheduled_ordered(
  Scheduler_Context *context,
  Scheduler_Node *node,
  Chain_Node_order order,
  Scheduler_SMP_Insert insert_ready,
  Scheduler_SMP_Insert insert_scheduled
)
{
  return _Scheduler_SMP_Enqueue_scheduled_ordered(
    context,
    node,
    order,
    _Scheduler_priority_SMP_Extract_from_ready,
    _Scheduler_priority_SMP_Get_highest_ready,
    insert_ready,
    insert_scheduled,
    _Scheduler_priority_SMP_Move_from_ready_to_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy
  );
}

static bool _Scheduler_priority_SMP_Enqueue_scheduled_lifo(
  Scheduler_Context *context,
  Scheduler_Node *node
)
{
  return _Scheduler_priority_SMP_Enqueue_scheduled_ordered(
    context,
    node,
    _Scheduler_SMP_Insert_priority_lifo_order,
    _Scheduler_priority_SMP_Insert_ready_lifo,
    _Scheduler_SMP_Insert_scheduled_lifo
  );
}

static bool _Scheduler_priority_SMP_Enqueue_scheduled_fifo(
  Scheduler_Context *context,
  Scheduler_Node *node
)
{
  return _Scheduler_priority_SMP_Enqueue_scheduled_ordered(
    context,
    node,
    _Scheduler_SMP_Insert_priority_fifo_order,
    _Scheduler_priority_SMP_Insert_ready_fifo,
    _Scheduler_SMP_Insert_scheduled_fifo
  );
}

void _Scheduler_priority_SMP_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Unblock(
    context,
    thread,
    node,
    _Scheduler_priority_SMP_Do_update,
    _Scheduler_priority_SMP_Enqueue_fifo
  );
}

static bool _Scheduler_priority_SMP_Do_ask_for_help(
  Scheduler_Context *context,
  Thread_Control    *the_thread,
  Scheduler_Node    *node
)
{
  return _Scheduler_SMP_Ask_for_help(
    context,
    the_thread,
    node,
    _Scheduler_SMP_Insert_priority_lifo_order,
    _Scheduler_priority_SMP_Insert_ready_lifo,
    _Scheduler_SMP_Insert_scheduled_lifo,
    _Scheduler_priority_SMP_Move_from_scheduled_to_ready,
    _Scheduler_SMP_Get_lowest_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy
  );
}

void _Scheduler_priority_SMP_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Update_priority(
    context,
    thread,
    node,
    _Scheduler_priority_SMP_Extract_from_ready,
    _Scheduler_priority_SMP_Do_update,
    _Scheduler_priority_SMP_Enqueue_fifo,
    _Scheduler_priority_SMP_Enqueue_lifo,
    _Scheduler_priority_SMP_Enqueue_scheduled_fifo,
    _Scheduler_priority_SMP_Enqueue_scheduled_lifo,
    _Scheduler_priority_SMP_Do_ask_for_help
  );
}

bool _Scheduler_priority_SMP_Ask_for_help(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  return _Scheduler_priority_SMP_Do_ask_for_help( context, the_thread, node );
}

void _Scheduler_priority_SMP_Reconsider_help_request(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Reconsider_help_request(
    context,
    the_thread,
    node,
    _Scheduler_priority_SMP_Extract_from_ready
  );
}

void _Scheduler_priority_SMP_Withdraw_node(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  Thread_Scheduler_state   next_state
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Withdraw_node(
    context,
    the_thread,
    node,
    next_state,
    _Scheduler_priority_SMP_Extract_from_ready,
    _Scheduler_priority_SMP_Get_highest_ready,
    _Scheduler_priority_SMP_Move_from_ready_to_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy
  );
}

void _Scheduler_priority_SMP_Add_processor(
  const Scheduler_Control *scheduler,
  Thread_Control          *idle
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Add_processor(
    context,
    idle,
    _Scheduler_priority_SMP_Has_ready,
    _Scheduler_priority_SMP_Enqueue_scheduled_fifo,
    _Scheduler_SMP_Do_nothing_register_idle
  );
}

Thread_Control *_Scheduler_priority_SMP_Remove_processor(
  const Scheduler_Control *scheduler,
  Per_CPU_Control         *cpu
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  return _Scheduler_SMP_Remove_processor(
    context,
    cpu,
    _Scheduler_priority_SMP_Extract_from_ready,
    _Scheduler_priority_SMP_Enqueue_fifo
  );
}

void _Scheduler_priority_SMP_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Yield(
    context,
    thread,
    node,
    _Scheduler_priority_SMP_Extract_from_ready,
    _Scheduler_priority_SMP_Enqueue_fifo,
    _Scheduler_priority_SMP_Enqueue_scheduled_fifo
  );
}
