/*
 * Copyright (C) 2011-2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/AIR/raw/master/air/LICENSE
 */
/**
 * @file router.c
 *
 * @author cdcs
 *
 * @brief Tasks and auxiliary functions that route requests between physical
 * and logical devices.
 *
 * The router component in responsible for applying routing information, that
 * is defined by the user in the configuration, to incoming and outgoing packets
 * Predefined headers are appended to outgoing write requests and incoming
 * headers are compared to a routing table to determine to who they belong.
 */

#include <iop.h>
#include <iop_error.h>
#include <iop_support.h>
#include <iop_mms.h>
#include <iop_functions.h>

static void send_remote_reply(iop_wrapper_t *wrapper, iop_port_t *port){


    size_t max_size = 0;
    size_t size = get_payload_size(wrapper->buffer);

    /* get maximum size allowed by the port */
    if (port->type == AIR_QUEUING_PORT) {

        air_queuing_port_configuration_t *config =
                (air_queuing_port_configuration_t *)port->configuration;

        max_size = config->max_message_size;

    } else if (port->type == AIR_SAMPLING_PORT) {

        air_sampling_port_configuration_t *config =
                (air_sampling_port_configuration_t *)port->configuration;

        max_size = config->max_message_size;
    } else {

        /* unsupported port type */
        iop_raise_error(CONFIGURATION_ERROR);
    }

    /* fix maximum size */
    if (size > max_size) {

        /** @todo use max size? is this an error? */
        size = max_size;
    }

    /* send data */
    air_status_code_e rc = air_syscall_write_port(
            port->type,
            port->id,
            (air_message_ptr_t)get_payload(wrapper->buffer),
            (size_t)size,
            (void *)NULL);

    /* check if any errors occurred */
    if (rc != AIR_NO_ERROR && rc != AIR_NOT_AVAILABLE) {
        iop_raise_error(WRITE_ERROR_S);
    }
    if (rc == AIR_NOT_AVAILABLE) {
            iop_debug("AIR not available qport\n");
    } 

}

/**
 *  \brief Applies logical routing information to an incoming request
 *  \param [in] current_dev: device that contains the route to be applied
 * 	\param [in] req_wrapper: request wrapper to be routed
 *
 *  This function applies the routes defined in a logical device to requests of
 *  that same logical device.
 *  The procedure to apply a route to a request is to copy the header of the
 *  route to that request and append it to the target physical device.
 *  If the device which the route is aimed at is ETH0, the headers need to be
 *  completed using the IP stack.
 */
void route_request(iop_logical_device_t *ldev, iop_wrapper_t *wrapper){

    /* check if the input arguments are valid */
    if (ldev == NULL || wrapper == NULL) {

        return;
    }

    /* iterate all routes for this device*/
    int i;
    int replication = 0;
    for (i = 0; i < ldev->routes.length; ++i){

        /* route pointer */
        iop_logical_route_t *route =
                &((iop_logical_route_t *)ldev->routes.elements)[i];

        /* is this route active? */
        if (1 == route->schedule[air_schedule.current_schedule_index]) {

            /* since the route is active we will follow it */
            replication++;

            /*
             * If we have more than one route to follow we need to make a copy
             * of the request for each other route. There will be one copy of
             * the request per route.
             */
            if (replication > 1) {

                /* store original request wrapper*/
                iop_wrapper_t *original = wrapper;

                /* Get a new wrapper*/
                wrapper = obtain_free_wrapper();

                /* check if a new wrapper was actually available */
                if (NULL == wrapper){
                    iop_raise_error(NO_REQUESTS);
                    break;
                }

                /* copy IOP buffer */
                wrapper->timer = original->timer;
                copy_iop_buffer(wrapper->buffer, original->buffer);
            }
			
#ifdef ROUTER_DEBUG
			int j = 0;
			iop_debug("%dWRAPPER->BUFFER: ", j);
			for (j = 0; j <  wrapper->buffer->header_size + wrapper->buffer->payload_size; j++) {
				iop_debug("%c ", *((uint8_t *)wrapper->buffer->v_addr+j));
				if (j == wrapper->buffer->header_size - 1)
					iop_debug(" | size: %d\n", j+1);
			} iop_debug("\nsize: %d\n", j);

			iop_debug("%dROUTER HEADER: ", j);
			can_header_t *can_header = (can_header_t *)route->header;
			iop_debug("ID: %d Ext: %d RTR: %d ", can_header->id, can_header->extended ,can_header->rtr);
			iop_debug("\nsize: %d\n", wrapper->buffer->header_size);
#endif
            /* setup route header */
            route->device->header_copy(route->device, wrapper, route->header);
#ifdef ROUTER_DEBUG			
			iop_debug("%dWRAPPER->BUFFER: ", j);
			for (j = 0; j <  wrapper->buffer->header_size + wrapper->buffer->payload_size; j++) {
				iop_debug("%x ", *((uint8_t *)wrapper->buffer->v_addr+j));
				if (j == wrapper->buffer->header_size - 1)
					iop_debug(" | size: %d\n", j+1);
			} iop_debug("\nsize: %d\n", j);
#endif			
            /* append request with header to the physical device's queue*/
            iop_chain_append(&route->device->sendqueue, &wrapper->node);
        }
    }

    /* free request if no route was available */
    if (0 == replication && wrapper != NULL) {
        release_wrapper(wrapper);
    }
}

