/**
 *  @file 
 *  @brief This file contains the driver for the Gaisler's MIL-STD1553B interface
 *
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT
 *
 *  @author Cl�udio Silva
 *
 */
 
/*  GR1553B BC driver
 *
 *  COPYRIGHT (c) 2010.
 *  Aeroflex Gaisler.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  2010-03-15, Daniel Hellstrom <daniel@gaisler.com>
 *    Created
 *
 *
 * OVERVIEW
 * ========
 *  See header file and list header file.
 *
 */


#include <iop_error.h>
#include <IOPgr1553b.h>
#include <IOPgr1553bc.h>
#include <IOPmilstd_config.h>
#include <IOPdriverconfig_interface.h>
#include <bsp.h>

/** returns the first address found after p that is aligned with c  */
#define MEM_ALIGN(p,c) ((((unsigned int)(p))+((c)-1)) & ~((c)-1))

static grb_priv *bdevs;

/** @todo this should not be here !!!!*/
static write_cmd_shortcut_t shortcut_mem[128];
static int shortcut_offset = 0;

/* Reset software and BC hardware into a known "unused/init" state */
void gr1553bc_device_init(grb_priv *priv)
{
	/* obtain device private structures (used by other functions )*/
	bdevs = priv;

	/* RESET HARDWARE REGISTERS */
	/* Stop BC if not already stopped */
	GR1553BC_WRITE_REG(&priv->regs->bc_ctrl, GR1553BC_KEY | 0x0204);

	/* 
	 * Since RT can not be used at the same time as BC, we stop 
	 * RT rx, it should already be stopped...
	 */
	GR1553BC_WRITE_REG(&priv->regs->rt_cfg, GR1553RT_KEY);

	/* Clear all BC register registers */
	GR1553BC_WRITE_REG(&priv->regs->bc_bd, 0);
	GR1553BC_WRITE_REG(&priv->regs->bc_abd, 0);
	GR1553BC_WRITE_REG(&priv->regs->bc_timer, 0);
	GR1553BC_WRITE_REG(&priv->regs->bc_wake, 0);
	GR1553BC_WRITE_REG(&priv->regs->bc_irqptr, 0);
	GR1553BC_WRITE_REG(&priv->regs->bc_busmsk, 0);

	/* Clear IRQ log pointer */
	GR1553BC_WRITE_REG(&priv->regs->bc_irqptr,0);

	/* Initialize the bus controller list */
	gr1553bc_init_list();

	/* start the list */
	gr1553bc_start_sync();
}


void gr1553bc_close(grb_priv *priv)
{
	/* Stop Hardware */
	gr1553bc_stop(priv, 0x3);

	/* Reset the device */
	gr1553bc_device_uninit(priv);

}


/* Stop BC lists */
void gr1553bc_stop(grb_priv *priv, int options)
{
	/* Control register contents */
	uint32_t ctrl;

	/* Write BC key */
	ctrl = GR1553BC_KEY;

	/* Verify which lists we have to stop processing */
	if(options & 0x1){

		/* Stop synchronous list */
		ctrl |= GR1553B_BC_ACT_SCSTP;
	}

	if(options & 0x2){

		/* Stop assynchronous list */
		ctrl |= GR1553B_BC_ACT_ASSTP;
	}

	/* Write new ctrl register to stop the list processing */
	GR1553BC_WRITE_REG(&priv->regs->bc_ctrl, ctrl);

	/* indicate that the device is stoped */
	priv->open = 0;

}

void gr1553bc_device_uninit(grb_priv *priv)
{
	/* Stop BC if not already stopped */
	GR1553BC_WRITE_REG(&priv->regs->bc_ctrl, GR1553BC_KEY | 0x0204);

	/* 
	 * Since RT can not be used at the same time as BC, we stop 
	 * RT rx, it should already be stopped...
	 */
	GR1553BC_WRITE_REG(&priv->regs->rt_cfg, GR1553RT_KEY);

	/* Turn off IRQ generation */
	GR1553BC_WRITE_REG(&priv->regs->irq, 0);
}

