/**
 *  @file
 *  confdefs.h
 *
 *  @brief configuration table template that will be instantiated by an
 *  application based on the setting of a number of macros.  The macros are
 *  documented in the Configuring a System chapter of the Classic API User's
 *  Guide
 *
 *  The model is to estimate the memory required for each configured item
 *  and sum those estimates.  The estimate can be too high or too low for
 *  a variety of reasons:
 *
 *  Reasons estimate is too high:
 *    + FP contexts (not all tasks are FP)
 *
 *  Reasons estimate is too low:
 *    + stacks greater than minimum size
 *    + messages
 *    + application must account for device driver resources
 *    + application must account for add-on library resource requirements
 *
 *  NOTE:  Eventually this may be able to take into account some of
 *         the above.  This procedure has evolved from just enough to
 *         support the RTEMS Test Suites into something that can be
 *         used remarkably reliably by most applications.
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
 *  234     | 09/10/2008  | mcoutinho    | IPR 66
 *  297     | 15/10/2008  | mcoutinho    | IPR 65
 *  297     | 15/10/2008  | mcoutinho    | IPR 67
 *  594     | 17/11/2008  | mcoutinho    | IPR 64
 *  655     | 19/11/2008  | mcoutinho    | IPR 69
 *  4266    | 07/09/2009  | mcoutinho    | IPR 153
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6514    | 08/03/2010  | mcoutinho    | IPR 1942
 *  7244    | 12/04/2010  | mcoutinho    | IPR 1910
 *  8936    | 22/09/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @defgroup SUPER_API Super API
 *
 *  @brief Super API is the another API provided by RTEMS
 *
 *  The application can interface with RTEMS through the Super API component.
 *  The application should use this interface (as well as possibly others) to
 *  communicate with RTEMS.
 *  @{
 */

/**
 *  @addtogroup SUPER_API_CONFIG Configuration
 *  @{
 */

#ifndef __CONFIGURATION_TEMPLATE_h
#define __CONFIGURATION_TEMPLATE_h

#include <rtems/score/cpu.h>
#include <rtems/score/isr.h>
#include <bsp.h>

