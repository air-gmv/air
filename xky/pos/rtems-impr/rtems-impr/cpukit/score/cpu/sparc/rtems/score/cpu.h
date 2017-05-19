/**
 *  @file
 *  cpu.h
 *
 *  @brief information pertaining to the port of the executive to the SPARC
 *  processor.
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
 *  597     | 17/11/2008  | mcoutinho    | IPR 64
 *  597     | 17/11/2008  | mcoutinho    | IPR 69
 *  1904    | 16/03/2009  | mcoutinho    | IPR 60
 *  4546    | 28/09/2009  | mcoutinho    | IPR 778
 *  4691    | 07/10/2009  | mcoutinho    | IPR 462
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup SUPER_CORE_SPARC_CPU_SUPPORT Super Core SPARC CPU Support
 *  @{
 */

#ifndef _RTEMS_SCORE_CPU_H
#define _RTEMS_SCORE_CPU_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <xky.h>
#include <rtems/score/sparc.h>               /* pick up machine definitions */
#ifndef ASM
#include <stdint.h>
#endif

   /* conditional compilation parameters */

   /**
    *  @brief determine if SPARC enable dispatch function is inline
    *
    *  If TRUE, then they are inlined.
    *  If FALSE, then a subroutine call is made.
    */
#define CPU_INLINE_ENABLE_DISPATCH       TRUE

   /**
    *  @brief determine if the search loops should be unrolled one time
    *
    *  Should the body of the search loops in _Thread_queue_Enqueue_priority
    *  be unrolled one time?  In unrolled each iteration of the loop examines
    *  two "nodes" on the chain being searched.  Otherwise, only one node
    *  is examined per iteration.
    *
    *  If TRUE, then the loops are unrolled.
    *  If FALSE, then the loops are not unrolled.
    *
    *  This parameter could go either way on the SPARC.  The interrupt flash
    *  code is relatively lengthy given the requirements for nops following
    *  writes to the psr.  But if the clock speed were high enough, this would
    *  not represent a great deal of time.
    */
#define CPU_UNROLL_ENQUEUE_PRIORITY      TRUE

   /**
    *  @brief determine if SPARC has software interrupt stack
    *
    *  If TRUE, then a stack is allocated in _ISR_Handler_initialization.
    *  If FALSE, nothing is done.
    *
    *  The SPARC does not have a dedicated HW interrupt stack and one has
    *  been implemented in SW.
    */
#define CPU_HAS_SOFTWARE_INTERRUPT_STACK   TRUE

   /**
    *  @brief determine if SPARC has ardware support for interrupt stack
    *
    *  If TRUE, then it must be installed during initialization.
    *  If FALSE, then no installation is performed.
    *
    *  The SPARC does not have a dedicated HW interrupt stack.
    */
#define CPU_HAS_HARDWARE_INTERRUPT_STACK  FALSE

   /**
    *  @brief determine if SPARC allocates the interrupt stack
    *
    *  If TRUE, then the memory is allocated during initialization.
    *  If FALSE, then the memory is allocated during initialization.
    */
#define CPU_ALLOCATE_INTERRUPT_STACK      TRUE

   /**
    *  @brief determine if SPARC ISR arguments contains the frame pointer
    *
    *  Does the RTEMS invoke the user's ISR with the vector number and
    *  a pointer to the saved interrupt frame (1) or just the vector
    *  number (0)?
    */
#define CPU_ISR_PASSES_FRAME_POINTER 0

   /**
    *  @brief determine if SPARC has hardware floating point unit
    *
    *  If TRUE, then the FLOATING_POINT task attribute is supported.
    *  If FALSE, then the FLOATING_POINT task attribute is ignored.
    */
#if ( SPARC_HAS_FPU == 1 )
#define CPU_HARDWARE_FP     TRUE
#else
#define CPU_HARDWARE_FP     FALSE
#endif
#define CPU_SOFTWARE_FP     FALSE

   /**
    *  @brief determine if all tasks have floating point
    *
    *  Are all tasks FLOATING_POINT tasks implicitly?
    *
    *  If TRUE, then the FLOATING_POINT task attribute is assumed.
    *  If FALSE, then the FLOATING_POINT task attribute is followed.
    */
#define CPU_ALL_TASKS_ARE_FP     FALSE

   /**
    *  @brief determine if idle thread has floating point
    *
    *  Should the IDLE task have a floating point context?
    *
    *  If TRUE, then the IDLE task is created as a FLOATING_POINT task
    *  and it has a floating point context which is switched in and out.
    *  If FALSE, then the IDLE task does not have a floating point context.
    */
