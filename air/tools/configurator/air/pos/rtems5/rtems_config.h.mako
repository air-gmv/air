<%
    import air
    import utils.templates as makoutils
    import parsers.a653.definitions as a653_definitions

    ## convert ticks per second to microsec per tick
    ##int(1E6/ ticks_per_second)
    def tps_to_mpt():
        return int(1E6/ app_configuration.ticks_per_second)

%>\
${makoutils.cfileHeader("rtems_config.h", "RTEMS configuration")}

#include <rtems.h>

/**
 * @brief Standard RTEMS configuration
 */

%if air.PERMISSION_FPU in partition.permissions:
    #define CONFIGURE_INIT_TASK_ATTRIBUTES                  RTEMS_FLOATING_POINT
%endif


#define CONFIGURE_MICROSECONDS_PER_TICK ${tps_to_mpt()}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 20
#define CONFIGURE_MAXIMUM_TIMERS 4

#ifdef IS_TASTE_APP
    #define CONFIGURE_UNLIMITED_OBJECTS
    #define CONFIGURE_UNIFIED_WORK_AREAS
#endif

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_PROCESSORS ${partition.core_count}

#define CONFIGURE_INIT


#include <rtems/confdefs.h>