/* Pause GR1553 BC transfers */
void gr1553bc_pause_list()
{
	uint32_t ctrl;
	grb_priv *priv;

	/* current minor device internal data structure */
	priv = bdevs;

	/* Suspend list processing */
	ctrl = GR1553BC_KEY | GR1553B_BC_ACT_SCSUS;

	/* write action register */
	GR1553BC_WRITE_REG(&priv->regs->bc_ctrl, ctrl);

}

/* Restart GR1553 BC transfers, after being paused */
void gr1553bc_continue_list()
{
	uint32_t ctrl;
	grb_priv *priv;

	/* current minor device internal data structure */
	priv = bdevs;

	/* restart list processing  */
	ctrl = GR1553BC_KEY | GR1553B_BC_ACT_SCSRT;

	/* write action register */
	GR1553BC_WRITE_REG(&priv->regs->bc_ctrl, ctrl);

}


void *find_shortcut(iop_chain_control *rtaddress_chain, milstd_header_t *hdr, unsigned int size){
	
	write_cmd_shortcut_t *shortcut;
	void *rc = NULL;
	
	/* verify if there any shortcuts for this rtaddress */
	if(!iop_chain_is_empty(rtaddress_chain)){
		
		/* get first shortcut */
		shortcut = (write_cmd_shortcut_t *)(iop_chain_head(rtaddress_chain)->next);
		
		/* iterate over every available shortcut */
		while(shortcut != (write_cmd_shortcut_t *)iop_chain_tail(rtaddress_chain)){
			
			if((shortcut->subaddr == hdr->address) &&
			   (shortcut->wcmc == (size/2))){
			 
				rc = shortcut->cmd_ptr;
			}
			
			
			shortcut = (write_cmd_shortcut_t *)shortcut->node.next;
		}
	
	} else{
	
		/* No shortcut available */
		rc = NULL;
	}
	
	return rc;
}

void create_write_cmd_shortcut(iop_chain_control *rtaddress_chain, bc_command_t *ul, void *desc)
{
	write_cmd_shortcut_t *shortcut;
	
	/* verify if we depleted the available shortcuts */
	if(shortcut_offset == 128){
	
		/* no more shortcuts, this was not configured correclty */
		iop_raise_error(CONFIGURATION_ERROR);
	}
	
	/* obtain empty shortcut */
	shortcut = &shortcut_mem[shortcut_offset];
	
	/* next shortcut*/
	shortcut_offset++;
	
	/* fill subaddres */
	shortcut->subaddr = (ul->subaddr[0] & 0x1f);
	
	/* fill wordcount/mode code */
	shortcut->wcmc = (ul->ccw & 0x1f);
	
	/* insert pointer to the bc command which has the above addresses*/
	shortcut->cmd_ptr = desc;
	
	/* append data to aimed device */
	iop_chain_append(rtaddress_chain,
							  &shortcut->node);
	
}

static inline uint32_t detect_bc_to_rt(bc_command_t *ul)
{
	
	uint32_t rc = 0;
	
	/*  detect transfers which envolve data originating from the BC*/
	if((((ul->ccw >> 12) & 0x1) == 0) && ((ul->subaddr[1] == 0) ||
	      (ul->subaddr[0] == 0) || (ul->subaddr[0] == 31))){
		  
		  /* this is a bc to rt transfer */
		  rc = 1;
	}
	
	return rc;
}

static inline uint32_t create_descriptor_word0(int exttrig, int exclusive, 
											   int retrymode, int nretry, 
											   int gap, int time)
{
	uint32_t word = 0;
	
	/* external trigger, exclusive time slot, retry mode, number of retries */
	word = (((exttrig & 0x1) << 30) | ((exclusive & 0x1) << 29) |
		   ((retrymode & 0x3) << 23) | ((nretry & 0x7) << 20));
	
	/* does this transfer use extra timeout values? */
	if(gap > 0){
		/* activate extra time*/
		word |= (1 << 18);
	} 
	
	/* insert slot time specified by the user */
	word |= ((time/4) & GR1553BC_TR_TIME);
	
	return word;
	
}