#define CPU_IDLE_TASK_IS_FP      FALSE

   /**
    *  @brief determine if should use deferred FPU switch
    *
    *  Should the saving of the floating point registers be deferred
    *  until a context switch is made to another different floating point
    *  task?
    *
    *  If TRUE, then the floating point context will not be stored until
    *  necessary.  It will remain in the floating point registers and not
    *  disturned until another floating point task is switched to.
    *
    *  If FALSE, then the floating point context is saved when a floating
    *  point task is switched out and restored when the next floating point
    *  task is restored.  The state of the floating point registers between
    *  those two operations is not specified.
    *
    *  Recommended: FALSE, this improves system robustness
    */
#define CPU_USE_DEFERRED_FP_SWITCH       FALSE

   /**
    *  @brief determine if SPARC provide their own idle thread body
    *
    *  If TRUE, then the routine _CPU_Thread_Idle_body
    *  must be provided and is the default IDLE thread body instead of
    *  _CPU_Thread_Idle_body.
    *
    *  If FALSE, then use the generic IDLE thread body if the BSP does
    *  not provide one.
    */
#define CPU_PROVIDES_IDLE_THREAD_BODY    FALSE

   /**
    *  @brief determine if the SPARC stack groes up
    *
    *  Does the stack grow up (toward higher addresses) or down
    *  (toward lower addresses)?
    *
    *  If TRUE, then the grows upward.
    *  If FALSE, then the grows toward smaller addresses.
    *
    *  The stack grows to lower addresses on the SPARC.
    */
#define CPU_STACK_GROWS_UP               FALSE

   /**
    *  @brief force alignment of critical data structures
    *
    *  The following is the variable attribute used to force alignment
    *  of critical data structures.  On some processors it may make
    *  sense to have these aligned on tighter boundaries than
    *  the minimum requirements of the compiler in order to have as
    *  much of the critical data area as possible in a cache line.
    *
    *  The SPARC does not appear to have particularly strict alignment
    *  requirements.  This value was chosen to take advantages of caches.
    */
#define CPU_STRUCTURE_ALIGNMENT          __attribute__ ((aligned (16)))

   /**
    *  @brief SPARC is big endian
    */
#define CPU_BIG_ENDIAN                           TRUE

   /**
    *  @brief SPARC is not little endian
    **/
#define CPU_LITTLE_ENDIAN                        FALSE

   /**
    *  @brief number of bits actually used in the
    *  interrupt field of the task mode.
    *
    *  How those bits map to the
    *  CPU interrupt levels is defined by the routine _CPU_ISR_Set_level().
    *
    *  The SPARC has 16 interrupt levels in the PIL field of the PSR.
    */
#define CPU_MODES_INTERRUPT_MASK   0x0000000F

#ifndef ASM

   /**
    *  @brief SPARC minimum stack frame
    *
    *  This structure represents the organization of the minimum stack frame
    *  for the SPARC.  More framing information is required in certain situaions
    *  such as when there are a large number of out parameters or when the callee
    *  must save floating point registers.
    **/
   typedef struct
   {
      uint32_t l0;
      uint32_t l1;
      uint32_t l2;
      uint32_t l3;
      uint32_t l4;
      uint32_t l5;
      uint32_t l6;
      uint32_t l7;
      uint32_t i0;
      uint32_t i1;
      uint32_t i2;
      uint32_t i3;
      uint32_t i4;
      uint32_t i5;
      uint32_t i6_fp;
      uint32_t i7;
      void *structure_return_address;
      /*
       *  The following are for the callee to save the register arguments in
       *  should this be necessary.
       */
      uint32_t saved_arg0;
      uint32_t saved_arg1;
      uint32_t saved_arg2;
      uint32_t saved_arg3;
      uint32_t saved_arg4;
      uint32_t saved_arg5;
      uint32_t pad0;
   } CPU_Minimum_stack_frame;

#endif /* ASM */

#define CPU_STACK_FRAME_L0_OFFSET             0x00
#define CPU_STACK_FRAME_L1_OFFSET             0x04
#define CPU_STACK_FRAME_L2_OFFSET             0x08
#define CPU_STACK_FRAME_L3_OFFSET             0x0c
#define CPU_STACK_FRAME_L4_OFFSET             0x10
#define CPU_STACK_FRAME_L5_OFFSET             0x14
#define CPU_STACK_FRAME_L6_OFFSET             0x18
#define CPU_STACK_FRAME_L7_OFFSET             0x1c
#define CPU_STACK_FRAME_I0_OFFSET             0x20
#define CPU_STACK_FRAME_I1_OFFSET             0x24
#define CPU_STACK_FRAME_I2_OFFSET             0x28
#define CPU_STACK_FRAME_I3_OFFSET             0x2c
#define CPU_STACK_FRAME_I4_OFFSET             0x30
#define CPU_STACK_FRAME_I5_OFFSET             0x34
#define CPU_STACK_FRAME_I6_FP_OFFSET          0x38
#define CPU_STACK_FRAME_I7_OFFSET             0x3c
#define CPU_STRUCTURE_RETURN_ADDRESS_OFFSET   0x40
#define CPU_STACK_FRAME_SAVED_ARG0_OFFSET     0x44
#define CPU_STACK_FRAME_SAVED_ARG1_OFFSET     0x48
#define CPU_STACK_FRAME_SAVED_ARG2_OFFSET     0x4c
#define CPU_STACK_FRAME_SAVED_ARG3_OFFSET     0x50
#define CPU_STACK_FRAME_SAVED_ARG4_OFFSET     0x54
#define CPU_STACK_FRAME_SAVED_ARG5_OFFSET     0x58
#define CPU_STACK_FRAME_PAD0_OFFSET           0x5c

