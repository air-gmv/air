/*
 * Copyright (C) 2008-2020  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */

#include <imaspex.h>
#include <pprintf.h>
#include <rtems.h>
#include <rtems/rtems/clock.h>
#include <rtems/rtems/sem.h>
#include <rtems/rtems/tasks.h>

#define TPS 200 /*ticks per second specified in the XML*/

SAMPLING_PORT_ID_TYPE SEND_PORT;
QUEUING_PORT_ID_TYPE qpid;
int g_bp = 0;

/*---------------------------------------------------------
 *		function: test										*
 *			outputs a simple string via a sampling port		*
------------------------------------------------------------*/

void test(uintptr_t self_id)
{

    char message[18] = "This is sample 0\0";
    int i = 0;

    RETURN_CODE_TYPE rc;

    for (int j = 0; j <= 3; j++)
    {
        (void)printf("Partition %d sending msg: %s..\n", self_id, message);
        WRITE_SAMPLING_MESSAGE(SEND_PORT, (MESSAGE_ADDR_TYPE)message, 17, &rc);
        if (NO_ERROR != rc)
        {
            (void)printf("WRITE_SAMPLING_MESSAGE error %d\n", rc);
        }

        /*identify the string with an integer index*/
        i++;
        if (i == 10)
        {
            i = 0;
        }
        message[15] = 0x30 + i;

        (void)printf("Partition %d sending queuing: %s..\n", self_id, message);
        g_bp = 10;

        SEND_QUEUING_MESSAGE(qpid, (MESSAGE_ADDR_TYPE)message, 17, INFINITE_TIME_VALUE, &rc);
        if (rc != NO_ERROR)
        {
            (void)printf("SEND_QUEUING_MESSAGE error %d\n", rc);
        }

        /*identify the string with an integer index*/
        i++;
        if (i == 10)
        {
            i = 0;
        }
        message[15] = 0x30 + i;

        rtems_task_wake_after(0.6 * TPS);
    }
    air_syscall_shutdown_module();
}

int entry_func()
{

    RETURN_CODE_TYPE rc;

    rtems_name name = 10000;
    rtems_mode mode = RTEMS_PREEMPT;
    rtems_mode mode_mask = RTEMS_PREEMPT_MASK;
    rtems_id id;

    PARTITION_ID_TYPE self_id;

    /*Getting my own partition id*/
    GET_PARTITION_ID(&self_id, &rc);
    if (NO_ERROR != rc)
    {
        (void)printf("GET_PARTITION_ID error %d\n", rc);
    }

    (void)printf("Initializing partition %d...\n", self_id);

    /*Creating Source sampling Port*/
    SAMPLING_PORT_NAME_TYPE NAME = "SEND_SAMP";
    MESSAGE_SIZE_TYPE SIZE = 1024;
    SYSTEM_TIME_TYPE PERIOD = 1500000000;

    CREATE_SAMPLING_PORT(NAME, SIZE, SOURCE, PERIOD, &SEND_PORT, &rc);
    if (NO_ERROR != rc)
    {
        (void)printf("CREATE_SAMPLING_PORT error %d\n", rc);
    }

    /* Creating Queueing port */
    CREATE_QUEUING_PORT("QSAMPLE", 1024, 32, SOURCE, FIFO, &qpid, &rc);
    if (NO_ERROR != rc)
    {
        (void)printf("CREATE_QUEUING_PORT error %d\n", rc);
    }

    if (RTEMS_SUCCESSFUL == rtems_task_create(name, 15, 4096, mode, mode_mask, &id))
    {
        rtems_task_start(id, test, self_id);
    }

    SET_PARTITION_MODE(NORMAL, &rc);
    if (NO_ERROR != rc)
    {
        (void)printf("SET_PARTITION_MODE error %d\n", rc);
    }

    return RTEMS_SUCCESSFUL;
}