static inline uint32_t create_descriptor_word1(bc_command_t *ul, int timeout, int *bcrt)
{
	uint32_t word = 0;
	
	/* detect transfers which envolve data originating from the BC */
	if(detect_bc_to_rt(ul) == 1){
		
		*bcrt = 1;
		
		/* every outgoing transfer is a dummy until we have data to be sent */
		word |= GR1553BC_TR_DUMMY_1;
	}
	
	
	/* bus selection bit */
	word |= ((((ul->ccw) >> 13) & 0x1) << 30);
	
	/* RT timeout */
	word |= ((timeout & 0xF) << 26);
	
	/* rt address 2*/
	word |= ((ul->rtaddr[1] & 0x1F) << 21);
	
	/* rt subaddress 2*/
	word |= ((ul->subaddr[1] & 0x1F) << 16);
	
	/* rt address 1*/
	word |= ((ul->rtaddr[0] & 0x1F) << 11);
	
	/* transmit/receive bit */
	word |= (((ul->ccw >> 12) & 0x1) << 10);
	
	/* rt subaddress 1*/
	word |= ((ul->subaddr[0] & 0x1F) << 5);
	
	/* mode code / word count */
	word |= ((ul->ccw & 0x1F));
	
	return word;
}

static inline air_status_code_e bc_verify_command_status(uint32_t sw)
{	
	air_status_code_e status = AIR_SUCCESSFUL;
	
	/* verify transfer status */
	if((sw & 0x7) != 0){
		
		status = AIR_INTERNAL_ERROR;
		
		/* something has gone wrong with the transfer */
		iop_raise_error(HW_PROBLEM);
	}
	
	return status;
}

static inline void dummy_command(volatile uint32_t *cw)
{
	/* activate dummy bit */
	(*cw) |= GR1553BC_TR_DUMMY_1;
	
}

static inline void clear_acessed_bit(volatile uint32_t *sw)
{
	/* activate dummy bit */
	(*sw) |= GR1553BC_STATUS_ACESSED;
	
}
 

