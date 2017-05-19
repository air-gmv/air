/**
 *  @file
 *  exinit.c
 *
 *  @brief initialize the RTEMS Super Core and API
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  595     | 17/11/2008  | mcoutinho    | IPR 64
 *  595     | 17/11/2008  | mcoutinho    | IPR 65
 *  4650    | 02/10/2009  | mcoutinho    | IPR 153
 *  5247    | 30/10/2009  | mcoutinho    | IPR 828
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  7128    | 09/04/2010  | mcoutinho    | IPR 1931
 *  7253    | 12/04/2010  | mcoutinho    | IPR 2266
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_API Super API
 *  @{
 */

/**
 *  @addtogroup SUPER_API_INITIALIZATION Initialization Manager
 *  @{
 */

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/extension.h>
#include <rtems/fatal.h>
#include <rtems/init.h>
#include <rtems/io.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/apiext.h>
#include <rtems/score/heap.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/priority.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>
#include <rtems/score/userext.h>
#include <rtems/score/watchdog.h>
#include <rtems/score/wkspace.h>
#include <rtems/sptables.h>
#include <rtems/rtems/rtemsapi.h>

/**
 * @brief internal objects information
 **/
RTEMS_STATIC_ATTRIBUTE Objects_Information *_Internal_Objects[ OBJECTS_INTERNAL_CLASSES_LAST + 1 ];


const unsigned char __log2table[256] = {
                                        7 , 7 , 6 , 6 , 5 , 5 , 5 , 5 , 4 , 4 , 4 , 4 , 4 , 4 , 4 , 4 ,
                                        3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 ,
                                        2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,
                                        2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,
                                        1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
                                        1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
                                        1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
                                        1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
                                        0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
                                        0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
                                        0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
                                        0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
                                        0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
                                        0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
                                        0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
                                        0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
};

#ifdef RTEMS_MULTIPROCESSING
const rtems_multiprocessing_table
_Initialization_Default_multiprocessing_table = {
                                                 1 , /* local node number */
                                                 1 , /* maximum number nodes in system */
                                                 0 , /* maximum number global objects */
                                                 0 , /* maximum number proxies */
                                                 NULL , /* pointer to MPCI address table */
};
#endif


volatile Priority_Bit_map_control _Priority_Major_bit_map;


Priority_Bit_map_control _Priority_Bit_map[16] CPU_STRUCTURE_ALIGNMENT;


rtems_cpu_table _CPU_Table;


rtems_configuration_table *_Configuration_Table;


void rtems_check_configuration_parameters(rtems_configuration_table *configuration_table)
{
    /*
     * Check the configuration values
     */

    /* check the configuration table */
    if(configuration_table == NULL)
    {
        /* raise internal error
         *   internal error core
         *   internal
         *   no configuration table */
        _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                 TRUE ,
                                 INTERNAL_ERROR_NO_CONFIGURATION_TABLE);
    }

    /* check the number of maximum extensions verus the number of initial extensions */
    if(configuration_table->maximum_extensions < configuration_table->number_of_initial_extensions)
    {
        /* raise internal error
         *   internal error core
         *   internal
         *   too many initial extensions */
        _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                 TRUE ,
                                 INTERNAL_ERROR_TOO_MANY_INITIAL_EXTENSIONS);
    }

    /* check the number of microseconds per clock tick */
    if(configuration_table->microseconds_per_tick == 0)
    {
        /* raise internal error
         *   internal error core
         *   internal
         *   invalid number of microseconds per clock tick */
        _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                 TRUE ,
                                 INTERNAL_ERROR_INVALID_MICROSECONDS_PER_TICK);
    }

    /* check the number of ticks per timeslice */
    if(configuration_table->ticks_per_timeslice == 0)
    {
        /* raise internal error
         *   internal error core
         *   internal
         *   invalid number of clock ticks per timeslice */
        _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                 TRUE ,
                                 INTERNAL_ERROR_INVALID_TICKS_PER_TIMESLICE);
    }

    /* check the number of maximum drivers versus the number of device drivers defined */
    if(configuration_table->maximum_drivers < configuration_table->number_of_device_drivers)
    {
        /* raise internal error
         *   internal error core
         *   internal
         *   too many device drivers */
        _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                 TRUE ,
                                 INTERNAL_ERROR_TOO_MANY_DEVICE_DRIVERS);
    }

    /* check the number of device drivers versus the device driver table */
    if(configuration_table->number_of_device_drivers != 0 &&
       configuration_table->Device_driver_table == NULL)
    {
        /* raise internal error
         *   internal error core
         *   internal
         *   no device driver table */
        _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                 TRUE ,
                                 INTERNAL_ERROR_NO_DEVICE_DRIVER_TABLE);
    }

    /* check the number of initial extensions versus the extension table */
    if(configuration_table->number_of_initial_extensions != 0 &&
       configuration_table->User_extension_table == NULL)
    {
        /* raise internal error
         *   internal error core
         *   internal
         *   no user extension table */
        _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                 TRUE ,
                                 INTERNAL_ERROR_NO_USER_EXTENSION_TABLE);
    }
    /* check if the RTEMS API configuration exists */
    if(configuration_table->RTEMS_api_configuration == NULL)
    {
        /* raise internal error
         *   internal error core
         *   internal
         *   no API configuration table */
        _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                 TRUE ,
                                 INTERNAL_ERROR_NO_API_CONFIGURATION_TABLE);
    }

    /* check the maximum number of tasks */
    if(configuration_table->RTEMS_api_configuration->maximum_tasks == 0)
    {
        /* raise internal error
         *   internal error core
         *   internal
         *   invalid number of maximum tasks */
        _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                 TRUE ,
                                 INTERNAL_ERROR_INVALID_NUMBER_TASKS);
    }

    /* check the number of initialization tasks */
    if(configuration_table->RTEMS_api_configuration->number_of_initialization_tasks == 0)
    {
        /* raise internal error
         *   internal error core
         *   internal
         *   invalid number of initialization tasks */
        _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                 TRUE ,
                                 INTERNAL_ERROR_INVALID_NUMBER_INITIALIZATION_TASKS);
    }

    /* check the initializations task table */
    if(configuration_table->RTEMS_api_configuration->User_initialization_tasks_table == NULL)
    {
        /* raise internal error
         *   internal error core
         *   internal
         *   no initialization task table */
        _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                 TRUE ,
                                 INTERNAL_ERROR_NO_USER_INITIALIZATION_TASKS_TABLE);
    }
}


