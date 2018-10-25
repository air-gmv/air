/** 
 * @file
 * @author pfnf
 * @brief
 */

#include <iop.h>
#include <pprintf.h>
#include <iop_error.h>
 
rtems_status_code iop_raise_error(int error){

    pprintf("IO ERROR (%i)!\n", error);
	//air_syscall_set_partition_mode(-1, IDLE);
	
	return RTEMS_SUCCESSFUL;
}

