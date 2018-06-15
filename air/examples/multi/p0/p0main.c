/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2008-2013
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file 
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE. 
 * ==========================================================================*/

#include <air.h>
#ifdef RTEMS48I
	#include <pprintf.h>
#endif

int producer() {
	
	for (;;)
	{
      #ifdef RTEMS48I
        pprintf("In partition PO loop t20\n");
      #else
        printf("In partition PO loop t20\n");
      #endif
	    rtems_task_wake_after(20);
      #ifdef RTEMS48I
        pprintf("P0 : Waking up\n");
      #else
  	    printf("P0 : Waking up\n");
      #endif
	}
	
	return AIR_SUCCESSFUL;
}


