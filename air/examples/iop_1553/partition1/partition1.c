/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2008-2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file 
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE. 
 * ==========================================================================*/

#include <rtems.h>
 
#include <rtems/rtems/tasks.h> 
#include <rtems/rtems/sem.h> 
#include <rtems/rtems/clock.h> 
 
#include <a653.h>
#include <imaspex.h>

#include <string.h>

#include <pprintf.h>

/* inter-partition ports maximum size definitions */
#define MAX_MESSAGE_SIZE 1024 // bytes
#define IOP_REFRESH_RATE 1000000000 // 1s in ns


/*TOREMOVE: FOR COMPATIBILITY WITH FORMER CODE*/
#define false 0
#define true 1
typedef int boolean_T;


typedef enum
{
    E_RCV_APP_RT1   = 0,
    E_RCV_APP_RT2,
    E_RCV_APP_RT3,
    E_RCV_APP_RT4,
    E_RCV_COUNT
}eSamplingRecvId;

typedef enum
{
    E_SEND_APP_RT1   = 0,
    E_SEND_APP_RT2,
    E_SEND_APP_RT3,
    E_SEND_APP_RT4,
    E_SEND_COUNT
}eSemplingSendId;

/* size in bytes of expected received data */
int rtRxDataSize[E_RCV_COUNT] = {24,24,64,64};

/* size in bytes of data to send */
int rtTxDataSize[E_RCV_COUNT] = {2,2,4,4};

typedef struct
{
    SAMPLING_PORT_ID_TYPE   portId;
    boolean_T               received;
}sRcvSamplingPort;

typedef struct
{
    SAMPLING_PORT_ID_TYPE   portId;
}sSendSamplingPort;

/* Reception sampling inter-partition ports*/
static sRcvSamplingPort RCV_SAMPLING_PORT[E_RCV_COUNT];
/* Transmision sampling inter-partition ports*/
static sSendSamplingPort SEND_SAMPLING_PORT[E_SEND_COUNT];

/*
 * read sampling port data
 */
MESSAGE_SIZE_TYPE read_sampling_port(eSamplingRecvId port, MESSAGE_ADDR_TYPE received)
{
    RETURN_CODE_TYPE rc;

    UPDATED_TYPE updated;
    MESSAGE_SIZE_TYPE size = 0;
    READ_UPDATED_SAMPLING_MESSAGE(RCV_SAMPLING_PORT[port].portId, received, &size, &updated, &rc);
    if(INVALID_PARAM == rc)
    {
 //       pprintf("P1 Rx port%d error %d\n", port, rc);
    }
    else if(EMPTY_PORT == updated)
    {
 //       pprintf("P1 Empty Sampling Port %d\n", port);
    }
    else if(CONSUMED_MESSAGE == updated)
    {
 //       pprintf("P1 Message in the sampling port has already been read.\n");
    }
    else if(NEW_MESSAGE == updated)
    {
 //       pprintf("P1 READ_UPDATED_SAMPLING_MESSAGE returned %d bytes.\n", size);
        RCV_SAMPLING_PORT[port].received = true;
    }
    else
    {
 //       pprintf("P1 Unexpected READ_UPDATED_SAMPLING_MESSAGE behavior\n");
    }
    return size;
}


/*
 * Send data to RTs: 
 * RT1 1word
 * RT2 1word
 * RT3 2word
 * RT4 2word
 */
void send_iop_data()
{
    RETURN_CODE_TYPE rc;
    /* create data array of maximum size */
    uint8_t msg[MAX_MESSAGE_SIZE];
    /* pointer to writing position in the data array */
    uint8_t* writeTo = &msg[0];
    /* size of data to send */
    int size = 0;

    int i, j;

	static uint16_t dataword[2] = {0,1};

    dataword[0]++;
    dataword[1]++;

    //Example of writing words to enumerated RTs

    for(i=E_SEND_APP_RT1; i<E_SEND_COUNT; i++)
    {
        writeTo = &msg[0];
        for (j=0; j<rtTxDataSize[i]/sizeof(uint16_t); j++)
        {
            memcpy(writeTo, &dataword[j], sizeof(uint16_t));
            writeTo = writeTo + sizeof(uint16_t);
        }
        size = writeTo - (&msg[0]);

        SAMPLING_PORT_ID_TYPE port = SEND_SAMPLING_PORT[i].portId;

        /* send over port */
        WRITE_SAMPLING_MESSAGE(port, (MESSAGE_ADDR_TYPE )msg, size, &rc );
        if (NO_ERROR != rc)
        {
            pprintf("P1 SEND_APP_RT%d ERROR %d\n", i+1, rc);
        }
    }
}


