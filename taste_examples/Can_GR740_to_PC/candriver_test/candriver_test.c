/* User code: This file will not be overwritten by TASTE. */

#include "candriver_test.h"
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#ifndef M_PI
#define M_PI 3.1416
#endif

static float velocity_ref_deg = 1.f;
static asn1SccT_Int32 cmd;

uint64_t getTimeInMicroseconds()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
}

void candriver_test_startup()
{
    /* Write your initialization code here,
       but do not make any call to a required interface. */
    printf("[candriver_test] startup\n");
}

void candriver_test_PI_samples(const asn1SccT_Int32 *IN_smpls)
{
    /* Write your code here! */
    //float position_rad = IN_smpls->elements.arr[0].position;
    float position_deg = 180.f / M_PI;
    printf("[candriver_test] Current pos: %f\n", position_deg);
    if ((velocity_ref_deg > 0.f) && (position_deg < 90.f))
    {
	velocity_ref_deg = 1.f;
    }
    if ((velocity_ref_deg < 0.f) && (position_deg > 10.f))
    {
	velocity_ref_deg = -1.f;
    }
    if ((velocity_ref_deg > 0.f) && (position_deg > 90.f))
    {
	velocity_ref_deg = -0.5f;
    }
    if ((velocity_ref_deg < 0.f) && (position_deg < 10.f))
    {
	velocity_ref_deg = 0.5f;
    }
    printf("[candriver_test] New velocity reference: %f\n", velocity_ref_deg);

    candriver_test_RI_commands(&cmd);
}