static int update_command(struct gr1553bc_bd_tr *desc, libio_rw_args_t *rw_args){
	
	int rc = 0;
	
	/* transmit receive bit */
	uint8_t trbit;
	
	/* rt subaddress 1 */
	uint8_t subaddress1;
	
	/* rt subaddress 2 */
	uint8_t subaddress2;
	
	/*word count / mode code */
	uint8_t wcmc;
	
	/* milstd header */
	milstd_header_t *hdr;

	trbit = ((desc->settings[1] >> 10) & 0x1);
	
	subaddress2 = ((desc->settings[1] >> 16) & 0x1f);
	
	subaddress1 =((desc->settings[1] >> 5) & 0x1f);
	
	wcmc = (desc->settings[1] & 0x1f);
	
	/*  detect transfers which envolve data originating from the BC*/
	if((trbit == 0) && ((subaddress2 == 0) ||
	  (subaddress1 == 0) || (subaddress1 == 31))){
		  
		/* this is a BC-RT command. Verify if data was sent correctly  */
		if(bc_verify_command_status(desc->status) == AIR_SUCCESSFUL){
			
			/* we shall wait for new data from the user. dummy this command */
			dummy_command(&desc->settings[1]);
		}
		/* this descriptor is updated. mark it as not accessed */
		clear_acessed_bit(&(desc->status));
		
	} else{ /*< there is data to be read */
	
		/*  Verify if data was received correctly  */
		if(bc_verify_command_status(desc->status) == AIR_SUCCESSFUL){
			
			/* detect mode code */
			if((subaddress1 == 0) || (subaddress1 == 31)){
				
				/* mode codes 16/18/19 transmit one data word */
				if((wcmc == 16) || (wcmc == 18) || (wcmc == 19)){
					
					/* copy received data to user buffer */
					memcpy(rw_args->data, (void *)get_virtual_addr(desc->dptr), 2);
					
					/* header length */
					rw_args->hdr_len = 1;
					
					/* data length */
					rw_args->data_len = 2;
					
					/* data length */
					rw_args->bytes_moved = 2;
					
					/* copy milstd header */
					hdr = (milstd_header_t *)rw_args->hdr;
					
					/* RT address 1 */
					hdr->desc = ((desc->settings[1] >> 11) & 0x1f);
					
					/* Sub address 1 */
					hdr->address = subaddress1;
					
					/* data was successfuly read*/
					rc = 1;
					
				}
			
			} else {
			
				/* word count of zero really means 32 words */
				if(wcmc == 0){
				
					wcmc = 32;
				}
				
				/* copy received data to user buffer */
				memcpy(rw_args->data, (void *)get_virtual_addr(desc->dptr), wcmc*2);
				
				/* header length */
				rw_args->hdr_len = sizeof(milstd_header_t);
				
				/* data length */
				rw_args->data_len = wcmc*2;
				
				/* data length */
				rw_args->bytes_moved = wcmc*2;
				
				/* copy milstd header */
				hdr = (milstd_header_t *)rw_args->hdr;
				
				/* RT address 1 */
				hdr->desc = ((desc->settings[1] >> 11) & 0x1f);
				
				/* Sub address 1 */
				hdr->address = subaddress1;
				
				/* data was successfuly read*/
				rc = 1;
				
			}
			
		}
		
		/* this descriptor is updated. mark it as not accessed */
		clear_acessed_bit(&(desc->status));
		
	}
	
	return rc;
}

/**
 * @brief Iterates through a BC command list, extracts any data a does maintenance operations
 *
 * @param [in,out] rw_args Input/Output buffer and respective sizes
 *
 * @return status of the operation:
 *  	- AIR_SUCCESSFUL: Data was successfully read.
 * 		- AIR_TIMED_OUT: End of list.
 *
 * This function will obtain the list that is currently being processed by
 * the bc core and will process the already completed commands. Each command is
 * checked for errors and available data. Depending on the command type, some 
 * commands will be updated, others will have their data extracted. The function 
 * returns when there is data available to be read or when the command list as 
 * reached its end. The function starts processing the list in a previously read 
 * position. It then follows the branching structure of the list. 
 */
air_status_code_e grbc_process_completed_commands(libio_rw_args_t *rw_args)
{
	/* return code */
	air_status_code_e status;
	
	/* Current device */
	grb_priv *bDev;
	
	/* indicates if data was sucessfully read or if the list has ended */
	int end = 0;
	
	/* current bc descriptor */
	struct gr1553bc_bd_tr *current;
	
	/* current minor device internal data structure */
	bDev = bdevs;
	
	status = AIR_SUCCESSFUL;
	
	/* @todo place a limit in the number of reads... */
	while(end == 0){
	
		/* next descriptor/command to be read */
		current = bDev->last_read;
		
		/* detect if this is a branch or a data transfer command */
		if(((current->settings[0] & GR1553BC_BD_TYPE) >> 31) == 0){
		
			/* verify if this descriptor was acessed by the hw */
			if(((current->status & GR1553BC_STATUS_ACESSED) >> 31) == 1){
			
				/* this descriptor was not acessed. try again later */
				status = AIR_TIMED_OUT; 
				
				/* we reached the end of the command list */
				end = 1;
				
			} else{
				
				/* descriptor was acessed, so obtain any data and check for errors */
				if(update_command(current, rw_args) == 1){
				
					/* data was successfully read */
					end = 1;
				}
				
				/* update last_read */
				bDev->last_read = current + 1;
			
			}
			
		} else { /*< branch */
		
			/*follow branch and update last_read */
			bDev->last_read = (struct gr1553bc_bd_tr *) get_virtual_addr(current->settings[1]);
			
		}
		
	}
	
	return status;
}

