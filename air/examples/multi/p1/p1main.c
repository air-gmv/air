/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2008-2013
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file 
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE. 
 * ==========================================================================*/

#include <air.h>
//#include <pprintf.h>

int receiver() {
	
	for (;;)
	{
	  printf("In partition P1 loop t20\n");
	  rtems_task_wake_after(20);
  	  printf("P1 : Waking up\n");
	}

	return AIR_SUCCESSFUL;
}
