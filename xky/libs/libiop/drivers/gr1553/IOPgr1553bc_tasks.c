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


#include <pprintf.h>


#include <iop.h>
#include <IOPlibio.h>
#include <iop_error.h>
#include <iop_support.h>
#include <IOPgr1553b.h>
#include <IOPgr1553bc.h>
#include <IOPgr1553rt.h>
#include <IOPmilstd_config.h>
#include <iop_mms.h>

static IOP_data_t *internal_data;
static logical_device_t *ldev;
static physical_device_t *pdev;

/**
 *  @brief Task that processes bus controller command lists 
 *	@param [in] arg: minor
 *
 */
rtems_task grbc_manager(rtems_task_argument minor){
	
	/* return code of several operations*/
	rtems_status_code status;
	
	/* pointer to the request wrapper being currently handled */
	request_wrapper_t *req_wrapper;
	
	/* pointer to the reply wrapper being currently handled */
	reply_wrapper_t *reply_wrapper;
	
	/* pointer to current request*/
	service_request_t *req;
	
	/* pointer to "my" device: grb*/
	physical_device_t *grb;
	
	/* Is data valid or should be skipped */
	int skip = 0;
	
	/* generic iterator */
	int i = 0;

	/* pointer to the current schedule index */
	static int *current_schedule;
	
	/* structure used to communicate with the driver layer */
	libio_rw_args_t rw_args;
	
	/* maximum number of packets that can be read on a single period*/
	int max;
	
	/* holds the number of packets that can be read on a single period for all devs */
	reads_per_period_t *read_config;

	
	/****/
	/* Get pointer to the current schedule index */
	current_schedule = &(internal_data->current_schedule);
	
	/* Obtain IOP internal data structures */
	internal_data = iop_get_internal_data();
	
	/*Get IO partition data*/
	ldev = internal_data->ldev;
	pdev = internal_data->pdev;
	
	/* get my physical device structure: GRB0 */
	grb = &pdev[GRB0+minor];
	
	/* Zero out driver interface structure */
	memset(&rw_args, 0, sizeof(libio_rw_args_t));
	
	/* obtain read configuration */
	read_config = iop_get_number_phydev_reads_per_period();
	
	/* Initialize the bus controller list */
	gr1553bc_init_list(minor);
	
	/* start the list */
	gr1553bc_start_list_processing(minor);
	
    /* main task cycle*/
    pprintf(" :: IOP - grbc-manager start!\n");
    for(;;){

        /* wait for next partition release point */
        iop_task_sleep(0);

		/* obtain maximum number of packets that can be read on a single period*/
		max = read_config[*current_schedule][GRB0];
		
		/* pause list */
		gr1553bc_pause_list(minor);
		
		/* Only read up to max packets */
		for(i=0; i<max; i++){
		
			if(!skip){
				/* get an empty reply wrapper */
				reply_wrapper = extract_reply_wrapper(&internal_data->empty_replys);
				reply_wrapper->timer = 0;
			}
			
			skip = 1;
			
			/* if previous operation suceeded */
			if(reply_wrapper != NULL){
			
				/*
				 * we need to fill the structure we are going to pass to the driver
				 * The driver needs to know where the new data and header should be
				 * placed
				 */
				/* copy pointer to data field */
				rw_args.data = (char *)&reply_wrapper->reply.generic_data[0];
				
				/* copy pointer to header field */
				rw_args.hdr = (char *) &reply_wrapper->milstd_hdr;
				
				/* This field will be filled by the driver */
				rw_args.data_len = 0;
				
				/* Header length is always 1 byte*/
				rw_args.hdr_len = 1;
				
				/* to be filled by the driver */
				rw_args.bytes_moved = 0;
				
				/* read from ethernet */
				status = grbc_process_completed_commands(minor, &rw_args);
				
				/* check if the read operation was successful */
				if(status == RTEMS_SUCCESSFUL){
					
					skip = 0;
					
				} else if (status == RTEMS_TIMEOUT){ 
					/* the list has ended */
				} else {
					/* this branch never is used */
					/** @todo this error needs to be better evaluated */
					iop_raise_error(HW_READ_ERROR);
				}
				
				
				if(!skip){

					/* we have valid data to send to the user! */
					reply_wrapper->reply.ssize = rw_args.bytes_moved;
					
					/* read operation was successfull */
					reply_wrapper->reply.status = RTEMS_SUCCESSFUL;
					
					/* append prefetched reply to physical device's queue */
					iop_chain_append(&grb->rcvqueue, &reply_wrapper->node);
				}
				
			} else{
				iop_raise_error(NO_REPLIES);
			}
		}
		
		/* Iterate while we have write requests to be done */
		while(!iop_chain_is_empty(&grb->sendqueue)){
		
			/* Extract first request wrapper from chain */
			req_wrapper = extract_request_wrapper(&grb->sendqueue);
				
			/* Extract request from wrapper */
			req = &req_wrapper->request;
			
			/* @todo freeze list? */
			
			status = RTEMS_SUCCESSFUL;
			
			/* Control and Write operations are processed diferently */
			if(req->operation == CONTROL){
				/* todo */
				
			} else{
				
				/* in a bc write operation we must append incoming data to the correct BC command on the bc list */
				status = grbc_merge_data_with_command(minor, (uint8_t *)&req->generic_data[0], &req_wrapper->milstd_hdr, req->ssize);
				
			}
			
			/* User wants a reply? */
			if(req_wrapper->doreply){
				
				/* Get an empty reply */
				reply_wrapper = extract_reply_wrapper(&internal_data->empty_replys);
				
				/* Copy status and written size*/
				reply_wrapper->reply.status = status;
				reply_wrapper->reply.ssize = 0;
				
				/* append reply to outgoing queue*/
				append_reply(req_wrapper, reply_wrapper);
			
			} else {
					
				/* release Wrapper*/
				free_request_wrapper(req_wrapper);
					
				
			}

		}
		
		/* continue list */
		gr1553bc_continue_list(minor);
	}
}

