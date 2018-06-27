/**
 *  @file 
 *  @brief Contains the Remote Terminal driver for Gaisler's MIL-STD1553B interface
 *
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT
 *
 *  @author Cl�udio Silva
 *
 *  @note adapted from Gaisler's GR1553B RT driver
 *	
 */
/*  GR1553B RT driver
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
 */

#include <rtems.h>
#include <stdint.h>
#include <amba.h>
#include <ambapp.h>
#include <pprintf.h>

#include <IOPgr1553b.h>
#include <IOPgr1553rt.h>

#include <ambaext.h>

#include <iop_error.h>
#include <IOPlibio.h>
#include <iop.h>

#include <IOPmilstd_config.h>
#include <IOPdriverconfig_interface.h>



#define GR1553RT_WRITE_MEM(adr, val) *(volatile uint32_t *)(adr) = (val)
#define GR1553RT_READ_MEM(adr) (*(volatile uint32_t *)(adr))

#define GR1553RT_WRITE_REG(adr, val) *(volatile uint32_t *)(adr) = (val)
#define GR1553RT_READ_REG(adr) (*(volatile uint32_t *)(adr))

static grb_priv *bdevs;

static void gr1553rt_init_table(grb_priv *priv){
	
	/* subaddress table */
	struct gr1553rt_sa *table = priv->sa_table;
	
	/*block descriptor */
	struct gr1553rt_bd *bd = priv->bds_start;
	
	/* Pointer to current data buffer */
	milstd_data_buf *dbuf;
	
	/* Number of data buffers per subaddress */
	unsigned int buf_per_sub = priv->user_config->databufs_per_sub;
	
	/* generic iterators */
	int i, j, z = 0;
	
	
	/* calculate how many subaddresses are enabled */
	for(i=0; i<32; i++){
		
		/* verify if the current desc is enabled */
		if(((priv->user_config->enabled_subs >> i) & 0x1)){
			z++;
		}
	}
	
	/* 
	 * We have one descriptor control block per databuf. 
	 * The number of bds needed will be the number of enabled descriptors times
	 * the number of databufs per subaddress times 2(RX and TX bufs).
	 * Data buffer memory starts at the end of the BD table.
	 */
	priv->buf_mem_start = (milstd_data_buf *) &priv->bds_start[buf_per_sub*2*z];
	
	dbuf = priv->buf_mem_start;

	/* Fill the subaddress table */
	for(i=0; i<32; i++){
		
		/* verify if the user enabled this subaddress */
		if(((priv->user_config->enabled_subs >> i) & 0x1)){
			
			/* Enabled RX,TX and Broadcast. Maximum transfer size 32*uint16_t*/
			table[i].ctrl = 0x38080;
			
			/* Insert data buffers descriptors in this subaddress entry */
			for(j = 0; j < buf_per_sub; j++){
				
				/* we create the RX and TX chains at the same time */
				bd[j].ctrl = 0x80000000;
				bd[j+buf_per_sub].ctrl = 0x0;
				
				/* create a cirular buffer of bds */
				if((buf_per_sub - 1) == j){
					
					/* last bd links to the first one */
					bd[j].next = (uint32_t)&bd[0];
					bd[j+buf_per_sub].next = (uint32_t)&bd[buf_per_sub];
					
				} else{
				
					/* link to next bd in circular buffer */
					bd[j].next = (uint32_t)&bd[j+1];
					bd[j+buf_per_sub].next = (uint32_t)&bd[j+buf_per_sub+1];
				}
				
				/* Insert data pointer */
				bd[j].dptr = (uint32_t)&dbuf[j];
				bd[j+buf_per_sub].dptr = (uint32_t)&dbuf[j+buf_per_sub];
		
			}
			
			/* insert circular bd chain pointer in txptr*/
			table[i].txptr = (uint32_t)&bd[0];
			priv->last_tx[i] = &bd[0];
			
			/* insert circular bd chain pointer in rxptr*/
			table[i].rxptr = (uint32_t)&bd[buf_per_sub];
			priv->last_rx[i] = &bd[buf_per_sub];
			
			/* update pointer for new subaddres entry */
			bd = &bd[buf_per_sub*2];
			dbuf = &dbuf[buf_per_sub*2];
			
		} else{
		
			/* Disable this subaddress */
			table[i].ctrl = 0;
			
			/* insert invalid pointer in tx bd ptr*/
			table[i].txptr = 0x3;
			priv->last_tx[i] = NULL;
			
			/* insert invalid pointer in rx bd ptr*/
			table[i].rxptr = 0x3;
			priv->last_rx[i] = NULL;

		}
		
	}
	
}

