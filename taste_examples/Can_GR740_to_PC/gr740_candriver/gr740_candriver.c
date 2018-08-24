/* User code: This file will not be overwritten by TASTE. */

#include "gr740_candriver.h"

/*
	IMPORTANT NOTE
	The driver manager is setup by the orchestrator and/or polyorb-HI-C.
        It is therefore needed that the CONFIGURE_DRIVER_AMBAPP_GAISLER_GRCAN has to be passed to the build system.
        Currently, this is done through a so called TASTE Directive which is setup as a contextual parameter.

	IMPORTANT NOTE II:
        Whenever possible do NOT declare local variables of considerable size!!!!
        The default stack size is about 5KB which is easily depleted by e.g. asn1SccBase_commands_Joints.
        So its better to declare them globally and make one for each use/function.
        Otherwise, you should generate a ConcurrencyView.pro and modify the stack sizes there (max sum of sizes: 256KByte)
*/

#ifndef CONFIGURE_DRIVER_AMBAPP_GAISLER_GRCAN
#error "Need GRCAN driver"
#endif

/* RTEMS include files */
#include <bsp/grcan.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>
#ifndef M_PI
#define M_PI 3.1416
#endif

#ifndef CAN_INTERFACE
#define CAN_INTERFACE 0
#endif

#ifndef CAN_BAUDRATE
#define CAN_BAUDRATE 1000000
#endif

/* CAN Channel select */
enum {
	CAN_CHAN_SEL_A,
	CAN_CHAN_SEL_B,
	CAN_CHAN_SEL_NUM
};

#ifndef CAN_CHANNEL
#define CAN_CHANNEL CAN_CHAN_SEL_A
#endif

static void *candev = NULL;
static const struct grcan_selection CAN_CHAN_SEL[CAN_CHAN_SEL_NUM] = {
	{
		/* Channel A */
		.selection = 0,
		.enable0 = 0,
		.enable1 = 1,
	},
	{
		/* Channel B */
		.selection = 1,
		.enable0 = 1,
		.enable1 = 0,
	},
};

/* Driver specific data */
#define CANDRIVER_MAX_READ_ATTEMPTS	10
static bool requestTransmitted = false;
static unsigned responseReadAttempts = 0;

static CANMsg telemetryRequest;
static CANMsg telemetryResponse;
static CANMsg telecommand;

uint64_t getTimeInMicroseconds()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
}

void gr740_candriver_startup()
{
    /* Write your initialization code here,
       but do not make any call to a required interface. */
    int ret;
    int nCANDevices;
    printf("[gr740_candriver_startup] startup\n");

    nCANDevices = grcan_dev_count();
    if (!nCANDevices)
    {
	printf("[gr740_candriver_startup] No CAN devices found\n");
	return;
    }
    if (CAN_INTERFACE >= nCANDevices)
    {
	printf("[gr740_candriver_startup] CAN interface %u not available\n", CAN_INTERFACE);
	return;
    }
    printf("[gr740_candriver_startup] init CAN interface %u\n", CAN_INTERFACE);
    candev = grcan_open(CAN_INTERFACE);
    if (candev == NULL)
    {
	printf("[gr740_candriver_startup] failed to open CAN device %u\n", CAN_INTERFACE);
	return;
    }
    printf("[gr740_candriver_startup] select baudrate %u\n", CAN_BAUDRATE);
    ret = grcan_set_speed(candev, CAN_BAUDRATE);
    if (ret)
    {
	grcan_close(candev);
	printf("[gr740_candriver_startup] failed to set baudrate %u\n", CAN_BAUDRATE);
	return;
    }
    printf("[gr740_candriver_startup] select channel %u\n", CAN_CHANNEL);
    ret = grcan_set_selection(candev, &CAN_CHAN_SEL[CAN_CHANNEL]);
    if (ret)
    {
	grcan_close(candev);
	printf("[gr740_candriver_startup] failed to set channel %u\n", CAN_CHANNEL);
	return;
    }
    printf("[gr740_candriver_startup] enable txcomplete\n");
    ret = grcan_set_txcomplete(candev, 1);
    if (ret)
    {
	grcan_close(candev);
	printf("[gr740_candriver_startup] txcomplete failed\n");
	return;
    }
    printf("[gr740_candriver_startup] disable rxcomplete\n");
    ret = grcan_set_rxcomplete(candev, 0);
    if (ret)
    {
	grcan_close(candev);
	printf("[gr740_candriver_startup] rxcomplete failed\n");
	return;
    }
    printf("[gr740_candriver_startup] enable blocking write\n");
    ret = grcan_set_txblock(candev, 1);
    if (ret)
    {
	grcan_close(candev);
	printf("[gr740_candriver_startup] txblock failed\n");
	return;
    }
    printf("[gr740_candriver_startup] enable non-blocking read\n");
    ret = grcan_set_rxblock(candev, 0);
    if (ret)
    {
	grcan_close(candev);
	printf("[gr740_candriver_startup] rxblock failed\n");
	return;
    }
    printf("[gr740_candriver_startup] reset stats\n");
    ret = grcan_clr_stats(candev);
    if (ret)
    {
	grcan_close(candev);
	printf("[gr740_candriver_startup] clearing stats failed\n");
	return;
    }
    printf("[gr740_candriver_startup] start device\n");
    ret = grcan_start(candev);
    if (ret)
    {
	grcan_close(candev);
	printf("[gr740_candriver_startup] could not start device\n");
	return;
    }
    printf("[gr740_candriver_startup] initialization complete :)\n");

}

