/**
 * @file
 *
 * @brief Inlined Routines from the Thread Handler
 *
 * This file contains the macro implementation of the inlined
 * routines from the Thread handler.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2014, 2017 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_THREADIMPL_H
#define _RTEMS_SCORE_THREADIMPL_H

#include <rtems/score/thread.h>
#include <rtems/score/assert.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/schedulernodeimpl.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/status.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/freechain.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/config.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreThread
 */
/**@{**/

/**
 *  The following structure contains the information necessary to manage
 *  a thread which it is  waiting for a resource.
 */
#define THREAD_STATUS_PROXY_BLOCKING 0x1111111

/**
 *  Self for the GNU Ada Run-Time
 */
extern void *rtems_ada_self;

typedef struct {
  Objects_Information Objects;

  Freechain_Control Free_thread_queue_heads;
} Thread_Information;

/**
 *  The following defines the information control block used to
 *  manage this class of objects.
 */
extern Thread_Information _Thread_Internal_information;

/**
 *  The following points to the thread whose floating point
 *  context is currently loaded.
 */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
extern Thread_Control *_Thread_Allocated_fp;
#endif

#if defined(RTEMS_SMP)
#define THREAD_OF_SCHEDULER_HELP_NODE( node ) \
  RTEMS_CONTAINER_OF( node, Thread_Control, Scheduler.Help_node )
#endif

typedef bool ( *Thread_Visitor )( Thread_Control *the_thread, void *arg );

void _Thread_Iterate(
  Thread_Visitor  visitor,
  void           *arg
);

void _Thread_Initialize_information(
  Thread_Information  *information,
  Objects_APIs         the_api,
  uint16_t             the_class,
  uint32_t             maximum,
  bool                 is_string,
  uint32_t             maximum_name_length
);

/**
 *  @brief Initialize thread handler.
 *
 *  This routine performs the initialization necessary for this handler.
 */
void _Thread_Handler_initialization(void);

/**
 *  @brief Create idle thread.
 *
 *  This routine creates the idle thread.
 *
 *  @warning No thread should be created before this one.
 */
void _Thread_Create_idle(void);

/**
 *  @brief Start thread multitasking.
 *
 *  This routine initiates multitasking.  It is invoked only as
 *  part of initialization and its invocation is the last act of
 *  the non-multitasking part of the system initialization.
 */
void _Thread_Start_multitasking( void ) RTEMS_NO_RETURN;

/**
 *  @brief Allocate the requested stack space for the thread.
 *
 *  Allocate the requested stack space for the thread.
 *  Set the Start.stack field to the address of the stack.
 *
 *  @param[in] the_thread is the thread where the stack space is requested
 *  @param[in] stack_size is the stack space is requested
 *
 *  @retval actual size allocated after any adjustment
 *  @retval zero if the allocation failed
 */
size_t _Thread_Stack_Allocate(
  Thread_Control *the_thread,
  size_t          stack_size
);

/**
 *  @brief Deallocate thread stack.
 *
 *  Deallocate the Thread's stack.
 */
void _Thread_Stack_Free(
  Thread_Control *the_thread
);

/**
 *  @brief Initialize thread.
 *
 *  This routine initializes the specified the thread.  It allocates
 *  all memory associated with this thread.  It completes by adding
 *  the thread to the local object table so operations on this
 *  thread id are allowed.
 *
 *  @note If stack_area is NULL, it is allocated from the workspace.
 *
 *  @note If the stack is allocated from the workspace, then it is
 *        guaranteed to be of at least minimum size.
 */
bool _Thread_Initialize(
  Thread_Information                   *information,
  Thread_Control                       *the_thread,
  const struct _Scheduler_Control      *scheduler,
  void                                 *stack_area,
  size_t                                stack_size,
  bool                                  is_fp,
  Priority_Control                      priority,
  bool                                  is_preemptible,
  Thread_CPU_budget_algorithms          budget_algorithm,
  Thread_CPU_budget_algorithm_callout   budget_callout,
  uint32_t                              isr_level,
  Objects_Name                          name
);

/**
 *  @brief Initializes thread and executes it.
 *
 *  This routine initializes the executable information for a thread
 *  and makes it ready to execute.  After this routine executes, the
 *  thread competes with all other threads for CPU time.
 *
 *  @param the_thread The thread to be started.
 *  @param entry The thread entry information.
 */
bool _Thread_Start(
  Thread_Control                 *the_thread,
  const Thread_Entry_information *entry,
  ISR_lock_Context               *lock_context
);

void _Thread_Restart_self(
  Thread_Control                 *executing,
  const Thread_Entry_information *entry,
  ISR_lock_Context               *lock_context
) RTEMS_NO_RETURN;

bool _Thread_Restart_other(
  Thread_Control                 *the_thread,
  const Thread_Entry_information *entry,
  ISR_lock_Context               *lock_context
);

void _Thread_Yield( Thread_Control *executing );

Thread_Life_state _Thread_Change_life(
  Thread_Life_state clear,
  Thread_Life_state set,
  Thread_Life_state ignore
);

Thread_Life_state _Thread_Set_life_protection( Thread_Life_state state );

/**
 * @brief Kills all zombie threads in the system.
 *
 * Threads change into the zombie state as the last step in the thread
 * termination sequence right before a context switch to the heir thread is
 * initiated.  Since the thread stack is still in use during this phase we have
 * to postpone the thread stack reclamation until this point.  On SMP
 * configurations we may have to busy wait for context switch completion here.
 */
void _Thread_Kill_zombies( void );

void _Thread_Exit(
  Thread_Control    *executing,
  Thread_Life_state  set,
  void              *exit_value
);

void _Thread_Join(
  Thread_Control       *the_thread,
  States_Control        waiting_for_join,
  Thread_Control       *executing,
  Thread_queue_Context *queue_context
);

void _Thread_Cancel(
  Thread_Control *the_thread,
  Thread_Control *executing,
  void           *exit_value
);

typedef struct {
  Thread_queue_Context  Base;
  Thread_Control       *cancel;
} Thread_Close_context;

/**
 * @brief Closes the thread.
 *
 * Closes the thread object and starts the thread termination sequence.  In
 * case the executing thread is not terminated, then this function waits until
 * the terminating thread reached the zombie state.
 */
void _Thread_Close(
  Thread_Control       *the_thread,
  Thread_Control       *executing,
  Thread_Close_context *context
);

RTEMS_INLINE_ROUTINE bool _Thread_Is_ready( const Thread_Control *the_thread )
{
  return _States_Is_ready( the_thread->current_state );
}

States_Control _Thread_Clear_state_locked(
  Thread_Control *the_thread,
  States_Control  state
);

/**
 * @brief Clears the specified thread state.
 *
 * In case the previous state is a non-ready state and the next state is the
 * ready state, then the thread is unblocked by the scheduler.
 *
 * @param[in] the_thread The thread.
 * @param[in] state The state to clear.  It must not be zero.
 *
 * @return The previous state.
 */
