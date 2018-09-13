/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <rtems.h>

#include <bsp.h> /* for device driver prototypes */

const char rtems_test_name[] = "GNSS SW ARCH";

task_param_t task_param[20] =
{
/*  name task  id task   expected cpu     cpuset   run     cpu  */
  {   "INIT"  ,  0x0   ,       3       , {{0x8}} , false  , -1}, /* INIT */
  {   "TA01"  ,  0x0   ,       3       , {{0x8}} , false  , -1}, /* TA01 */
  {   "TA02"  ,  0x0   ,       3       , {{0x8}} , false  , -1}, /* TA02 */
  {   "TA03"  ,  0x0   ,       3       , {{0x8}} , false  , -1}, /* TA03 */
  {   "TA04"  ,  0x0   ,       0       , {{0x1}} , false  , -1}, /* TA04 */
  {   "TA05"  ,  0x0   ,       1       , {{0x2}} , false  , -1}, /* TA05 */
  {   "TA06"  ,  0x0   ,       2       , {{0x4}} , false  , -1}, /* TA06 */
  {   "TA07"  ,  0x0   ,       3       , {{0x8}} , false  , -1}, /* TA07 */
  {   "TA08"  ,  0x0   ,       2       , {{0x4}} , false  , -1}, /* TA08 */
  {   "TA09"  ,  0x0   ,       1       , {{0x2}} , false  , -1}, /* TA09 */
  {   "TA10"  ,  0x0   ,       0       , {{0x1}} , false  , -1}  /* TA10 */
};

/* INIT TASK */ 
int entry_point( rtems_task_argument argument )
{
  rtems_status_code status;    // status of rtems function
  rtems_time_of_day time;      // rtems system time
  uint32_t          i;         // variable for for loop 
   
  locked_print_initialize(); 
  
    /* SET clock & date */ 
  time.year   = 2018;
  time.month  = 03;
  time.day    = 12; 
  time.hour   = 9;
  time.minute = 0;
  time.second = 0;
  time.ticks  = 0;

  status = rtems_clock_set( &time );
    
 /* Create tasks */ 
    locked_printf("  \n> Creating Tasks\n");
    
    
  for ( i=1; i <= 10; i++ ) 
  {
    status = rtems_task_create(
                                rtems_build_name( task_param[ i ].name[0],task_param[ i ].name[1],task_param[ i ].name[2], task_param[ i ].name[3]),
                                2,
                                RTEMS_MINIMUM_STACK_SIZE,
                                RTEMS_DEFAULT_MODES,
                                RTEMS_FLOATING_POINT,
                                &task_param[ i ].id
                              );
 //   directive_failed( status, "task create" );
  }
  
/* Print info about created tasks */
  locked_printf("  ------------------------------------------\n");
    for (i = 1 ; i <= 10; i++)
    {
       print_tasks_info(task_param[ i ].id, i);
    }
/* Rtems tasks set affinity cpu */
    for (i = 1; i <=  10; i++) 
    {
        status = rtems_task_set_affinity( task_param[ i ].id, sizeof(cpu_set_t), &task_param[i].cpuset );
        rtems_test_assert(status == RTEMS_SUCCESSFUL);
        printf("> Set %s affinity to cpu %d\n", task_param[i].name, task_param[i].expected_cpu );
    }
/* ############################## */
 
/* TASKS START */   
    status = rtems_task_start( task_param[ 1 ].id, Periodic_task, 1 );
     status = rtems_task_start( task_param[ 2 ].id, Periodic_task1, 2 );
      status = rtems_task_start( task_param[ 3 ].id, Periodic_task2, 3 );
       status = rtems_task_start( task_param[ 4 ].id, Periodic_task3, 4 );
        status = rtems_task_start( task_param[ 5 ].id, Periodic_task4, 5 );
         status = rtems_task_start( task_param[ 6 ].id, Periodic_task5, 6 );
          status = rtems_task_start( task_param[ 7 ].id, Periodic_task6, 7 );
           status = rtems_task_start( task_param[ 8 ].id, Periodic_task7, 8 );
            status = rtems_task_start( task_param[ 9 ].id, Periodic_task8, 9 );
             status = rtems_task_start( task_param[ 10 ].id, Periodic_task9, 10 );
        
    directive_failed( status, "task start" );  
   
/* Delete Init task*/
 rtems_task_delete( RTEMS_SELF );
}
