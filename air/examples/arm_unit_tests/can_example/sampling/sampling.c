/*
 * Copyright (C) 2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
#include <a653.h>
#include <imaspex.h>

#include <pprintf.h>

SAMPLING_PORT_ID_TYPE SEND_PORT;
QUEUING_PORT_ID_TYPE qpid;

/*---------------------------------------------------------
 *        function: test                                    *
 *            outputs a simple string via a sampling port   *
------------------------------------------------------------*/

void error_message(RETURN_CODE_TYPE rc){

    switch(rc){
        case INVALID_CONFIG:
            pprintf("WRITE_SAMPLING_MESSAGE error Invalid Config\n");
            break;
        case INVALID_PARAM:
            pprintf("WRITE_SAMPLING_MESSAGE error Invalid Param\n");
            break;
        case INVALID_MODE:
            pprintf("WRITE_SAMPLING_MESSAGE error Invalid Mode\n");
        default:
            break;
    }
}


void test(PARTITION_ID_TYPE self_id) {

    char sample[8] = "hello 0\n";
    int number = 48; // '0'
    /* get the number of ticks per second */
    air_u32_t us_per_tick = air_syscall_get_us_per_tick();
    air_u32_t tps = 1000000 / us_per_tick;
    air_u32_t interval = 0;

    air_u32_t start = air_syscall_get_elapsed_ticks();
    air_u32_t time = start;

    unsigned char msg[4096]="empty\0";
    int i;
    MESSAGE_SIZE_TYPE len;
    RETURN_CODE_TYPE rc = NO_ERROR;

    while(1) {

        pprintf("RECEIVE_QUEUING_MESSAGE receiving msg\n");

        RECEIVE_QUEUING_MESSAGE(qpid, INFINITE_TIME_VALUE, msg, &len, &rc);
        if (rc == NO_ERROR)
            pprintf ("Partition %d: %s\n", self_id, msg);

        pprintf("WRITE_SAMPLING_MESSAGE sending msg\n");

        //pprintf ("Partition sending: %s\n", sample);
        WRITE_SAMPLING_MESSAGE (SEND_PORT, (MESSAGE_ADDR_TYPE )sample, 8, &rc);
        if (NO_ERROR != rc) {
            pprintf("WRITE_SAMPLING_MESSAGE error %d\n", rc);
            error_message(rc);
        }

        //for(){}

        /*identify the string with an integer index*/
        //sample[1] += 1;

        if (sample[1] == ':')
            sample[1] = '0';

        interval = tps * 0.7;
        WRITE_SAMPLING_MESSAGE (SEND_PORT, '\n', 100, &rc );
        //pprintf("interval %i\n", interval);

        number += 1;
        if (number > 57)
            number = 48;
        sample[6] = (char)number;

        while (air_syscall_get_elapsed_ticks() - start <  .2*tps) {}
        start = air_syscall_get_elapsed_ticks();
        pprintf("\nstart = %d\n", start);

    }
}

int entry_func() {

    RETURN_CODE_TYPE rc;
/*
    rtems_name     name         = 10000;
    rtems_mode    mode        = RTEMS_PREEMPT ;
#ifdef RTEMS48I
    rtems_mode    mode_mask        = RTEMS_PREEMPT_MASK;
#else
    rtems_attribute    attribute_set    = RTEMS_LOCAL | RTEMS_FLOATING_POINT;
#endif
    rtems_id    id;
*/
    PARTITION_ID_TYPE self_id;

    /*Getting my own partition id*/
    GET_PARTITION_ID(&self_id, &rc);
    if(NO_ERROR != rc) {
        pprintf("GET_PARTITION_ID error %d\n", rc);
    }

    pprintf("Initializing partition %d...\n", self_id);

    /*Creating Source sampling Port*/
    SAMPLING_PORT_NAME_TYPE NAME = "ssampling";
    MESSAGE_SIZE_TYPE SIZE = 1024;
    SYSTEM_TIME_TYPE PERIOD = 1000000000ll;
    CREATE_SAMPLING_PORT (NAME, SIZE, SOURCE, PERIOD, &SEND_PORT, &rc);
    if (NO_ERROR != rc) {
        pprintf("CREATE_SAMPLING_PORT error %d\n\n", rc);
    }
    pprintf("CREATED SAMPLING_PORT\n");

    CREATE_QUEUING_PORT("dqueuing", 1024, 32, DESTINATION, FIFO, &qpid, &rc );
    if(NO_ERROR != rc){
        pprintf("CREATE_QUEUING_PORT error %d\n", rc);
    }
    pprintf("CREATED QUEUING_PORT\n");

/*
#ifdef RTEMS48I
    if (RTEMS_SUCCESSFUL == rtems_task_create (name, 15, 4096, mode, mode_mask, &id)) {
        rtems_task_start (id, test, self_id);
    }
#else
    if (RTEMS_SUCCESSFUL == rtems_task_create (name, 15, 4096, mode, attribute_set, &id)) {
        rtems_task_start (id, (rtems_task_entry)test, self_id);
    }
#endif*/
    SET_PARTITION_MODE(NORMAL, &rc);
    if (NO_ERROR != rc) {
        pprintf("SET_PARTITION_MODE error %d\n", rc);
    }

    test(self_id);
    return 1;
}