#define CPU_MINIMUM_STACK_FRAME_SIZE          0x60



#ifndef ASM

   /**
    *  @brief context of a SPARC CPU
    *
    *  Generally there are 2 types of context to save.
    *     1. Interrupt registers to save
    *     2. Task level registers to save
    *
    *  This means we have the following 3 context items:
    *     1. task level context stuff::  Context_Control
    *     2. floating point task stuff:: Context_Control_fp
    *     3. special interrupt level context :: Context_Control_interrupt
    *
    *  On the SPARC, we are relatively conservative in that we save most
    *  of the CPU state in the context area.  The ET (enable trap) bit and
    *  the CWP (current window pointer) fields of the PSR are considered
    *  system wide resources and are not maintained on a per-thread basis.
    */
   typedef struct
   {
      /*
       *  Using a double g0_g1 will put everything in this structure on a
       *  double word boundary which allows us to use double word loads
       *  and stores safely in the context switch.
       */
      double g0_g1;
      uint32_t g2;
      uint32_t g3;
      uint32_t g4;
      uint32_t g5;
      uint32_t g6;
      uint32_t g7;

      uint32_t l0;
      uint32_t l1;
      uint32_t l2;
      uint32_t l3;
      uint32_t l4;
      uint32_t l5;
      uint32_t l6;
      uint32_t l7;

      uint32_t i0;
      uint32_t i1;
      uint32_t i2;
      uint32_t i3;
      uint32_t i4;
      uint32_t i5;
      uint32_t i6_fp;
      uint32_t i7;

      uint32_t o0;
      uint32_t o1;
      uint32_t o2;
      uint32_t o3;
      uint32_t o4;
      uint32_t o5;
      uint32_t o6_sp;
      uint32_t o7;

      uint32_t psr;
      uint32_t isr_dispatch_disable;
   } Context_Control;

#endif /* ASM */

   /*
    *  Offsets of fields with Context_Control for assembly routines.
    */

#define G0_OFFSET    0x00
#define G1_OFFSET    0x04
#define G2_OFFSET    0x08
#define G3_OFFSET    0x0C
#define G4_OFFSET    0x10
#define G5_OFFSET    0x14
#define G6_OFFSET    0x18
#define G7_OFFSET    0x1C

#define L0_OFFSET    0x20
#define L1_OFFSET    0x24
#define L2_OFFSET    0x28
#define L3_OFFSET    0x2C
#define L4_OFFSET    0x30
#define L5_OFFSET    0x34
#define L6_OFFSET    0x38
#define L7_OFFSET    0x3C

#define I0_OFFSET    0x40
#define I1_OFFSET    0x44
#define I2_OFFSET    0x48
#define I3_OFFSET    0x4C
#define I4_OFFSET    0x50
#define I5_OFFSET    0x54
#define I6_FP_OFFSET 0x58
#define I7_OFFSET    0x5C

#define O0_OFFSET    0x60
#define O1_OFFSET    0x64
#define O2_OFFSET    0x68
#define O3_OFFSET    0x6C
#define O4_OFFSET    0x70
#define O5_OFFSET    0x74
#define O6_SP_OFFSET 0x78
#define O7_OFFSET    0x7C

#define PSR_OFFSET   0x80
#define ISR_DISPATCH_DISABLE_STACK_OFFSET 0x84

   /**
    *  @brief context control size (in number of bytes)
    **/
#define CONTEXT_CONTROL_SIZE 0x88


#ifndef ASM

   /**
    *  @brief floating point context area.
    */
   typedef struct
   {
      double f0_f1;
      double f2_f3;
      double f4_f5;
      double f6_f7;
      double f8_f9;
      double f10_f11;
      double f12_f13;
      double f14_f15;
      double f16_f17;
      double f18_f19;
      double f20_f21;
      double f22_f23;
      double f24_f25;
      double f26_f27;
      double f28_f29;
      double f30_f31;
      uint32_t fsr;
   } Context_Control_fp;