States_Control _Thread_Clear_state(
  Thread_Control *the_thread,
  States_Control  state
);

States_Control _Thread_Set_state_locked(
  Thread_Control *the_thread,
  States_Control  state
);

/**
 * @brief Sets the specified thread state.
 *
 * In case the previous state is the ready state, then the thread is blocked by
 * the scheduler.
 *
 * @param[in] the_thread The thread.
 * @param[in] state The state to set.  It must not be zero.
 *
 * @return The previous state.
 */
States_Control _Thread_Set_state(
  Thread_Control *the_thread,
  States_Control  state
);

/**
 *  @brief Initializes enviroment for a thread.
 *
 *  This routine initializes the context of @a the_thread to its
 *  appropriate starting state.
 *
 *  @param[in] the_thread is the pointer to the thread control block.
 */
void _Thread_Load_environment(
  Thread_Control *the_thread
);

void _Thread_Entry_adaptor_idle( Thread_Control *executing );

void _Thread_Entry_adaptor_numeric( Thread_Control *executing );

void _Thread_Entry_adaptor_pointer( Thread_Control *executing );

/**
 *  @brief Wrapper function for all threads.
 *
 *  This routine is the wrapper function for all threads.  It is
 *  the starting point for all threads.  The user provided thread
 *  entry point is invoked by this routine.  Operations
 *  which must be performed immediately before and after the user's
 *  thread executes are found here.
 *
 *  @note On entry, it is assumed all interrupts are blocked and that this
 *  routine needs to set the initial isr level.  This may or may not
 *  actually be needed by the context switch routine and as a result
 *  interrupts may already be at there proper level.  Either way,
 *  setting the initial isr level properly here is safe.
 */
void _Thread_Handler( void );

/**
 * @brief Executes the global constructors and then restarts itself as the
 * first initialization thread.
 *
 * The first initialization thread is the first RTEMS initialization task or
 * the first POSIX initialization thread in case no RTEMS initialization tasks
 * are present.
 */
void _Thread_Global_construction(
  Thread_Control                 *executing,
  const Thread_Entry_information *entry
) RTEMS_NO_RETURN;

RTEMS_INLINE_ROUTINE void _Thread_State_acquire_critical(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _Thread_queue_Do_acquire_critical( &the_thread->Join_queue, lock_context );
}

RTEMS_INLINE_ROUTINE void _Thread_State_acquire(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_ISR_disable( lock_context );
  _Thread_State_acquire_critical( the_thread, lock_context );
}

RTEMS_INLINE_ROUTINE Thread_Control *_Thread_State_acquire_for_executing(
  ISR_lock_Context *lock_context
)
{
  Thread_Control *executing;

  _ISR_lock_ISR_disable( lock_context );
  executing = _Thread_Executing;
  _Thread_State_acquire_critical( executing, lock_context );

  return executing;
}

RTEMS_INLINE_ROUTINE void _Thread_State_release_critical(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _Thread_queue_Do_release_critical( &the_thread->Join_queue, lock_context );
}

RTEMS_INLINE_ROUTINE void _Thread_State_release(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _Thread_State_release_critical( the_thread, lock_context );
  _ISR_lock_ISR_enable( lock_context );
}

#if defined(RTEMS_DEBUG)
RTEMS_INLINE_ROUTINE bool _Thread_State_is_owner(
  const Thread_Control *the_thread
)
{
  return _Thread_queue_Is_lock_owner( &the_thread->Join_queue );
}
#endif

/**
 * @brief Performs the priority actions specified by the thread queue context
 * along the thread queue path.
 *
 * The caller must be the owner of the thread wait lock.
 *
 * @param start_of_path The start thread of the thread queue path.
 * @param queue_context The thread queue context specifying the thread queue
 *   path and initial thread priority actions.
 *
 * @see _Thread_queue_Path_acquire_critical().
 */
void _Thread_Priority_perform_actions(
  Thread_Control       *start_of_path,
  Thread_queue_Context *queue_context
);

/**
 * @brief Adds the specified thread priority node to the corresponding thread
 * priority aggregation.
 *
 * The caller must be the owner of the thread wait lock.
 *
 * @param the_thread The thread.
 * @param priority_node The thread priority node to add.
 * @param queue_context The thread queue context to return an updated set of
 *   threads for _Thread_Priority_update().  The thread queue context must be
 *   initialized via _Thread_queue_Context_clear_priority_updates() before a
 *   call of this function.
 *
 * @see _Thread_Wait_acquire().
 */
void _Thread_Priority_add(
  Thread_Control       *the_thread,
  Priority_Node        *priority_node,
  Thread_queue_Context *queue_context
);

/**
 * @brief Removes the specified thread priority node from the corresponding
 * thread priority aggregation.
 *
 * The caller must be the owner of the thread wait lock.
 *
 * @param the_thread The thread.
 * @param priority_node The thread priority node to remove.
 * @param queue_context The thread queue context to return an updated set of
 *   threads for _Thread_Priority_update().  The thread queue context must be
 *   initialized via _Thread_queue_Context_clear_priority_updates() before a
 *   call of this function.
 *
 * @see _Thread_Wait_acquire().
 */
void _Thread_Priority_remove(
  Thread_Control       *the_thread,
  Priority_Node        *priority_node,
  Thread_queue_Context *queue_context
);

/**
 * @brief Propagates a thread priority value change in the specified thread
 * priority node to the corresponding thread priority aggregation.
 *
 * The caller must be the owner of the thread wait lock.
 *
 * @param the_thread The thread.
 * @param priority_node The thread priority node to change.
 * @param prepend_it In case this is true, then the thread is prepended to
 *   its priority group in its home scheduler instance, otherwise it is
 *   appended.
 * @param queue_context The thread queue context to return an updated set of
 *   threads for _Thread_Priority_update().  The thread queue context must be
 *   initialized via _Thread_queue_Context_clear_priority_updates() before a
 *   call of this function.
 *
 * @see _Thread_Wait_acquire().
 */
void _Thread_Priority_changed(
  Thread_Control       *the_thread,
  Priority_Node        *priority_node,
  bool                  prepend_it,
  Thread_queue_Context *queue_context
);

/**
 * @brief Changes the thread priority value of the specified thread priority
 * node in the corresponding thread priority aggregation.
 *
 * The caller must be the owner of the thread wait lock.
 *
 * @param the_thread The thread.
 * @param priority_node The thread priority node to change.
 * @param new_priority The new thread priority value of the thread priority
 *   node to change.
 * @param prepend_it In case this is true, then the thread is prepended to
 *   its priority group in its home scheduler instance, otherwise it is
 *   appended.
 * @param queue_context The thread queue context to return an updated set of
 *   threads for _Thread_Priority_update().  The thread queue context must be
 *   initialized via _Thread_queue_Context_clear_priority_updates() before a
 *   call of this function.
 *
 * @see _Thread_Wait_acquire().
 */
