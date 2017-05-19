/**
 *  @file
 *  cpu.c
 *
 *  @brief common code to address the SPARC CPU. Contains functions to handle
 *  interrupts and CPU and FPU context
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  1880    | 13/03/2009  | mcoutinho    | IPR 43
 *  1903    | 16/03/2009  | mcoutinho    | IPR 58
 *  3603    | 02/07/2009  | mcoutinho    | IPR 97
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  7253    | 12/04/2010  | mcoutinho    | IPR 2266
 *  9293    | 03/01/2011  | mcoutinho    | IPR 2813
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

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/rtems/cache.h>
#include <libcpu/cache.h>
#include <rtems/score/cpu.h>
#include <bsp.h>
#include <xky.h>

Context_Control_fp _CPU_Null_fp_context CPU_STRUCTURE_ALIGNMENT;


void *_CPU_Interrupt_stack_low;


void *_CPU_Interrupt_stack_high;


volatile uint32_t _CPU_ISR_Dispatch_disable;


#define HIGH_BITS_MASK   0xFFFFFC00
#define HIGH_BITS_SHIFT  10
#define LOW_BITS_MASK    0x000003FF


/**
 * @brief trap table of the system
 */
extern void *trap_table[CPU_INTERRUPT_NUMBER_OF_VECTORS];

/**
 *  @brief This initializes the set of opcodes placed in each trap
 *  table entry.  The routine which installs a handler is responsible
 *  for filling in the fields for the _handler address and the _vector
 *  trap type.
 *
 *  The constants following this structure are masks for the fields which
 *  must be filled in when the handler is installed.
 */
const RTEMS_STATIC_ATTRIBUTE CPU_Trap_table_entry _CPU_Trap_slot_template = {
                                                                             0xa1480000 , /* mov   %psr, %l0           */
                                                                             0x29000000 , /* sethi %hi(_handler), %l4  */
                                                                             0x81c52000 , /* jmp   %l4 + %lo(_handler) */
                                                                             0xa6102000 /* mov   _vector, %l3        */
};

void _CPU_Initialize(
                     rtems_cpu_table *cpu_table ,
                     void (*thread_dispatch) /* ignored on this CPU */
                     )
{
#if (SPARC_HAS_FPU == 1)

    /*  check if the current partition have FPU permissions */
    xky_partition_status_t status;
    xky_syscall_get_partition_status(-1, &status);
    if ((status.permissions & XKY_PERMISSION_FPU_CONTROL) != 0) {

        /* auxiliary pointer to the FPU context */
        Context_Control_fp *pointer;

        /* initialize the pointer to the default floating point context area */

        /* this seems to be the most appropriate way to obtain an initial
         * FP context on the SPARC.  The NULL fp context is copied it to
         * the task's FP context during Context_Initialize */
        pointer = &_CPU_Null_fp_context;

        /* save the FP context */
        _CPU_Context_save_fp(&pointer);
    }
#endif

    /* since no tasks have been created yet and no interrupts have occurred,
     * there is no way that the currently executing thread can have an
     * _ISR_Dispatch stack frame on its stack */
    _CPU_ISR_Dispatch_disable = 0;
}


uint32_t _CPU_ISR_Get_level(void)
{
    /* interrupt level */
    uint32_t level;

    /* get the SPARC interrupt level */
    sparc_get_interrupt_level(level);

    /* return the interrupt level */
    return level;
}


void _CPU_ISR_install_raw_handler(
                                  uint32_t vector ,
                                  proc_ptr new_handler ,
                                  proc_ptr *old_handler
                                  )
{
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

    /* need to flush icache after installing an interrupt vector */
    rtems_cache_invalidate_entire_instruction();
#endif
}


void _CPU_ISR_install_vector(
                             uint32_t vector ,
                             proc_ptr new_handler ,
                             proc_ptr *old_handler
                             )
{
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
                             Context_Control *the_context ,
                             uint32_t *stack_base ,
                             uint32_t size ,
                             uint32_t new_level ,
                             void *entry_point ,
                             boolean is_fp
                             )
{
    /* highest "stack aligned" address */
    uint32_t stack_high;

    /* PSR value */
    uint32_t tmp_psr;

    /* On CPUs with stacks which grow down (i.e. SPARC), we build the stack
     * based on the stack_high address */

    /* add the to the stack address the size of the stack */
    stack_high = ((uint32_t) (stack_base) + size);

    /* then align it */
    stack_high &= ~(CPU_STACK_ALIGNMENT - 1);

    /* see the README in this directory for a diagram of the stack */

    /* set the entry point */
    the_context->o7 = ((uint32_t) entry_point) - 8;

    /* set the stack pointer */
    the_context->o6_sp = stack_high - CPU_MINIMUM_STACK_FRAME_SIZE;

    /* set the frame pointer */
    the_context->i6_fp = 0;

    /* build the PSR for the task.  Most everything can be 0 and the
     * CWP is corrected during the context switch.
     * the EF bit determines if the floating point unit is available.
     * the FPU is ONLY enabled if the context is associated with an FP task
     * and this SPARC model has an FPU */

    /* get the PSR register value */
    tmp_psr = xky_sparc_get_psr();

    /* reset the PSR[PIL] value */
    tmp_psr &= ~SPARC_PSR_PIL_MASK;

    /* and set the new PSR[PIL] value */
    tmp_psr |= (new_level << 8) & SPARC_PSR_PIL_MASK;

    /* disable the PSR[EF] value */
    tmp_psr &= ~SPARC_PSR_EF_MASK;

    /* if this version of SPARC has FPU */
#if (SPARC_HAS_FPU == 1)

    /* If the bit PSR[EF] is not set, then a task gets a fault when it accesses
     * a floating point register.  This is a nice way to detect floating
     * point tasks which are not currently declared as such */

    /* check if the task is floating point */
    if(is_fp)
    {
        /* if it is, tehn set the PSR[EF] to 1 */
        tmp_psr |= SPARC_PSR_EF_MASK;
    }

#endif

    /* set the task PSR value */
    the_context->psr = tmp_psr;

    /* since THIS thread is being created, there is no way that THIS
     * thread can have an _ISR_Dispatch stack frame on its stack,
     * so reset the ISR disparch disable to 0 */
    the_context->isr_dispatch_disable = 0;
}

/**  
 *  @}
 */

/**
 *  @}
 */
