/**
 *  @file
 *  thread.h
 *
 *  @brief contains all constants and structures associated
 *  with the thread control block.
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
 *  234     | 09/10/2008  | mcoutinho    | IPR 67
 *  604     | 17/11/2008  | mcoutinho    | IPR 64
 *  604     | 17/11/2008  | mcoutinho    | IPR 69
 *  604     | 17/11/2008  | mcoutinho    | IPR 70
 *  3247    | 20/05/2009  | mcoutinho    | IPR 516
 *  3703    | 09/07/2009  | mcoutinho    | IPR 548
 *  3759    | 13/07/2009  | mcoutinho    | IPR 553
 *  3902    | 21/07/2009  | mcoutinho    | IPR 99
 *  4429    | 21/09/2009  | mcoutinho    | IPR 685
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  8587    | 21/07/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

#ifndef _RTEMS_SCORE_THREAD_H
#define _RTEMS_SCORE_THREAD_H

/**
 *  @defgroup ScoreThread Thread Handler
 *
 *  @brief This handler encapsulates functionality related to the management of
 *  threads.  This includes the creation, deletion, and scheduling of threads.
 */
/**@{*/

#ifdef __cplusplus
extern "C"
{
#endif


#include <rtems/score/context.h>
#include <rtems/score/cpu.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mppkt.h>
#endif
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/stack.h>
#include <rtems/score/states.h>
#include <rtems/score/tod.h>
#include <rtems/score/tqdata.h>
#include <rtems/score/watchdog.h>

   /**
    *  @brief "return type" of a thread.
    *
    *  @note  This cannot always be right.  Some APIs have void
    *         tasks/threads, others return pointers, others may
    *         return a numeric value.  Hopefully a pointer is
    *         always at least as big as an uint32_t  . :)
    */
   typedef void *Thread;

   /**
    *  @brief define the ways in which the entry point for a
    *  thread can be invoked.  Basically, it can be passed any
    *  combination/permutation of a pointer and an uint32_t   value.
    *
    *  @note For now, we are ignoring the return type.
    */
   typedef enum
   {
      THREAD_START_NUMERIC ,
      THREAD_START_POINTER ,
      THREAD_START_BOTH_POINTER_FIRST ,
      THREAD_START_BOTH_NUMERIC_FIRST
   } Thread_Start_types;

   /**
    * @brief very simple style thread entry point.
    */
   typedef Thread(*Thread_Entry)( ); /* basic type */

   /**
    * @brief thread entry point which takes a single
    *  unsigned thirty-two bit integer as an argument.
    */
   typedef Thread(*Thread_Entry_numeric)( uint32_t );

   /**
    * @brief thread entry point which takes a single
    *  untyped pointer as an argument.
    */
   typedef Thread(*Thread_Entry_pointer)(void *);


   /**
    *  @brief algorithms used to manage the thread cpu budget.
    *
    *  Reset Timeslice:   At each context switch, reset the time quantum.
    *  Exhaust Timeslice: Only reset the quantum once it is consumed.
    *  Callout:           Execute routine when budget is consumed.
    */
   typedef enum
   {
      THREAD_CPU_BUDGET_ALGORITHM_NONE ,
      THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE ,
   } Thread_CPU_budget_algorithms;

   /**
    * @brief defines the Thread Control Block structure.
    */
   typedef struct Thread_Control_struct Thread_Control;

   /**
    * @brief defines thes the entry point for the thread specific timeslice
    * budget management algorithm.
    */
   typedef void (*Thread_CPU_budget_algorithm_callout )( Thread_Control * );

   /**
    * @brief Declare a pointer to a CORE_mutex_Control
    **/
   typedef struct CORE_mutex_Control *ptrCORE_mutex_Control;

   /**
    *  @brief Per Task Variable Manager Structure Forward Reference
    *
    *  Forward reference to the per task variable structure.
    */
   struct rtems_task_variable_tt;

   /**
    *  @brief Per Task Variable Manager Structure
    *
    *  This is the internal structure used to manager per Task Variables.
    */
   typedef struct
   {
      /**
       * @brief This field points to the next per task variable for this task.
       */
      struct rtems_task_variable_tt *next;

      /**
       * @brief This field points to the physical memory location of this per
       *  task variable.
       */
      void **ptr;

      /**
       * @brief global value for this per task variable.
       */
      void *gval;

      /**
       * @brief this thread's value for this per task variable.
       */
      void *tval;

      /**
       * @brief points to the destructor for this per task variable.
       */
      void (*dtor )(void *);
   } rtems_task_variable_t;

   /**
    *  @brief information which defines
    *  the starting state of a thread.
    */
   typedef struct
   {
      /**
       * @brief starting address for the thread.
       */
      Thread_Entry entry_point;
      /**
       * @brief indicates the how task is invoked.
       */
      Thread_Start_types prototype;
      /**
       * @brief pointer argument passed at thread start.
       */
      void *pointer_argument;
      /**
       * @brief numeric argument passed at thread start.
       */
      uint32_t numeric_argument;

      /*-------------- initial execution modes ----------------- */

      /**
       * @brief indicates whether the thread was preemptible when
       * it started.
       */
      boolean is_preemptible;
      /** @brief indicates the CPU budget algorith.
       */
      Thread_CPU_budget_algorithms budget_algorithm;
      /**
       * @brief routine to invoke when the CPU allotment is
       *  consumed.
       */
      Thread_CPU_budget_algorithm_callout budget_callout;
      /**
       * @brief initial ISR disable level of this thread.
       */
      uint32_t isr_level;
      /**
       * @brief initial priority.
       */
      Priority_Control initial_priority;
      /**
       * @brief indicates whether the SuperCore allocated the stack.
       */
      boolean core_allocated_stack;
      /**
       * @brief stack information.
       */
      Stack_Control Initial_stack;
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
      /**
       * @brief initial FP context area address.
       */
      Context_Control_fp *fp_context;
#endif
      /**
       * @brief initial stack area address.
       */
      void *stack;
   } Thread_Start_information;

   /**
    * @brief define a thread status when its blocking on a proxy
    **/
#define THREAD_STATUS_PROXY_BLOCKING 0x1111111

   /**
    *  @brief information necessary to manage
    *  a thread which it is  waiting for a resource.
    */
   typedef struct
   {
      /**
       * @brief chain node of the thread on the mutex blocking thread list
       */
      Chain_Node node;

      /**
       * @brief Id of the object this thread is waiting upon.
       */
      Objects_Id id;

      /**
       * @brief used to return an integer while when blocked.
       */
      uint32_t count;

      /**
       * @brief first pointer to a user return argument.
       */
      void *return_argument;

      /**
       * @brief second pointer to a user return argument.
       */
      void *return_argument_1;

      /**
       * @brief This field contains any options in effect on this blocking operation.
       */
      uint32_t option;

      /**
       *  @brief contain the return status from a blocking operation.
       *
       *  @note The following assumes that all API return codes can be
       *        treated as an uint32_t.
       */
      uint32_t return_code;

      /**
       * @brief chain header for the second through Nth tasks
       *  of the same priority blocked waiting on the same object.
       */
      Chain_Control Block2n;
      /**
       * @brief points to the thread queue on which this thread is blocked.
       */
      Thread_queue_Control *queue;

      /**
       * @brief Chain of mutexes hold by this thread
       **/
      Chain_Control mutexes_hold;

      /**
       * @brief Chain of threads blocked by mutexes with inheritance protocol
       **/
      Chain_Control threads_blocked;

      /**
       * @brief the thread is blocked on this mutexes with priority inheritance
       * (if not NULL)
       **/
      ptrCORE_mutex_Control mutex_blocked;

      /**
       * @brief pointer to the thread that holds this information
       **/
      Thread_Control *thread;

   } Thread_Wait_information;

#if defined(RTEMS_MULTIPROCESSING)

   /**
    *  @brief control block used to manage
    *  each thread proxy.
    *
    *  @note It is critical that proxies and threads have identical
    *        memory images for the shared part.
    */
   typedef struct
   {
      /**
       * @brief object management structure for each proxy.
       */
      Objects_Control Object;

      /**
       * @brief current execution state of this proxy.
       */
      States_Control current_state;

      /**
       * @brief current priority state of this proxy.
       */
      Priority_Control current_priority;

      /**
       * @brief base priority of this proxy.
       */
      Priority_Control real_priority;

      /**
       * @brief This field is the number of mutexes currently held by this proxy.
       */
      uint32_t resource_count;

      /**
       * @brief blocking information for this proxy.
       */
      Thread_Wait_information Wait;

      /**
       * @brief Watchdog used to manage proxy delays and timeouts.
       */
      Watchdog_Control Timer;

      /**
       * @brief received response packet in an MP system.
       */
      MP_packet_Prefix *receive_packet;

      /****************** end of common block ********************/

      /**
       * @brief manage the set of proxies in the system.
       */
      Chain_Node Active;
   } Thread_Proxy_control;
#endif

   /**
    *  @brief control block used to manage each thread.
    *
    *  @note It is critical that proxies and threads have identical
    *        memory images for the shared part.
    */
   typedef enum
   {
      /**
       * @brief Classic RTEMS API.
       */
      THREAD_API_RTEMS
   } Thread_APIs;

   /**
    * @brief define the first API which has threads.
    */
#define THREAD_API_FIRST THREAD_API_RTEMS

   /**
    * @brief define the last API which has threads.
    */
#define THREAD_API_LAST  THREAD_API_RTEMS

   /**
    *  @brief Thread Control Block (TCB).
    */
   struct Thread_Control_struct
   {
      /**
       * @brief object management structure for each thread.
       */
      Objects_Control Object;

      /**
       * @brief current execution state of this thread.
       */
      States_Control current_state;

      /**
       * @brief current priority state of this thread.
       */
      Priority_Control current_priority;

      /**
       * @brief base priority of this thread.
       */
      Priority_Control real_priority;

      /**
       * @brief number of mutexes currently held by this thread.
       */
      uint32_t resource_count;

      /**
       * @brief blocking information for this thread.
       */
      Thread_Wait_information Wait;

      /**
       * @brief Watchdog used to manage thread delays and timeouts.
       */
      Watchdog_Control Timer;
#if defined(RTEMS_MULTIPROCESSING)
      /**
       * @brief received response packet in an MP system.
       */
      MP_packet_Prefix *receive_packet;
#endif

      /*================= end of common block =================*/

      /**
       * @brief number of nested suspend calls.
       */
      uint32_t suspend_count;

      /**
       * @brief is true if the thread is offered globally
       */
      boolean is_global;

      /**
       * @brief is true if the thread is preemptible.
       */
      boolean is_preemptible;

      /**
       * @brief GNAT self context pointer.
       */
      void *rtems_ada_self;

      /**
       * @brief length of the time quantum that this thread is
       *  allowed to consume.  The algorithm used to manage limits on CPU usage
       *  is specified by budget_algorithm.
       */
      uint32_t cpu_time_budget;

      /**
       * @brief algorithm used to manage this thread's time
       *  quantum.  The algorithm may be specified as none which case,
       *  no limit is in place.
       */
      Thread_CPU_budget_algorithms budget_algorithm;

      /**
       * @brief method invoked with the budgeted time is consumed.
       */
      Thread_CPU_budget_algorithm_callout budget_callout;

      /**
       * @brief points to the Ready FIFO for this priority.
       */
      Chain_Control *ready;

      /**
       * @brief contains precalculated priority map indices.
       */
      Priority_Information Priority_map;

      /**
       * @brief contains information about the starting state of
       *  this thread.
       */
      Thread_Start_information Start;

      /**
       * @brief contains the context of this thread.
       */
      Context_Control Registers;

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
      /**
       * @brief points to the floating point context for this thread.
       *  If NULL, the thread is integer only.
       */
      Context_Control_fp *fp_context;
#endif

      /**
       * @brief This array contains the API extension area pointers.
       */
      void *API_Extensions[ THREAD_API_LAST + 1 ];

      /**
       * @brief points to the user extension pointers.
       */
      void **extensions;

      /**
       * @brief points to the set of per task variables.
       */
      rtems_task_variable_t *task_variables;
   };

   /**
    * @brief Self for the GNU Ada Run-Time
    */
   extern void *rtems_ada_self;

   /**
    *  @brief define the information control block used to
    *  manage this class of objects.
    */
   extern Objects_Information _Thread_Internal_information;

   /**
    *  @brief define the thread control pointers used to access
    *  and manipulate the idle thread.
    */
   extern Thread_Control *_Thread_Idle;

   /**
    *  @brief context of the "thread"
    *  which invoked the start multitasking routine.  This context is
    *  restored as the last action of the stop multitasking routine.  Thus
    *  control of the processor can be returned to the environment
    *  which initiated the system.
    */
   extern Context_Control _Thread_BSP_context;

   /**
    *  @brief declare the dispatch critical section nesting
    *  counter which is used to prevent context switches at inopportune
    *  moments.
    */
   extern volatile uint32_t _Thread_Dispatch_disable_level;

   /**
    *  @brief hold how many user extensions are in the system.  This
    *  is used to determine how many user extension data areas to allocate
    *  per thread.
    */
   extern uint32_t _Thread_Maximum_extensions;

   /**
    *  @brief manage the length of a timeslice quantum.
    */
   extern uint32_t _Thread_Ticks_per_timeslice;

   /**
    *  @brief point to the array of FIFOs used to manage the
    *  set of ready threads.
    */
   extern Chain_Control *_Thread_Ready_chain;

   /**
    *  @brief point to the thread which is currently executing.
    *  This thread is implicitly manipulated by numerous directives.
    */
   extern Thread_Control *_Thread_Executing;

   /**
    *  @brief point to the highest priority ready thread
    *  in the system.  Unless the current thread is not preemptibl,
    *  then this thread will be context switched to when the next
    *  dispatch occurs.
    */
   extern Thread_Control *_Thread_Heir;

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
#if ( CPU_USE_DEFERRED_FP_SWITCH == TRUE )
   /**
    *  @brief point to the thread whose floating point
    *  context is currently loaded.
    */
   extern Thread_Control *_Thread_Allocated_fp;
#endif
#endif


   /**
    *  @brief initialize the Thread Handler
    *
    *  This routine initializes all thread manager related data structures.
    *
    *  @param[in] ticks_per_timeslice clock ticks per quantum
    *  @param[in] maximum_extensions number of maximum extensions
    *  @param[in] maximum_proxies number of proxies to initialize
    */
   void _Thread_Handler_initialization(
                                       uint32_t ticks_per_timeslice ,
                                       uint32_t maximum_extensions
#if defined(RTEMS_MULTIPROCESSING)
       ,
                                       uint32_t maximum_proxies
#endif
       );

   /**
    *  @brief create idle thread
    *
    *  @note No thread should be created before this one.
    */
   void _Thread_Create_idle(void);

   /**
    *  @brief start multitasking
    *
    *  This kernel routine readies the requested thread, the thread chain
    *  is adjusted.  A new heir thread may be selected.
    *  INTERRUPT LATENCY:
    *  ready chain
    *  select heir
    *
    *  @note This routine uses the "blocking" heir selection mechanism.
    *  This ensures the correct heir after a thread restart.
    */
   void _Thread_Start_multitasking(void);

   /**
    *  @brief perform thread dispatching to heir thread
    *
    *  This routine is responsible for transferring control of the
    *  processor from the executing thread to the heir thread.  As part
    *  of this process, it is responsible for the following actions:
    *  + saving the context of the executing thread
    *  + restoring the context of the heir thread
    *  + dispatching any signals for the resulting executing thread
    */
   void _Thread_Dispatch(void);

   /**
    *  @brief allocate thread stack
    *
    *  Allocate the requested stack space for the thread.
    *  return the actual size allocated after any adjustment
    *  or return zero if the allocation failed.
    *  Set the Start.stack field to the address of the stack
    *
    *  @param[in] the_thread the thread to allocate the stack
    *  @param[in] stack_size the size of the stack
    *
    *  @return returns the size of the allocate stack
    */
   size_t _Thread_Stack_Allocate(
                                 Thread_Control *the_thread ,
                                 size_t stack_size
                                 );

   /**
    *  @brief free thread stack
    *
    *  Deallocate the Thread's stack.
    *
    *  @param[in] the_thread the thread to free the stack
    */
   void _Thread_Stack_Free(
                           Thread_Control *the_thread
                           );

   /**
    *  @brief initialize a thread
    *
    *  This routine initializes the specified the thread.  It allocates
    *  all memory associated with this thread.  It completes by adding
    *  the thread to the local object table so operations on this
    *  thread id are allowed.
    *
    *  @param[in] information the object information
    *  @param[in] the_thread the thread to initialize
    *  @param[in] stack_size the thread stack size
    *  @param[in] is_fp if the thread is floating point
    *  @param[in] priority the thread priority
    *  @param[in] is_preemptible the thread preemptable option
    *  @param[in] budget_algorithm the thread budget algortithm
    *  @param[in] budget_callout the thread budget callout function
    *  @param[in] isr_level the thread interrupt level
    *  @param[in] name the thread name
    *
    *  @return returns TRUE if the thread was correctly initialized. False otherwise.
    *
    */
   boolean _Thread_Initialize(
                              Objects_Information *information ,
                              Thread_Control *the_thread ,
                              size_t stack_size ,
                              boolean is_fp ,
                              Priority_Control priority ,
                              boolean is_preemptible ,
                              Thread_CPU_budget_algorithms budget_algorithm ,
                              Thread_CPU_budget_algorithm_callout budget_callout ,
                              uint32_t isr_level ,
                              Objects_Name name
                              );

   /**
    *  @brief start a thread
    *
    *  This routine initializes the executable information for a thread
    *  and makes it ready to execute.  After this routine executes, the
    *  thread competes with all other threads for CPU time.
    *
    *  @param[in] the_thread the thread to start
    *  @param[in] the_prototype the prototype type of the entry point
    *  @param[in] entry_point the entry point
    *  @param[in] pointer_argument the pointer argument
    *  @param[in] numeric_argument the numberic argument
    *
    *  @return returns TRUE if the thread has started and false otherwise
    *
    */
   boolean _Thread_Start(
                         Thread_Control *the_thread ,
                         Thread_Start_types the_prototype ,
                         void *entry_point ,
                         void *pointer_argument ,
                         uint32_t numeric_argument
                         );

   /**
    *  @brief restart a thread
    *
    *  This support routine restarts the specified task in a way that the
    *  next time this thread executes, it will begin execution at its
    *  original starting point.
    *
    *  @param[in] the_thread the thread to restart
    *  @param[in] pointer_argument the pointer argument
    *  @param[in] numeric_argument the numberic argument
    *
    *  @return returns TRUE if the thread has successfuly restarted and FALSE
    *  otherwise
    */
   boolean _Thread_Restart(
                           Thread_Control *the_thread ,
                           void *pointer_argument ,
                           uint32_t numeric_argument
                           );

   /**
    *  @brief reset a thread
    *
    *  This routine resets a thread to its initial stat but does
    *  not actually restart it.  Some APIs do this in separate
    *  operations and this division helps support this.
    *
    *  @param[in] the_thread the thread to reset
    *  @param[in] pointer_argument the pointer argument
    *  @param[in] numeric_argument the numberic argument
    */
   void _Thread_Reset(
                      Thread_Control *the_thread ,
                      void *pointer_argument ,
                      uint32_t numeric_argument
                      );

   /**
    *  @brief close a thread
    *
    *  This routine frees all memory associated with the specified
    *  thread and removes it from the local object table so no further
    *  operations on this thread are allowed.
    *
    *  @param[in] information the thread object information
    *  @param[in] the_thread the thread to close
    */
   void _Thread_Close(
                      Objects_Information *information ,
                      Thread_Control *the_thread
                      );

   /**
    *  @brief places a thread in the ready state
    *
    *  This kernel routine readies the requested thread, the thread chain
    *  is adjusted.  A new heir thread may be selected.
    *  INTERRUPT LATENCY:
    *  ready chain
    *  select heir
    *
    *  @param[in] the_thread pointer to thread control block
    *
    *  @note This routine uses the "blocking" heir selection mechanism.
    *  This ensures the correct heir after a thread restart.
    */
   void _Thread_Ready(
                      Thread_Control *the_thread
                      );

   /**
    *  @brief remove state(s) from a thread
    *
    *  This kernel routine clears the appropriate states in the
    *  requested thread.  The thread ready chain is adjusted if
    *  necessary and the Heir thread is set accordingly.
    *  INTERRUPT LATENCY:
    *  priority map
    *  select heir
    *
    *  @param[in] the_thread pointer to thread control block
    *  @param[in] state state set to clear
    */
   void _Thread_Clear_state(
                            Thread_Control *the_thread ,
                            States_Control state
                            );

   /**
    *  @brief set state(s) to a thread
    *
    *  This kernel routine sets the requested state in the THREAD.  The
    *  THREAD chain is adjusted if necessary.
    *  INTERRUPT LATENCY:
    *  ready chain
    *  select map
    *
    *  @param[in] the_thread pointer to thread control block
    *  @param[in] state state to be set
    */
   void _Thread_Set_state(
                          Thread_Control *the_thread ,
                          States_Control state
                          );

   /**
    *  @brief set the thread in the transient state
    *
    *  This kernel routine places the requested thread in the transient state
    *  which will remove it from the ready queue, if necessary.  No
    *  rescheduling is necessary because it is assumed that the transient
    *  state will be cleared before dispatching is enabled.
    *  INTERRUPT LATENCY:
    *  only case
    *
    *  @param[in] the_thread pointer to thread control block
    */
   void _Thread_Set_transient(
                              Thread_Control *the_thread
                              );

   /**
    *  @brief reset the timeslice of the running thread
    *
    *  This routine will remove the running thread from the ready chain
    *  and place it immediately at the rear of this chain and then the
    *  timeslice counter is reset.  The heir THREAD will be updated if
    *  the running is also the currently the heir.
    *  INTERRUPT LATENCY:
    *  ready chain
    *  select heir
    */
   void _Thread_Reset_timeslice(void);

   /**
    *  @brief tickle the timeslice of the running thread
    *
    *  This scheduler routine determines if timeslicing is enabled
    *  for the currently executing thread and, if so, updates the
    *  timeslice count and checks for timeslice expiration.
    */
   void _Thread_Tickle_timeslice(void);

   /**
    *  @brief the running thread yields the processor
    *
    *  This kernel routine will remove the running THREAD from the ready chain
    *  and place it immediatly at the rear of this chain.  Reset timeslice
    *  and yield the processor functions both use this routine, therefore if
    *  reset is TRUE and this is the only thread on the chain then the
    *  timeslice counter is reset.  The heir THREAD will be updated if the
    *  running is also the currently the heir.
    *  INTERRUPT LATENCY:
    *  ready chain
    *  select heir
    */
   void _Thread_Yield_processor(void);

   /**
    *  @brief rotate thread ready queue
    *
    *  This routine is invoked to rotate the ready queue for the
    *  given priority.  It can be used to yeild the processor
    *  by rotating the executing threads ready queue.
    *
    *  @param[in] priority the priority to rotate the thread ready queue
    */
   void _Thread_Rotate_Ready_Queue(
                                   Priority_Control priority
                                   );

   /**
    *  @brief load the starting thread environment
    *
    *  Load starting environment for another thread from its start area in the
    *  thread.  Only called from t_restart and t_start.
    *
    *  @param[in] the_thread thread control block pointer
    */
   void _Thread_Load_environment(
                                 Thread_Control *the_thread
                                 );

   /**
    *  @brief entry point for all threads
    *
    *  This routine is the "primal" entry point for all threads.
    *  _Context_Initialize() dummies up the thread's initial context
    *  to cause the first Context_Switch() to jump to _Thread_Handler().
    *  This routine is the default thread exitted error handler.  It is
    *  returned to when a thread exits.  The configured fatal error handler
    *  is invoked to process the exit.
    *
    *  On entry, it is assumed all interrupts are blocked and that this
    *  routine needs to set the initial isr level.  This may or may not
    *  actually be needed by the context switch routine and as a result
    *  interrupts may already be at there proper level.  Either way,
    *  setting the initial isr level properly here is safe.
    *  Currently this is only really needed for the posix port,
    *  ref: _Context_Switch in unix/cpu.c
    */
   void _Thread_Handler(void);

   /**
    *  @brief thread delay ended
    *
    *  This routine processes a thread whose delay period has ended.
    *  It is called by the watchdog handler.
    *
    *  @param[in] id thread id
    *  @param[in] ignored ignored
    */
   void _Thread_Delay_ended(
                            Objects_Id id ,
                            void *ignored
                            );

   /**
    *  @brief change the priority of a thread
    *
    *  This kernel routine changes the priority of the thread.  The
    *  thread chain is adjusted if necessary.
    *  INTERRUPT LATENCY:
    *  ready chain
    *  select heir
    *
    *  @param[in] the_thread pointer to thread control block
    *  @param[in] new_priority ultimate priority
    *  @param[in] prepend_it TRUE if the thread should be prepended to the chain
    */
   void _Thread_Change_priority(
                                Thread_Control *the_thread ,
                                Priority_Control new_priority ,
                                boolean prepend_it
                                );

   /**
    *  @brief set the priority of a thread
    *
    *  This directive enables and disables several modes of
    *  execution for the requesting thread.
    *  Output: NONE
    *
    *  @param[in] the_thread pointer to thread priority
    *  @param[in] new_priority new priority
    */
   void _Thread_Set_priority(
                             Thread_Control *the_thread ,
                             Priority_Control new_priority
                             );

   /**
    *  @brief suspend a thread
    *
    *  This kernel routine sets the SUSPEND state in the THREAD.  The
    *  THREAD chain and suspend count are adjusted if necessary.
    *  INTERRUPT LATENCY:
    *  ready chain
    *  select map
    *
    *  @param[in] the_thread pointer to thread control block
    */
   void _Thread_Suspend(
                        Thread_Control *the_thread
                        );

   /**
    *  @brief resume a thread
    *
    *  This kernel routine clears the SUSPEND state if the suspend_count
    *  drops below one.  If the force parameter is set the suspend_count
    *  is forced back to zero. The thread ready chain is adjusted if
    *  necessary and the Heir thread is set accordingly.
    *  INTERRUPT LATENCY:
    *  priority map
    *  select heir
    *
    *  @param[in] the_thread pointer to thread control block
    *  @param[in] force force the suspend count back to 0
    */
   void _Thread_Resume(
                       Thread_Control *the_thread ,
                       boolean force
                       );

   /**
    *  @brief evaluate the running thread mode and determine if a context switch is
    *  necessary
    *
    *  This routine evaluates the current scheduling information for the
    *  system and determines if a context switch is required.  This
    *  is usually called after changing an execution mode such as preemptability
    *  for a thread.
    *
    *  @return return TRUE if a thread context switch is necessary and FALSE otherwise
    */
   boolean _Thread_Evaluate_mode(void);

#if (CPU_PROVIDES_IDLE_THREAD_BODY == FALSE)

   /**
    *  @brief supplies the idle thread body
    *
    *  This kernel routine is the idle thread.  The idle thread runs any time
    *  no other thread is ready to run.  This thread loops forever with
    *  interrupts enabled.
    *
    *  @param[in] ignored this parameter is ignored
    *
    *  @return does not return (this function runs in a loop)
    */
   Thread _Thread_Idle_body(
                            uint32_t ignored
                            );


   /**
    *  @brief free all mutexes held by a thread.
    *
    *  This routine is typically called when the thread is deleted or restarted
    *
    *  @param[in] thread the thread that is going to free all mutexes that it holds
    *
    *  @note thread dispatching should be disabled when calling this function
    **/
   void _Thread_Free_mutexes(Thread_Control *thread);
#endif

   /**
    * @brief define the type for a method which operates on a single thread.
    */
   typedef void (*rtems_per_thread_routine )( Thread_Control * );

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/thread.inl>
#endif
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/threadmp.h>
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
