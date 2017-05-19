<%
    import xky
    import utils.templates as makoutils
%>\
${makoutils.cfileHeader("rtems_config.h", "RTEMS configuration")}

#include <rtems.h>

/**
 * @brief Standard RTEMS configuration
 */
#define CONFIGURE_INIT
#define CONFIGURE_MAXIMUM_TASKS                         64
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_EXTRA_TASK_STACKS                     RTEMS_MINIMUM_STACK_SIZE
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_MAXIMUM_SEMAPHORES                    1
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
%if xky.PERMISSION_FPU in partition.permissions:
#define CONFIGURE_INIT_TASK_ATTRIBUTES                  RTEMS_FLOATING_POINT
%endif
#define CONFIGURE_MAXIMUM_PERIODS                       1

#include <rtems/confdefs.h>
