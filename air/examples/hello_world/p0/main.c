/*  Main
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: init.c,v 1.13 2003/09/04 18:53:41 joel Exp $
 */

#define CONFIGURE_INIT

#include <air.h>
#include <rtems/rtems/tasks.h>

#ifdef GTEST                    /* */

#include <stdarg.h>
#include <rtems/bspIo.h>        /* printk */

void pprintf(const char *fmt, ...);

#else

#include <pprintf.h>

#endif /* GTEST */

int andp = 7;


void
entry_point(void)
{

    while (1)
    {
        andp = andp + 1;
        pprintf("\n\n*** RTEMS HELLO WORLD TEST **********\n");
        rtems_task_wake_after(10);
    }
}

#ifdef GTEST                    /* LLLG XXX */

void
pprintf(const char *fmt, ...)
{

    /* format and print string */
    va_list ap;

    va_start(ap, fmt);
    printk(fmt, ap);
    va_end(ap);
}


/*
 * Test network throughput
 */
int
yadayadayada(int value)
{
    printk(">>\t>>\t>> yadayadayada(%d)\n", value);
    return value;
}

#endif /* GTEST LLLG XXX */
