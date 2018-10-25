#ifndef SYSTEM_H_
#define SYSTEM_H_

/* SpaceWire parameters */
#define SPW_PROT_ID 155

#include <rtems.h>

/* configuration information */
#define CONFIGURE_INIT
#define GMVNGMP 

#include <interface.h>

#include <bsp.h> /* for device driver prototypes */

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MAXIMUM_PROCESSORS   4 
#define CONFIGURE_MAXIMUM_PRIORITY 255

#define CONFIGURE_MAXIMUM_TASKS             10
#define CONFIGURE_MAXIMUM_SEMAPHORES        10
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES    40

#define SCHEDULER_1 rtems_build_name('W', 'O', 'R', 'K')

#define CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP
#include <rtems/scheduler.h>

//RTEMS_SCHEDULER_PRIORITY_AFFINITY_SMP(work, CONFIGURE_MAXIMUM_PRIORITY + 1);

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES RTEMS_SCHEDULER_TABLE_PRIORITY_AFFINITY_SMP(work, SCHEDULER_1 )

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY)  
  
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS    10
#define CONFIGURE_MAXIMUM_DRIVERS                   32
#define CONFIGURE_MAXIMUM_PERIODS                   10

#define CONFIGURE_STACK_CHECKER_ENABLED
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE


#ifdef MEMORY_ALLOCATION
    #define CONFIGURE_INIT_TASK_STACK_SIZE	(1024*1024)
#else
    #define CONFIGURE_INIT_TASK_STACK_SIZE	CONFIGURE_MINIMUM_TASK_STACK_SIZE * 2
#endif

#define CONFIGURE_INIT_TASK_PRIORITY    1
#define CONFIGURE_INIT_TASK_INITIAL_MODES (RTEMS_PREEMPT | \
                                           RTEMS_NO_TIMESLICE | \
                                           RTEMS_NO_ASR | \
                                           RTEMS_INTERRUPT_LEVEL(0))
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES | RTEMS_FLOATING_POINT

#ifdef MEMORY_ALLOCATION
    #define CONFIGURE_EXECUTIVE_RAM_SIZE (200*1024*1024)
#else
    #define CONFIGURE_EXECUTIVE_RAM_SIZE (2 * 2048 * 1024)//(1024*1024)//(2048*1024)
#endif

#ifdef MEMORY_ALLOCATION
    #define CONFIGURE_EXECUTIVE_RAM_SIZE (200*1024*1024)
#else
    #define CONFIGURE_EXECUTIVE_RAM_SIZE (2 * 2048 * 1024)//(1024*1024)//(2048*1024)
#endif

#define CONFIGURE_MAXIMUM_TIMERS                      10
#define CONFIGURE_MALLOC_STATISTICS
#define CONFIGURE_MICROSECONDS_PER_TICK 1000
#define RTEMS_POSIX_API

/* Configure Driver manager */
#if defined(RTEMS_DRVMGR_STARTUP) && defined(LEON3) /* if --drvmgr was given to configure */
 /* Add Timer and UART Driver for this example */
 #ifdef CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
  #define CONFIGURE_DRIVER_AMBAPP_GAISLER_GPTIMER
 #endif
 #ifdef CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
  #define CONFIGURE_DRIVER_AMBAPP_GAISLER_APBUART
 #endif
#endif

#define CONFIGURE_DRIVER_AMBAPP_GAISLER_SPW_ROUTER /* SpaceWire Router  */
#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GRSPW2     /* SpaceWire Packet driver */
#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GRETH

#ifdef INCLUDE_PCI_DRIVERS
/* Configure PCI Library to auto configuration. This can be substituted with
 * a static configuration by setting PCI_LIB_STATIC, see pci/. Static
 * configuration can be generated automatically by print routines in PCI
 * library.
 */
#define RTEMS_PCI_CONFIG_LIB
/*#define CONFIGURE_PCI_LIB PCI_LIB_STATIC*/
#define CONFIGURE_PCI_LIB PCI_LIB_AUTO

/*#define CONFIGURE_DRIVER_AMBAPP_GAISLER_PCIF*//* GRLIB PCIF Host driver  */
// #define CONFIGURE_DRIVER_AMBAPP_GAISLER_GRPCI   /* GRPCI Host driver */
// #define CONFIGURE_DRIVER_AMBAPP_GAISLER_GRPCI2  /* GRPCI2 Host Driver */
// #define CONFIGURE_DRIVER_PCI_GR_RASTA_IO        /* GR-RASTA-IO PCI Target Driver */
// #define CONFIGURE_DRIVER_PCI_GR_701             /* GR-701 PCI Target Driver */
// #define CONFIGURE_DRIVER_PCI_GR_RASTA_TMTC      /* GR-RASTA-TMTC PCI Target Driver */
#endif

/*******************************************/

#ifdef LEON2
  /* PCI support for AT697 */
  #define CONFIGURE_DRIVER_LEON2_AT697PCI
  /* AMBA PnP Support for GRLIB-LEON2 */
  #define CONFIGURE_DRIVER_LEON2_AMBAPP
#endif

#include <rtems/confdefs.h>
#include <drvmgr/drvmgr_confdefs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

/* This is used in config.c to set up networking */
#define ENABLE_NETWORK

#include "config.c"
  
#endif // SYSTEM_H_ 