RTEMS_INLINE_ROUTINE void _Thread_Priority_change(
  Thread_Control       *the_thread,
  Priority_Node        *priority_node,
  Priority_Control      new_priority,
  bool                  prepend_it,
  Thread_queue_Context *queue_context
)
{
  _Priority_Node_set_priority( priority_node, new_priority );
  _Thread_Priority_changed(
    the_thread,
    priority_node,
    prepend_it,
    queue_context
  );
}

/**
 * @brief Replaces the victim priority node with the replacement priority node
 * in the corresponding thread priority aggregation.
 *
 * The caller must be the owner of the thread wait lock.
 *
 * @param the_thread The thread.
 * @param victim_node The victim thread priority node.
 * @param replacement_node The replacement thread priority node.
 *
 * @see _Thread_Wait_acquire().
 */
void _Thread_Priority_replace(
  Thread_Control *the_thread,
  Priority_Node  *victim_node,
  Priority_Node  *replacement_node
);

/**
 * @brief Adds a priority node to the corresponding thread priority
 * aggregation.
 *
 * The caller must be the owner of the thread wait lock.
 *
 * @param the_thread The thread.
 * @param priority_node The thread priority node to add.
 * @param queue_context The thread queue context to return an updated set of
 *   threads for _Thread_Priority_update().  The thread queue context must be
 *   initialized via _Thread_queue_Context_clear_priority_updates() before a
 *   call of this function.
 *
 * @see _Thread_Priority_add(), _Thread_Priority_change(),
 *   _Thread_Priority_changed() and _Thread_Priority_remove().
 */
void _Thread_Priority_update( Thread_queue_Context *queue_context );

#if defined(RTEMS_SMP)
void _Thread_Priority_and_sticky_update(
  Thread_Control *the_thread,
  int             sticky_level_change
);
#endif

/**
 * @brief Returns true if the left thread priority is less than the right
 * thread priority in the intuitive sense of priority and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Priority_less_than(
  Priority_Control left,
  Priority_Control right
)
{
  return left > right;
}

/**
 * @brief Returns the highest priority of the left and right thread priorities
 * in the intuitive sense of priority.
 */
RTEMS_INLINE_ROUTINE Priority_Control _Thread_Priority_highest(
  Priority_Control left,
  Priority_Control right
)
{
  return _Thread_Priority_less_than( left, right ) ? right : left;
}

RTEMS_INLINE_ROUTINE Objects_Information *_Thread_Get_objects_information(
  Objects_Id id
)
{
  uint32_t the_api;

  the_api = _Objects_Get_API( id );

  if ( !_Objects_Is_api_valid( the_api ) ) {
    return NULL;
  }

  /*
   * Threads are always first class :)
   *
   * There is no need to validate the object class of the object identifier,
   * since this will be done by the object get methods.
   */
  return _Objects_Information_table[ the_api ][ 1 ];
}

/**
 * @brief Gets a thread by its identifier.
 *
 * @see _Objects_Get().
 */
Thread_Control *_Thread_Get(
  Objects_Id         id,
  ISR_lock_Context  *lock_context
);

RTEMS_INLINE_ROUTINE Per_CPU_Control *_Thread_Get_CPU(
  const Thread_Control *thread
)
{
#if defined(RTEMS_SMP)
  return thread->Scheduler.cpu;
#else
  (void) thread;

  return _Per_CPU_Get();
#endif
}

RTEMS_INLINE_ROUTINE void _Thread_Set_CPU(
  Thread_Control *thread,
  Per_CPU_Control *cpu
)
{
#if defined(RTEMS_SMP)
  thread->Scheduler.cpu = cpu;
#else
  (void) thread;
  (void) cpu;
#endif
}

/**
 * This function returns true if the_thread is the currently executing
 * thread, and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_executing (
  const Thread_Control *the_thread
)
{
  return ( the_thread == _Thread_Executing );
}

#if defined(RTEMS_SMP)
/**
 * @brief Returns @a true in case the thread executes currently on some
 * processor in the system, otherwise @a false.
 *
 * Do not confuse this with _Thread_Is_executing() which checks only the
 * current processor.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Is_executing_on_a_processor(
  const Thread_Control *the_thread
)
{
  return _CPU_Context_Get_is_executing( &the_thread->Registers );
}
#endif

/**
 * This function returns true if the_thread is the heir
 * thread, and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_heir (
  const Thread_Control *the_thread
)
{
  return ( the_thread == _Thread_Heir );
}

/**
 * This routine clears any blocking state for the_thread.  It performs
 * any necessary scheduling operations including the selection of
 * a new heir thread.
 */

RTEMS_INLINE_ROUTINE void _Thread_Unblock (
  Thread_Control *the_thread
)
{
  _Thread_Clear_state( the_thread, STATES_BLOCKED );
}

/**
 * This function returns true if the floating point context of
 * the_thread is currently loaded in the floating point unit, and
 * false otherwise.
 */

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
RTEMS_INLINE_ROUTINE bool _Thread_Is_allocated_fp (
  const Thread_Control *the_thread
)
{
  return ( the_thread == _Thread_Allocated_fp );
}
#endif

/*
 *  If the CPU has hardware floating point, then we must address saving
 *  and restoring it as part of the context switch.
 *
 *  The second conditional compilation section selects the algorithm used
 *  to context switch between floating point tasks.  The deferred algorithm
 *  can be significantly better in a system with few floating point tasks
 *  because it reduces the total number of save and restore FP context
 *  operations.  However, this algorithm can not be used on all CPUs due
 *  to unpredictable use of FP registers by some compilers for integer
 *  operations.
 */

RTEMS_INLINE_ROUTINE void _Thread_Save_fp( Thread_Control *executing )
{
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
#if ( CPU_USE_DEFERRED_FP_SWITCH != TRUE )
  if ( executing->fp_context != NULL )
    _Context_Save_fp( &executing->fp_context );
#endif
#endif
}

RTEMS_INLINE_ROUTINE void _Thread_Restore_fp( Thread_Control *executing )
{
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
#if ( CPU_USE_DEFERRED_FP_SWITCH == TRUE )
  if ( (executing->fp_context != NULL) &&
       !_Thread_Is_allocated_fp( executing ) ) {
    if ( _Thread_Allocated_fp != NULL )
      _Context_Save_fp( &_Thread_Allocated_fp->fp_context );
    _Context_Restore_fp( &executing->fp_context );
    _Thread_Allocated_fp = executing;
  }
#else
  if ( executing->fp_context != NULL )
    _Context_Restore_fp( &executing->fp_context );
#endif
#endif
}

/**
 * This routine is invoked when the currently loaded floating
 * point context is now longer associated with an active thread.
 */

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
RTEMS_INLINE_ROUTINE void _Thread_Deallocate_fp( void )
{
  _Thread_Allocated_fp = NULL;
}
#endif