#endif /* ASM */

   /*
    *  Offsets of fields with Context_Control_fp for assembly routines.
    */

#define FO_F1_OFFSET     0x00
#define F2_F3_OFFSET     0x08
#define F4_F5_OFFSET     0x10
#define F6_F7_OFFSET     0x18
#define F8_F9_OFFSET     0x20
#define F1O_F11_OFFSET   0x28
#define F12_F13_OFFSET   0x30
#define F14_F15_OFFSET   0x38
#define F16_F17_OFFSET   0x40
#define F18_F19_OFFSET   0x48
#define F2O_F21_OFFSET   0x50
#define F22_F23_OFFSET   0x58
#define F24_F25_OFFSET   0x60
#define F26_F27_OFFSET   0x68
#define F28_F29_OFFSET   0x70
#define F3O_F31_OFFSET   0x78
#define FSR_OFFSET       0x80

   /**
    *  @brief The context floating point size
    **/
#define CONTEXT_CONTROL_FP_SIZE 0x84

#ifndef ASM

   /**
    *  @brief Context saved on stack for an interrupt.
    *
    *  @note The PSR, PC, and NPC are only saved in this structure for the
    *  benefit of the user's handler.
    */
   typedef struct
   {
      CPU_Minimum_stack_frame Stack_frame;

      uint32_t psr;
      uint32_t pc;
      uint32_t npc;

      uint32_t g1;
      uint32_t g2;
      uint32_t g3;
      uint32_t g4;
      uint32_t g5;
      uint32_t g6;
      uint32_t g7;

      uint32_t i0;
      uint32_t i1;
      uint32_t i2;
      uint32_t i3;
      uint32_t i4;
      uint32_t i5;
      uint32_t i6_fp;
      uint32_t i7;

      uint32_t y;

      uint32_t tpc;
      
   } CPU_Interrupt_frame;

#endif /* ASM */

   /*
    *  Offsets of fields with CPU_Interrupt_frame for assembly routines.
    */

#define ISF_STACK_FRAME_OFFSET 0x00
#define ISF_PSR_OFFSET         CPU_MINIMUM_STACK_FRAME_SIZE + 0x00
#define ISF_PC_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x04
#define ISF_NPC_OFFSET         CPU_MINIMUM_STACK_FRAME_SIZE + 0x08
#define ISF_G1_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x0c
#define ISF_G2_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x10
#define ISF_G3_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x14
#define ISF_G4_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x18
#define ISF_G5_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x1c
#define ISF_G6_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x20
#define ISF_G7_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x24
#define ISF_I0_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x28
#define ISF_I1_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x2c
#define ISF_I2_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x30
#define ISF_I3_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x34
#define ISF_I4_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x38
#define ISF_I5_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x3c
#define ISF_I6_FP_OFFSET       CPU_MINIMUM_STACK_FRAME_SIZE + 0x40
#define ISF_I7_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x44
#define ISF_Y_OFFSET           CPU_MINIMUM_STACK_FRAME_SIZE + 0x48
#define ISF_TPC_OFFSET         CPU_MINIMUM_STACK_FRAME_SIZE + 0x4c

   /**
    *  @brief SPARC minimum stack frame size
    **/
