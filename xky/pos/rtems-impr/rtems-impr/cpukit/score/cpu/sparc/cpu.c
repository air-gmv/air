/**
 *  @file
 *
 *  @brief SPARC CPU Dependent Source
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2017 embedded brains GmbH
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/percpu.h>
#include <rtems/score/tls.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/cache.h>
#include <libcpu/cache.h>
#include <rtems/score/cpu.h>
#include <bsp.h>
#include <xky.h>

/**
 * @brief trap table of the system from AIR
 */
extern void *trap_table[CPU_INTERRUPT_NUMBER_OF_VECTORS];

#if SPARC_HAS_FPU == 1
  RTEMS_STATIC_ASSERT(
    offsetof( Per_CPU_Control, cpu_per_cpu.fsr)
      == SPARC_PER_CPU_FSR_OFFSET,
    SPARC_PER_CPU_FSR_OFFSET
  );

  #if defined(SPARC_USE_LAZY_FP_SWITCH)
    RTEMS_STATIC_ASSERT(
      offsetof( Per_CPU_Control, cpu_per_cpu.fp_owner)
        == SPARC_PER_CPU_FP_OWNER_OFFSET,
      SPARC_PER_CPU_FP_OWNER_OFFSET
    );
  #endif
#endif

#define SPARC_ASSERT_OFFSET(field, off) \
  RTEMS_STATIC_ASSERT( \
    offsetof(Context_Control, field) == off ## _OFFSET, \
    Context_Control_offset_ ## field \
  )

SPARC_ASSERT_OFFSET(g5, G5);
SPARC_ASSERT_OFFSET(g7, G7);

RTEMS_STATIC_ASSERT(
  offsetof(Context_Control, l0_and_l1) == L0_OFFSET,
  Context_Control_offset_L0
);

RTEMS_STATIC_ASSERT(
  offsetof(Context_Control, l0_and_l1) + 4 == L1_OFFSET,
  Context_Control_offset_L1
);

SPARC_ASSERT_OFFSET(l2, L2);
SPARC_ASSERT_OFFSET(l3, L3);
SPARC_ASSERT_OFFSET(l4, L4);
SPARC_ASSERT_OFFSET(l5, L5);
SPARC_ASSERT_OFFSET(l6, L6);
SPARC_ASSERT_OFFSET(l7, L7);
SPARC_ASSERT_OFFSET(i0, I0);
SPARC_ASSERT_OFFSET(i1, I1);
SPARC_ASSERT_OFFSET(i2, I2);
SPARC_ASSERT_OFFSET(i3, I3);
SPARC_ASSERT_OFFSET(i4, I4);
SPARC_ASSERT_OFFSET(i5, I5);
SPARC_ASSERT_OFFSET(i6_fp, I6_FP);
SPARC_ASSERT_OFFSET(i7, I7);
SPARC_ASSERT_OFFSET(o6_sp, O6_SP);
SPARC_ASSERT_OFFSET(o7, O7);
SPARC_ASSERT_OFFSET(psr, PSR);
SPARC_ASSERT_OFFSET(isr_dispatch_disable, ISR_DISPATCH_DISABLE_STACK);

#if defined(RTEMS_SMP)
SPARC_ASSERT_OFFSET(is_executing, SPARC_CONTEXT_CONTROL_IS_EXECUTING);
#endif

#define SPARC_ASSERT_ISF_OFFSET(field, off) \
  RTEMS_STATIC_ASSERT( \
    offsetof(CPU_Interrupt_frame, field) == ISF_ ## off ## _OFFSET, \
    CPU_Interrupt_frame_offset_ ## field \
  )

SPARC_ASSERT_ISF_OFFSET(psr, PSR);
SPARC_ASSERT_ISF_OFFSET(pc, PC);
SPARC_ASSERT_ISF_OFFSET(npc, NPC);
SPARC_ASSERT_ISF_OFFSET(g1, G1);
SPARC_ASSERT_ISF_OFFSET(g2, G2);
SPARC_ASSERT_ISF_OFFSET(g3, G3);
SPARC_ASSERT_ISF_OFFSET(g4, G4);
SPARC_ASSERT_ISF_OFFSET(g5, G5);
SPARC_ASSERT_ISF_OFFSET(g7, G7);
SPARC_ASSERT_ISF_OFFSET(i0, I0);
SPARC_ASSERT_ISF_OFFSET(i1, I1);
SPARC_ASSERT_ISF_OFFSET(i2, I2);
SPARC_ASSERT_ISF_OFFSET(i3, I3);
SPARC_ASSERT_ISF_OFFSET(i4, I4);
SPARC_ASSERT_ISF_OFFSET(i5, I5);
SPARC_ASSERT_ISF_OFFSET(i6_fp, I6_FP);
SPARC_ASSERT_ISF_OFFSET(i7, I7);
SPARC_ASSERT_ISF_OFFSET(y, Y);
SPARC_ASSERT_ISF_OFFSET(tpc, TPC);