rtems_interrupt_level rtems_initialize_executive_early(
                                                       rtems_configuration_table *configuration_table ,
                                                       rtems_cpu_table * cpu_table
                                                       )
{
    /* interrupt level */
    rtems_interrupt_level bsp_level;

#if defined(RTEMS_MULTIPROCESSING)
    /* multiprocessing table */
    rtems_multiprocessing_table *multiprocessing_table;
#endif

#if defined(RTEMS_MULTIPROCESSING)
    /* Initialize the system state based on whether this is an MP system. */

    /* get the multiprocessing table */
    multiprocessing_table = configuration_table->User_multiprocessing_table;

    /* if there is no multiprocessing table */
    if(multiprocessing_table == NULL)
    {
        /* then initialize the system state with no multiprocessing */
        _System_state_Handler_initialization(TRUE);
    }
    else
    {
        /* else, initialize the system state with multiprocessing */
        _System_state_Handler_initialization(FALSE);
    }
#else
    /* initialize the system state handler with no multiprocessing */
    _System_state_Handler_initialization();
#endif

    /* dispatching and interrupts are disabled until the end of the
     * initialization sequence.  This prevents an inadvertent context
     * switch before the executive is initialized */
    _ISR_Disable(bsp_level);

    /* check if the cpu_table address is valid */
    if(cpu_table == NULL)
    {
        /* raise internal error */
        _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                 TRUE ,
                                 INTERNAL_ERROR_NO_CPU_TABLE);
    }

    /* grab our own copy of the user's CPU table */
    _CPU_Table = *cpu_table;

    /* provided just for user convenience */
    _Configuration_Table = configuration_table;

#if defined(RTEMS_MULTIPROCESSING)

    /* get a copy of the multiprocessing table */
    _Configuration_MP_table = multiprocessing_table;
#endif


#if defined(RTEMS_MULTIPROCESSING)

    /* Internally we view single processor systems as a very restricted
     * multiprocessor system */

    /* if the multiprocessing table does not exist (this should never be true) */
    if(multiprocessing_table == NULL)
    {
        /* set the default multiprocessing table */
        multiprocessing_table = (void *) & _Initialization_Default_multiprocessing_table;
    }

