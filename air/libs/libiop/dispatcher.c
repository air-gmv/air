/** 
 * @file
 * @author cdcs
 * @brief Contains the dispatcher tasks and auxiliary functions
 *
 * The dispatcher is the IOP component that is responsible for distributing
 * incoming requests through the various logical devices and operations.
 * It is also responsible for sending replies to those requests.
 */  

#include <iop.h>
#include <iop_mms.h>
#include <iop_error.h>
#include <iop_support.h>
#include <dispatcher.h>

/* reference time for window execution time */
extern rtems_interval last_task_ticks;

#ifdef CODE_ON_HOLD
/**
 *  \brief Validates an incomming service request
 *  \param [in] *incoming: pointer to request wrapper to be validated
 *
 *  The values of the service request are validated against possible values
 */
rtems_status_code validate_service_request(iop_wrapper_t *incoming){

    rtems_status_code status = RTEMS_SUCCESSFUL;

    /* check incomming validity *
    if(incoming == NULL){
        status = RTEMS_INVALID_ADDRESS;
        return status;
    }

    /* check operation*
    if((incoming->request.operation != WRITE) &&
       (incoming->request.operation != WRITE_ACK) &&
       (incoming->request.operation != READ)){

        status = RTEMS_NOT_IMPLEMENTED;

    }

    /* check size *
    if(incoming->request.ssize > (sizeof(service_request_t) - REQUEST_HEADER_SIZE)){
        status = RTEMS_TOO_MANY;
    }

    /** @todo validate device in incoming request */

    return status;

}
#endif


#ifdef CODE_ON_HOLD
/**
 *  \brief Dispacthes a request based on the operation and target device
 *  \param [in] *incoming: pointer to request wrapper to be dispatched
 *  \param [in] reply_addr: address to where a possible reply shall be sent
 *
 *  This function will add the incoming request to the chain of the target
 *  device. The chain is chosen based on the operation requested and in the
 *  target device.
 *
 *  @todo remove return code. Validation is made in other function
 */
rtems_status_code process_service_request(iop_wrapper_t *incoming, int reply_addr) {

    rtems_status_code rc = RTEMS_SUCCESSFUL;

    /* check if request is valid */
    if (NULL == incoming){
        return RTEMS_INVALID_ADDRESS;
    }

    /* get service request that is included on our wrapper*
    service_request_t *request = &incoming->request;

    /* get logical device targeted by the request *
    iop_logical_device_t *ldev =
            ((iop_logical_device_t **)
                    usr_configuration.logical_devices.elements)[request->device];





    /* we just received this request, therefore the timer is 0 *
    incoming->timer = 0;

    /* What is the operation requested?*
    switch(request->operation){

        case READ:

            /* we want a reply*
            incoming->doreply = 1;

            /* copy "reply to" address*
            incoming->reply_to = reply_addr;

            /* see if there is pre fetched data *
            if(iop_chain_is_empty(&ldev->rcvqueue)){

                /*There is no data on this device yet, add request to queue*
                iop_chain_append(
                        &ldev->pending_rcvqueue, &incoming->node);

            /* no pre-fetched data *
            } else{

                /* send out pre-fetched reply let's first obtain it*
                reply_wrapper_t *reply_wrapper =
                        extract_reply_wrapper(&ldev->rcvqueue);

                /* append request information to the pre-fetched reply and sent it *
                append_reply(incoming, reply_wrapper);
            }

            break;

        case WRITE:

            /* we don't want a reply *
            incoming->doreply = 0;

            /* append Current request to the send queue of the logical device *
            iop_chain_append(&ldev->sendqueue, &incoming->node);

            break;

        case CONTROL:
        case WRITE_ACK:

            /* we want a reply*
            incoming->doreply = 1;

            /* copy "reply to" address*
            incoming->reply_to = reply_addr;

            /* append current request to send chain of the logical device*
            iop_chain_append(&ldev->sendqueue, &incoming->node);
            break;

        default:
            /** @todo will we provide IOCTL in the request/reply system? *
            rc = RTEMS_NOT_IMPLEMENTED;
            break;
    }
*/
    return rc;
}
#endif


/** 
 *  \brief reads all data from a queuing port that is used by the remote port
 *  system. Read data is placed on a request structure which is initialized 
 *  with default values and then forwarded to the target logical device
 *
 *  Any error found during read is notified to FDIR.
 *  
 *  \param [in] rport: current remote port being processed
 */