/** 
 * @brief Appends incoming data to a milstd command belonging to a bc command list.
 *
 * @param [in] data input data buffer
 * @param [in] hdr Mil-std header comprising a RT address and a subaddress
 * @param [in] size data buffer length in bytes
 *
 * @return air_status_code_e; status of the operation:
 *  	- AIR_SUCCESSFUL: Data was successfully written to the list
 * 		- AIR_NOT_AVAILABLE: this data is not linked to any command in the list
 *
 * This function inserts incoming user data in the bc command list. For that
 * purpose it uses a shortcut mapping that matches every command in the list
 * with a milstd header.  The header in the incoming data is used as a key
 * to that mapping, which yields the data transfer command affecting that header. 
 * This header is composed by one RT address and one subaddress.
 *
 */
air_status_code_e grbc_merge_data_with_command(uint8_t *data, milstd_header_t *hdr, uint32_t size)
{
	
	/* return code */
	air_status_code_e status;
	
	/* Current device */
	grb_priv *bDev = bdevs;
	
	/* Current descriptor */
	struct gr1553bc_bd_tr *desc;
	
	iop_chain_control *aux = &bDev->shortcut[(hdr->desc & 0x1f)];
	
	/* find if this data request can be matched to any descriptor */
	desc = (struct gr1553bc_bd_tr *)find_shortcut(aux, hdr, size);
	
	/* verify if we obtained a valid descriptor */
	if(desc == NULL){
		
		/* no descriptor was obatined */
		status = AIR_NOT_AVAILABLE;
	
	} else{
	
		/* lets copy the data */
		memcpy((void *)get_virtual_addr(desc->dptr), (void *)data, size);
		
		/* and enable the descriptor. Clear dummy bit */
		desc->settings[1] &= ~GR1553BC_TR_DUMMY_1;
		
		/* set not acessed bit in status word*/
		desc->status |= (1<<31);
		
		/* data was appended sucessfully */
		status = AIR_SUCCESSFUL;
	}
	
	return status;
}

/**
 * @brief Erase the commands belonging to a bc async command list.
 *
 * @return air_status_code_e; status of the operation:
 *      - AIR_SUCCESSFUL: Data was successfully erased
 *      - AIR_DEVICE_ERROR: the BC did not finish executing all the previous async list
 *
 * This function removes the previous asynchronous command list and the asynchronous buffer.
 *
 */
air_status_code_e gr1553bc_erase_async_data()
{
    /* Check if the asynchronous list is still running */
    if((GR1553BC_READ_REG(&bdevs->regs->bc_ctrl) & GR1553B_BC_ACT_ASSRT) != 0){
        /* This is not necessarily a failure as this does not prevent appending new data
         * but may require special handling */
        iop_raise_error(TIME_ERROR);
        return AIR_DEVICE_ERROR;
    }

    iop_debug("Erasing 1553 asynchronous list\n");

    /* Erase the asynchronous blocks */
    memset(bdevs->async, 0, iop_milstd_get_async_command_list_size() * 4 * 4);
    memset(bdevs->async_buf_mem_start, 0, iop_milstd_get_data_buffers_size() * 16);

    return AIR_SUCCESSFUL;
}
/**
 * @brief Appends incoming data to a milstd command belonging to a bc async command list.
 *
 * @param [in] data input data buffer
 * @param [in] hdr Mil-std header comprising a RT address and a subaddress
 * @param [in] size data buffer length in bytes
 *
 * @return air_status_code_e; status of the operation:
 *      - AIR_SUCCESSFUL: Data was successfully written to the list
 *      - AIR_INVALID_SIZE: Size has to be maximum 32 words (one transaction)
 *      - AIR_NOT_AVAILABLE: Too many commands in the list
 *
 * This function inserts incoming user data in the bc async command list. When inserting the
 * first data, the previous data needs to be erased through gr1553bc_erase_async_data. This
 * function will add the data in the available buffer until reaching the end of the buffer.
 * The maximum number of time this function can be called before calling
 * gr1553bc_erase_async_data is defined by ASYNCHRONOUS_COMMAND_LIST_SIZE.
 *
 */