/**
 * This function returns true if dispatching is disabled, and false
 * otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_context_switch_necessary( void )
{
  return ( _Thread_Dispatch_necessary );
}

/**
 * This function returns true if the_thread is NULL and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_null (
  const Thread_Control *the_thread
)
{
  return ( the_thread == NULL );
}

/**
 * @brief Is proxy blocking.
 *
 * status which indicates that a proxy is blocking, and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Is_proxy_blocking (
  uint32_t   code
)
{
  return (code == THREAD_STATUS_PROXY_BLOCKING);
}

RTEMS_INLINE_ROUTINE uint32_t _Thread_Get_maximum_internal_threads(void)
{
  /* Idle threads */
  uint32_t maximum_internal_threads =
    rtems_configuration_get_maximum_processors();

  /* MPCI thread */
#if defined(RTEMS_MULTIPROCESSING)
  if ( _System_state_Is_multiprocessing ) {
    ++maximum_internal_threads;
  }
#endif

  return maximum_internal_threads;
}

RTEMS_INLINE_ROUTINE Thread_Control *_Thread_Internal_allocate( void )
{
  return (Thread_Control *)
    _Objects_Allocate_unprotected( &_Thread_Internal_information.Objects );
}

/**
 * @brief Gets the heir of the processor and makes it executing.
 *
 * Must be called with interrupts disabled.  The thread dispatch necessary
 * indicator is cleared as a side-effect.
 *
 * @return The heir thread.
 *
 * @see _Thread_Dispatch(), _Thread_Start_multitasking() and
 * _Thread_Dispatch_update_heir().
 */
RTEMS_INLINE_ROUTINE Thread_Control *_Thread_Get_heir_and_make_it_executing(
  Per_CPU_Control *cpu_self
)
{
  Thread_Control *heir;

  heir = cpu_self->heir;
  cpu_self->dispatch_necessary = false;
  cpu_self->executing = heir;

  return heir;
}

RTEMS_INLINE_ROUTINE void _Thread_Update_CPU_time_used(
  Thread_Control  *the_thread,
  Per_CPU_Control *cpu
)
{
  Timestamp_Control last;
  Timestamp_Control ran;

  last = cpu->cpu_usage_timestamp;
  _TOD_Get_uptime( &cpu->cpu_usage_timestamp );
  _Timestamp_Subtract( &last, &cpu->cpu_usage_timestamp, &ran );
  _Timestamp_Add_to( &the_thread->cpu_time_used, &ran );
}

#if defined( RTEMS_SMP )
RTEMS_INLINE_ROUTINE void _Thread_Dispatch_update_heir(
  Per_CPU_Control *cpu_self,
  Per_CPU_Control *cpu_for_heir,
  Thread_Control  *heir
)
{
  _Thread_Update_CPU_time_used( cpu_for_heir->heir, cpu_for_heir );

  cpu_for_heir->heir = heir;

  _Thread_Dispatch_request( cpu_self, cpu_for_heir );
}
#endif

void _Thread_Get_CPU_time_used(
  Thread_Control    *the_thread,
  Timestamp_Control *cpu_time_used
);

RTEMS_INLINE_ROUTINE void _Thread_Action_control_initialize(
  Thread_Action_control *action_control
)
{
  _Chain_Initialize_empty( &action_control->Chain );
}

RTEMS_INLINE_ROUTINE void _Thread_Action_initialize(
  Thread_Action *action
)
{
  _Chain_Set_off_chain( &action->Node );
}

RTEMS_INLINE_ROUTINE void _Thread_Add_post_switch_action(
  Thread_Control        *the_thread,
  Thread_Action         *action,
  Thread_Action_handler  handler
)
{
  Per_CPU_Control *cpu_of_thread;

  _Assert( _Thread_State_is_owner( the_thread ) );

  cpu_of_thread = _Thread_Get_CPU( the_thread );

  action->handler = handler;

  _Thread_Dispatch_request( _Per_CPU_Get(), cpu_of_thread );

  _Chain_Append_if_is_off_chain_unprotected(
    &the_thread->Post_switch_actions.Chain,
    &action->Node
  );
}

RTEMS_INLINE_ROUTINE bool _Thread_Is_life_restarting(
  Thread_Life_state life_state
)
{
  return ( life_state & THREAD_LIFE_RESTARTING ) != 0;
}

RTEMS_INLINE_ROUTINE bool _Thread_Is_life_terminating(
  Thread_Life_state life_state
)
{
  return ( life_state & THREAD_LIFE_TERMINATING ) != 0;
}

RTEMS_INLINE_ROUTINE bool _Thread_Is_life_change_allowed(
  Thread_Life_state life_state
)
{
  return ( life_state
    & ( THREAD_LIFE_PROTECTED | THREAD_LIFE_CHANGE_DEFERRED ) ) == 0;
}

RTEMS_INLINE_ROUTINE bool _Thread_Is_life_changing(
  Thread_Life_state life_state
)
{
  return ( life_state
    & ( THREAD_LIFE_RESTARTING | THREAD_LIFE_TERMINATING ) ) != 0;
}

RTEMS_INLINE_ROUTINE bool _Thread_Is_joinable(
  const Thread_Control *the_thread
)
{
  _Assert( _Thread_State_is_owner( the_thread ) );
  return ( the_thread->Life.state & THREAD_LIFE_DETACHED ) == 0;
}

RTEMS_INLINE_ROUTINE void _Thread_Resource_count_increment(
  Thread_Control *the_thread
)
{
#if defined(RTEMS_SCORE_THREAD_ENABLE_RESOURCE_COUNT)
  ++the_thread->resource_count;
#else
  (void) the_thread;
#endif
}

RTEMS_INLINE_ROUTINE void _Thread_Resource_count_decrement(
  Thread_Control *the_thread
)
{
#if defined(RTEMS_SCORE_THREAD_ENABLE_RESOURCE_COUNT)
  --the_thread->resource_count;
#else
  (void) the_thread;
#endif
}