#ifdef __cplusplus
extern "C"
{
#endif

   /**
    * @brief initialization tasks table
    */
   extern rtems_initialization_tasks_table Initialization_tasks[];

   /**
    * @brief device drivers table
    */
   extern rtems_driver_address_table Device_drivers[];

   /**
    * @brief RTEMS Configuration structure
    */
   extern rtems_configuration_table Configuration;


   /* if multiprocessing is defined */
#if defined(RTEMS_MULTIPROCESSING)

   /**
    * @brief
    * Multiprocessing configuration structure
    */
   extern rtems_multiprocessing_table Multiprocessing_configuration;

#endif /* RTEMS_MULTIPROCESSING */



   /**
    * The application default error handler
    **/

   /* if the application has its own error handler */
#ifdef CONFIGURE_HAS_OWN_APP_SAFE_STATE_HANDLER

   /* application has its own app_safe_state_handler function, nothing to define */

   /* else, lets define it */
#else

   /* if we should set the global symbols */
#ifdef CONFIGURE_INIT


   /* the default app_safe_state_handler always returns false, which means
    * that the system would enter an infinite loop */
   boolean app_safe_state_handler(
                                  Internal_errors_Source the_source ,
                                  boolean is_internal ,
                                  uint32_t the_error
                                  )
   {
      return TRUE; /* assume error is always handled and try to continue */
   }

#endif /* CONFIGURE_INIT */

#endif /* CONFIGURE_HAS_OWN_APP_SAFE_STATE_HANDLER */



   /*
    *  Interrupt Stack Space
    *
    *  NOTE: There is currently no way for the application to override
    *        the interrupt stack size set by the BSP.
    */

   /* if the CPU defined the interrupt stack to 0 */
#if (CPU_ALLOCATE_INTERRUPT_STACK == 0)

   /* then undefined the configure interrupt stack information */
#undef CONFIGURE_INTERRUPT_STACK_MEMORY

   /* and define it also to 0 */
#define CONFIGURE_INTERRUPT_STACK_MEMORY 0

   /* else */
#else

   /* if the application does not define the interrupt stack size */
#ifndef CONFIGURE_INTERRUPT_STACK_MEMORY

   /* then define it to the minimum size */
#define CONFIGURE_INTERRUPT_STACK_MEMORY RTEMS_MINIMUM_STACK_SIZE

#endif /* CONFIGURE_INTERRUPT_STACK_MEMORY */

#endif /* CPU_ALLOCATE_INTERRUPT_STACK */



   /*
    *  Default User Initialization Task Table.  This table guarantees that
    *  one user initialization table is defined.
    */

   /* if the application wants rtems to configure the init table */
#ifdef CONFIGURE_RTEMS_INIT_TASKS_TABLE

   /* if the application has its own initialization task table */
#ifdef CONFIGURE_HAS_OWN_INIT_TASK_TABLE

   /* The user is defining their own table information and setting the
    * appropriate variables */

   /* else */
#else


   /* if the application does not define the initialization task name */
#ifndef CONFIGURE_INIT_TASK_NAME

   /* then we define it to "UI1 " */
#define CONFIGURE_INIT_TASK_NAME          rtems_build_name( 'U', 'I', '1', ' ' )

#endif /* CONFIGURE_INIT_TASK_NAME */


   /* if the application does not define the initialization task stack size */
#ifndef CONFIGURE_INIT_TASK_STACK_SIZE

   /* then we define it to the minimum size */
#define CONFIGURE_INIT_TASK_STACK_SIZE    RTEMS_MINIMUM_STACK_SIZE

#endif /* CONFIGURE_INIT_TASK_STACK_SIZE */


   /* if the application does not define the initialization task priority */
#ifndef CONFIGURE_INIT_TASK_PRIORITY

   /* then we define it to 1 */
#define CONFIGURE_INIT_TASK_PRIORITY      1

#endif /* CONFIGURE_INIT_TASK_PRIORITY */

   /* if the application does not define the initialization task attributes */
#ifndef CONFIGURE_INIT_TASK_ATTRIBUTES

   /* then we define it to the default attributes */
#define CONFIGURE_INIT_TASK_ATTRIBUTES    RTEMS_DEFAULT_ATTRIBUTES

#endif /* CONFIGURE_INIT_TASK_ATTRIBUTES */


   /* if the application does not define the initialization task entry point */
#ifndef CONFIGURE_INIT_TASK_ENTRY_POINT

   /* then we define it to the Init function */
#define CONFIGURE_INIT_TASK_ENTRY_POINT   Init

#endif /* CONFIGURE_INIT_TASK_ENTRY_POINT */


   /* if the application does not define the initialization task initial mode */
#ifndef CONFIGURE_INIT_TASK_INITIAL_MODES

   /* then we define it to no-preempt */
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_NO_PREEMPT

#endif /* CONFIGURE_INIT_TASK_INITIAL_MODES */


   /* if the application does not define the initialization task argument */
#ifndef CONFIGURE_INIT_TASK_ARGUMENTS

   /* then we define it to 0 */
#define CONFIGURE_INIT_TASK_ARGUMENTS     0


#endif /* CONFIGURE_INIT_TASK_ARGUMENTS */



   /* if we should set the global symbols */
#ifdef CONFIGURE_INIT

   /* set the initialization task table to one task */
   rtems_initialization_tasks_table Initialization_tasks[] = {
      { CONFIGURE_INIT_TASK_NAME ,
       CONFIGURE_INIT_TASK_STACK_SIZE ,
       CONFIGURE_INIT_TASK_PRIORITY ,
       CONFIGURE_INIT_TASK_ATTRIBUTES ,
       CONFIGURE_INIT_TASK_ENTRY_POINT ,
       CONFIGURE_INIT_TASK_INITIAL_MODES ,
       CONFIGURE_INIT_TASK_ARGUMENTS }
   };
#endif /* CONFIGURE_INIT */

   /* set the initialization task table */
#define CONFIGURE_INIT_TASK_TABLE Initialization_tasks

   /* determine the number of initial tasks */
#define CONFIGURE_INIT_TASK_TABLE_SIZE \
  sizeof(CONFIGURE_INIT_TASK_TABLE) / sizeof(rtems_initialization_tasks_table)

#endif    /* CONFIGURE_HAS_OWN_INIT_TASK_TABLE */

#else     /* CONFIGURE_RTEMS_INIT_TASKS_TABLE */

   /* dont configure initial task table */
#define CONFIGURE_INIT_TASK_TABLE      NULL
#define CONFIGURE_INIT_TASK_TABLE_SIZE 0
#define CONFIGURE_INIT_TASK_STACK_SIZE 0

#endif /* CONFIGURE_RTEMS_INIT_TASKS_TABLE */




   /*
    *  Default Device Driver Table.  Each driver needed by the test is explicitly
    *  choosen by that test.  There is always a null driver entry.
    */

   /* NULL driver table entry */
#define NULL_DRIVER_TABLE_ENTRY \
 { NULL, NULL, NULL, NULL, NULL, NULL }


   /* if the application wants clock driver */
#ifdef CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

   /* then include the header file of the clock driver */
#include <rtems/clockdrv.h>

#endif /* CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER */

   /* if the application wants the stub driver */
#ifdef CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER

   /* then include the header file of the stub driver */
#include <rtems/devnull.h>

#endif /* CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER */



   /* if the application wants to define its own device driver table */
#ifndef CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE

   /* if we should set the global symbols */
#ifdef CONFIGURE_INIT

   /* set the device driver table */
   rtems_driver_address_table Device_drivers[] = {

                                                  /* if the application wants the clock driver */
#ifdef CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

                                                  /* then include the clock driver table entry */
                                                  CLOCK_DRIVER_TABLE_ENTRY ,
#endif /* CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER */

                                                  /* if the application wants the stub driver */
#ifdef CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER

                                                  /* then include the stub driver table entry */
                                                  DEVNULL_DRIVER_TABLE_ENTRY ,
#endif /* CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER */

                                                  /* if the application wants an extra driver */
#ifdef CONFIGURE_APPLICATION_EXTRA_DRIVERS

                                                  /* then it should define this MACRO as the driver table entry */
                                                  CONFIGURE_APPLICATION_EXTRA_DRIVERS ,
#endif /* CONFIGURE_APPLICATION_EXTRA_DRIVERS */

                                                  /* if the application wants the stub driver */
#ifdef CONFIGURE_APPLICATION_NEEDS_NULL_DRIVER

                                                  /* then add the stub driver */
                                                  NULL_DRIVER_TABLE_ENTRY

                                                  /* else, if the application did not define the clock, stub or extra drivers
       * then */
#elif !defined(CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER) && \
    !defined(CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER) && \
    !defined(CONFIGURE_APPLICATION_EXTRA_DRIVERS)

                                                  /* include a single device table entry of the null driver */
                                                  NULL_DRIVER_TABLE_ENTRY

#endif /* CONFIGURE_APPLICATION_NEEDS_NULL_DRIVER */
   };

#endif /* CONFIGURE_INIT */

#endif  /* CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE */

   /* Number of drivers per node */
#define CONFIGURE_NUMBER_OF_DRIVERS \
  ((sizeof(Device_drivers) / sizeof(rtems_driver_address_table)))

   /* if the application does not define the maximum number of device drivers */
#ifndef CONFIGURE_MAXIMUM_DRIVERS

   /* then define it to the number of drivers in the device table */
#define CONFIGURE_MAXIMUM_DRIVERS CONFIGURE_NUMBER_OF_DRIVERS

#endif


   /* Default Configuration Table */
#ifndef CONFIGURE_HAS_OWN_CONFIGURATION_TABLE

   /* if the application does not define the RAM work area */
#ifndef CONFIGURE_EXECUTIVE_RAM_WORK_AREA

   /* then dont define it (set to NULL) */
#define CONFIGURE_EXECUTIVE_RAM_WORK_AREA     NULL

#endif /* CONFIGURE_HAS_OWN_CONFIGURATION_TABLE */

   /* if the application does not define the maximum number of tasks */
#ifndef CONFIGURE_MAXIMUM_TASKS

   /* then define it to 0 */
#define CONFIGURE_MAXIMUM_TASKS               0

#endif /* CONFIGURE_MAXIMUM_TASKS */

   /* if the application does not define the maximum number of timers */
#ifndef CONFIGURE_MAXIMUM_TIMERS

   /* then define it to 0 */
#define CONFIGURE_MAXIMUM_TIMERS              0

#endif /* CONFIGURE_MAXIMUM_TIMERS */

   /* if the application does not define the maximum number of semaphores */
#ifndef CONFIGURE_MAXIMUM_SEMAPHORES

   /* then define it to 0 */
#define CONFIGURE_MAXIMUM_SEMAPHORES          0

#endif /* CONFIGURE_MAXIMUM_SEMAPHORES */

   /* if the application does not define the maximum number of message queues */
#ifndef CONFIGURE_MAXIMUM_MESSAGE_QUEUES

   /* then define it to 0 */
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES      0

#endif /* CONFIGURE_MAXIMUM_MESSAGE_QUEUES */

   /* if the application does not define the maximum number of rate monotonic periods */
#ifndef CONFIGURE_MAXIMUM_PERIODS

   /* then define it to 0 */
#define CONFIGURE_MAXIMUM_PERIODS             0

#endif /* CONFIGURE_MAXIMUM_PERIODS */


   /* if the application does not define the maximum number of user extensions */
#ifndef CONFIGURE_MAXIMUM_USER_EXTENSIONS

   /* then define it to 0 */
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS     0
#endif /* CONFIGURE_MAXIMUM_USER_EXTENSIONS */

   /* if the application does not define the number of microseconds per clock tick */
#ifndef CONFIGURE_MICROSECONDS_PER_TICK

   /* then define it to 10 ms */
#define CONFIGURE_MICROSECONDS_PER_TICK RTEMS_MILLISECONDS_TO_MICROSECONDS(10)

#endif /* CONFIGURE_MICROSECONDS_PER_TICK */

   /* if the application does not define the number of clock ticks per timeslice */
#ifndef CONFIGURE_TICKS_PER_TIMESLICE

   /* then define it to 50 clock ticks */
#define CONFIGURE_TICKS_PER_TIMESLICE        50

#endif /* CONFIGURE_TICKS_PER_TIMESLICE */




   /*
    *  Initial Extension Set
    */

   /* if we should set the global symbols */
#ifdef CONFIGURE_INIT

   /* if the applications wants to configure initial extensions */
#if defined(CONFIGURE_INITIAL_EXTENSIONS)

   /* then configure them */
   rtems_extensions_table Configuration_Initial_Extensions[] = {
                                                                /* set the application initial extensions */
                                                                CONFIGURE_INITIAL_EXTENSIONS
   };

   /* initial extension table */
#define CONFIGURE_INITIAL_EXTENSION_TABLE Configuration_Initial_Extensions

   /* determine the number of initial extensions */
#define CONFIGURE_NUMBER_OF_INITIAL_EXTENSIONS \
    ((sizeof(Configuration_Initial_Extensions) / \
      sizeof(rtems_extensions_table)))

#else /* CONFIGURE_INITIAL_EXTENSIONS */

   /* dont define the initial extension table */
#define CONFIGURE_INITIAL_EXTENSION_TABLE NULL

   /* define the number of initial extensions as 0 */
#define CONFIGURE_NUMBER_OF_INITIAL_EXTENSIONS 0

#endif /* CONFIGURE_INITIAL_EXTENSIONS */

#endif /* CONFIGURE_INIT */



   /*
    *  Calculate the RAM size based on the maximum number of objects configured.
    */

   /* if the application does not determine the RAM size */
#ifndef CONFIGURE_EXECUTIVE_RAM_SIZE

   /* then RTEMS calculates it */

   /* determine the common size for each object */
#define CONFIGURE_OBJECT_TABLE_STUFF \
  ( sizeof(Objects_Control *) + sizeof(rtems_name *) + sizeof(rtems_name) )

   /* determine the size for all tasks */
#define CONFIGURE_MEMORY_FOR_TASKS(_tasks) \
  (((_tasks) + 1 ) * \
   ((sizeof(Thread_Control) + CONTEXT_FP_SIZE + \
      STACK_MINIMUM_SIZE + sizeof( RTEMS_API_Control ) + \
      CONFIGURE_OBJECT_TABLE_STUFF)) \
  )

   /* determine the size for all timers */
#define CONFIGURE_MEMORY_FOR_TIMERS(_timers) \
  ((_timers) * ( sizeof(Timer_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )

   /* determine the size for all semaphores */
#define CONFIGURE_MEMORY_FOR_SEMAPHORES(_semaphores) \
  ((_semaphores) * \
   ( sizeof(Semaphore_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )

   /* determine the size for all message queues */
#define CONFIGURE_MEMORY_FOR_MESSAGE_QUEUES(_queues) \
  ( (_queues) * \
    ( sizeof(Message_queue_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )

  /* determine the size of the message queue */
#define CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(_messages, _size) \
  ((_messages) * ((_size) + sizeof(CORE_message_queue_Buffer_control)))

   /* determine the size for all rate monotonic periods */
#define CONFIGURE_MEMORY_FOR_PERIODS(_periods) \
  ( (_periods) * \
    ( sizeof(Rate_monotonic_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )

   /* determine the size for all user extensions */
#define CONFIGURE_MEMORY_FOR_USER_EXTENSIONS(_extensions) \
  ( (_extensions) * \
    ( sizeof(Extension_Control) + CONFIGURE_OBJECT_TABLE_STUFF ) )


   /* if the application is multiprocessing */
#ifdef CONFIGURE_MP_APPLICATION

   /* if the application does not have its own multiprocessing table */
#ifndef CONFIGURE_HAS_OWN_MULTIPROCESING_TABLE

   /* then RTEMS determines the extra memory for multiprocessing */

   /* determine the memory required for proxies */
#define CONFIGURE_MEMORY_FOR_PROXIES(_proxies) \
  ( ((_proxies) + 1) * ( sizeof(Thread_Proxy_control) )  )

   /* determine the memory required for global objects */
#define CONFIGURE_MEMORY_FOR_GLOBAL_OBJECTS(_global_objects) \
  ((_global_objects)  * ( sizeof(Objects_MP_Control) )  )

   /* determine the memory required for multiprocessing (sum of the previous) */
#define CONFIGURE_MEMORY_FOR_MP \
  ( CONFIGURE_MEMORY_FOR_PROXIES(CONFIGURE_MP_MAXIMUM_PROXIES) + \
    CONFIGURE_MEMORY_FOR_GLOBAL_OBJECTS(CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS) + \
    CONFIGURE_MEMORY_FOR_TASKS(1) \
  )

#endif  /* CONFIGURE_HAS_OWN_MULTIPROCESING_TABLE */

#else /* CONFIGURE_MP_APPLICATION */

   /* application is not multiprocessing, memory required is 0 */
#define CONFIGURE_MEMORY_FOR_MP  0

#endif /* CONFIGURE_MP_APPLICATION */


   /* Account for tasks with stacks greater than minimum size [byte] */

   /* if the application does not set extra memory for tasks stacks */
#ifndef CONFIGURE_EXTRA_TASK_STACKS

   /* then RTEMS also does not consider it */
#define CONFIGURE_EXTRA_TASK_STACKS 0

#endif /* CONFIGURE_EXTRA_TASK_STACKS */


   /* Account for pending message buffers [byte]  */

   /* if the application does not set extra memory message queues messages */
#ifndef CONFIGURE_MESSAGE_BUFFER_MEMORY

   /* then RTEMS also does not consider it */
#define CONFIGURE_MESSAGE_BUFFER_MEMORY 0

#endif /* CONFIGURE_MESSAGE_BUFFER_MEMORY */


   /* Catch all for extra memory [KiB] in case something broken and underestimates.
    * Historically this was used for message buffers */

   /* if the application does not set extra memory for other purposes */
#ifndef CONFIGURE_MEMORY_OVERHEAD

   /* then RTEMS also does not consider it */
#define CONFIGURE_MEMORY_OVERHEAD 0

#endif /* CONFIGURE_MEMORY_OVERHEAD */


   /* determine the memory overhead of the system */
#define CONFIGURE_MEMORY_FOR_SYSTEM_OVERHEAD                                         \
  ( CONFIGURE_MEMORY_FOR_TASKS(1) +                  /* IDLE */                      \
    ((PRIORITY_MAXIMUM+1) * sizeof(Chain_Control)) + /* Ready chains */              \
    256 +                                 /* name/ptr table overhead */              \
    CONFIGURE_INTERRUPT_STACK_MEMORY +    /* interrupt stack */                      \
    sizeof (ISR_Handler_entry) * ISR_NUMBER_OF_VECTORS /* ISR handler table entry */ \
   )

   /* Now account for any extra memory that initialization tasks or threads
    * may have requested */

   /* determine the extra memory for initialization tasks stack */
#define CONFIGURE_INITIALIZATION_THREADS_STACKS \
   ((CONFIGURE_INIT_TASK_STACK_SIZE - RTEMS_MINIMUM_STACK_SIZE))

   /* Determine the size of the RTEMS objects and resources in the RAM */
#define CONFIGURE_EXECUTIVE_RAM_SIZE                                                            \
(( CONFIGURE_MEMORY_FOR_TASKS(CONFIGURE_MAXIMUM_TASKS) + /* tasks */                            \
   CONFIGURE_INITIALIZATION_THREADS_STACKS +             /* init tasks stack */                 \
   CONFIGURE_MEMORY_FOR_TIMERS(CONFIGURE_MAXIMUM_TIMERS) + /* timers */                         \
   CONFIGURE_MEMORY_FOR_SEMAPHORES(CONFIGURE_MAXIMUM_SEMAPHORES) +  /* semaphores */            \
   CONFIGURE_MEMORY_FOR_MESSAGE_QUEUES(CONFIGURE_MAXIMUM_MESSAGE_QUEUES) + /* message queues */ \
   CONFIGURE_MEMORY_FOR_PERIODS(CONFIGURE_MAXIMUM_PERIODS) + /* rate monotonic */               \
   CONFIGURE_MEMORY_FOR_USER_EXTENSIONS(CONFIGURE_MAXIMUM_USER_EXTENSIONS) + /* extensions */   \
   CONFIGURE_MEMORY_FOR_MP + /* multiprocessing */                                              \
   CONFIGURE_MEMORY_FOR_SYSTEM_OVERHEAD + /* system overhead */                                 \
   CONFIGURE_MESSAGE_BUFFER_MEMORY + /* message queue messages */                               \
   ( CONFIGURE_MEMORY_OVERHEAD * 1024 ) + /* memory overhead */                                 \
   ( CONFIGURE_EXTRA_TASK_STACKS ) /* extra stacks */                                           \
 ) & ~0x7 ) /* align it */


#endif /* CONFIGURE_EXECUTIVE_RAM_SIZE */



   /* if we should set the global symbols */
#ifdef CONFIGURE_INIT

   /* define the RTEMS API */
   rtems_api_configuration_table Configuration_RTEMS_API = {
                                                            CONFIGURE_MAXIMUM_TASKS ,
                                                            CONFIGURE_MAXIMUM_TIMERS ,
                                                            CONFIGURE_MAXIMUM_SEMAPHORES ,
                                                            CONFIGURE_MAXIMUM_MESSAGE_QUEUES ,
                                                            CONFIGURE_MAXIMUM_PERIODS ,
                                                            CONFIGURE_INIT_TASK_TABLE_SIZE ,
                                                            CONFIGURE_INIT_TASK_TABLE
   };

   /* define the RTEMS configuration */
   rtems_configuration_table Configuration = {
                                              CONFIGURE_EXECUTIVE_RAM_WORK_AREA ,
                                              CONFIGURE_EXECUTIVE_RAM_SIZE ,
                                              CONFIGURE_MAXIMUM_USER_EXTENSIONS ,
                                              CONFIGURE_MICROSECONDS_PER_TICK ,
                                              CONFIGURE_TICKS_PER_TIMESLICE ,
                                              CONFIGURE_MAXIMUM_DRIVERS ,
                                              CONFIGURE_NUMBER_OF_DRIVERS , /* number of device drivers */
                                              Device_drivers , /* pointer to driver table */
                                              CONFIGURE_NUMBER_OF_INITIAL_EXTENSIONS , /* number of initial extensions */
                                              CONFIGURE_INITIAL_EXTENSION_TABLE , /* pointer to initial extensions */
#if defined(RTEMS_MULTIPROCESSING)
      CONFIGURE_MULTIPROCESSING_TABLE , /* pointer to MP config table */
#endif  
      &Configuration_RTEMS_API , /* pointer to RTEMS API config */
   };
#endif /* CONFIGURE_INIT */

#endif /* CONFIGURE_HAS_OWN_CONFIGURATION_TABLE */

   /* if we should set the global symbols */
#ifdef CONFIGURE_INIT

#ifdef CONFIGURE_RTEMS_INIT_TASKS_TABLE

   void (*_RTEMS_tasks_Initialize_user_tasks_p )(void) =
   _RTEMS_tasks_Initialize_user_tasks_body;

#else /* CONFIGURE_RTEMS_INIT_TASKS_TABLE */

   void (*_RTEMS_tasks_Initialize_user_tasks_p )(void) = NULL;

#endif /* CONFIGURE_RTEMS_INIT_TASKS_TABLE */

#endif /* CONFIGURE_INIT */


#ifdef __cplusplus
}
#endif



/*  Some warnings and error checking  */

/* Make sure a task/thread of some sort is configured */
#if (CONFIGURE_MAXIMUM_TASKS == 0)
#warning "CONFIGURATION WARNING: Using confdefs.h and the maximum number of tasks is 0"
#endif

/* Make sure at least one of the initialization task/thread tables was defined */
#if !defined(CONFIGURE_RTEMS_INIT_TASKS_TABLE)
#warning "CONFIGURATION WARNING: Using confdefs.h and the threads initialization table is not defined!"
#endif


/* Check the number of configured threads */
#if (CONFIGURE_MAXIMUM_TASKS > 64)
#warning "CONFIGURATION WARNING: Using confdefs.h and the maximum number of threads larger than 64. RTEMS has not been tested with so many threads!"
#endif

/* Check the number of configured message queues */
#if (CONFIGURE_MAXIMUM_MESSAGE_QUEUES > 256)
#warning "CONFIGURATION WARNING: Using confdefs.h and the maximum number of message queues is larger than 256. RTEMS has not been tested with so many message queues!"
#endif

/* Check the number of configured rate monotonic periods */
#if (CONFIGURE_MAXIMUM_PERIODS > 64)
#warning "CONFIGURATION WARNING: Using confdefs.h and the maximum number of rate monotonic periods is larger than 64. RTEMS has not been tested with so many rate monotonic periods!"
#endif

/* Check the number of configured semaphores */
#if (CONFIGURE_MAXIMUM_SEMAPHORES > 256)
#warning "CONFIGURATION WARNING: Using confdefs.h and the maximum number of semaphores is larger than 256. RTEMS has not been tested with so many semaphores!"
#endif

/* Check the number of configured timers */
#if (CONFIGURE_MAXIMUM_TIMERS > 64)
#warning "CONFIGURATION WARNING: Using confdefs.h and the maximum number of timers larger than 64. RTEMS has not been tested with so many timers!"
#endif

/* Check the number of configured user extensions */
#if (CONFIGURE_MAXIMUM_USER_EXTENSIONS > 16)
#warning "CONFIGURATION WARNING: Using confdefs.h and the maximum number of user extensions is larger than 16. RTEMS has not been tested with so many user extensions!"
#endif


#endif /* __CONFIGURATION_TEMPLATE_h */

/* end of include file */

/**  
 *  @}
 */

/**  
 *  @}
 */
