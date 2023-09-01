/*
 * Copyright (C) 2011-2018  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */

#include <air.h>
#include <rtems.h>

#ifdef RTEMS48I
#include <pprintf.h>
#else
#include <stdio.h>
#include <string.h>
#if defined(printf)
#else
#include <pprintf.h>
#endif
#endif

#ifndef RTEMS48I
static char *my_ctime(time_t t)
{
    static char b[32];
    ctime_r(&t, b);
    b[strlen(b) - 1] = '\0';
    return b;
}
#endif

void entry_point(void)
{
    struct timespec start;
    int ticks_per_sec = 1000000 / air_syscall_get_us_per_tick();

    for (int i = 0; i < 10; i++)
    {
#ifdef RTEMS48I
        rtems_clock_get_uptime(&start);

        pprintf("\n\n*** RTEMS48I HELLO WORLD TEST **** PART1 ***\n");
        pprintf("Time is %ld:%9ld", (long)start.tv_sec, start.tv_nsec);
#else
        clock_gettime(CLOCK_MONOTONIC, &start);

        (void)printf("\n\n*** RTEMS5 HELLO WORLD TEST **** PART1 ***\n");
        (void)printf("Time is : %s:%9ld\n", my_ctime(start.tv_sec), start.tv_nsec);
#endif

        rtems_task_wake_after(ticks_per_sec / 10);
    }
}
