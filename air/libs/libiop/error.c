/** 
 * @file
 * @author pfnf
 * @brief
 */

#include <iop.h>
#include <iop_error.h>

air_status_code_e iop_raise_error(int error){

    /*TODO Establish the default IOP_ERROR error convertion to air_error_e for HM*/
    iop_debug("IO ERROR (%i)!\n", error);
    air_syscall_raise_hm_event(error);

    return AIR_SUCCESSFUL;
}