void gr1553rt_device_init(grb_priv *priv){
	/* obtain device private structures (used by other functions )*/
	bdevs = iop_grb_get_priv_mem();

	/* obtain cores memory */
	priv->mem_start = (uint32_t *)iop_get_grb_mem();
	
	/* align memory to 128kb boundary */
	priv->sa_table = (struct gr1553rt_sa *)(((uint32_t)priv->mem_start + 0x1ff) & (~0x1ff));
	
	/* Subaddress table occupies 128kb. The remaining memory is used for descriptors */
	priv->bds_start = (struct gr1553rt_bd *)(priv->sa_table + 128);
	
	/* initialize the subaddress table, descriptors and */
	gr1553rt_init_table(priv);
	
	/*** Initialize Registers ***/

	/* Subaddress table base */
	priv->regs->rt_tab = (unsigned int) priv->sa_table;

	/* Mode code configuration */
	priv->regs->rt_mcctrl = priv->user_config->modecode;

	/* RT Time Tag resolution */
	priv->regs->rt_ttag = (priv->user_config->time_res << 16);

	/* Event log base and size. we don't use it*/
	priv->regs->rt_evsz  = 0;
	priv->regs->rt_evlog = 0;
	priv->regs->rt_evirq = 0;

	/* Disable all interrups*/
	priv->regs->irq = 0xFFFFFFFF;
	priv->regs->irq = 0;
	priv->regs->imask = 0;

	/* Enable and set RT address */
	priv->regs->rt_cfg = GR1553RT_KEY |
			(priv->user_config->rtaddress << GR1553B_RT_CFG_RTADDR_BIT) |
			GR1553B_RT_CFG_RTEN;

	/* Tell software RT is started */
	priv->started = 1;

}

