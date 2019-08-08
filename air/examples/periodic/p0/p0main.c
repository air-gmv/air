/*
 * Copyright (C) 2008-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
 
#include <rtems.h>
 
#include <rtems/rtems/tasks.h> 
#include <rtems/rtems/sem.h> 
#include <rtems/rtems/clock.h> 

#include <a653.h>
#include <imaspex.h>

#include <pprintf.h>

void test2(uintptr_t self_id) {
    rtems_name        name_1, name_2;
    rtems_id          period_1, period_2;
    rtems_status_code status;
    int elapsed;

    name_2 = rtems_build_name( 'P', 'E', 'R', '2' );
    name_1 = rtems_build_name( 'P', 'E', 'R', '1' );

    status=  rtems_rate_monotonic_create( name_1, &period_1 );
    if( RTEMS_SUCCESSFUL != status )
        pprintf("create 1 failed with status: %d\n", status);

    status=  rtems_rate_monotonic_create( name_2, &period_2 );
    if( RTEMS_SUCCESSFUL != status )
        pprintf("create 2 failed with status: %d\n", status);

    while ( 1 )
    {
        status = rtems_rate_monotonic_period( period_1, 100 );
        if( RTEMS_SUCCESSFUL != status )
            pprintf("RM100 failed with status: %d\n", status);

        status = rtems_rate_monotonic_period( period_2, 40 );
        if( RTEMS_SUCCESSFUL != status )
            pprintf("RM40 failed with status: %d\n", status);

        elapsed = air_syscall_get_elapsed_ticks();
        pprintf("Perform first set of actions between ticks 0 and 39 every 100 ticks.\tstart@%d\n", elapsed);

        /* period_2 is active for 40 tick so this will block untill 40 ticks have passed*/
        status = rtems_rate_monotonic_period( period_2, 30 );
        if( RTEMS_SUCCESSFUL != status )
            pprintf("RM30 failed with status: %d\n", status);

        elapsed = air_syscall_get_elapsed_ticks();
        pprintf("  Perform second set of actions between ticks 40 and 69 every 100 ticks.current=%d\n", elapsed);

        /*we finished so we need to cancel this one otherwise we'll get a timeout*/
        (void) rtems_rate_monotonic_cancel( period_2 );
    }
    /* missed period so delete it */
    /*status = rtems_rate_monotonic_delete( period_2 );
    if ( status != 0 ) {
        printf( "rtems_rate_monotonic_delete failed with status of %d.\n", status );
        exit( 1 );
    }*/
}

int producer() {

    RETURN_CODE_TYPE rc;

    rtems_name  name        = 10000;
    rtems_mode  mode        = RTEMS_PREEMPT ;
    rtems_mode  mode_mask   = RTEMS_PREEMPT_MASK;
    rtems_id    id;

    PARTITION_ID_TYPE self_id;

    /*Getting my own partition id*/
    GET_PARTITION_ID(&self_id, &rc);
    if(NO_ERROR != rc)
        pprintf("GET_PARTITION_ID error %d\n", rc);

    if (RTEMS_SUCCESSFUL == rtems_task_create (name, 15, 4096, mode, mode_mask, &id))
        rtems_task_start (id, test2, self_id);

    SET_PARTITION_MODE(NORMAL, &rc);
    if (NO_ERROR != rc)
        pprintf("SET_PARTITION_MODE error %d\n", rc);

    return RTEMS_SUCCESSFUL;
}