#if defined(RTEMS_SCORE_THREAD_ENABLE_RESOURCE_COUNT)
/**
 * @brief Returns true if the thread owns resources, and false otherwise.
 *
 * Resources are accounted with the Thread_Control::resource_count resource
 * counter.  This counter is used by mutex objects for example.
 *
 * @param[in] the_thread The thread.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Owns_resources(
  const Thread_Control *the_thread
)
{
  return the_thread->resource_count != 0;
}
#endif

#if defined(RTEMS_SMP)
RTEMS_INLINE_ROUTINE void _Thread_Scheduler_cancel_need_for_help(
  Thread_Control  *the_thread,
  Per_CPU_Control *cpu
)
{
  _Per_CPU_Acquire( cpu );

  if ( !_Chain_Is_node_off_chain( &the_thread->Scheduler.Help_node ) ) {
    _Chain_Extract_unprotected( &the_thread->Scheduler.Help_node );
    _Chain_Set_off_chain( &the_thread->Scheduler.Help_node );
  }

  _Per_CPU_Release( cpu );
}
#endif

RTEMS_INLINE_ROUTINE const Scheduler_Control *_Thread_Scheduler_get_home(
  const Thread_Control *the_thread
)
{
#if defined(RTEMS_SMP)
  return the_thread->Scheduler.home;
#else
  (void) the_thread;
  return &_Scheduler_Table[ 0 ];
#endif
}

RTEMS_INLINE_ROUTINE Scheduler_Node *_Thread_Scheduler_get_home_node(
  const Thread_Control *the_thread
)
{
#if defined(RTEMS_SMP)
  _Assert( !_Chain_Is_empty( &the_thread->Scheduler.Wait_nodes ) );
  return SCHEDULER_NODE_OF_THREAD_WAIT_NODE(
    _Chain_First( &the_thread->Scheduler.Wait_nodes )
  );
#else
  return the_thread->Scheduler.nodes;
#endif
}

RTEMS_INLINE_ROUTINE Scheduler_Node *_Thread_Scheduler_get_node_by_index(
  const Thread_Control *the_thread,
  size_t                scheduler_index
)
{
#if defined(RTEMS_SMP)
  return (Scheduler_Node *)
    ( (uintptr_t) the_thread->Scheduler.nodes
      + scheduler_index * _Scheduler_Node_size );
#else
  _Assert( scheduler_index == 0 );
  (void) scheduler_index;
  return the_thread->Scheduler.nodes;
#endif
}

#if defined(RTEMS_SMP)
RTEMS_INLINE_ROUTINE void _Thread_Scheduler_acquire_critical(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_Acquire( &the_thread->Scheduler.Lock, lock_context );
}

RTEMS_INLINE_ROUTINE void _Thread_Scheduler_release_critical(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_Release( &the_thread->Scheduler.Lock, lock_context );
}

#if defined(RTEMS_SMP)
void _Thread_Scheduler_process_requests( Thread_Control *the_thread );
#endif

RTEMS_INLINE_ROUTINE void _Thread_Scheduler_add_request(
  Thread_Control         *the_thread,
  Scheduler_Node         *scheduler_node,
  Scheduler_Node_request  request
)
{
  ISR_lock_Context       lock_context;
  Scheduler_Node_request current_request;

  _Thread_Scheduler_acquire_critical( the_thread, &lock_context );

  current_request = scheduler_node->Thread.request;

  if ( current_request == SCHEDULER_NODE_REQUEST_NOT_PENDING ) {
    _Assert(
      request == SCHEDULER_NODE_REQUEST_ADD
        || request == SCHEDULER_NODE_REQUEST_REMOVE
    );
    _Assert( scheduler_node->Thread.next_request == NULL );
    scheduler_node->Thread.next_request = the_thread->Scheduler.requests;
    the_thread->Scheduler.requests = scheduler_node;
  } else if ( current_request != SCHEDULER_NODE_REQUEST_NOTHING ) {
    _Assert(
      ( current_request == SCHEDULER_NODE_REQUEST_ADD
        && request == SCHEDULER_NODE_REQUEST_REMOVE )
      || ( current_request == SCHEDULER_NODE_REQUEST_REMOVE
        && request == SCHEDULER_NODE_REQUEST_ADD )
    );
    request = SCHEDULER_NODE_REQUEST_NOTHING;
  }

  scheduler_node->Thread.request = request;

  _Thread_Scheduler_release_critical( the_thread, &lock_context );
}

RTEMS_INLINE_ROUTINE void _Thread_Scheduler_add_wait_node(
  Thread_Control *the_thread,
  Scheduler_Node *scheduler_node
)
{
  _Chain_Append_unprotected(
    &the_thread->Scheduler.Wait_nodes,
    &scheduler_node->Thread.Wait_node
  );
  _Thread_Scheduler_add_request(
    the_thread,
    scheduler_node,
    SCHEDULER_NODE_REQUEST_ADD
  );
}

RTEMS_INLINE_ROUTINE void _Thread_Scheduler_remove_wait_node(
  Thread_Control *the_thread,
  Scheduler_Node *scheduler_node
)
{
  _Chain_Extract_unprotected( &scheduler_node->Thread.Wait_node );
  _Thread_Scheduler_add_request(
    the_thread,
    scheduler_node,
    SCHEDULER_NODE_REQUEST_REMOVE
  );
}
#endif

/**
 * @brief Returns the priority of the thread.
 *
 * Returns the user API and thread wait information relevant thread priority.
 * This includes temporary thread priority adjustments due to locking
 * protocols, a job release or the POSIX sporadic server for example.
 *
 * @return The priority of the thread.
 */
RTEMS_INLINE_ROUTINE Priority_Control _Thread_Get_priority(
  const Thread_Control *the_thread
)
{
  Scheduler_Node *scheduler_node;

  scheduler_node = _Thread_Scheduler_get_home_node( the_thread );
  return _Priority_Get_priority( &scheduler_node->Wait.Priority );
}

/**
 * @brief Acquires the thread wait default lock inside a critical section
 * (interrupts disabled).
 *
 * @param[in] the_thread The thread.
 * @param[in] lock_context The lock context used for the corresponding lock
 *   release.
 *
 * @see _Thread_Wait_release_default_critical().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_acquire_default_critical(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_Acquire( &the_thread->Wait.Lock.Default, lock_context );
}

/**
 * @brief Acquires the thread wait default lock and returns the executing
 * thread.
 *
 * @param[in] lock_context The lock context used for the corresponding lock
 *   release.
 *
 * @return The executing thread.
 *
 * @see _Thread_Wait_release_default().
 */
RTEMS_INLINE_ROUTINE Thread_Control *_Thread_Wait_acquire_default_for_executing(
  ISR_lock_Context *lock_context
)
{
  Thread_Control *executing;

  _ISR_lock_ISR_disable( lock_context );
  executing = _Thread_Executing;
  _Thread_Wait_acquire_default_critical( executing, lock_context );

  return executing;
}

