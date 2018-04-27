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
#ifdef RTEMS48I
	#include <pprintf.h>
#endif

int andp=7;


void entry_point(void) {

	while(1)
	{

	  andp=andp+1;
	#ifdef RTEMS48I
	  pprintf( "\n\n*** RTEMS HELLO WORLD TEST **********\n" );
	#else
	  printf( "\n\n*** RTEMS HELLO WORLD TEST **********\n" );
	#endif
	  rtems_task_wake_after(10);

}
//  exit( 0 );

}