static void process_remote_port(iop_port_t *port){

    air_status_code_e rc = AIR_NO_ERROR;
    iop_debug(" IOP :: process_remote_port\n");
    while (rc != AIR_NOT_AVAILABLE) {
        /* get a empty request wrapper from the wrapper chain*/
        iop_wrapper_t *wrapper = obtain_free_wrapper();

        /* check if an empty wrapper was available */
        if (wrapper == NULL) {
            iop_raise_error(NO_REQUESTS);
            break;
        }

        /* get the message space */
        uint8_t *message = (uint8_t *)
                ((uintptr_t)wrapper->buffer->v_addr + sizeof(iop_header_t));

        /* read regular message */
        air_sampling_port_status_t status;
        status.operation = AIR_SAMPLING_MODE_REGULAR;

        /* read from port */
        size_t size;
        rc = air_syscall_read_port(
                port->type,
                (air_identifier_t)port->id,
                (air_message_ptr_t)message,
                (size_t *)&size,
                &status);

        /* if no errors occurred */
        if (rc == AIR_NO_ERROR && size > 0 &&
            (port->type == AIR_QUEUING_PORT ||
            (port->type == AIR_SAMPLING_PORT &&
             status.last_msg_validity == AIR_MESSAGE_VALID))) {
            

            /* setup the wrapper properties */
            wrapper->buffer->payload_off = sizeof(iop_header_t);
            wrapper->buffer->payload_size = size;
            wrapper->buffer->header_off = 0;
            wrapper->buffer->header_size = sizeof(iop_header_t);


            /* append data to aimed device */
            iop_chain_append(
                    &port->device->sendqueue, &wrapper->node);

            /* if this an sampling port, the processing is over */
            if (port->type == AIR_SAMPLING_PORT) {
                rc = AIR_NOT_AVAILABLE;
            }

        } else {
            /* release the wrapper */
            release_wrapper(wrapper);
            rc = AIR_NOT_AVAILABLE;
        }
        rtems_task_wake_after(1);
    }
}

/** 
 *  \brief iterates over all queuing ports that are used by the request/reply
 *   system and receives any incomming requests. These requests are then
 *   dispatched according with their target device and operation.
 *
 *   Each received request is validated against its possible values. If a request
 *   is found not valid and the user requested a ACK, a reply is generated to
 *   inform the user that its request failed. If the user requested a non ACK
 *   service and the request is not valid, the request is simply freed without
 *   infroming the user. 
 *
 **/
static void process_request_port(iop_port_t *port){

    air_status_code_e rc = AIR_NO_ERROR;

    /*
    while (rc != AIR_NOT_AVAILABLE) {

        /* get a empty request wrapper from the wrapper chain*
        request_wrapper_t *incoming_request =
                extract_request_wrapper(&usr_configuration.empty_requests);

        /* check if an empty wrapper was available *
        if (incoming_request == NULL) {
            iop_raise_error(NO_REQUESTS);
            break;
        }

        /* read from port *
        size_t size;
        rc = air_syscall_read_port(
                port->type,
                (air_identifier_t)port->id,
                (air_message_ptr_t)&incoming_request->request.generic_data[0],
                (size_t *)&size,
                NULL);

        /* if no errors occurred *
        if (rc == AIR_NO_ERROR) {

            /* validate service_request before processing it *
            rtems_status_code status =
                    validate_service_request(incoming_request);

            /* if request is valid *
            if (status == RTEMS_SUCCESSFUL) {

                /* dispatch current incoming request *
                process_service_request(incoming_request, port->id);

            /* request is not valid, but the user wants a reply *
            } else if (incoming_request->doreply == 1) {

                /* get a new reply wrapper *
                reply_wrapper_t *reply_wrapper = extract_reply_wrapper(
                                        &usr_configuration.empty_replys);

                /* request is not valid *
                reply_wrapper->reply.status = RTEMS_INVALID_NUMBER;

                /* send Reply and release request *
                append_reply(incoming_request, reply_wrapper);

                /* avoid Releasing the incoming request twice *
                status = RTEMS_SUCCESSFUL;

            /* release request *
            } else {

                /* release request *
                free_request_wrapper(incoming_request);
            }

            /* if this an sampling port, the processing is over *
            if (port->type == AIR_SAMPLING_PORT) {
                 rc = AIR_NOT_AVAILABLE;
            }
        } else {

            /* release request *
            free_request_wrapper(incoming_request);
        }
    }*/
}