/**
 *  \brief Applies physical routing to a reply
 *  \param [in] current_dev: physical device whose routes are to be applied
 * 	\param [in] reply_wrapper: wrapper that contains incomming data, including the header
 *  \param [in] dev: physical_device from where this data comes
 *
 *
 *  This function is used to route data received in a given bus to logical
 *  devices or remote ports. The function iterates over all routes defined
 *  for the bus where the data was received. It checks if the current route
 *  is active and if the route applies to the current data.
 *  A certain route is applicable to a set of data if the data header
 *  coincides with the header defined in the the route.
 *
 *  When the headers coincide, the data is appended to the target logical
 *  device or send directly to queuing port. This destination depends if the
 *  route is aimed at the remote port or requets system.
 *
 *  If no routes are applicable to a set of data, then the data is discarded.
 *	@see compare_headers
 */
void route_reply(iop_physical_device_t *pdev, iop_wrapper_t *wrapper) {

	if (pdev == NULL || wrapper == NULL){
		return;
	}

    /* iterate all routes for this device */
    int i;
    for (i = 0; i < pdev->routes.length; ++i){

        /* route pointer */
        iop_physical_route_t *route =
                &((iop_physical_route_t *)pdev->routes.elements)[i];

        /* is this route active? */
        if (1 == route->schedule[air_schedule.current_schedule_index]) {

            /* Determine if this data is for this route based on the header */
            if (pdev->header_compare(wrapper, route->header)) {

                /* check if this is a static route */
                if (route->port){

                    /* send data */
                    send_remote_reply(wrapper, route->port);
                }
            }
        }
    }

    /* free wrapper */
    release_wrapper(wrapper);
}

/**
 * @brief Task that routes requests
 */
//rtems_task pre_router(rtems_task_argument arg){
void pre_router(){

//	iop_debug("\n :: IOP - pre-router running!\n");

	/* loop through all logical devices */
	int i;
	for (i = 0; i < usr_configuration.logical_devices.length; ++i) {

		/* get logical device */
		iop_logical_device_t *ldev =
				((iop_logical_device_t **)
						usr_configuration.logical_devices.elements)[i];

		/* this logical device has write requests pending */
		while (!iop_chain_is_empty(&ldev->sendqueue)){

			/* extract first request wrapper from send queue */
			iop_wrapper_t *req_wrapper =
					obtain_wrapper(&ldev->sendqueue);



			/* apply routing information to current request */
			route_request(ldev, req_wrapper);
		}
	}
}

/**
 *  @brief Task that routes replies
 *  Translates replies from physical to logical devices
 */
void pos_router(){

	int i;

	iop_debug("\n :: IOP - pos-router running!\n");

	/* iterate over all physical devices */
	for (i = 0; i < usr_configuration.physical_devices.length; ++i) {

		/* get physical device */
		iop_physical_device_t *pdev =
				((iop_physical_device_t **)
						usr_configuration.physical_devices.elements)[i];
               
		/* See if data was received on this device from HW */
		while (!iop_chain_is_empty(&(pdev->rcvqueue))){
			/* extract first request wrapper from send queue */
			iop_wrapper_t *reply_wrapper = obtain_wrapper(&(pdev->rcvqueue));
			/* apply routing information to this data */
			route_reply(pdev, reply_wrapper);

		}
               
	}
}