air_status_code_e gr1553bc_add_async_data(uint8_t *data, milstd_header_t *hdr, uint32_t size)
{
    int i;
    int wcmc;
    void *buffer;

    /* Verify that the data fits in one slot */
    if(size > 64){
        iop_raise_error(WRITE_ERROR_Q);
        return AIR_INVALID_SIZE;
    }

    /* In case the size is odd, the single byte should still be sent
     * 0 means 32 words */
    wcmc = ((size + 1) / 2) % 32;

    /* Find the next available slot in the asynchronous command list */
    for(i = 0; i < iop_milstd_get_async_command_list_size(); i++){
        /* An empty list should have a null pointer*/
        if(bdevs->async[i].dptr == 0){
            break;
        }
    }

    /* If the end of the list is reached then there is no available slot */
    if(i == iop_milstd_get_async_command_list_size()){
        iop_raise_error(QUEUE_OVERFLOW);
        return AIR_NOT_AVAILABLE;
    }

    iop_debug("Adding %i words to RT %i and SA %i\n",wcmc,hdr->desc,hdr->address);
    /* Current bc descriptor */
    struct gr1553bc_bd_tr *desc;

    /* Map the memory to a transfer descriptor */
    desc = &bdevs->async[i];

    /* Retry alternating both buses. Slot time is set to 0 */
    desc->settings[0] = ((0x1) << 23);

    /* Bus A selected, transmitting to RT 'desc' and subaddress 'address' with 'wcmc' words */
    desc->settings[1] = ((hdr->desc & 0x1F) << 11) + ((hdr->address & 0x1F) << 5) + wcmc;

    /* Get the buffer's location */
    buffer = &bdevs->async_buf_mem_start[i][0];

    /* add dptr */
    desc->dptr = (uint32_t) air_syscall_get_physical_addr((uintptr_t)buffer);

    /* lets copy the data */
    memcpy(buffer, (void *)data, size);

    return AIR_SUCCESSFUL;
}