rtems_status_code gr1553rt_read(rtems_device_minor_number minor, void *arg){
	
	unsigned int sw;
	unsigned int wc;
	int read = 0;
	int i, z;
	
	/* This core's internal structure */
	grb_priv *priv = &bdevs[minor];
	
	/* user arguments*/
	libio_rw_args_t *rw_args = (libio_rw_args_t *) arg;
	
	/* block descriptor */
	struct gr1553rt_bd *bd;
	
	/* function return code */
	rtems_status_code status = RTEMS_SUCCESSFUL;
	
	/* Number of data buffers per subaddress */
	unsigned int buf_per_sub = priv->user_config->databufs_per_sub;
	
	
	/* Verify if the user correctly provided data and header*/
	if((rw_args->data == NULL) || (rw_args->hdr == NULL)){
		return RTEMS_INVALID_ADDRESS;
	}
	
	/* check for HW based errors */
	
		
	/* iterate over the whole subaddress table until we find anything */
	for(i=0; ((i<32) && (read == 0)); i++){
		
		/* obtain buffer descriptor */
		bd = (struct gr1553rt_bd *)priv->last_rx[i];
		
		z = 0;
		
		/* verify if the data valid bit is set and iterate trough the BDs */
		while((((bd->ctrl & 0x80000000) >> 31) == 1) 
				&& (read == 0) && (z < buf_per_sub)){
			
			/* obtain word count */
			wc = ((bd->ctrl >> 3) & 0x1F);
			
			/* wordcount 0 means 32 words */
			if(wc == 0){
				wc = 32;
			}
			
			/* obtain transfer result */
			sw = (bd->ctrl & 0x7);
			
			/* if there is valid data on this descriptor */
			if((wc > 0) /*&& (sw == 0)*/){
			
				/* Insert data size */
				rw_args->bytes_moved = rw_args->data_len = wc*2;
				
				/* copy data to user's buffer */
				memcpy(rw_args->data, (void *)bd->dptr, wc*2);
			
				/* copy subaddress */
				rw_args->hdr[0] = i;
			
				/* header length is always 1*/
				rw_args->hdr_len = 1;
				
				/* re-enable the block descriptor (unset data valid bit)*/
				bd->ctrl = 0;
			
				/* update last_rx pointer to next bd in circular buffer */
				priv->last_rx[i] = (struct gr1553rt_bd *)bd->next;
			
				/* mark as read */
				read = 1;
				
				status = RTEMS_SUCCESSFUL;
			}
			
			/* if there was an error we reenable the descriptor */
			if(sw != 0){
				
				/* TODO: report error to someone */
				
				/* re-enable the block descriptor (unset data valid bit)*/
				//bd->ctrl = 0;
				
				/* update last_rx pointer to next bd in circular buffer */
				//priv->last_rx[i] = (struct gr1553rt_bd *)bd->next;
			
			}
			
			bd = (struct gr1553rt_bd *)bd->next;
			
			z++;
			
		}
	
	}
	
	/* we were unable to read */
	if(read == 0){
		
		/* EWOULDBLOCK */
		status = RTEMS_RESOURCE_IN_USE;
	}
	
	return status;
	
}


rtems_status_code gr1553rt_write(rtems_device_minor_number minor, void *arg){
	
	 /* This core's internal structure */
	grb_priv *priv = &bdevs[minor];
	
	/* user arguments*/
	libio_rw_args_t *rw_args = (libio_rw_args_t *) arg;
	
	/* subaddress table */
	struct gr1553rt_sa *table = priv->sa_table;
	
	/* block descriptor */
	struct gr1553rt_bd *bd;
	
	unsigned int suba;
	unsigned int wc;
	
	
	
	/* Verify if user request does not exceed the maximum data size for milstd*/
	if(rw_args->data_len > 64){
		return RTEMS_INVALID_SIZE;
	}
	
	/* Verify if the user correctly provided data and header*/
	if((rw_args->data == NULL) || (rw_args->hdr == NULL)){
		return RTEMS_INVALID_ADDRESS;
	}
	
	/* Get target subaddress */
	suba = *((unsigned char *)rw_args->hdr) & 0x7F;
	
	/* verify if we have a valid subaddress */
	if((suba < 0) || (suba > 32)){
		return RTEMS_INVALID_ADDRESS;
	}
	
	/* obtain table entry for this subaddress */
	table = &priv->sa_table[suba];
	
	/* TODO TODO TODO TODO TODO last tx or table->next??? */
	/* obtain current bd */
	bd = (struct gr1553rt_bd *) table->txptr;
	
	/* get word count */
	wc = rw_args->data_len/2;
	
	/* word count zero means 32 words */
	if(wc == 32){
		wc = 0;
	}
	
	/* verify if a previous transfer was completed */
	if((bd->ctrl & 0x80000000) == 0){
		
		/* TODO report error to someone */
	}
	
	/* check for errors in a previous transfer */
	if((bd->ctrl & 0x7) != 0){
		/* TODO report error to someone */
		
	}
	
	/* copy data buffer */
	memcpy((void *)bd->dptr, (void *)rw_args->data, rw_args->data_len);
	
	/* insert word count in descriptor control word */
	bd->ctrl = ((wc & 0x1F) << 3);
	
	/* inform user of how much data was written*/
    rw_args->bytes_moved = wc+2;
	
	return RTEMS_SUCCESSFUL;
}
