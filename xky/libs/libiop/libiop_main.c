#include <iop.h>
#include <iop_support.h>

/**
 *  \return RTEMS_SUCCESSFUL if the operation was completed correctly
 *  or the status_code returned by rtems_task_create or rtems_task_start
 *
 *  \brief Initializes IOP tasks
 *
 */

void iop_main_loop(void){

    iop_debug(" :: creating & launching worker tasks\n");

    int i;

    rtems_interval begin, predi, prero, write,
		readt, posdi, posro;

    iop_physical_device_t *devs[usr_configuration.physical_devices.length];
    for(i =0; i < usr_configuration.physical_devices.length; i++){
    	devs[i] = get_physical_device(i);
    }

    for(;;)
    {

    	pre_dispatcher();
    	pre_router();

		/* run all the device drivers writer and reader functions */
    	for(i = 0; i < usr_configuration.physical_devices.length; i++){
    		devs[i]->reader_task(devs[i]);
    		devs[i]->writer_task(devs[i]);

    	}
    	pos_dispatcher();
    	pos_router();
    }

    return RTEMS_SUCCESSFUL;
}