#define SPARC_ASSERT_FP_OFFSET(field, off) \
  RTEMS_STATIC_ASSERT( \
    offsetof(Context_Control_fp, field) == SPARC_FP_CONTEXT_OFFSET_ ## off, \
    Context_Control_fp_offset_ ## field \
  )

SPARC_ASSERT_FP_OFFSET(f0_f1, F0_F1);
SPARC_ASSERT_FP_OFFSET(f2_f3, F2_F3);
SPARC_ASSERT_FP_OFFSET(f4_f5, F4_F5);
SPARC_ASSERT_FP_OFFSET(f6_f7, F6_F7);
SPARC_ASSERT_FP_OFFSET(f8_f9, F8_F9);
SPARC_ASSERT_FP_OFFSET(f10_f11, F10_F11);
SPARC_ASSERT_FP_OFFSET(f12_f13, F12_F13);
SPARC_ASSERT_FP_OFFSET(f14_f15, F14_F15);
SPARC_ASSERT_FP_OFFSET(f16_f17, F16_F17);
SPARC_ASSERT_FP_OFFSET(f18_f19, F18_F19);
SPARC_ASSERT_FP_OFFSET(f20_f21, F20_F21);
SPARC_ASSERT_FP_OFFSET(f22_f23, F22_F23);
SPARC_ASSERT_FP_OFFSET(f24_f25, F24_F25);
SPARC_ASSERT_FP_OFFSET(f26_f27, F26_F27);
SPARC_ASSERT_FP_OFFSET(f28_f29, F28_F29);
SPARC_ASSERT_FP_OFFSET(f30_f31, F30_F31);
SPARC_ASSERT_FP_OFFSET(fsr, FSR);

RTEMS_STATIC_ASSERT(
  sizeof(SPARC_Minimum_stack_frame) == SPARC_MINIMUM_STACK_FRAME_SIZE,
  SPARC_MINIMUM_STACK_FRAME_SIZE
);

/* https://devel.rtems.org/ticket/2352 */
RTEMS_STATIC_ASSERT(
  sizeof(CPU_Interrupt_frame) % CPU_ALIGNMENT == 0,
  CPU_Interrupt_frame_alignment
);

/*
 *  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS: NONE
 *
 *  Output Parameters: NONE
 *
 *  NOTE: There is no need to save the pointer to the thread dispatch routine.
 *        The SPARC's assembly code can reference it directly with no problems.
 */

void _CPU_Initialize(void)
{
#if defined(SPARC_USE_LAZY_FP_SWITCH)
  /* @brief AIR checks check if the current partition have FPU permissions */
  xky_partition_status_t status;
  xky_syscall_get_partition_status(-1, &status);
  if ((status.permissions & XKY_PERMISSION_FPU_CONTROL) != 0) {        
    __asm__ volatile (
      ".global SPARC_THREAD_CONTROL_REGISTERS_FP_CONTEXT_OFFSET\n"
      ".set SPARC_THREAD_CONTROL_REGISTERS_FP_CONTEXT_OFFSET, %0\n"
      ".global SPARC_THREAD_CONTROL_FP_CONTEXT_OFFSET\n"
      ".set SPARC_THREAD_CONTROL_FP_CONTEXT_OFFSET, %1\n"
      :
      : "i" (offsetof(Thread_Control, Registers.fp_context)),
        "i" (offsetof(Thread_Control, fp_context))
    );
  }
#endif
}

uint32_t   _CPU_ISR_Get_level( void )
{
  uint32_t   level;

  sparc_get_interrupt_level( level );

  return level;
}

/*
 *  _CPU_ISR_install_raw_handler
 *
 *  This routine installs the specified handler as a "raw" non-executive
 *  supported trap handler (a.k.a. interrupt service routine).
 *
 *  Input Parameters:
 *    vector      - trap table entry number plus synchronous
 *                    vs. asynchronous information
 *    new_handler - address of the handler to be installed
 *    old_handler - pointer to an address of the handler previously installed
 *
 *  Output Parameters: NONE
 *    *new_handler - address of the handler previously installed
 *
 *  NOTE:
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

void _CPU_ISR_install_raw_handler(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{

  /* @brief AIR fully replace the function with specific version as seen below.
   * What is significant it that we use AIR *trap_table* instead of 
   * RTEMS tbr to set handler. */

  /* vector number */
  uint32_t real_vector;

  /* interrupt level */
  rtems_interrupt_level level;

  /* get the "real" trap number for this vector ignoring the synchronous
   * versus asynchronous indicator included with our vector numbers */
  real_vector = SPARC_REAL_TRAP_NUMBER(vector);

  /* enter critical section to protect the _ISR_Vector_table variable */
  rtems_interrupt_disable(level);

  /* get the old handler */
  *old_handler = trap_table[real_vector];

  /* apply the new handler */
  trap_table[real_vector] = new_handler;

  /* leave critical section */
  rtems_interrupt_enable(level);

  /* if the CPU has instruction cache */