/**
 *  @brief Task that writes pending write requests to GRBRT
 *	@param [in] arg: device minor number
 *
 *
 *  Obtains write requests from the sendqueue chain and writes them to GRBRT.
 *  In case of a failed write, if the user requested a reply he is informed that
 *  the write failed and the write request is discarded immediately. Is then up
 *  to the user to take the necessary actions.\n
 *  If the user didn't request a reply then the write will be retried until the
 *  request times out.
 */
rtems_task grbrt_writer(rtems_task_argument minor){
	
	/* return code of several operations*/
	rtems_status_code status;
	
	/* pointer to the request wrapper being currently handled */
	request_wrapper_t *req_wrapper;
	
	/* pointer to the reply wrapper being currently handled */
	reply_wrapper_t *reply_wrapper;
	
	/* pointer to current request*/
	service_request_t *req;
	
	/* pointer to "my" device: grbrt*/
	physical_device_t *grbrt;

	
	/* structure used to communicate with the driver layer */
	struct greth_args wargs;
	
	/* Chain used to contain packets which need to be re-sent*/
	iop_chain_control error;
	
	
	/****/
	
	/* Obtain IOP internal data structures */
	internal_data = iop_get_internal_data();
	
	/*Get IO partition data*/
	ldev = internal_data->ldev;
	pdev = internal_data->pdev;
	
	/* get my physical device structure: GRB0 */
	grbrt = &pdev[GRB0+minor];
	
	/* Zero out driver interface structure */
	memset(&wargs, 0, sizeof(struct greth_args));
	
	/* Initialize error retry chain */
	iop_chain_initialize_empty(&error);

    /* main task cycle*/
    pprintf(" :: IOP - grbrt-eriter start!\n");
    for(;;){

        /* wait for next partition release point */
        iop_task_sleep(0);
	
		/* Iterate while we have write requests to be done */
		while(!iop_chain_is_empty(&grbrt->sendqueue)){
			
			/* Extract first request wrapper from chain */
			req_wrapper = extract_request_wrapper(&grbrt->sendqueue);
			
			/* Extract request from wrapper */
			req = &req_wrapper->request;
			
			status = RTEMS_SUCCESSFUL;
			
			/* 
			 * we need to fill the structure we are going to pass to the driver
			 * The driver needs to know where the data and header are and their
			 * sizes.
			 */
			wargs.data = (char *)&req->generic_data[0];
			
			/* copy header pointer*/
			wargs.hdr = (char *)&req_wrapper->milstd_hdr.desc;
			
			/* copy data size */ 
			wargs.data_len = req->ssize;
			
			/* copy header length. MilSTD header is just two chars*/
			wargs.hdr_len = 1;
			
			/* Driver will return in this field how many bytes where written */
			wargs.bytes_moved = 0;
			
			/* Write to the device */
			if(gr1553rt_write(minor, (void *) &wargs) != RTEMS_SUCCESSFUL){
				
				/* we couldn't write, raise an error */
				iop_raise_error(HW_WRITE_ERROR);
				pprintf("Couldn't write request to GRB RT\n");
				status = RTEMS_INTERNAL_ERROR;
			}
			
			/* User wants a reply? */
			if(req_wrapper->doreply){
				
				/* Get an empty reply */
				reply_wrapper = extract_reply_wrapper(&internal_data->empty_replys);
				
				/* Copy status and written size*/
				reply_wrapper->reply.status = status;
				reply_wrapper->reply.ssize = 0;
				
				/* append reply to outgoing queue*/
				append_reply(req_wrapper, reply_wrapper);
			
			} else {
				/* write was successful */
				if(status == RTEMS_SUCCESSFUL){
					
					/* release Wrapper*/
					free_request_wrapper(req_wrapper);
					
				} else{
					
					/* Add the request to the temporary error chain*/
					iop_chain_append(&error, &req_wrapper->node);
				
				}
			}
		}
		
		/* 
		 * copy failed requests back to the sendqueue for retrial
		 * The requests that cannot be sent will eventually timeout.
		 */
		while(!iop_chain_is_empty(&error)){
			
			/* extract request from failed request chain */
			req_wrapper = extract_request_wrapper(&error);
			
			/* append previously failed request to the send chain */
			iop_chain_append(&grbrt->sendqueue,
										&req_wrapper->node);
		}
	}
}


