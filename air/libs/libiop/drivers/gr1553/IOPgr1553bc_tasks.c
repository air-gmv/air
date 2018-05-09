/** 
 * 	@file IOPgr1553bc_tasks.c
 * 
 *  COPYRIGHT (c) 2011\n
 *  GMV-SKYSOFT\n
 *
 *	@author Cl�udio Silva
 *
 * 	@brief Tasks that manage a gr1553b bus controller
 */


#include <iop_error.h>
#include <iop_support.h>
#include <IOPgr1553bc.h>
#include <IOPmilstd_config.h>
#include <iop_mms.h>

/**
 *  @brief Task reading new 1553 data from the BC
 *
 */
rtems_task grbc_read(iop_physical_device_t *pdev){

    /* return code of several operations */
    rtems_status_code status = RTEMS_SUCCESSFUL;

    /* structure used to communicate with the driver layer */
    libio_rw_args_t rw_args;

    /* Is data valid or should be skipped */
    int skip = 0;

    /* generic iterator */
    int i = 0;

    /* pointer to the reply wrapper being currently handled */
    iop_wrapper_t *reply_wrapper;

    /* maximum number of packets that can be read on a single period*/
    int max = pdev->reads_per_period[air_schedule.current_schedule_index];

    /* Zero out driver interface structure */
    memset(&rw_args, 0, sizeof(libio_rw_args_t));

    for(i=0; i<max; i++){

        if(!skip){
            /* get an empty reply wrapper */
            reply_wrapper = obtain_free_wrapper();
            reply_wrapper->timer = 0;
        }

        skip = 1;

        /* if previous operation succeeded */
        if(reply_wrapper != NULL){

            /*
             * we need to fill the structure we are going to pass to the driver
             * The driver needs to know where the new data and header should be
             * placed
             */
            /* copy pointer to data field */
            reply_wrapper->buffer->header_off = 0;
            reply_wrapper->buffer->header_size = sizeof(milstd_header_t);
            reply_wrapper->buffer->payload_off = sizeof(milstd_header_t);

            rw_args.data = (char *)get_payload(reply_wrapper->buffer);

            /* copy pointer to header field */
            rw_args.hdr = (char *)get_buffer(reply_wrapper->buffer);

            /* This field will be filled by the driver */
            rw_args.data_len = 0;

            /* Header length is always 1 byte*/
            rw_args.hdr_len = 1;

            /* to be filled by the driver */
            rw_args.bytes_moved = 0;

            /* read from milbus */
            status = grbc_process_completed_commands(&rw_args);

            /* check if the read operation was successful */
            if(status == RTEMS_SUCCESSFUL){

                skip = 0;

            } else if (status == RTEMS_TIMEOUT){
                /* the list has ended */
            } else {
                /* this branch is never used */
                /** @todo this error needs to be better evaluated */
                iop_raise_error(HW_READ_ERROR);
            }


            if(!skip){

                /* we have valid data to send to the user! */
                reply_wrapper->buffer->payload_size = rw_args.data_len;

                /* append prefetched reply to physical device's queue */
                iop_chain_append(&pdev->rcvqueue, &reply_wrapper->node);
                reply_wrapper = NULL;
            }

            /* release Wrapper if not used */
            if (reply_wrapper != NULL)
            {
                release_wrapper(reply_wrapper);
            }

        } else{
            iop_raise_error(NO_REPLIES);
        }
    }
}
/**
 *  @brief Task writing new 1553 data to the BC
 *
 */

rtems_task grbc_write(iop_physical_device_t *pdev){

    /* return code of several operations */
    rtems_status_code status = RTEMS_SUCCESSFUL;

    /* Flag to initialise the pointer */
    int init = 0;

    /* Go through all available */
    while(!iop_chain_is_empty(&pdev->sendqueue)){

        iop_debug("GRBC writing\n");
        if (init == 0)
        {
            iop_debug("GRBC erasing\n");
            init = 1;
            // Clean existing commands
            gr1553bc_erase_async_data();
        }
        /* pointer to the request wrapper being currently handled */
        iop_wrapper_t *req_wrapper;

        /* Extract first request wrapper from chain */
        req_wrapper = obtain_wrapper(&pdev->sendqueue);

        /* Extract header */
        iop_header_t* hdr = (iop_header_t*)get_buffer(req_wrapper->buffer);

        iop_debug("BC RT%i SA%i\n",((milstd_header_t*)hdr)->desc, ((milstd_header_t*)hdr)->address);

        /* in a bc write operation we must append incoming data to the correct BC command on the bc list */
        status = gr1553bc_add_async_data(get_payload(req_wrapper->buffer), &hdr->milstd_hdr, get_payload_size(req_wrapper->buffer));

        /* release Wrapper*/
        release_wrapper(req_wrapper);
    }

    if (init == 1)
    {
        iop_debug("GRBC starting\n");
        // Start asynchronous commanding
        gr1553bc_start_async();
    }
}
