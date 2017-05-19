/**
 *  @file
 *  system.h
 *
 *  @brief contains information that is included in every
 *  function in the executive.  This must be the first include file
 *  included in all internal RTEMS files.
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
 *  606     | 17/11/2008  | mcoutinho    | IPR 65
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6116    | 18/02/2010  | mcoutinho    | IPR 1818
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  6512    | 08/03/2010  | mcoutinho    | IPR 1942
 *  7253    | 12/04/2010  | mcoutinho    | IPR 2266
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @defgroup SUPER_CORE Super Core

 *  @brief Super Core component contains the kernel of RTEMS
 *
 *  Other RTEMS components interface with the Super Core to perform several
 *  functionalities. It contains the core of the RTEMS functionality
 *
 *  @{
 */

#ifndef _RTEMS_SYSTEM_H
#define _RTEMS_SYSTEM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/score/types.h>
#include <stddef.h>
#include <time.h>
#include <stdint.h>

   /**
    *  @brief boolean value
    **/
   typedef uint32_t boolean;

   /**
    *  @brief single precision float
    **/
   typedef float single_precision;

   /**
    *  @brief double precision float
    **/
   typedef double double_precision;
   
   /**
    * if the tests are enabled then all functions and global variables cannot
    * be declared as static or the functions as inline
    **/
#ifdef RTEMS_TEST_ENABLED
#define RTEMS_STATIC_ATTRIBUTE
#define RTEMS_INLINE_ATTRIBUTE

#else

   /* define the static attribute */
#define RTEMS_STATIC_ATTRIBUTE static

   /**
    *  define the inline attribute
    *  The following (in conjunction with compiler arguments) are used
    *  to choose between the use of inline functions and macro
    *  functions.   The inline implementation allows better
    *  type checking with no cost in code size or execution speed.
    */
#ifdef __GNUC__
#define RTEMS_INLINE_ATTRIBUTE inline
#else
#define RTEMS_INLINE_ATTRIBUTE __inline__
#endif

#endif /* RTEMS_TEST_ENABLED */

   /**
    * @brief define a RTEMS inline function
    */
#define RTEMS_INLINE_ROUTINE RTEMS_STATIC_ATTRIBUTE RTEMS_INLINE_ATTRIBUTE

   /**
    *  @brief ensure that the compiler writes memory in correct order (does not
    *  change the order of certain operations)
    *
    *  The following macro is a compiler specific way to ensure that memory
    *  writes are not reordered around certian points.  This specifically can
    *  impact interrupt disable and thread dispatching critical sections.
    */
#ifdef __GNUC__
#define RTEMS_COMPILER_MEMORY_BARRIER() asm volatile("" ::: "memory")
#else
#define RTEMS_COMPILER_MEMORY_BARRIER()
#endif

   /*
    *  Include a base set of files.
    */

   /**
    * @brief pointer to a ISR handler
    *
    * @note eventually proc_ptr needs to disappear
    */
   typedef void * proc_ptr;


#include <rtems/score/cpu.h>


   /**
    * @brief determine the boolean constant TRUE
    */
#define TRUE     (1)


   /**
    * @brief determine the boolean constant FALSE
    */
#define FALSE     (0)


#include <rtems/score/cpu.h>        /* processor specific information */

   /**
    *  @brief obtain the offset of a field in a structure.
    */
#define RTEMS_offsetof(type, field) \
       ((uint32_t  ) &(((type *) 0)->field))

   /**
    *  @brief CPU dependent information table.
    */
   extern rtems_cpu_table _CPU_Table;

   /*
    *  Macros to access CPU Table fields required by ALL ports.
    */

   /**
    * @brief get CPU Specific Configuration Table
    */
#define rtems_cpu_configuration_get_table() (&_CPU_Table)


   /**
    * @brief get CPU pretasking hook
    */
#define rtems_cpu_configuration_get_pretasking_hook() \
   (_CPU_Table.pretasking_hook)

   /**
    * @brief get CPU predriver hook
    */
#define rtems_cpu_configuration_get_predriver_hook() \
   (_CPU_Table.predriver_hook)

   /**
    * @brief get CPU postdriver hook
    */
#define rtems_cpu_configuration_get_postdriver_hook() \
   (_CPU_Table.postdriver_hook)

   /**
    * @brief get CPU specific IDLE task entry point
    */
#define rtems_cpu_configuration_get_idle_task() \
   (_CPU_Table.idle_task)

   /**
    * @brief determine if RTEMS should do zeroing the Executive Workspace.
    */
#define rtems_cpu_configuration_get_do_zero_of_workspace() \
   (_CPU_Table.do_zero_of_workspace)

   /**
    * @brief get idle task stack size
    */
#define rtems_cpu_configuration_get_idle_task_stack_size() \
   (_CPU_Table.idle_task_stack_size)

   /**
    * @brief get interrupt stack size
    */
#define rtems_cpu_configuration_get_interrupt_stack_size() \
   (_CPU_Table.interrupt_stack_size)

#if defined(RTEMS_MULTIPROCESSING)
   /**
    * @brief get MPCI receiver server stack
    */
#define rtems_cpu_configuration_get_extra_mpci_receive_server_stack() \
   (_CPU_Table.extra_mpci_receive_server_stack)
#endif

   /**
    * @brief get CPU stack allocation hook
    */
#define rtems_cpu_configuration_get_stack_allocate_hook() \
   (_CPU_Table.stack_allocate_hook)

   /**
    * @brief get CPUstack allocation free hook
    */
#define rtems_cpu_configuration_get_stack_free_hook() \
   (_CPU_Table.stack_free_hook)

   /**
    * @brief get maximum length of a Classic API name
    */
#define RTEMS_MAXIMUM_NAME_LENGTH sizeof(Objects_Name)

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

/**
 *  @}
 */