#endif

    /* initialize the CPU */
    _CPU_Initialize(cpu_table , _Thread_Dispatch);

    /* initialize the API extensions */
    _API_extensions_Initialization();

    /* initialize the thread dispatch */
    _Thread_Dispatch_initialization();

    /* initialize the workspace */
    _Workspace_Handler_initialization((void *) configuration_table->work_space_start ,
                                      configuration_table->work_space_size);

    /* initialize the user extension handler */
    _User_extensions_Handler_initialization(configuration_table->number_of_initial_extensions ,
                                            configuration_table->User_extension_table);

    /* initialize the ISR Handler */
    _ISR_Handler_initialization();

    /* initialize the Object Handler 
     *  if multiprocessing is enabled, then
     *    pass the maximum number of nodes
     *    and the maximum number of global objects */
    _Objects_Handler_initialization(
#if defined(RTEMS_MULTIPROCESSING)
        multiprocessing_table->node ,
                                    multiprocessing_table->maximum_nodes ,
                                    multiprocessing_table->maximum_global_objects
#endif
        );

    /* set the object information internal RTEMS objects */
    _Objects_Information_table[OBJECTS_INTERNAL_API] = _Internal_Objects;

    /* initialize the Priority Handler */
    _Priority_Handler_initialization();

    /* initialize the Watchdog Handler */
    _Watchdog_Handler_initialization();

    /* initialize the TOD Handler */
    _TOD_Handler_initialization(configuration_table->microseconds_per_tick);

    /* initialize the Thread Handler */
    _Thread_Handler_initialization(configuration_table->ticks_per_timeslice ,
                                   configuration_table->maximum_extensions
#if defined(RTEMS_MULTIPROCESSING)
        ,
                                   multiprocessing_table->maximum_proxies
#endif
        );

#if defined(RTEMS_MULTIPROCESSING)

    /* initialize the Multiprocessing Handler */
    _MPCI_Handler_initialization(multiprocessing_table->User_mpci_table ,
                                 RTEMS_TIMEOUT);
#endif

    /* MANAGERS */

    /* initialize the RTEMS API Managers */
    _RTEMS_API_Initialize(configuration_table);

    /* initialize the Extension Manager */
    _Extension_Manager_initialization(configuration_table->maximum_extensions);

    /* initialize the Input/Output Manager */
    _IO_Manager_initialization(configuration_table->Device_driver_table ,
                               configuration_table->number_of_device_drivers ,
                               configuration_table->maximum_drivers);

    /* set the system state to "before multitasking" */
    _System_state_Set(SYSTEM_STATE_BEFORE_MULTITASKING);

    /* No threads should be created before this point!!!
     * _Thread_Executing and _Thread_Heir are not set.
     * At this point all API extensions are in place.  After the call to
     * _Thread_Create_idle() _Thread_Executing and _Thread_Heir will be set */
    _Thread_Create_idle();

    /* Scheduling can properly occur now as long as we avoid dispatching */

    /* check if the BSP has installed a pretasking hook */
    if(cpu_table->pretasking_hook)
    {
        /* it has, so call it */
        ( *cpu_table->pretasking_hook )( );
    }

#if defined(RTEMS_MULTIPROCESSING)

    /* create server to handle remote packets */
    _MPCI_Create_server();
#endif

    /* run the API and BSPs predriver hook */
    _API_extensions_Run_predriver();

    /* check if there is a predriver hook */
    if(_CPU_Table.predriver_hook)
    {
        /* there is, so call it */
        ( *_CPU_Table.predriver_hook )( );
    }

    /* initialize all the device drivers and initialize the MPCI layer.
     * @note:  The MPCI may be build upon a device driver */
    _IO_Initialize_all_drivers();

#if defined(RTEMS_MULTIPROCESSING)

    /* if the system state has multiprocessing */
    if(_System_state_Is_multiprocessing)
    {
        /* then initialize the multiprocessor packet server */
        _MPCI_Initialization();

        /* send packet */
        _MPCI_Internal_packets_Send_process_packet(MPCI_PACKETS_SYSTEM_VERIFY);
    }
#endif

    /* Run the APIs and BSPs postdriver hooks */

    /* The API extensions are supposed to create user initialization tasks */
    _API_extensions_Run_postdriver();

    /* if there exists a post driver hook */
    if(_CPU_Table.postdriver_hook)
    {
        /* then call it */
        ( *_CPU_Table.postdriver_hook )( );
    }

    /* return the previous interrupt level */
    return bsp_level;
}


void rtems_initialize_executive_late(
                                     rtems_interrupt_level bsp_level
                                     )
{
    /* set the system state to begin multitasking, almost on "UP" :) */
    _System_state_Set(SYSTEM_STATE_BEGIN_MULTITASKING);

    /* start multitasking, loose control of CPU here */
    _Thread_Start_multitasking();

    /* restore the interrupt level to what the BSP had.  Technically,
     * this is unnecessary since the BSP should have all interrupts
     * disabled when rtems_initialize_executive is invoked.  But this keeps
     * the ISR Disable/Enable calls paired */
    _ISR_Enable(bsp_level);
}

/**  
 *  @}
 */

/**  
 *  @}
 */

