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

#include <rtems.h>
#include <amba.h>
#include <ambapp.h>
#include <ambaext.h>
#include <stdint.h>



#include <iop_error.h>
#include <IOPgr1553b.h>
#include <IOPgr1553bc.h>
#include <IOPgr1553b_config.h>
#include <IOPmilstd_config.h>

#include <pprintf.h>

/** returns the first address found after p that is aligned with c  */
#define MEM_ALIGN(p,c) ((((unsigned int)(p))+(c)) & ~((c)-1))

static grb_priv *bdevs;

/** @todo this should not be here !!!!*/
static write_cmd_shortcut_t shortcut_mem[128];
static int shortcut_offset = 0;


/* Reset software and BC hardware into a known "unused/init" state */
void gr1553bc_device_init(grb_priv *priv)
{	
	/* obtain device private structures (used by other functions )*/
	bdevs = iop_grb_get_priv_mem();

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
void gr1553bc_pause_list(unsigned int minor)
{
	uint32_t ctrl;
	grb_priv *priv;
	
	/* current minor device internal data structure */
	priv = &bdevs[minor];
	
	/* Suspend list processing */
	ctrl = GR1553BC_KEY | GR1553B_BC_ACT_SCSUS;
	
	/* write action register */
	GR1553BC_WRITE_REG(&priv->regs->bc_ctrl, ctrl);

}

/* Restart GR1553 BC transfers, after being paused */
void gr1553bc_continue_list(unsigned int minor)
{
	uint32_t ctrl;
	grb_priv *priv;
	
	/* current minor device internal data structure */
	priv = &bdevs[minor];
	
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
			
			if((shortcut->subaddr == hdr->desc) && 
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
		   ((retrymode & 0x1) << 23) | ((nretry & 0x7) << 20));
	
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

static inline rtems_status_code bc_verify_command_status(uint32_t sw)
{	
	rtems_status_code status = RTEMS_SUCCESSFUL;
	
	/* verify transfer status */
	if((sw & 0x3) != 0){
		
		status = RTEMS_INTERNAL_ERROR;
		
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
		if(bc_verify_command_status(desc->status) == RTEMS_SUCCESSFUL){
			
			/* we shall wait for new data from the user. dummy this command */
			dummy_command(&desc->settings[1]);
		
		}
		
		/* this descriptor is updated. mark it as not accessed */
		clear_acessed_bit(&(desc->status));
		
	} else{ /*< there is data to be read */
	
		/*  Verify if data was received correctly  */
		if(bc_verify_command_status(desc->status) == RTEMS_SUCCESSFUL){
			
			/* detect mode code */
			if((subaddress1 == 0) || (subaddress1 == 31)){
				
				/* mode codes 16/18/19 transmit one data word */
				if((wcmc == 16) || (wcmc == 18) || (wcmc == 19)){
					
					/* copy received data to user buffer */
					memcpy(rw_args->data, (void *)desc->dptr, 2);
					
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
				memcpy(rw_args->data, (void *)desc->dptr, wcmc*2);
				
				/* header length */
				rw_args->hdr_len = 1;
				
				/* data length */
				rw_args->data_len = wcmc*2;
				
				/* data length */
				rw_args->bytes_moved = wcmc*2;
				
				/* copy milstd header */
				hdr = (milstd_header_t *)rw_args->hdr;
				
				/* RT address 1 */
				hdr->desc = subaddress1;
				
				/* Sub address 1 */
				hdr->address = ((desc->settings[1] >> 11) & 0x1f);
				
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
 * @param [in] minor minor device number whose list is to be processed
 * @param [in,out] rw_args Input/Output buffer and respective sizes
 *
 * @return status of the operation:
 *  	- RTEMS_SUCESSFULL: Data was sucessfully read.
 * 		- RTEMS_TIMEOUT: End of list.
 *
 * This function will obtain the list that is currently being processed by
 * the bc core and will process the already completed commands. Each command is
 * checked for errors and available data. Depending on the command type, some 
 * commands will be updated, others will have their data extracted. The function 
 * returns when there is data available to be read or when the command list as 
 * reached its end. The function starts processing the list in a previously read 
 * position. It then follows the branching structure of the list. 
 */
rtems_status_code grbc_process_completed_commands(unsigned int minor, libio_rw_args_t *rw_args)
{
	/* return code */
	rtems_status_code status;
	
	/* Current device */
	grb_priv *bDev;
	
	/* indicates if data was sucessfully read or if the list has ended */
	int end = 0;
	
	/* current bc descriptor */
	struct gr1553bc_bd_tr *current;
	
	/* current minor device internal data structure */
	bDev = &bdevs[minor];
	
	status = RTEMS_SUCCESSFUL;
	
	/* @todo place a limit in the number of reads... */
	while(end == 0){
	
		/* next descriptor/command to be read */
		current = bDev->last_read;
		
		/* detect if this is a branch or a data transfer command */
		if(((current->settings[0] & GR1553BC_BD_TYPE) >> 31) == 0){
		
			/* verify if this descriptor was acessed by the hw */
			if(((current->status & GR1553BC_STATUS_ACESSED) >> 31) == 1){
			
				/* this descriptor was not acessed. try again later */
				status = RTEMS_TIMEOUT; 
				
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
			bDev->last_read = (struct gr1553bc_bd_tr *)current->settings[1];
			
		}
		
	}
	
	return status;
}

/** 
 * @brief Appends incomming data to a milstd command belonging to a bc command list.
 *
 * @param [in] minor bc minor device number whose data belongs to.
 * @param [in] data input data buffer
 * @param [in] hdr Mil-std header comprising a RT address and a subaddress
 * @param [in] size data buffer length in bytes
 *
 * @return rtems_status_code; status of the operation:
 *  	- RTEMS_SUCESSFULL: Data was sucessfully written to the list
 * 		- RTEMS_NOT_DEFINED: this data is not linked to any command in the list
 *
 * This function inserts incoming user data in the bc command list. For that
 * purpose it uses a shortcut mapping that matches every command in the list
 * with a milstd header.  The header in the incoming data is used as a key
 * to that mapping, which yields the data transfer command affecting that header. 
 * This header is composed by one RT address and one subaddress.
 *
 */
rtems_status_code grbc_merge_data_with_command(unsigned int minor, uint8_t *data,
										  milstd_header_t *hdr, unsigned int size)
{
	
	/* return code */
	rtems_status_code status;
	
	/* Current device */
	grb_priv *bDev = &bdevs[minor];
	
	/* Current descriptor */
	struct gr1553bc_bd_tr *desc;
	
	iop_chain_control *aux = &bDev->shortcut[(hdr->address & 0x1f)];
	
	/* find if this data request can be matched to any descriptor */
	desc = (struct gr1553bc_bd_tr *)find_shortcut(aux, hdr, size);
	
	/* verify if we obtained a valid descriptor */
	if(desc == NULL){
		
		/* no descriptor was obatined */
		status = RTEMS_NOT_DEFINED;
	
	} else{
	
		/* lets copy the data */
		memcpy((void *)desc->dptr, (void *)data, size);
		
		/* and enable the descriptor. Clear dummy bit */
		desc->settings[1] &= ~(1<<31);
		
		/* set not acessed bit in status word*/
		desc->status |= (1<<31);
		
		/* data was appended sucessfully */
		status = RTEMS_SUCCESSFUL;
	}
	
	return status;
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
	
	/* user command list*/
	user_list = &priv->cl[offset];
	
	/* Map the memory to a transfer descriptor */
	desc = &priv->sync[offset];
	
	/* Verify if this is non data transfer command */
	if((user_list->ccw) & TB_BIT){
		
		/* Unconditional Branch */
		word0 = GR1553BC_UNCOND_JMP;
		
		/* Calculate branch address */
		word1 = (uint32_t) (&priv->sync[0] + (user_list->branch_offset));
		
		/* No data pointer */
		dptr = (uint32_t) NULL;
	
	} else if((user_list->ccw) & END_BIT){ /*< End the list */
		
		/* End of list marker */
		word0 = 0x800000FF;
		
		/* Clear jump address */
		word1 = 0;
		
		/* No data pointer */
		dptr = (uint32_t) NULL;
		
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
			 * current command. This shorctut mapping is used when new outgoing
			 * data arrives  
			 */
			create_write_cmd_shortcut(&priv->shortcut[(user_list->rtaddr[0] & 0x1f)],
									                user_list, (void *) desc);
		}
		
		/* Calculate data buffer position */
		dptr = (uint32_t) &priv->buf_mem_start[(*data_offset)][0];
		
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

void gr1553bc_init_list(unsigned int minor)
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
	bDev = &bdevs[minor];
	
	/* get user defined command list */
	cl = iop_milstd_get_command_list();
	
	/* get user defined BC command list size */
	cl_size = iop_milstd_get_command_list_size();
	
	/* store user command list information */
	bDev->cl = cl;
	bDev->cl_size = cl_size;
	
	/* obtain device's internal memory */
	mem = iop_get_grb_bc_mem();
	
	/* Align memory to a 16 bytes boundary */
	bDev->mem_start = (void *) MEM_ALIGN(mem, GR1553BC_BD_ALIGN);
	
	/* first memory zone will be used to contain the command list*/
	bDev->sync = bDev->mem_start;
	
	/* next block to be read */
	bDev->last_read = bDev->mem_start;
	
	/* memory after the command list is used to store data buffers */
	bDev->buf_mem_start = (milstd_data_buf *)(((uint32_t *)bDev->sync) + (cl_size*4));
	
	/* iterate over all user defined bc commands */
	for(i = 0; i < cl_size; i++){
	
		/* lets translate the bc command list to something the hw can understand */
		translate_command(bDev, i, &data_offset);
	
	}
}

/**
 * @brief Starts the core to process the synchronous list
 * @param priv Device's internal structure.
 *
 */
void gr1553bc_start_sync(grb_priv *priv)
{
	uint32_t ctrl;
	
	/* BC control register is protected by a key */
	ctrl = GR1553BC_KEY;
	
	/* check if we have a list */
	if(priv->sync != NULL){
		
		/* Activate synchronous list processing */
		ctrl |= GR1553B_BC_ACT_SCSRT;
		
		/* write transfer list pointer register */
		GR1553BC_WRITE_REG(&priv->regs->bc_bd, (uint32_t)priv->sync);
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
 * @brief Starts the core to process the synchronous list
 * @param minor minor device number of the BC device to be started.
 *
 */
void gr1553bc_start_list_processing(unsigned int minor)
{
	/* Current device */
	grb_priv *bDev;
	
	/* Get device's internal structure */
	bDev = &bdevs[minor];
	
	/* Start this list */
	gr1553bc_start_sync(bDev);

}



