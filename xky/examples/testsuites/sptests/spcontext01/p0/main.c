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
#include <stdio.h>
#include <stdlib.h>
#include <xky.h>
#include <pprintf.h>

int andp=7;


void entry_point(void) {

while(1)
{

  andp=andp+1;
  pprintf( "\n\n*** RTEMS HELLO WORLD TEST **********\n" );

//  printf( "\n\n*** RTEMS HELLO WORLD TEST **********\n" );
//  printf( "Hello World (:\n" );
//  printf( "*** END OF RTEMS HELLO WORLD TEST ***\n" );

}
//  exit( 0 );

}
