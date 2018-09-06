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
#define CONFIGURE_MICROSECONDS_PER_TICK ${tps_to_mpt()}

#define CONFIGURE_MAXIMUM_PROCESSORS ${partition.core_count}
#define CONFIGURE_MAXIMUM_POSIX_THREADS     10
#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_INIT


#include <rtems/confdefs.h>