#if (HAS_INSTRUCTION_CACHE == TRUE)


  /*
   * There is no instruction cache snooping, so we need to invalidate
   * the instruction cache to make sure that the processor sees the
   * changes to the trap table. This step is required on both single-
   * and multiprocessor systems.
   *
   * In a SMP configuration a change to the trap table might be
   * missed by other cores. If the system state is up, the other
   * cores can be notified using SMP messages that they need to
   * flush their icache. If the up state has not been reached
   * there is no need to notify other cores. They will do an
   * automatic flush of the icache just after entering the up
   * state, but before enabling interrupts.
   */
  rtems_cache_invalidate_entire_instruction();
#endif
  
}

void _CPU_ISR_install_vector(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  /* @brief AIR fully replace the function with specific version as seen below.
   * What is significant it that we use AIR *trap_table* instead of 
   * RTEMS tbr to set handler. */
    /* vector number */
    uint32_t real_vector;

    /* auxiliary pointer */
    proc_ptr ignored;

    /* interrupt level */
    rtems_interrupt_level level;

    /* get the "real" trap number for this vector ignoring the synchronous
     * versus asynchronous indicator included with our vector numbers */
    real_vector = SPARC_REAL_TRAP_NUMBER(vector);

    /* enter critical section to protect the _ISR_Vector_table variable */
    rtems_interrupt_disable(level);

    /* get the old handler from the internal RTEMS table */
    *old_handler = _ISR_Vector_table[ real_vector ];

    /* leave critical section */
    rtems_interrupt_enable(level);

    /* install the wrapper so this ISR can be invoked properly */
    _CPU_ISR_install_raw_handler(vector , _ISR_Handler , &ignored);

    /* enter critical section to protect the _ISR_Vector_table variable */
    rtems_interrupt_disable(level);

    /* we put the actual user ISR address in '_ISR_vector_table'. This will
     * be used by the _ISR_Handler so the user gets control */
    _ISR_Vector_table[ real_vector ] = new_handler;

    /* leave critical section */
    rtems_interrupt_enable(level);

}

void _CPU_Context_Initialize(
  Context_Control  *the_context,
  uint32_t         *stack_base,
  uint32_t          size,
  uint32_t          new_level,
  void             *entry_point,
  bool              is_fp,
  void             *tls_area
)
{
    uint32_t     stack_high;  /* highest "stack aligned" address */
    uint32_t     tmp_psr;

    /*
     *  On CPUs with stacks which grow down (i.e. SPARC), we build the stack
     *  based on the stack_high address.
     */

    stack_high = ((uint32_t)(stack_base) + size);
    stack_high &= ~(CPU_STACK_ALIGNMENT - 1);

    /*
     *  See the README in this directory for a diagram of the stack.
     */

    the_context->o7    = ((uint32_t) entry_point) - 8;
    the_context->o6_sp = stack_high - SPARC_MINIMUM_STACK_FRAME_SIZE;
    the_context->i6_fp = 0;

    /*
     *  Build the PSR for the task.  Most everything can be 0 and the
     *  CWP is corrected during the context switch.
     *
     *  The EF bit determines if the floating point unit is available.
     *  The FPU is ONLY enabled if the context is associated with an FP task
     *  and this SPARC model has an FPU.
     */

    tmp_psr = xky_sparc_get_psr();
    tmp_psr &= ~SPARC_PSR_PIL_MASK;
    tmp_psr |= (new_level << 8) & SPARC_PSR_PIL_MASK;
    tmp_psr &= ~SPARC_PSR_EF_MASK;      /* disabled by default */

    /* _CPU_Context_restore_heir() relies on this */
    _Assert( ( tmp_psr & SPARC_PSR_ET_MASK ) != 0 );

#if (SPARC_HAS_FPU == 1)
    /*
     *  If this bit is not set, then a task gets a fault when it accesses
     *  a floating point register.  This is a nice way to detect floating
     *  point tasks which are not currently declared as such.
     */

    if ( is_fp )
      tmp_psr |= SPARC_PSR_EF_MASK;
#endif
    the_context->psr = tmp_psr;

  /*
   *  Since THIS thread is being created, there is no way that THIS
   *  thread can have an _ISR_Dispatch stack frame on its stack.
   */
    the_context->isr_dispatch_disable = 0;

  if ( tls_area != NULL ) {
    void *tcb = _TLS_TCB_after_TLS_block_initialize( tls_area );

    the_context->g7 = (uintptr_t) tcb;
  }
}
