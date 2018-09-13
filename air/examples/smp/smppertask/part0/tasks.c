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

#include "system.h"
#include "tasks.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <time.h>

#define PER 2000

extern task_param_t task_param[20];

rtems_task Periodic_task(rtems_task_argument arg)
{
    rtems_name        name;
    rtems_id          period;
    rtems_status_code status;
    name = rtems_build_name( 'P', 'E', 'R', '1' );
    status = rtems_rate_monotonic_create( name, &period );
    
    int tick = 0 ;
    if ( status != 0 ) {
        printf( "rtems_monotonic_create failed with status of %d.\n", status );
        exit( 1 );
    }
    while ( 1 ) {
        if ( rtems_rate_monotonic_period( period, PER ) == RTEMS_TIMEOUT )
            break;
        
        locked_printf("> TICK1   %d ",tick);
        tick++;
         PrintTask( task_param[ 1 ].id); 
         
        /* Perform some periodic actions */
    }
    /* missed period so delete period and SELF */
    status = rtems_rate_monotonic_delete( period );
    if ( status != 0 ) {
        printf( "rtems_rate_monotonic_delete failed with status of %d.\n", status );
        exit( 1 );
    }
    status = rtems_task_delete( RTEMS_SELF );    /* should not return */
    printf( "rtems_task_delete returned with status of %d.\n", status );
    exit( 1 );
}

rtems_task Periodic_task1(rtems_task_argument arg)
{
    rtems_name        name;
    rtems_id          period;
    rtems_status_code status;
    name = rtems_build_name( 'P', 'E', 'R', '2' );
    status = rtems_rate_monotonic_create( name, &period );
    
    int tick = 0 ;
    if ( status != 0 ) {
        printf( "rtems_monotonic_create failed with status of %d.\n", status );
        exit( 1 );
    }
    while ( 1 ) {
        if ( rtems_rate_monotonic_period( period, PER ) == RTEMS_TIMEOUT )
            break;
        
        locked_printf("> TICK2   %d ",tick);
        tick++;
         PrintTask( task_param[ 2 ].id); 
        /* Perform some periodic actions */
    }
    /* missed period so delete period and SELF */
    status = rtems_rate_monotonic_delete( period );
    if ( status != 0 ) {
        printf( "rtems_rate_monotonic_delete failed with status of %d.\n", status );
        exit( 1 );
    }
    status = rtems_task_delete( RTEMS_SELF );    /* should not return */
    printf( "rtems_task_delete returned with status of %d.\n", status );
    exit( 1 );
}