/**
 *  @brief Task that polls grb rt for new data packets
 *  @param [in] arg: not used
 *
 *  This tasks polls for new data and places it on a reply structure.
 *  Data is validated against acceptable values.
 */
rtems_task grbrt_reader(rtems_task_argument minor){
	
	/* Is data valid or should be skipped */
	int skip = 0;
	
	/* return code of several operations*/
	rtems_status_code status;
	
	/* pointer to the reply wrapper currently being handled */
	reply_wrapper_t *reply_wrapper;
	
	/* pointer to this task's device: grb */
	physical_device_t *grbrt;
	
	/* structure used to communicate with the driver layer */
	struct greth_args rw_args;
	
	/* iterator */
	int i;
	
	/* maximum number of packets that can be read on a single period*/
	int max;

	/* pointer to the current schedule index */
	static int *current_schedule;
	
	/* holds the number of packets that can be read on a single period for all devs */
	reads_per_period_t *read_config;
	
	
	/*******/
	/* Get pointer to the current schedule index */
	current_schedule = &(internal_data->current_schedule);
	
	/* Get IO partition data*/
	ldev = internal_data->ldev;
	pdev = internal_data->pdev;
	
	/* get my physical device structure: GRB0 */
	grbrt = &pdev[GRB0+minor];
	
	/* Zero out driver interface structure */
	memset(&rw_args, 0, sizeof(struct greth_args));
	
	/* obtain read configuration */
	read_config = iop_get_number_phydev_reads_per_period();

    /* main task cycle*/
    pprintf(" :: IOP - grbrt-reader start!\n");
    for(;;){

        /* wait for next partition release point */
        iop_task_sleep(0);

		/* obtain maximum number of packets that can be read on a single period*/
		max = read_config[*current_schedule][GRB0+minor];
	
		/* Only read up to max packets */
		for(i=0; i<max; i++){
		
			if(!skip){
				/* get an empty reply wrapper */
				reply_wrapper = extract_reply_wrapper(&internal_data->empty_replys);
				reply_wrapper->timer = 0;
			}
			
			skip = 1;
			
			/* if previous operation suceeded */
			if(reply_wrapper != NULL){
			
				/*
				 * we need to fill the structure we are going to pass to the driver
				 * The driver needs to know where the new data and header should be
				 * placed
				 */
				/* copy pointer to data field */
				rw_args.data = (char *)&reply_wrapper->reply.generic_data[0];
				
				/* copy pointer to header field */
				rw_args.hdr = (char *) &reply_wrapper->milstd_hdr.desc;
				
				/* This field will be filled by the driver */
				rw_args.data_len = 0;
				
				/* Header length is always 1 byte*/
				rw_args.hdr_len = 1;
				
				/* to be filled by the driver */
				rw_args.bytes_moved = 0;
				
				/* read from ethernet */
				status = gr1553rt_read(minor,(void *) &rw_args);
				
				/* check if the read operation was successful */
				if(status == RTEMS_SUCCESSFUL){
					
					skip = 0;
					
				} else {
				
					/** @todo this error needs to be better evaluated */
					iop_raise_error(HW_READ_ERROR);
				}
				
				
				if(!skip){

					/* we have valid data to send to the user!*/
					reply_wrapper->reply.ssize = rw_args.bytes_moved;
					
					/* read operation was successfull*/
					reply_wrapper->reply.status = RTEMS_SUCCESSFUL;
					
					/* In RT mode, the RT address is not used for routing*/
					reply_wrapper->milstd_hdr.address = 0;
					
					/*append prefetched reply to physical device's queue*/
					iop_chain_append(&grbrt->rcvqueue, &reply_wrapper->node);
				}
				
			} else{
				iop_raise_error(NO_REPLIES);
			}
		}
	}
}
