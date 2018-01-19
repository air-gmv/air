/**
 * @file
 * 
 * @brief Dispatch Thread
 * @ingroup ScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2014, 2016 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threaddispatch.h>
#include <rtems/score/assert.h>
#include <rtems/score/isr.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/userextimpl.h>
#include <rtems/score/wkspace.h>
#include <rtems/config.h>

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
Thread_Control *_Thread_Allocated_fp;
#endif

CHAIN_DEFINE_EMPTY( _User_extensions_Switches_list );

#if defined(RTEMS_SMP)
static void _Thread_Ask_for_help( Thread_Control *the_thread )
{
  Chain_Node       *node;
  const Chain_Node *tail;

  node = _Chain_First( &the_thread->Scheduler.Scheduler_nodes );
  tail = _Chain_Immutable_tail( &the_thread->Scheduler.Scheduler_nodes );

  do {
    Scheduler_Node          *scheduler_node;
    const Scheduler_Control *scheduler;
    ISR_lock_Context         lock_context;
    bool                     success;

    scheduler_node = SCHEDULER_NODE_OF_THREAD_SCHEDULER_NODE( node );
    scheduler = _Scheduler_Node_get_scheduler( scheduler_node );

    _Scheduler_Acquire_critical( scheduler, &lock_context );
    success = ( *scheduler->Operations.ask_for_help )(
      scheduler,
      the_thread,
      scheduler_node
    );
    _Scheduler_Release_critical( scheduler, &lock_context );

    if ( success ) {
      break;
    }

    node = _Chain_Next( node );
  } while ( node != tail );
}

static bool _Thread_Can_ask_for_help( const Thread_Control *executing )
{
  return executing->Scheduler.helping_nodes > 0
    && _Thread_Is_ready( executing );
}
#endif

static void _Thread_Preemption_intervention( Per_CPU_Control *cpu_self )
{
#if defined(RTEMS_SMP)
  _Per_CPU_Acquire( cpu_self );

  while ( !_Chain_Is_empty( &cpu_self->Threads_in_need_for_help ) ) {
    Chain_Node       *node;
    Thread_Control   *the_thread;
    ISR_lock_Context  lock_context;

    node = _Chain_Get_first_unprotected( &cpu_self->Threads_in_need_for_help );
    _Chain_Set_off_chain( node );
    the_thread = THREAD_OF_SCHEDULER_HELP_NODE( node );

    _Per_CPU_Release( cpu_self );
    _Thread_State_acquire( the_thread, &lock_context );
    _Thread_Ask_for_help( the_thread );
    _Thread_State_release( the_thread, &lock_context );
    _Per_CPU_Acquire( cpu_self );
  }

  _Per_CPU_Release( cpu_self );
#else
  (void) cpu_self;
#endif
}

static void _Thread_Post_switch_cleanup( Thread_Control *executing )
{
#if defined(RTEMS_SMP)
  Chain_Node       *node;
  const Chain_Node *tail;

  if ( !_Thread_Can_ask_for_help( executing ) ) {
    return;
  }

  node = _Chain_First( &executing->Scheduler.Scheduler_nodes );
  tail = _Chain_Immutable_tail( &executing->Scheduler.Scheduler_nodes );

  do {
    Scheduler_Node          *scheduler_node;
    const Scheduler_Control *scheduler;
    ISR_lock_Context         lock_context;

    scheduler_node = SCHEDULER_NODE_OF_THREAD_SCHEDULER_NODE( node );
    scheduler = _Scheduler_Node_get_scheduler( scheduler_node );

    _Scheduler_Acquire_critical( scheduler, &lock_context );
    ( *scheduler->Operations.reconsider_help_request )(
      scheduler,
      executing,
      scheduler_node
    );
    _Scheduler_Release_critical( scheduler, &lock_context );

    node = _Chain_Next( node );
  } while ( node != tail );
#else
  (void) executing;
#endif
}

static Thread_Action *_Thread_Get_post_switch_action(
  Thread_Control *executing
)
{
  Chain_Control *chain = &executing->Post_switch_actions.Chain;

  return (Thread_Action *) _Chain_Get_unprotected( chain );
}

static void _Thread_Run_post_switch_actions( Thread_Control *executing )
{
  ISR_lock_Context  lock_context;
  Thread_Action    *action;

  _Thread_State_acquire( executing, &lock_context );
  _Thread_Post_switch_cleanup( executing );
  action = _Thread_Get_post_switch_action( executing );

  while ( action != NULL ) {
    _Chain_Set_off_chain( &action->Node );

    ( *action->handler )( executing, action, &lock_context );

    _Thread_State_acquire( executing, &lock_context );
    action = _Thread_Get_post_switch_action( executing );
  }

  _Thread_State_release( executing, &lock_context );
}

void _Thread_Do_dispatch( Per_CPU_Control *cpu_self, ISR_Level level )
{
  Thread_Control *executing;

  _Assert( cpu_self->thread_dispatch_disable_level == 1 );

#if defined(RTEMS_SCORE_ROBUST_THREAD_DISPATCH)
  if (
    !_ISR_Is_enabled( level )
#if defined(RTEMS_SMP)
      && rtems_configuration_is_smp_enabled()
#endif
  ) {
    _Internal_error( INTERNAL_ERROR_BAD_THREAD_DISPATCH_ENVIRONMENT );
  }
#endif

  executing = cpu_self->executing;

  do {
    Thread_Control *heir;

    _Thread_Preemption_intervention( cpu_self );
    heir = _Thread_Get_heir_and_make_it_executing( cpu_self );

    /*
     *  When the heir and executing are the same, then we are being
     *  requested to do the post switch dispatching.  This is normally
     *  done to dispatch signals.
     */
    if ( heir == executing )
      goto post_switch;

    /*
     *  Since heir and executing are not the same, we need to do a real
     *  context switch.
     */
    if ( heir->budget_algorithm == THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE )
      heir->cpu_time_budget = rtems_configuration_get_ticks_per_timeslice();

    _ISR_Local_enable( level );

    _User_extensions_Thread_switch( executing, heir );
    _Thread_Save_fp( executing );
    _Context_Switch( &executing->Registers, &heir->Registers );
    _Thread_Restore_fp( executing );

    /*
     * We have to obtain this value again after the context switch since the
     * heir thread may have migrated from another processor.  Values from the
     * stack or non-volatile registers reflect the old execution environment.
     */
    cpu_self = _Per_CPU_Get();

    _ISR_Local_disable( level );
  } while ( cpu_self->dispatch_necessary );

post_switch:
  _Assert( cpu_self->thread_dispatch_disable_level == 1 );
  cpu_self->thread_dispatch_disable_level = 0;
  _Profiling_Thread_dispatch_enable( cpu_self, 0 );

  _ISR_Local_enable( level );

  _Thread_Run_post_switch_actions( executing );
}

void _Thread_Dispatch( void )
{
  ISR_Level        level;
  Per_CPU_Control *cpu_self;

  _ISR_Local_disable( level );

  cpu_self = _Per_CPU_Get();

  if ( cpu_self->dispatch_necessary ) {
    _Profiling_Thread_dispatch_disable( cpu_self, 0 );
    _Assert( cpu_self->thread_dispatch_disable_level == 0 );
    cpu_self->thread_dispatch_disable_level = 1;
    _Thread_Do_dispatch( cpu_self, level );
  } else {
    _ISR_Local_enable( level );
  }
}

void _Thread_Dispatch_direct( Per_CPU_Control *cpu_self )
{
  ISR_Level level;

  if ( cpu_self->thread_dispatch_disable_level != 1 ) {
    _Internal_error( INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL );
  }

  _ISR_Local_disable( level );
  _Thread_Do_dispatch( cpu_self, level );
}