/*
 * resets the 1553 received flags
 */
void reset_milbus_received()
{
    int i = 0;
    for (i = 0; i < E_RCV_COUNT; i++)
    {
        RCV_SAMPLING_PORT[i].received = false;
    }
}


/*
 * receive the rt data from IOP
 * returns TRUE if any of the expected received data is OK
 * RT1 12word
 * RT2 12word
 * RT3 32word
 * RT4 32word
 */
boolean_T get_iop_data()
{

    boolean_T rcvdData = false;
    /* received data length */
    MESSAGE_SIZE_TYPE len = 0;
    /* create data array of maximum size */
    uint8_t msg[MAX_MESSAGE_SIZE];
    /* pointer to reading position in the data array */
    uint8_t* readFrom = &msg[0];

    uint8_t i, j;

    uint16_t aux[32];

    /*Example for acessing data received from the enumerated RTs*/
    for(i=E_RCV_APP_RT1; i< E_RCV_COUNT; i++)
        if (RCV_SAMPLING_PORT[i].received == false)
        {
            len = read_sampling_port(i, (MESSAGE_ADDR_TYPE) msg);
            if (len == rtRxDataSize[i])
            {
     //           pprintf("P1 RCV_APP_RT%d returned %d bytes.\n", i+1, len);

                /* set read point to the start of the data array */
                readFrom = &msg[0];

                for (j=0; j<rtRxDataSize[i]/sizeof(uint16_t); j++)
                {
                    memcpy(&aux[j], readFrom, sizeof(uint16_t));
                    readFrom = readFrom + sizeof (uint16_t);

                    pprintf("%d ", aux[j]);
                }

                pprintf("\n\n");

                rcvdData = true;
            }
            else 
                if (len != 0)
                {
    //                pprintf("P1 RCV_APP_RT%d returned %d bytes, expected %d!\n", i+1, len, rtRxDataSize[i]);
                    rcvdData |= false;
                }
                else
                {
    //                pprintf("P1 RCV_APP_RT%d returned 0 bytes.\n", i+1);
                    rcvdData |= false;
                }
        }

    return rcvdData;
}


void run_partition(rtems_id task_id) 
{
    rtems_status_code status = NO_ERROR;
    int ticks_per_sec;

    /* get the number of ticks per second */
    ticks_per_sec = 1000000 / air_syscall_get_us_per_tick();
    pprintf("TPS is %i. Partition 1 will be scheduled each %d ticks.\n", ticks_per_sec, (ticks_per_sec/50));

    while (1)
    {
pprintf( "I P1\n");
        /*
         * handle inter-partition communication
         */
        if (get_iop_data() == true)
        {
            reset_milbus_received();

            /*TODO:do some received data processing*/


            //Send data to RTs
            send_iop_data();
        }

pprintf( "O P1\n");

        // delay process in ticks
        rtems_task_wake_after(ticks_per_sec/50);
    }

    status = rtems_task_delete( task_id );    /* should not return */
    pprintf( "rtems_task_delete returned with status of %d.\n", status );
}

/*
 * Function for setting up the
 * inter-partition communication ports
 */