#define CONTEXT_CONTROL_INTERRUPT_FRAME_SIZE CPU_MINIMUM_STACK_FRAME_SIZE + 0x50 
#ifndef ASM

   /**
    *  @brief information required to configure
    *  the processor specific parameters.
    */
   typedef struct
   {
      void (*pretasking_hook )(void);
      void (*predriver_hook )(void);
      void (*postdriver_hook )(void);
      void (*idle_task )(void);
      boolean do_zero_of_workspace;
      uint32_t idle_task_stack_size;
      uint32_t interrupt_stack_size;
#if defined(RTEMS_MULTIPROCESSING)
      uint32_t extra_mpci_receive_server_stack;
#endif
      void * ( *stack_allocate_hook )( uint32_t );
      void (*stack_free_hook )(void*);

      /* end of fields required on all CPUs */

   } rtems_cpu_table;

   /*
    *  Macros to access required entires in the CPU Table are in
    *  the file rtems/system.h.
    */

   /*
    *  Macros to access SPARC specific additions to the CPU Table
    */

   /* There are no CPU specific additions to the CPU Table for this port. */

   /**
    *  @brief null CPU floating point context
    *
    *  This variable contains the initialize context for the FP unit.
    *  It is filled in by _CPU_Initialize and copied into the task's FP
    *  context area during _CPU_Context_Initialize.
    */
   extern Context_Control_fp _CPU_Null_fp_context CPU_STRUCTURE_ALIGNMENT;

   /**
    *  @brief CPU interrupt stack low address
    *
    *  This stack is allocated by the Interrupt Manager and the switch
    *  is performed in _ISR_Handler.  These variables contain pointers
    *  to the lowest and highest addresses in the chunk of memory allocated
    *  for the interrupt stack.  Since it is unknown whether the stack
    *  grows up or down (in general), this give the CPU dependent
    *  code the option of picking the version it wants to use.  Thus
    *  both must be present if either is.
    *
    *  The SPARC supports a software based interrupt stack and these
    *  are required.
    */
   extern void *_CPU_Interrupt_stack_low;

   /**
    *  @brief CPU interrupt stack high address
    *
    *  This stack is allocated by the Interrupt Manager and the switch
    *  is performed in _ISR_Handler.  These variables contain pointers
    *  to the lowest and highest addresses in the chunk of memory allocated
    *  for the interrupt stack.  Since it is unknown whether the stack
    *  grows up or down (in general), this give the CPU dependent
    *  code the option of picking the version it wants to use.  Thus
    *  both must be present if either is.
    *
    *  The SPARC supports a software based interrupt stack and these
    *  are required.
    **/
   extern void *_CPU_Interrupt_stack_high;

   /**
    *  @brief CPU ISR dispatch disable level
    *
    *  This flag is context switched with each thread.  It indicates
    *  that THIS thread has an _ISR_Dispatch stack frame on its stack.
    *  By using this flag, we can avoid nesting more interrupt dispatching
    *  attempts on a previously interrupted thread's stack.
    */
   extern volatile uint32_t _CPU_ISR_Dispatch_disable;

   /**
    *  @brief define an entry in the SPARC's trap table.
    *
    *  @note The instructions chosen are RTEMS dependent although one is
    *        obligated to use two of the four instructions to perform a
    *        long jump.  The other instructions load one register with the
    *        trap type (a.k.a. vector) and another with the psr.
    */
   typedef struct
   {
      /* mov   %psr, %l0 */
      uint32_t mov_psr_l0;

      /* sethi %hi(_handler), %l4 */
      uint32_t sethi_of_handler_to_l4;

      /* jmp   %l4 + %lo(_handler) */
      uint32_t jmp_to_low_of_handler_plus_l4;

      /* mov   _vector, %l3 */
      uint32_t mov_vector_l3;

   } CPU_Trap_table_entry;

   /**
    *  @brief determine the size of the floating point context area.
    */
#define CPU_CONTEXT_FP_SIZE sizeof( Context_Control_fp )

#endif

   /*
    *  Amount of extra stack (above minimum stack size) required by
    *  MPCI receive server thread.  Remember that in a multiprocessor
    *  system this thread must exist and be able to process all directives.
    */
#if defined(RTEMS_MULTIPROCESSING)
#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 1024
#endif

   /**
    *  @brief define the number of entries in the ISR_Vector_table managed
    *  by the executive.
    *
    *  On the SPARC, there are really only 256 vectors.  However, the executive
    *  has no easy, fast, reliable way to determine which traps are synchronous
    *  and which are asynchronous.  By default, synchronous traps return to the
    *  instruction which caused the interrupt.  So if you install a software
    *  trap handler as an executive interrupt handler (which is desirable since
    *  RTEMS takes care of window and register issues), then the executive needs
    *  to know that the return address is to the trap rather than the instruction
    *  following the trap.
    *
    *  So vectors 0 through 255 are treated as regular asynchronous traps which
    *  provide the "correct" return address.  Vectors 256 through 512 are assumed
    *  by the executive to be synchronous and to require that the return address
    *  be fudged.
    *
    *  If you use this mechanism to install a trap handler which must reexecute
    *  the instruction which caused the trap, then it should be installed as
    *  an asynchronous trap.  This will avoid the executive changing the return
    *  address.
    */
#define CPU_INTERRUPT_NUMBER_OF_VECTORS     256

   /**
    *  @brief determine the CPU maximum interrupt vector number
    **/
#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER 511

   /**
    *  @brief the SPARC synchronous trap bit mask
    **/
#define SPARC_SYNCHRONOUS_TRAP_BIT_MASK     0x100

   /**
    *  @brief determine the SPARC asynchronous trap number
    **/
#define SPARC_ASYNCHRONOUS_TRAP( _trap )    (_trap)

   /**
    *  @brief determine the SPARC synchronous trap number
    **/
#define SPARC_SYNCHRONOUS_TRAP( _trap )     ((_trap) + 256 )

   /**
    *  @brief determine the SPARC real trap number
    **/
#define SPARC_REAL_TRAP_NUMBER( _trap )     ((_trap) % 256)

   /**
    *  @brief determine if CPU provides ISR is in progress
    *
    *  This is defined if the port has a special way to report the ISR nesting
    *  level.  Most ports maintain the variable _ISR_Nest_level.
    */