/**
 * @brief Acquires the thread wait default lock and disables interrupts.
 *
 * @param[in] the_thread The thread.
 * @param[in] lock_context The lock context used for the corresponding lock
 *   release.
 *
 * @see _Thread_Wait_release_default().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_acquire_default(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_ISR_disable( lock_context );
  _Thread_Wait_acquire_default_critical( the_thread, lock_context );
}

/**
 * @brief Releases the thread wait default lock inside a critical section
 * (interrupts disabled).
 *
 * The previous interrupt status is not restored.
 *
 * @param[in] the_thread The thread.
 * @param[in] lock_context The lock context used for the corresponding lock
 *   acquire.
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_release_default_critical(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_Release( &the_thread->Wait.Lock.Default, lock_context );
}

/**
 * @brief Releases the thread wait default lock and restores the previous
 * interrupt status.
 *
 * @param[in] the_thread The thread.
 * @param[in] lock_context The lock context used for the corresponding lock
 *   acquire.
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_release_default(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _Thread_Wait_release_default_critical( the_thread, lock_context );
  _ISR_lock_ISR_enable( lock_context );
}

#if defined(RTEMS_SMP)
#define THREAD_QUEUE_CONTEXT_OF_REQUEST( node ) \
  RTEMS_CONTAINER_OF( node, Thread_queue_Context, Lock_context.Wait.Gate.Node )

RTEMS_INLINE_ROUTINE void _Thread_Wait_remove_request_locked(
  Thread_Control            *the_thread,
  Thread_queue_Lock_context *queue_lock_context
)
{
  Chain_Node *first;

  _Chain_Extract_unprotected( &queue_lock_context->Wait.Gate.Node );
  first = _Chain_First( &the_thread->Wait.Lock.Pending_requests );

  if ( first != _Chain_Tail( &the_thread->Wait.Lock.Pending_requests ) ) {
    _Thread_queue_Gate_open( (Thread_queue_Gate *) first );
  }
}

RTEMS_INLINE_ROUTINE void _Thread_Wait_acquire_queue_critical(
  Thread_queue_Queue        *queue,
  Thread_queue_Lock_context *queue_lock_context
)
{
  _Thread_queue_Queue_acquire_critical(
    queue,
    &_Thread_Executing->Potpourri_stats,
    &queue_lock_context->Lock_context
  );
}

RTEMS_INLINE_ROUTINE void _Thread_Wait_release_queue_critical(
  Thread_queue_Queue        *queue,
  Thread_queue_Lock_context *queue_lock_context
)
{
  _Thread_queue_Queue_release_critical(
    queue,
    &queue_lock_context->Lock_context
  );
}
#endif

/**
 * @brief Acquires the thread wait lock inside a critical section (interrupts
 * disabled).
 *
 * @param[in] the_thread The thread.
 * @param[in] queue_context The thread queue context for the corresponding
 *   _Thread_Wait_release_critical().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_acquire_critical(
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
#if defined(RTEMS_SMP)
  Thread_queue_Queue *queue;

  _Thread_Wait_acquire_default_critical(
    the_thread,
    &queue_context->Lock_context.Lock_context
  );

  queue = the_thread->Wait.queue;
  queue_context->Lock_context.Wait.queue = queue;

  if ( queue != NULL ) {
    _Thread_queue_Gate_add(
      &the_thread->Wait.Lock.Pending_requests,
      &queue_context->Lock_context.Wait.Gate
    );
    _Thread_Wait_release_default_critical(
      the_thread,
      &queue_context->Lock_context.Lock_context
    );
    _Thread_Wait_acquire_queue_critical( queue, &queue_context->Lock_context );

    if ( queue_context->Lock_context.Wait.queue == NULL ) {
      _Thread_Wait_release_queue_critical(
        queue,
        &queue_context->Lock_context
      );
      _Thread_Wait_acquire_default_critical(
        the_thread,
        &queue_context->Lock_context.Lock_context
      );
      _Thread_Wait_remove_request_locked(
        the_thread,
        &queue_context->Lock_context
      );
      _Assert( the_thread->Wait.queue == NULL );
    }
  }
#else
  (void) the_thread;
  (void) queue_context;
#endif
}

/**
 * @brief Acquires the thread wait default lock and disables interrupts.
 *
 * @param[in] the_thread The thread.
 * @param[in] queue_context The thread queue context for the corresponding
 *   _Thread_Wait_release().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_acquire(
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
  _ISR_lock_ISR_disable( &queue_context->Lock_context.Lock_context );
  _Thread_Wait_acquire_critical( the_thread, queue_context );
}

/**
 * @brief Releases the thread wait lock inside a critical section (interrupts
 * disabled).
 *
 * The previous interrupt status is not restored.
 *
 * @param[in] the_thread The thread.
 * @param[in] queue_context The thread queue context used for corresponding
 *   _Thread_Wait_acquire_critical().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_release_critical(
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
#if defined(RTEMS_SMP)
  Thread_queue_Queue *queue;

  queue = queue_context->Lock_context.Wait.queue;

  if ( queue != NULL ) {
    _Thread_Wait_release_queue_critical(
      queue, &queue_context->Lock_context
    );
    _Thread_Wait_acquire_default_critical(
      the_thread,
      &queue_context->Lock_context.Lock_context
    );
    _Thread_Wait_remove_request_locked(
      the_thread,
      &queue_context->Lock_context
    );
  }

  _Thread_Wait_release_default_critical(
    the_thread,
    &queue_context->Lock_context.Lock_context
  );
#else
  (void) the_thread;
  (void) queue_context;
#endif
}

/**
 * @brief Releases the thread wait lock and restores the previous interrupt
 * status.
 *
 * @param[in] the_thread The thread.
 * @param[in] queue_context The thread queue context used for corresponding
 *   _Thread_Wait_acquire().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_release(
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
  _Thread_Wait_release_critical( the_thread, queue_context );
  _ISR_lock_ISR_enable( &queue_context->Lock_context.Lock_context );
}

/**
 * @brief Claims the thread wait queue.
 *
 * The caller must not be the owner of the default thread wait lock.  The
 * caller must be the owner of the corresponding thread queue lock.  The
 * registration of the corresponding thread queue operations is deferred and
 * done after the deadlock detection.  This is crucial to support timeouts on
 * SMP configurations.
 *
 * @param[in] the_thread The thread.
 * @param[in] queue The new thread queue.
 *
 * @see _Thread_Wait_claim_finalize() and _Thread_Wait_restore_default().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_claim(
  Thread_Control     *the_thread,
  Thread_queue_Queue *queue
)
{
  ISR_lock_Context lock_context;

  _Thread_Wait_acquire_default_critical( the_thread, &lock_context );

  _Assert( the_thread->Wait.queue == NULL );

#if defined(RTEMS_SMP)
  _Chain_Initialize_empty( &the_thread->Wait.Lock.Pending_requests );
  _Chain_Initialize_node( &the_thread->Wait.Lock.Tranquilizer.Node );
  _Thread_queue_Gate_close( &the_thread->Wait.Lock.Tranquilizer );
#endif

  the_thread->Wait.queue = queue;

  _Thread_Wait_release_default_critical( the_thread, &lock_context );
}

/**
 * @brief Finalizes the thread wait queue claim via registration of the
 * corresponding thread queue operations.
 *
 * @param[in] the_thread The thread.
 * @param[in] operations The corresponding thread queue operations.
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_claim_finalize(
  Thread_Control                *the_thread,
  const Thread_queue_Operations *operations
)
{
  the_thread->Wait.operations = operations;
}

/**
 * @brief Removes a thread wait lock request.
 *
 * On SMP configurations, removes a thread wait lock request.
 *
 * On other configurations, this function does nothing.
 *
 * @param[in] the_thread The thread.
 * @param[in] queue_lock_context The thread queue lock context used for
 *   corresponding _Thread_Wait_acquire().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_remove_request(
  Thread_Control            *the_thread,
  Thread_queue_Lock_context *queue_lock_context
)
{
#if defined(RTEMS_SMP)
  ISR_lock_Context lock_context;

  _Thread_Wait_acquire_default( the_thread, &lock_context );
  _Thread_Wait_remove_request_locked( the_thread, queue_lock_context );
  _Thread_Wait_release_default( the_thread, &lock_context );
#else
  (void) the_thread;
  (void) queue_lock_context;
#endif
}

/**
 * @brief Restores the default thread wait queue and operations.
 *
 * The caller must be the owner of the current thread wait queue lock.
 *
 * On SMP configurations, the pending requests are updated to use the stale
 * thread queue operations.
 *
 * @param[in] the_thread The thread.
 *
 * @see _Thread_Wait_claim().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_restore_default(
  Thread_Control *the_thread
)
{
#if defined(RTEMS_SMP)
  ISR_lock_Context  lock_context;
  Chain_Node       *node;
  const Chain_Node *tail;

  _Thread_Wait_acquire_default_critical( the_thread, &lock_context );

  node = _Chain_First( &the_thread->Wait.Lock.Pending_requests );
  tail = _Chain_Immutable_tail( &the_thread->Wait.Lock.Pending_requests );

  if ( node != tail ) {
    do {
      Thread_queue_Context *queue_context;

      queue_context = THREAD_QUEUE_CONTEXT_OF_REQUEST( node );
      queue_context->Lock_context.Wait.queue = NULL;

      node = _Chain_Next( node );
    } while ( node != tail );

    _Thread_queue_Gate_add(
      &the_thread->Wait.Lock.Pending_requests,
      &the_thread->Wait.Lock.Tranquilizer
    );
  } else {
    _Thread_queue_Gate_open( &the_thread->Wait.Lock.Tranquilizer );
  }
#endif

  the_thread->Wait.queue = NULL;
  the_thread->Wait.operations = &_Thread_queue_Operations_default;

#if defined(RTEMS_SMP)
  _Thread_Wait_release_default_critical( the_thread, &lock_context );
#endif
}

/**
 * @brief Tranquilizes the thread after a wait on a thread queue.
 *
 * After the violent blocking procedure this function makes the thread calm and
 * peaceful again so that it can carry out its normal work.
 *
 * On SMP configurations, ensures that all pending thread wait lock requests
 * completed before the thread is able to begin a new thread wait procedure.
 *
 * On other configurations, this function does nothing.
 *
 * It must be called after a _Thread_Wait_claim() exactly once
 *  - after the corresponding thread queue lock was released, and
 *  - the default wait state is restored or some other processor is about to do
 *    this.
 *
 * @param[in] the_thread The thread.
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_tranquilize(
  Thread_Control *the_thread
)
{
#if defined(RTEMS_SMP)
  _Thread_queue_Gate_wait( &the_thread->Wait.Lock.Tranquilizer );
#else
  (void) the_thread;
#endif
}

/**
 * @brief Cancels a thread wait on a thread queue.
 *
 * @param[in] the_thread The thread.
 * @param[in] queue_context The thread queue context used for corresponding
 *   _Thread_Wait_acquire().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_cancel(
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
  Thread_queue_Queue *queue;

  queue = the_thread->Wait.queue;

#if defined(RTEMS_SMP)
  if ( queue != NULL ) {
    _Assert( queue_context->Lock_context.Wait.queue == queue );
#endif

    ( *the_thread->Wait.operations->extract )(
      queue,
      the_thread,
      queue_context
    );
    _Thread_Wait_restore_default( the_thread );

#if defined(RTEMS_SMP)
    _Assert( queue_context->Lock_context.Wait.queue == NULL );
    queue_context->Lock_context.Wait.queue = queue;
  }
#endif
}

/**
 * @brief The initial thread wait flags value set by _Thread_Initialize().
 */
