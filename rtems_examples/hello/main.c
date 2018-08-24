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

#include <stdio.h>

int andp = 0;

extern void entry_point(void);

void
entry_point(void)
{

    while (1)
    {
        andp = andp + 1;
        printk("*** RTEMS HELLO WORLD TEST **********\n");
        rtems_task_wake_after(5);
    }
    //  exit( 0 );

}