#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE

   /**
    *  @brief minimum CPU stack size
    *
    *  Should be large enough to run all tests.  This ensures
    *  that a "reasonable" small application should not have any problems.
    *
    *  This appears to be a fairly generous number for the SPARC since
    *  represents a call depth of about 20 routines based on the minimum
    *  stack frame.
    */
#define CPU_STACK_MINIMUM_SIZE  (1024*4)

   /**
    *  @brief determine the CPU memory alignment
    *
    *  CPU's worst alignment requirement for data types on a byte boundary.  This
    *  alignment does not take into account the requirements for the stack.
    *
    *  On the SPARC, this is required for double word loads and stores.
    */
#define CPU_ALIGNMENT      8

   /**
    *  @brief CPU Heap alignment
    *
    *  This number corresponds to the byte alignment requirement for the
    *  heap handler.  This alignment requirement may be stricter than that
    *  for the data types alignment specified by CPU_ALIGNMENT.  It is
    *  common for the heap to follow the same alignment requirement as
    *  CPU_ALIGNMENT.  If the CPU_ALIGNMENT is strict enough for the heap,
    *  then this should be set to CPU_ALIGNMENT.
    *
    *  @note: This does not have to be a power of 2.  It does have to
    *         be greater or equal to than CPU_ALIGNMENT.
    */
#define CPU_HEAP_ALIGNMENT         CPU_ALIGNMENT


   /**
    *  @brief determine the CPU stack alignment
    *
    *  This number corresponds to the byte alignment requirement for the
    *  stack.  This alignment requirement may be stricter than that for the
    *  data types alignment specified by CPU_ALIGNMENT.  If the CPU_ALIGNMENT
    *  is strict enough for the stack, then this should be set to 0.
    *
    *  NOTE:  This must be a power of 2 either 0 or greater than CPU_ALIGNMENT.
    *
    *  The alignment restrictions for the SPARC are not that strict but this
    *  should unsure that the stack is always sufficiently alignment that the
    *  window overflow, underflow, and flush routines can use double word loads
    *  and stores.
    */
#define CPU_STACK_ALIGNMENT        16

#ifndef ASM


   /*
    *  ISR handler macros
    */

   /**
    *  @brief initialize the CPU vectors
    *
    *  Support routine to initialize the RTEMS vector table after it is allocated.
    */
#define _CPU_Initialize_vectors()

   /**
    *  @brief call the SPARC disable interrupts
    *
    *  Disable all interrupts for a critical section.  The previous
    *  level is returned in _level.
    */
#define _CPU_ISR_Disable( _level ) \
  (_level) = xky_sparc_disable_interrupts()

   /**
    *  @brief call the sparc enable interrupts
    *
    *  Enable interrupts to the previous level (returned by _CPU_ISR_Disable).
    *  This indicates the end of a critical section.  The parameter
    *  _level is not modified.
    */
#define _CPU_ISR_Enable( _level ) \
  xky_sparc_enable_interrupts( _level )

   /**
    *  @brief call the SPARC flash interrupts
    *
    *  This temporarily restores the interrupt to _level before immediately
    *  disabling them again.  This is used to divide long critical
    *  sections into two or more parts.  The parameter _level is not
    *  modified.
    */
#define _CPU_ISR_Flash( _level ) \
  sparc_flash_interrupts( _level )

   /**
    *  @brief set the CPU interrupt level
    *
    *  Map interrupt level in task mode onto the hardware that the CPU
    *  actually provides.  Currently, interrupt levels which do not
    *  map onto the CPU in a straight fashion are undefined.
    *  @return  returns the current interrupt level (PIL field of the PSR)
    */

#define _CPU_ISR_Set_level( _newlevel ) \
   xky_sparc_enable_interrupts( _newlevel )

   uint32_t _CPU_ISR_Get_level(void);

   /* end of ISR handler macros */

   /* Context handler macros */

   /**
    *  @brief initialize the basic non-FP context area associated
    *  with each thread.
    *
    *  @param[in] the_context pointer to the context area
    *  @param[in] stack_base address of memory for the SPARC
    *  @param[in] size size in bytes of the stack area
    *  @param[in] new_level interrupt level for this context area
    *  @param[in] entry_point the starting execution point for this this context
    *  @param[in] is_fp TRUE if this context is associated with an FP thread
    */
   void _CPU_Context_Initialize(
                                Context_Control *the_context ,
                                uint32_t *stack_base ,
                                uint32_t size ,
                                uint32_t new_level ,
                                void *entry_point ,
                                boolean is_fp
                                );

   /**
    *  @brief set the frame pointer to the _Thread_Handler function
    *
    *  This macro is invoked from _Thread_Handler to do whatever CPU
    *  specific magic is required that must be done in the context of
    *  the thread when it starts.
    *
    *  On the SPARC, this is setting the frame pointer so GDB is happy.
    *  Make GDB stop unwinding at _Thread_Handler, previous register window
    *  Frame pointer is 0 and calling address must be a function with starting
    *  with a SAVE instruction. If return address is leaf-function (no SAVE)
    *  GDB will not look at prev reg window fp.
    *
    *  _Thread_Handler is known to start with SAVE.
    */
