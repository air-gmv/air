<%
    import air
    import utils.templates as makoutils
%>\
${makoutils.cfileHeader("rtems_config.h", "RTEMS configuration")}

#include <rtems.h>

/**
 * @brief Standard RTEMS configuration
 */
#define CONFIGURE_INIT_TASK_ATTRIBUTES                  RTEMS_FLOATING_POINT

#define CONFIGURE_MICROSECONDS_PER_TICK 1000


#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS                         4
#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT


#include <rtems/confdefs.h>