void setup_ports(void)
{
    RETURN_CODE_TYPE rc;

    pprintf("Initializing ports ...\n");

    // init sampling ports fields
    MESSAGE_SIZE_TYPE size = MAX_MESSAGE_SIZE;
    SYSTEM_TIME_TYPE periodIop= IOP_REFRESH_RATE;

    // Creating RECV_APP_RT1 sampling Port
    CREATE_SAMPLING_PORT ("RECV_APP_RT1", size, DESTINATION, periodIop, &RCV_SAMPLING_PORT[E_RCV_APP_RT1].portId, &rc);
    if (NO_ERROR != rc)
    {
        pprintf("CREATE_SAMPLING_PORT RECV_APP_RT1 error %d\n", rc);
    }
    // Creating RECV_APP_RT2 sampling Port
    CREATE_SAMPLING_PORT ("RECV_APP_RT2", size, DESTINATION, periodIop, &RCV_SAMPLING_PORT[E_RCV_APP_RT2].portId, &rc);
    if (NO_ERROR != rc)
    {
        pprintf("CREATE_SAMPLING_PORT RECV_APP_RT2 error %d\n", rc);
    }
    // Creating RECV_APP_RT3 sampling Port
    CREATE_SAMPLING_PORT ("RECV_APP_RT3", size, DESTINATION, periodIop, &RCV_SAMPLING_PORT[E_RCV_APP_RT3].portId, &rc);
    if (NO_ERROR != rc)
    {
        pprintf("CREATE_SAMPLING_PORT RECV_APP_RT3 error %d\n", rc);
    }
    // Creating RECV_APP_RT4 sampling Port
    CREATE_SAMPLING_PORT ("RECV_APP_RT4", size, DESTINATION, periodIop, &RCV_SAMPLING_PORT[E_RCV_APP_RT4].portId, &rc);
    if (NO_ERROR != rc)
    {
        pprintf("CREATE_SAMPLING_PORT RECV_APP_RT4 error %d\n", rc);
    }

	
    // Creating SEND_APP_RT1 sampling Port
    CREATE_SAMPLING_PORT ("SEND_APP_RT1", size, SOURCE, periodIop, &SEND_SAMPLING_PORT[E_SEND_APP_RT1].portId, &rc);
    if (NO_ERROR != rc)
    {
        pprintf("CREATE_SAMPLING_PORT SEND_APP_RT1 error %d\n", rc);
    }
    // Creating SEND_APP_RT2 sampling Port
    CREATE_SAMPLING_PORT ("SEND_APP_RT2", size, SOURCE, periodIop, &SEND_SAMPLING_PORT[E_SEND_APP_RT2].portId, &rc);
    if (NO_ERROR != rc)
    {
        pprintf("CREATE_SAMPLING_PORT SEND_APP_RT2 error %d\n", rc);
    }
    // Creating SEND_APP_RT3 sampling Port
    CREATE_SAMPLING_PORT ("SEND_APP_RT3", size, SOURCE, periodIop, &SEND_SAMPLING_PORT[E_SEND_APP_RT3].portId, &rc);
    if (NO_ERROR != rc)
    {
        pprintf("CREATE_SAMPLING_PORT SEND_APP_RT3 error %d\n", rc);
    }
    // Creating SEND_APP_RT4 sampling Port
    CREATE_SAMPLING_PORT ("SEND_APP_RT4", size, SOURCE, periodIop, &SEND_SAMPLING_PORT[E_SEND_APP_RT4].portId, &rc);
    if (NO_ERROR != rc)
    {
        pprintf("CREATE_SAMPLING_PORT SEND_APP_RT4 error %d\n", rc);
    }

    // Initialize message reception status
    reset_milbus_received();
}

int entry_func() {

    RETURN_CODE_TYPE rc;
    PARTITION_ID_TYPE self_id;
    // Getting my own partition id
    GET_PARTITION_ID(&self_id, &rc);
    if(NO_ERROR != rc) {
        pprintf("GET_PARTITION_ID error %d\n", rc);
    }
    pprintf("Initializing partition %d...\n", self_id);

    // init inter-partition ports
    setup_ports();

    rtems_interval time_buffer = rtems_clock_get_ticks_since_boot();
    pprintf( "Time since boot up: %d ticks.\n", time_buffer);

    // start rtems task
    pprintf("Set-up Rtems Task ...\n");
    rtems_id task_id;
    rtems_name rname = rtems_build_name( 'P', 'A', 'R', '1' );
    if (RTEMS_SUCCESSFUL == rtems_task_create(rname, 1, RTEMS_MINIMUM_STACK_SIZE*3, RTEMS_PREEMPT, RTEMS_FLOATING_POINT, &task_id))
    {
        rtems_task_start (task_id, run_partition, task_id);
    }
    pprintf("Rtems Task has been setup.\n");
    time_buffer = rtems_clock_get_ticks_since_boot();
    pprintf( "Time since boot up: %d ticks.\n", time_buffer);

    SET_PARTITION_MODE(NORMAL, &rc);
    if (NO_ERROR != rc)
    {
        pprintf("SET_PARTITION_MODE error %d\n", rc);
    }

    return RTEMS_SUCCESSFUL;
}