static void translate_command(grb_priv *priv, unsigned int offset, unsigned int *data_offset)
{
	uint32_t word0 = 0;
	uint32_t word1 = 0;
	uint32_t dptr = 0;
	uint8_t nret = 0;
	uint8_t excl = 0;
	uint8_t wtrig = 0;
	int bc_to_rt = 0;
	
	/* pointer to used defined command list */
	bc_command_t *user_list;
	
	/* Current bc descriptor */
	struct gr1553bc_bd_tr *desc;
	
	/* Get list of matching physical/virtual addresses*/
	gr1553hwaddr *gr1553hwlist = iop_get_gr1553hwlist();

	/* user command list*/
	user_list = &priv->cl[offset];
	
	/* Map the memory to a transfer descriptor */
	desc = &priv->sync[offset];
	
	/* Verify if this is non data transfer command */
	if((user_list->ccw) & TB_BIT){
		
		/* Unconditional Branch */
		word0 = GR1553BC_UNCOND_JMP;
		
		/* Calculate branch address */
		gr1553hwlist[offset].v_addr = (uint32_t) (&priv->sync[0] + (user_list->branch_offset));
		gr1553hwlist[offset].p_addr = (uint32_t) air_syscall_get_physical_addr((uintptr_t)gr1553hwlist[offset].v_addr);
		word1 = gr1553hwlist[offset].p_addr;
		
		/* No data pointer */
		dptr = (uint32_t) NULL;
	
	} else if((user_list->ccw) & END_BIT){ /*< End the list */
		
		/* End of list marker */
		word0 = 0x800000FF;
		
		/* Clear jump address */
		word1 = 0;
		
		/* No data pointer */
		dptr = (uint32_t) NULL;
		
		gr1553hwlist[offset].v_addr = 0;
		gr1553hwlist[offset].p_addr = 0;
		
	} else { /*< Transfer */
	
		/* Get number of retries */
		nret = (((user_list->ccw) & NRET_BITMASK) >> 8);

		/* Exclusive time slot */
		excl = (((user_list->ccw) & EXCL_BIT) >> 16);

		/* Wait for external trigger */
		wtrig = (((user_list->ccw) & ETRIG_BIT) >> 17);
	
		/* Create the first descriptor word */
		word0 = create_descriptor_word0(wtrig, excl, 
										priv->user_config->retry_mode,
										nret, priv->user_config->msg_timeout,
										user_list->time_slot);
		
		/* Create the second descriptor word */
		word1 = create_descriptor_word1(user_list, priv->user_config->msg_timeout, &bc_to_rt);
		
		/* Create a shortcut to easily access the entry in the command list */
		if(bc_to_rt == 1){
		
			/* 
			 * a shortcut maps between rtaddress, subaddress, wc and the 
			 * current command. This shortcut mapping is used when new outgoing
			 * data arrives  
			 */
           		 if(!(user_list->ccw & DUMMY_BIT))
				create_write_cmd_shortcut(&priv->shortcut[(user_list->rtaddr[0] & 0x1f)],
									user_list, (void *) desc);

		}
		
		/* Calculate data buffer position */
		gr1553hwlist[offset].v_addr = (uint32_t) (&priv->buf_mem_start[(*data_offset)][0]);
		gr1553hwlist[offset].p_addr = (uint32_t) air_syscall_get_physical_addr((uintptr_t)gr1553hwlist[offset].v_addr);
		dptr = gr1553hwlist[offset].p_addr;
		
		/* increment data offset*/
		(*data_offset)++;
	}
	
	/* Store words in the final descriptors */
	desc->settings[0] = word0;
	desc->settings[1] = word1;
	
	/* add dptr */
	desc->dptr = dptr;
	
	/* First bit of status word will be cleared when the device is acessed */
	desc->status = 0x80000000;
	
}

void gr1553bc_init_list()
{
	/* Current device */
	grb_priv *bDev;
	
	/* User defined command list */
	bc_command_t *cl;
	
	uint32_t *mem;
	
	/* command list size */
	int cl_size;
	
	/* Iterator */
	int i;
	
	/* data offset used to split data memory in 64 bytes buffers */
	unsigned int data_offset = 0;
	
	/* Get device's internal structure */
	bDev = bdevs;
	
	/* get user defined command list */
	cl = iop_milstd_get_command_list();
	
	/* get user defined BC command list size */
	cl_size = iop_milstd_get_command_list_size();
	
	/* store user command list information */
	bDev->cl = cl;
	bDev->cl_size = cl_size;
	
	/* obtain device's internal memory */
	mem = iop_get_grb_mem();
	
	/* Align memory to a 16 bytes boundary */
	bDev->mem_start = (void *) MEM_ALIGN(mem, GR1553BC_BD_ALIGN);
	
	/* first memory zone will be used to contain the command list*/
	bDev->sync = bDev->mem_start;
	
	/* next block to be read */
	bDev->last_read = bDev->mem_start;
	
	/* memory after the command list is used to store data buffers */
	bDev->buf_mem_start = (milstd_data_buf *)(((uint32_t *)bDev->sync) + (cl_size*4));

	/* memory after the data buffers used to store the asynchronous command list */
	bDev->async = (struct gr1553bc_bd_tr *)(((uint32_t *)bDev->buf_mem_start) + (iop_milstd_get_data_buffers_size()*16));

	/* memory after the async command list is used to store async data buffers */
	bDev->async_buf_mem_start = (milstd_data_buf *)(((uint32_t *)bDev->async) + (iop_milstd_get_async_command_list_size()*4));
	
	/* iterate over all user defined bc commands */
	for(i = 0; i < cl_size; i++){
	
		/* lets translate the bc command list to something the hw can understand */
		translate_command(bDev, i, &data_offset);
	
	}
}

