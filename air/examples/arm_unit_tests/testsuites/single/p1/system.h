/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/test.h>

/* functions */

rtems_task Init(
  rtems_task_argument argument
);


rtems_task test_task(
  rtems_task_argument my_number
);

void
destroy_all_tasks(
  const char *who
);

bool status_code_bad(
  rtems_status_code status_code
);

extern void test1(void);
extern void test2(void);
extern void test3(void);


/* configuration information */

#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS            1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_INIT_TASK_ATTRIBUTES    RTEMS_FLOATING_POINT
#define CONFIGURE_INIT_TASK_STACK_SIZE    (RTEMS_MINIMUM_STACK_SIZE * 2)

#define CONFIGURE_EXTRA_TASK_STACKS       (1 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#include <rtems/confdefs.h>

#define MAX_TASKS         (1000)
#define TASK_INDEX_OFFSET (1)

extern rtems_id task_id[MAX_TASKS];

/*
 * Increment the task name.
 */

#define NEXT_TASK_NAME(c1, c2, c3, c4)  \
                 if (c4 == '9') {       \
                   if (c3 == '9') {     \
                     if (c2 == 'z') {   \
                       if (c1 == 'z') { \
                         printf("not enough task letters for names !!!\n"); \
                         exit( 1 );     \
                       } else           \
                         c1++;          \
                       c2 = 'a';        \
                     } else             \
                      c2++;             \
                     c3 = '0';          \
                   } else               \
                     c3++;              \
                   c4 = '0';            \
                 }                      \
                 else                   \
                   c4++                 \


/* end of include file */



/* end of include file */