rtems_task Periodic_task2(rtems_task_argument arg)
{
    rtems_name        name;
    rtems_id          period;
    rtems_status_code status;
    name = rtems_build_name( 'P', 'E', 'R', '3' );
    status = rtems_rate_monotonic_create( name, &period );
    
    int tick = 0 ;
    if ( status != 0 ) {
        printf( "rtems_monotonic_create failed with status of %d.\n", status );
        exit( 1 );
    }
    while ( 1 ) {
        if ( rtems_rate_monotonic_period( period, PER ) == RTEMS_TIMEOUT )
            break;
        
        locked_printf("> TICK3   %d ",tick);
        tick++;
         PrintTask( task_param[ 3 ].id); 
        /* Perform some periodic actions */
    }
    /* missed period so delete period and SELF */
    status = rtems_rate_monotonic_delete( period );
    if ( status != 0 ) {
        printf( "rtems_rate_monotonic_delete failed with status of %d.\n", status );
        exit( 1 );
    }
    status = rtems_task_delete( RTEMS_SELF );    /* should not return */
    printf( "rtems_task_delete returned with status of %d.\n", status );
    exit( 1 );
}
rtems_task Periodic_task3(rtems_task_argument arg)
{
    rtems_name        name;
    rtems_id          period;
    rtems_status_code status;
    name = rtems_build_name( 'P', 'E', 'R', '4' );
    status = rtems_rate_monotonic_create( name, &period );
    
    int tick = 0 ;
    if ( status != 0 ) {
        printf( "rtems_monotonic_create failed with status of %d.\n", status );
        exit( 1 );
    }
    while ( 1 ) {
        if ( rtems_rate_monotonic_period( period, PER ) == RTEMS_TIMEOUT )
            break;
        
        locked_printf("> TICK4   %d ",tick);
        tick++;
         PrintTask( task_param[ 4 ].id); 
        /* Perform some periodic actions */
    }
    /* missed period so delete period and SELF */
    status = rtems_rate_monotonic_delete( period );
    if ( status != 0 ) {
        printf( "rtems_rate_monotonic_delete failed with status of %d.\n", status );
        exit( 1 );
    }
    status = rtems_task_delete( RTEMS_SELF );    /* should not return */
    printf( "rtems_task_delete returned with status of %d.\n", status );
    exit( 1 );
}
rtems_task Periodic_task4(rtems_task_argument arg)
{
    rtems_name        name;
    rtems_id          period;
    rtems_status_code status;
    name = rtems_build_name( 'P', 'E', 'R', '5' );
    status = rtems_rate_monotonic_create( name, &period );
    
    int tick = 0 ;
    if ( status != 0 ) {
        printf( "rtems_monotonic_create failed with status of %d.\n", status );
        exit( 1 );
    }
    while ( 1 ) {
        if ( rtems_rate_monotonic_period( period, PER ) == RTEMS_TIMEOUT )
            break;
        
        locked_printf("> TICK5   %d ",tick);
        tick++;
         PrintTask( task_param[ 5 ].id); 
        /* Perform some periodic actions */
    }
    /* missed period so delete period and SELF */
    status = rtems_rate_monotonic_delete( period );
    if ( status != 0 ) {
        printf( "rtems_rate_monotonic_delete failed with status of %d.\n", status );
        exit( 1 );
    }
    status = rtems_task_delete( RTEMS_SELF );    /* should not return */
    printf( "rtems_task_delete returned with status of %d.\n", status );
    exit( 1 );
}
rtems_task Periodic_task5(rtems_task_argument arg)
{
    rtems_name        name;
    rtems_id          period;
    rtems_status_code status;
    name = rtems_build_name( 'P', 'E', 'R', '6' );
    status = rtems_rate_monotonic_create( name, &period );
    
    int tick = 0 ;
    if ( status != 0 ) {
        printf( "rtems_monotonic_create failed with status of %d.\n", status );
        exit( 1 );
    }
    while ( 1 ) {
        if ( rtems_rate_monotonic_period( period, PER ) == RTEMS_TIMEOUT )
            break;
        
        locked_printf("> TICK6   %d ",tick);
        tick++;
         PrintTask( task_param[ 6 ].id); 
        /* Perform some periodic actions */
    }
    /* missed period so delete period and SELF */
    status = rtems_rate_monotonic_delete( period );
    if ( status != 0 ) {
        printf( "rtems_rate_monotonic_delete failed with status of %d.\n", status );
        exit( 1 );
    }
    status = rtems_task_delete( RTEMS_SELF );    /* should not return */
    printf( "rtems_task_delete returned with status of %d.\n", status );
    exit( 1 );
}
rtems_task Periodic_task6(rtems_task_argument arg)
{
    rtems_name        name;
    rtems_id          period;
    rtems_status_code status;
    name = rtems_build_name( 'P', 'E', 'R', '7' );
    status = rtems_rate_monotonic_create( name, &period );
    
    int tick = 0 ;
    if ( status != 0 ) {
        printf( "rtems_monotonic_create failed with status of %d.\n", status );
        exit( 1 );
    }
    while ( 1 ) {
        if ( rtems_rate_monotonic_period( period, PER ) == RTEMS_TIMEOUT )
            break;
        
        locked_printf("> TICK7   %d ",tick);
        tick++;
         PrintTask( task_param[ 7 ].id); 
        /* Perform some periodic actions */
    }
    /* missed period so delete period and SELF */
    status = rtems_rate_monotonic_delete( period );
    if ( status != 0 ) {
        printf( "rtems_rate_monotonic_delete failed with status of %d.\n", status );
        exit( 1 );
    }
    status = rtems_task_delete( RTEMS_SELF );    /* should not return */
    printf( "rtems_task_delete returned with status of %d.\n", status );
    exit( 1 );
}
rtems_task Periodic_task7(rtems_task_argument arg)
{
    rtems_name        name;
    rtems_id          period;
    rtems_status_code status;
    name = rtems_build_name( 'P', 'E', 'R', '8' );
    status = rtems_rate_monotonic_create( name, &period );
    
    int tick = 0 ;
    if ( status != 0 ) {
        printf( "rtems_monotonic_create failed with status of %d.\n", status );
        exit( 1 );
    }
    while ( 1 ) {
        if ( rtems_rate_monotonic_period( period, PER ) == RTEMS_TIMEOUT )
            break;
        
        locked_printf("> TICK8   %d ",tick);
        tick++;
         PrintTask(task_param[ 8 ].id); 
        /* Perform some periodic actions */
    }
    /* missed period so delete period and SELF */
    status = rtems_rate_monotonic_delete( period );
    if ( status != 0 ) {
        printf( "rtems_rate_monotonic_delete failed with status of %d.\n", status );
        exit( 1 );
    }
    status = rtems_task_delete( RTEMS_SELF );    /* should not return */
    printf( "rtems_task_delete returned with status of %d.\n", status );
    exit( 1 );
}
rtems_task Periodic_task8(rtems_task_argument arg)
{
    rtems_name        name;
    rtems_id          period;
    rtems_status_code status;
    name = rtems_build_name( 'P', 'E', 'R', '9' );
    status = rtems_rate_monotonic_create( name, &period );
    
    int tick = 0 ;
    if ( status != 0 ) {
        printf( "rtems_monotonic_create failed with status of %d.\n", status );
        exit( 1 );
    }
    while ( 1 ) {
        if ( rtems_rate_monotonic_period( period, PER ) == RTEMS_TIMEOUT )
            break;
        
        locked_printf("> TICK9   %d ",tick);
        tick++;
         PrintTask( task_param[ 9 ].id); 
        /* Perform some periodic actions */
    }
    /* missed period so delete period and SELF */
    status = rtems_rate_monotonic_delete( period );
    if ( status != 0 ) {
        printf( "rtems_rate_monotonic_delete failed with status of %d.\n", status );
        exit( 1 );
    }
    status = rtems_task_delete( RTEMS_SELF );    /* should not return */
    printf( "rtems_task_delete returned with status of %d.\n", status );
    exit( 1 );
}
rtems_task Periodic_task9(rtems_task_argument arg)
{
    rtems_name        name;
    rtems_id          period;
    rtems_status_code status;
    name = rtems_build_name( 'P', 'E', '1', '0' );
    status = rtems_rate_monotonic_create( name, &period );
    
    int tick = 0 ;
    if ( status != 0 ) {
        printf( "rtems_monotonic_create failed with status of %d.\n", status );
        exit( 1 );
    }
    while ( 1 ) {
        if ( rtems_rate_monotonic_period( period, PER ) == RTEMS_TIMEOUT )
            break;
        
        locked_printf("> TICK10  %d ",tick);
        tick++;
        PrintTask( task_param[ 10 ].id); 
         
        if( tick == 10 ) 
        {              
              rtems_test_exit(0);
        }

        /* Perform some periodic actions */
    }
    /* missed period so delete period and SELF */
    status = rtems_rate_monotonic_delete( period );
    if ( status != 0 ) {
        printf( "rtems_rate_monotonic_delete failed with status of %d.\n", status );
        exit( 1 );
    }
    status = rtems_task_delete( RTEMS_SELF );    /* should not return */
    printf( "rtems_task_delete returned with status of %d.\n", status );
    exit( 1 );
}