void gr740_candriver_PI_update()
{
    int cnt;
    uint16_t position;
    float position_deg;
    float position_rad;
    asn1SccT_Int32 xxx;

    /* 
        Setup CAN message to request sample updates 
	To get telemetry, we have to request it by a CAN message with ID='0x7C0', RTR=1, LEN=0
	Refer to ESROCOS-Bridget CANbus ICD
    */
    if (!requestTransmitted)
    {
        telemetryRequest.extended = 0;
        telemetryRequest.rtr = 1;
        telemetryRequest.id = 0x7C0;
        telemetryRequest.len = 0;
#ifdef DEBUG
        printf("[gr740_candriver_PI_update] Transmitting telemetry request\n");
#endif
        cnt = grcan_write(candev, &telemetryRequest, 1);
        if (!cnt)
        {
            printf("[gr740_candriver_PI_update] Transmission failed\n");
            return;
        }
        requestTransmitted = true;
	responseReadAttempts = 0;
        return;
    }

    /*
	If we could transmit a request we have to wait for the DEVICE to respond.
	According to the ICD, we will get ID='0x1A0' RTR=0 LEN=6 T0,T1,P0,P1,V0,V1 where only P0 and P1 will be populated
	P0,P1 is a position where 0 equals to 0 deg and 0xFFFF equals to 360 deg
    */
    if (requestTransmitted)
    {
        //printf("[gr740_candriver_PI_update] Waiting for telemetry\n");
	cnt = grcan_read(candev, &telemetryResponse, 1);
        if (cnt < 1)
	{
            responseReadAttempts++;
	    if (responseReadAttempts >= CANDRIVER_MAX_READ_ATTEMPTS)
	    {
		printf("[gr740_candriver_PI_update] No response after %u read attempts. Trying to retransmit\n", CANDRIVER_MAX_READ_ATTEMPTS);
		requestTransmitted = false;
	    }
	    return;
	}
#ifdef DEBUG
        printf("[gr740_candriver_PI_update] Got a message. Checking ...\n");
#endif
	if (telemetryResponse.extended != 0)
	{
	    printf("[gr740_candriver_PI_update] Extended ID not supported. Another device present?\n");
	    return;
	}
	if (telemetryResponse.id != 0x1A0)
	{
	    printf("[gr740_candriver_PI_update] Wrong ID. Another device present?\n");
	    return;
	}
	if (telemetryResponse.rtr != 0)
	{
	    printf("[gr740_candriver_PI_update] RTR set. Bad device?\n");
	    return;
	}
	if (telemetryResponse.len != 6)
	{
	    printf("[gr740_candriver_PI_update] Got length %u. Bad device?\n", telemetryResponse.len);
	    return;
	}
	position = telemetryResponse.data[3];
	position <<= 8;
	position += telemetryResponse.data[2];
	position_deg = position * 360.f / 0xFFFF;
        position_rad = position_deg * M_PI / 180.f;
#ifdef DEBUG
        printf("[gr740_candriver_PI_update] Current position: %u In degrees: %f In radians: %f\n", position, position_deg, position_rad);
#endif

	// Call requested interface
	// time->microseconds, names->(nCount,arr->(nCount, arr)), elements->(nCount,arr->(position,speed,effort,raw,acceleration))
	gr740_candriver_RI_samples(&xxx);
    }
}

void gr740_candriver_PI_commands(const asn1SccT_Int32 *IN_cmds)
{
    /*
	Whenever we get an incoming joint command, we have to pass this information to the CAN device.
        According to the ICD, we have to transmit a frame with ID=0x182 RTR=0 LEN=3 MODE,DEMAND_0,DEMAND_1
	MODE is either 0 for idle or 2 vor velocity control
	DEMAND_0,DEMAND_1 is a velocity reference value where 0 means -1 rpm and 0xFFFF means +1 rpm
    */
    
    float velocity_deg;
    float velocity_rad;
    int cnt;
    uint16_t velocity;

    //if (IN_cmds->elements.nCount < 1)
    {
	printf("[gr740_candriver_PI_commands] Expecting at least one valid command entry\n");
	return;
    }
    //velocity_rad = IN_cmds->elements.arr[0].speed;
    velocity_deg = velocity_rad * 180.f / M_PI;
    velocity = velocity_deg * 0xFFFF / 6.f;

    telecommand.extended = 0;
    telecommand.rtr = 0;
    telecommand.id = 0x182;
    telecommand.len = 3;
    telecommand.data[0] = 2; /* Mode: Velocity control */
    telecommand.data[1] = velocity & 0x00FF; /* Velocity 0 */
    telecommand.data[2] = (velocity >> 8) & 0x00FF; /* Velocity 1 */
#ifdef DEBUG
    printf("[gr740_candriver_PI_commands] Transmitting new velocity command with %u\n", velocity);
#endif
    cnt = grcan_write(candev, &telecommand, 1);
    if (!cnt)
    {
        printf("[gr740_candriver_PI_commands] Transmission failed\n");
        return;
    }
}