#define _CPU_Context_Initialization_at_thread_begin() \
  do { \
    asm volatile ("set _Thread_Handler-4,%%i7\n"::); \
  } while (0)

   /**
    *  @brief restart the currently executing task
    *
    *  On the SPARC, this is is relatively painless but requires a small
    *  amount of wrapper code before using the regular restore code in
    *  of the context switch.
    */
#define _CPU_Context_Restart_self( _the_context ) \
    _CPU_Context_restore( (_the_context) );

   /**
    *  @brief find the FP start from the base and offset
    *
    * The FP context area for the SPARC is a simple structure and nothing
    *  special is required to find the "starting load point"
    */
#define _CPU_Context_Fp_start( _base, _offset ) \
   ( (void *) _Addresses_Add_offset( (_base), (_offset) ) )

   /**
    *  @brief initialize the FP context area passed to it to.
    *
    *  The SPARC allows us to use the simple initialization model
    *  in which an "initial" FP context was saved into _CPU_Null_fp_context
    *  at CPU initialization and it is simply copied into the destination
    *  context.
    */
#define _CPU_Context_Initialize_fp( _destination ) \
  do { \
   *(*(_destination)) = _CPU_Null_fp_context; \
  } while (0)

   /* end of Context handler macros */

   /* Bitfield handler macros */

   /*
    *  The SPARC port uses the generic C algorithm for bitfield scan if the
    *  CPU model does not have a scan instruction.
    */

#if ( SPARC_HAS_BITSCAN == 0 )

   /**
    *  @brief determine if SPARC use generic bitfielf code
    **/
#define CPU_USE_GENERIC_BITFIELD_CODE TRUE

   /**
    *  @brief determine if SPARC use generic bitfielf data
    **/
#define CPU_USE_GENERIC_BITFIELD_DATA TRUE

#else

#error "scan instruction not currently supported by RTEMS!!"

#endif


   /* end of Bitfield handler macros */

   /* Priority handler handler macros */

   /*
    *  The SPARC port uses the generic C algorithm for bitfield scan if the
    *  CPU model does not have a scan instruction.
    */