#define THREAD_WAIT_FLAGS_INITIAL 0x0U

/**
 * @brief Mask to get the thread wait state flags.
 */
#define THREAD_WAIT_STATE_MASK 0xffU

/**
 * @brief Indicates that the thread begins with the blocking operation.
 *
 * A blocking operation consists of an optional watchdog initialization and the
 * setting of the appropriate thread blocking state with the corresponding
 * scheduler block operation.
 */
#define THREAD_WAIT_STATE_INTEND_TO_BLOCK 0x1U

/**
 * @brief Indicates that the thread completed the blocking operation.
 */
#define THREAD_WAIT_STATE_BLOCKED 0x2U

/**
 * @brief Indicates that a condition to end the thread wait occurred.
 *
 * This could be a timeout, a signal, an event or a resource availability.
 */
#define THREAD_WAIT_STATE_READY_AGAIN 0x4U

/**
 * @brief Mask to get the thread wait class flags.
 */
#define THREAD_WAIT_CLASS_MASK 0xff00U

/**
 * @brief Indicates that the thread waits for an event.
 */
#define THREAD_WAIT_CLASS_EVENT 0x100U

/**
 * @brief Indicates that the thread waits for a system event.
 */
#define THREAD_WAIT_CLASS_SYSTEM_EVENT 0x200U

/**
 * @brief Indicates that the thread waits for an object.
 */
#define THREAD_WAIT_CLASS_OBJECT 0x400U

/**
 * @brief Indicates that the thread waits for a period.
 */
#define THREAD_WAIT_CLASS_PERIOD 0x800U

RTEMS_INLINE_ROUTINE void _Thread_Wait_flags_set(
  Thread_Control    *the_thread,
  Thread_Wait_flags  flags
)
{
#if defined(RTEMS_SMP)
  _Atomic_Store_uint( &the_thread->Wait.flags, flags, ATOMIC_ORDER_RELAXED );
#else
  the_thread->Wait.flags = flags;
#endif
}

RTEMS_INLINE_ROUTINE Thread_Wait_flags _Thread_Wait_flags_get(
  const Thread_Control *the_thread
)
{
#if defined(RTEMS_SMP)
  return _Atomic_Load_uint( &the_thread->Wait.flags, ATOMIC_ORDER_RELAXED );
#else
  return the_thread->Wait.flags;
#endif
}

RTEMS_INLINE_ROUTINE Thread_Wait_flags _Thread_Wait_flags_get_acquire(
  const Thread_Control *the_thread
)
{
#if defined(RTEMS_SMP)
  return _Atomic_Load_uint( &the_thread->Wait.flags, ATOMIC_ORDER_ACQUIRE );
#else
  return the_thread->Wait.flags;
#endif
}