/**
 * @brief Starts the core to process the synchronous list
 *
 */
void gr1553bc_start_sync()
{
	uint32_t ctrl;
	
	grb_priv *priv = bdevs;

	/* Get list of matching physical/virtual addresses*/
	gr1553hwaddr *gr1553hwlist = iop_get_gr1553hwlist();

	/* BC control register is protected by a key */
	ctrl = GR1553BC_KEY;
	
	/* check if we have a list */
	if(priv->sync != NULL){
		
		/* Activate synchronous list processing */
		ctrl |= GR1553B_BC_ACT_SCSRT;
		
		/* write transfer list pointer register */
		gr1553hwlist[iop_milstd_get_command_list_size()].v_addr = (uint32_t) priv->sync;
		gr1553hwlist[iop_milstd_get_command_list_size()].p_addr = (uint32_t)air_syscall_get_physical_addr((uintptr_t)priv->sync);
		GR1553BC_WRITE_REG(&priv->regs->bc_bd, gr1553hwlist[iop_milstd_get_command_list_size()].p_addr);
	}

	/* If not enabled before, we enable it now. */
	GR1553BC_WRITE_REG(&priv->regs->bc_ctrl, ctrl);

	/* Enable IRQ */
	if(priv->started == 0){

		/* device has started */
		priv->started = 1;

		/* we don't want interrupts!*/
		GR1553BC_WRITE_REG(&priv->regs->imask, 0);
	}

	return;
}

/**
 * @brief Starts the core to process the asynchronous list
 *
 */
void gr1553bc_start_async()
{
	uint32_t ctrl;
	/* Current device */
	grb_priv *priv = bdevs;

	/* Get list of matching physical/virtual addresses*/
	gr1553hwaddr *gr1553hwlist = iop_get_gr1553hwlist();

	/* Get device's internal structure */
	ctrl = GR1553BC_KEY;

	/* Start this list */
	if(priv->async != NULL){

        	/* Activate synchronous list processing */
        	ctrl |= GR1553B_BC_ACT_ASSRT;

		/* write transfer list pointer register */
		gr1553hwlist[iop_milstd_get_command_list_size() + 1].v_addr = (uint32_t) priv->async;
 		gr1553hwlist[iop_milstd_get_command_list_size() + 1].p_addr = (uint32_t)air_syscall_get_physical_addr((uintptr_t)priv->async);
		GR1553BC_WRITE_REG(&priv->regs->bc_abd, gr1553hwlist[iop_milstd_get_command_list_size() + 1].p_addr);
	}

	GR1553BC_WRITE_REG(&priv->regs->bc_ctrl, ctrl);

	/* Disable IRQ */
	if(priv->started == 0){

		/* device has started */
		priv->started = 1;

		/* we don't want interrupts!*/
		GR1553BC_WRITE_REG(&priv->regs->imask, 0);
	}

	return;
}

/**
 * @brief Returns the virtual address of a physical address
 *        which was already converted from a virtual address
 */
unsigned long get_virtual_addr(unsigned long p_addr)
{
	/* Iterator */
	int i;

	/* Get list of matching physical/virtual addresses*/
	gr1553hwaddr *gr1553hwlist = iop_get_gr1553hwlist();

	/* Go through all the addresses that were converted previously */
	for(i = 0; i < (iop_milstd_get_command_list_size() + 2); i++){
		/* Check if the physical address is matching and return the virtual address */
		if(gr1553hwlist[i].p_addr == p_addr){
			return gr1553hwlist[i].v_addr;
		}
	}
	return 0;
}