#if ( SPARC_HAS_BITSCAN == 1 )
#error "scan instruction not currently supported by RTEMS!!"
#endif

   /* end of Priority handler macros */



   /* functions */


   /**
    *  @brief initialize the CPU
    *
    *  This routine performs processor dependent initialization.
    *
    *  @param[in] cpu_table CPU table to initialize
    *  @param[in] thread_dispatch address of disptaching routine
    *
    *  @note There is no need to save the pointer to the thread dispatch routine.
    *  The SPARC's assembly code can reference it directly with no problems.
    */
   void _CPU_Initialize(
                        rtems_cpu_table *cpu_table ,
                        void (*thread_dispatch )
                        );


   /**
    *  @brief install an interrupt raw handler
    *
    *  This routine installs the specified handler as a "raw" non-executive
    *  supported trap handler (a.k.a. interrupt service routine).
    *  new_handler - address of the handler previously installed
    *
    *  On the SPARC, there are really only 256 vectors.  However, the executive
    *  has no easy, fast, reliable way to determine which traps are synchronous
    *  and which are asynchronous.  By default, synchronous traps return to the
    *  instruction which caused the interrupt.  So if you install a software
    *  trap handler as an executive interrupt handler (which is desirable since
    *  RTEMS takes care of window and register issues), then the executive needs
    *  to know that the return address is to the trap rather than the instruction
    *  following the trap.
    *  So vectors 0 through 255 are treated as regular asynchronous traps which
    *  provide the "correct" return address.  Vectors 256 through 512 are assumed
    *  by the executive to be synchronous and to require that the return address
    *  be fudged.
    *  If you use this mechanism to install a trap handler which must reexecute
    *  the instruction which caused the trap, then it should be installed as
    *  an asynchronous trap.  This will avoid the executive changing the return
    *  address.
    *
    *  @param[in] vector trap table entry number plus synchronous
    *  vs. asynchronous information
    *  @param[in] new_handler address of the handler to be installed
    *  @param[in] old_handler pointer to an address of the handler previously installed
    */
   void _CPU_ISR_install_raw_handler(
                                     uint32_t vector ,
                                     proc_ptr new_handler ,
                                     proc_ptr *old_handler
                                     );


   /**
    *  @brief install an interrupt routine on a vector
    *
    *  This kernel routine installs the RTEMS handler for the
    *  specified vector.
    *
    *  @param[in] vector interrupt vector number
    *  @param[in] new_handler replacement ISR for this vector number
    *  @param[in] old_handler pointer to former ISR for this vector number
    *  @param[out] old_handler former ISR for this vector number
    */
   void _CPU_ISR_install_vector(
                                uint32_t vector ,
                                proc_ptr new_handler ,
                                proc_ptr *old_handler
                                );

   /**
    *  @brief perform context switch (from a running context to a heir context)
    *
    *  This routine performs a normal non-FP context switch.
    *
    *  @param[out] run pointer to a structure where to save the current CPU
    *  context
    *  @param[in] heir pointer to a structure where to restore from the future
    *  CPU context
    *
    *  @note Implemented in assembly language
    */
   void _CPU_Context_switch(
                            Context_Control *run ,
                            Context_Control *heir
                            );

   /**
    *  @brief restore the CPU context
    *
    *  This routine is generally used only to restart self in an
    *  efficient manner.
    *
    *  @param[in] new_context CPU context to restore
    *
    *  @note It is unnecessary to reload some registers.
    *
    *  @note Implemented in assembly language
    */
   void _CPU_Context_restore(
                             Context_Control *new_context
                             );


   /**
    *  @brief save the floating point context
    *
    *  save the floating point context at *fp_context_ptr.  If the point to load
    *  the FP context from is changed then the pointer is modified by this routine.
    *
    *  @param[in] fp_context_ptr structure where to save the current FPU context
    *
    *  @note See the README in this directory for information on the
    *        management of the "EF" bit in the PSR.
    *
    *  @note Implemented in assembly language
    */
   void _CPU_Context_save_fp(
                             Context_Control_fp **fp_context_ptr
                             );


   /**
    *  @brief restore the floating point context
    *
    *  This routine is responsible for restoring the FP context
    *  at *fp_context_ptr.  If the point to load the FP context
    *  from is changed then the pointer is modified by this routine.
    *
    *  @param[out] fp_context_ptr structure where to restore from the future
    *  FPU context
    *
    *  @note See the README in this directory for information on the
    *        management of the "EF" bit in the PSR.
    *
    *  @note Implemented in assembly language
    */
   void _CPU_Context_restore_fp(
                                Context_Control_fp **fp_context_ptr
                                );

   /**
    *  @brief entry point to all RTEMS interrupts. performs necessary
    *  initialization, calls the application ISR (if any) and returns.
    *
    *  We enter this handler from the 4 instructions in the trap table with
    *  the following registers assumed to be set as shown:
    *
    *    l0 = PSR
    *    l1 = PC
    *    l2 = nPC
    *    l3 = trap type
    *
    *  This routine is the interrupt dispatcher.  ALL interrupts
    *  are vectored to this routine so that minimal context can be saved
    *  and setup performed before the application's high-level language
    *  interrupt service routine is invoked.   After the application's
    *  interrupt service routine returns control to this routine, it
    *  will determine if a thread dispatch is necessary.  If so, it will
    *  ensure that the necessary thread scheduling operations are
    *  performed when the outermost interrupt service routine exits.
    *
    *  @note By an executive defined convention, trap type is between 0 and 255 if
    *        it is an asynchonous trap and 256 and 511 if it is synchronous
    *
    *  @note Implemented in assembly language
    */
   void _ISR_Handler(void);

   /**
    *  @brief interrupt dispatcher. Invoked after all nested interrupts have been
    *  processed and dispatches the heir thread to be the next executing thread.
    *
    *  This routine provides a wrapper so that the routine
    *  @ref _Thread_Dispatch can be invoked when a reschedule is necessary
    *  at the end of the outermost interrupt service routine.  This
    *  wrapper is necessary to establish the processor context needed
    *  by _Thread_Dispatch and to save the processor context which is
    *  corrupted by _Thread_Dispatch.  This context typically consists
    *  of registers which are not preserved across routine invocations.
    *
    *  @note Implemented in assembly language
    */
   void _ISR_Dispatch(void);


   /**
    * @brief unsigned 32-bit multiplication
    *
    * @param[in] a first multipler
    * @param[in] b second multipler
    *
    * @return returns the multiplication of @a a by @a b
    *
    * @note Implemented in assembly language
    */
   uint32_t umul(uint32_t a , uint32_t b);


   /**
    * @brief swap 16 bits (8 lower with 8 higher bits)
    **/
#define CPU_swap_u16( value ) \
  (((value&0xff) << 8) | ((value >> 8)&0xff))

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif

/**  
 *  @}
 */

/**
 *  @}
 */