/**
 * @brief Tries to change the thread wait flags with release semantics in case
 * of success.
 *
 * Must be called inside a critical section (interrupts disabled).
 *
 * In case the wait flags are equal to the expected wait flags, then the wait
 * flags are set to the desired wait flags.
 *
 * @param[in] the_thread The thread.
 * @param[in] expected_flags The expected wait flags.
 * @param[in] desired_flags The desired wait flags.
 *
 * @retval true The wait flags were equal to the expected wait flags.
 * @retval false Otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Wait_flags_try_change_release(
  Thread_Control    *the_thread,
  Thread_Wait_flags  expected_flags,
  Thread_Wait_flags  desired_flags
)
{
  _Assert( _ISR_Get_level() != 0 );

#if defined(RTEMS_SMP)
  return _Atomic_Compare_exchange_uint(
    &the_thread->Wait.flags,
    &expected_flags,
    desired_flags,
    ATOMIC_ORDER_RELEASE,
    ATOMIC_ORDER_RELAXED
  );
#else
  bool success = ( the_thread->Wait.flags == expected_flags );

  if ( success ) {
    the_thread->Wait.flags = desired_flags;
  }

  return success;
#endif
}

/**
 * @brief Tries to change the thread wait flags with acquire semantics.
 *
 * In case the wait flags are equal to the expected wait flags, then the wait
 * flags are set to the desired wait flags.
 *
 * @param[in] the_thread The thread.
 * @param[in] expected_flags The expected wait flags.
 * @param[in] desired_flags The desired wait flags.
 *
 * @retval true The wait flags were equal to the expected wait flags.
 * @retval false Otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Wait_flags_try_change_acquire(
  Thread_Control    *the_thread,
  Thread_Wait_flags  expected_flags,
  Thread_Wait_flags  desired_flags
)
{
  bool success;
#if defined(RTEMS_SMP)
  return _Atomic_Compare_exchange_uint(
    &the_thread->Wait.flags,
    &expected_flags,
    desired_flags,
    ATOMIC_ORDER_ACQUIRE,
    ATOMIC_ORDER_ACQUIRE
  );
#else
  ISR_Level level;

  _ISR_Local_disable( level );

  success = _Thread_Wait_flags_try_change_release(
    the_thread,
    expected_flags,
    desired_flags
  );

  _ISR_Local_enable( level );
#endif

  return success;
}

/**
 * @brief Returns the object identifier of the object containing the current
 * thread wait queue.
 *
 * This function may be used for debug and system information purposes.  The
 * caller must be the owner of the thread lock.
 *
 * @retval 0 The thread waits on no thread queue currently, the thread wait
 *   queue is not contained in an object, or the current thread state provides
 *   insufficient information, e.g. the thread is in the middle of a blocking
 *   operation.
 * @retval other The object identifier of the object containing the thread wait
 *   queue.
 */
Objects_Id _Thread_Wait_get_id( const Thread_Control *the_thread );

RTEMS_INLINE_ROUTINE Status_Control _Thread_Wait_get_status(
  const Thread_Control *the_thread
)
{
  return (Status_Control) the_thread->Wait.return_code;
}

/**
 * @brief Cancels a blocking operation so that the thread can continue its
 * execution.
 *
 * In case this function actually cancelled the blocking operation, then the
 * thread wait return code is set to the specified status.
 *
 * A specialization of this function is _Thread_Timeout().
 *
 * @param[in] the_thread The thread.
 * @param[in] status The thread wait status.
 */
void _Thread_Continue( Thread_Control *the_thread, Status_Control status );

/**
 * @brief General purpose thread wait timeout.
 *
 * @param[in] the_watchdog The thread timer watchdog.
 */
void _Thread_Timeout( Watchdog_Control *the_watchdog );

RTEMS_INLINE_ROUTINE void _Thread_Timer_initialize(
  Thread_Timer_information *timer,
  Per_CPU_Control          *cpu
)
{
  _ISR_lock_Initialize( &timer->Lock, "Thread Timer" );
  timer->header = &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_MONOTONIC ];
  _Watchdog_Preinitialize( &timer->Watchdog, cpu );
}

RTEMS_INLINE_ROUTINE void _Thread_Add_timeout_ticks(
  Thread_Control    *the_thread,
  Per_CPU_Control   *cpu,
  Watchdog_Interval  ticks
)
{
  ISR_lock_Context lock_context;

  _ISR_lock_ISR_disable_and_acquire( &the_thread->Timer.Lock, &lock_context );

  the_thread->Timer.header =
    &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_MONOTONIC ];
  the_thread->Timer.Watchdog.routine = _Thread_Timeout;
  _Watchdog_Per_CPU_insert_ticks( &the_thread->Timer.Watchdog, cpu, ticks );

  _ISR_lock_Release_and_ISR_enable( &the_thread->Timer.Lock, &lock_context );
}

RTEMS_INLINE_ROUTINE void _Thread_Timer_insert_realtime(
  Thread_Control                 *the_thread,
  Per_CPU_Control                *cpu,
  Watchdog_Service_routine_entry  routine,
  uint64_t                        expire
)
{
  ISR_lock_Context lock_context;

  _ISR_lock_ISR_disable_and_acquire( &the_thread->Timer.Lock, &lock_context );

  the_thread->Timer.header =
    &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_REALTIME ];
  the_thread->Timer.Watchdog.routine = routine;
  _Watchdog_Per_CPU_insert_realtime( &the_thread->Timer.Watchdog, cpu, expire );

  _ISR_lock_Release_and_ISR_enable( &the_thread->Timer.Lock, &lock_context );
}

RTEMS_INLINE_ROUTINE void _Thread_Timer_remove( Thread_Control *the_thread )
{
  ISR_lock_Context lock_context;

  _ISR_lock_ISR_disable_and_acquire( &the_thread->Timer.Lock, &lock_context );

  _Watchdog_Per_CPU_remove(
    &the_thread->Timer.Watchdog,
#if defined(RTEMS_SMP)
    the_thread->Timer.Watchdog.cpu,
#else
    _Per_CPU_Get(),
#endif
    the_thread->Timer.header
  );

  _ISR_lock_Release_and_ISR_enable( &the_thread->Timer.Lock, &lock_context );
}

RTEMS_INLINE_ROUTINE void _Thread_Remove_timer_and_unblock(
  Thread_Control     *the_thread,
  Thread_queue_Queue *queue
)
{
  _Thread_Wait_tranquilize( the_thread );
  _Thread_Timer_remove( the_thread );

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Objects_Is_local_id( the_thread->Object.id ) ) {
    _Thread_Unblock( the_thread );
  } else {
    _Thread_queue_Unblock_proxy( queue, the_thread );
  }
#else
  (void) queue;
  _Thread_Unblock( the_thread );
#endif
}

Status_Control _Thread_Set_name(
  Thread_Control *the_thread,
  const char     *name
);

size_t _Thread_Get_name(
  const Thread_Control *the_thread,
  char                 *buffer,
  size_t                buffer_size
);

/** @}*/

#ifdef __cplusplus
}
#endif

#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/threadmp.h>
#endif

#endif
/* end of include file */