/**
 *  @brief Gets service requests and distributes them through the logical devs
 *
 *  This task polls the queueing ports for new service requests and distributes
 *  then accordingly with their target logical device and operation.
 *  It does the same operation for the remote ports.
 *
 *  Since this is the first task to run in each partition release point, it 
 *  also updates the request and reply timers.
 */
void pre_dispatcher(){


    uint32_t i;

	iop_debug("\n :: IOP - pre-dispatcher running!\n");

	/* Get execution window reference time */
    /* this  call is for RTEMS 4.8, it is not deprecated */
//	 rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &last_task_ticks);
    
     /* If we switch to RTEMS 5 use  this one */
        last_task_ticks = rtems_clock_get_ticks_since_boot();
    
	//iop_debug("  :: IOP - pre-dispatcher read this time %d\n", last_task_ticks);

	/* check for schedule changes */
	iop_change_schedule();

	/* update requests and replies timers */
	update_timers();

	/* iterate over all remote ports*/

	for (i = 0; i < usr_configuration.remote_ports.length; ++i){

		/* get port */
		iop_port_t *port = get_remote_port(i);
		/* only receive data from destination ports (linked to a device) */
		if(port->device != NULL){
			/* obtain and process data from current port */
			process_remote_port(port);
		}
	}

#ifdef CODE_ON_HOLD
	/* iterate over all request ports*/
	for (i = 0; i < usr_configuration.request_ports.length; i += 2){
		/* get port */
		iop_port_t *port =
				&((iop_port_t *)
						usr_configuration.request_ports.elements)[i];
		/* obtain request data from current port */
		process_request_port(port);
		iop_debug(" Processed port request\n");
	}
#endif
}

/**
 *  @brief Sends any pending replies through the queuing ports
 *
 *  This task extracts any replies from the pending_replys chain
 *  and send's them to their target partition.
 *  If the send failed, the reply is added to a temporary error
 *  chain. Next time the task runs it will try to send these replies
 *  again. In every retry a timer is incremented. When this timer reaches
 *  a user defined values, the reply is freed.
 */
void pos_dispatcher(){

//    iop_wrapper_t *reply_wrapper;
//    iop_chain_control error;
//    iop_chain_initialize_empty(&error);

    /* main task */
    //iop_debug(" :: IOP - pos-dispatcher start!\n");

    //OLD MAIN LOOP
	/* wait for next partition release point */
	//iop_task_sleep(0);
	

	iop_debug("\n :: IOP - pos-dispatcher doing nothing!\n");

//	rtems_interval time;
//	/*rtems_clock_get(RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &time); // use this for rtems 4.8 */
//  time = rtems_clock_get_ticks_since_boot(); // use this for rtems 5
//	char preamble[] = " pos-dispatcher time: ";
//	append_to_message(&msg_relay, preamble, 26); //52
//	append_time_to_message(&msg_relay, time, 22+26);//22+52
//	iop_debug("Relay at dispatcher: %s\n", msg_relay);


	/* verify if the number of retries was exceeded */
	//update_reply_timers(&error, usr_configuration.time_to_live);

	/* retry to send failed replies: see if there are failed replies *
	while (!iop_chain_is_empty(&error)) {

		/* extract reply from failed reply chain *
		reply_wrapper = extract_reply_wrapper(&error);

		/* append previously failed reply to the pending reply chain *
		iop_chain_append(&usr_configuration.pending_replys,
									&reply_wrapper->node);
	}

	/* while there are replies to be sent in the pending reply chain *
	while (!iop_chain_is_empty(&usr_configuration.pending_replys)) {

		/* Get a reply wrapper from the pending replies chain *
		reply_wrapper = extract_reply_wrapper(&usr_configuration.pending_replys);

		/* send reply *
		int rc = send_reply(reply_wrapper);

		/* there was an error while sending the reply *
		if (rc == 0){

			/* add the reply to the temporary error chain *
			iop_chain_append(&error, &reply_wrapper->node);
		}
	}
	*/
}
