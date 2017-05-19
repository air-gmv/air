/**
 *  @file 
 *  @brief This file contains the user configuration for the gr1553b driver
 *
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT
 *
 *  @author Cl�udio Silva
 *	
 */

#include <stdint.h>
#include <IOPgr1553b_config.h>
#include <IOPmilstd_config.h>

#define GR1553B_DEVICES 1
#define COMMAND_LIST_SIZE 4
#define DATA_BUFFERS 10

#define BC_MEMORY_SIZE ((COMMAND_LIST_SIZE*4) + (DATA_BUFFERS*16))

/* SA_TABLE_SIZE*2 + WORDS_PER_DESC(20)*DESC_PER_SUBADRESSES(12)*ENABLED_SUBADDRESSES(32)*2(RX and TX) */
#define RT_MEMORY_SIZE (256+(16+3+1)*32*12*2)

/* this memory will be 16 bytes aligned (+4) !*/
static uint32_t gr1553bcmem[BC_MEMORY_SIZE+4];

static uint32_t gr1553rtmem[RT_MEMORY_SIZE];

static grb_priv bdevs[GR1553B_DEVICES];

static grb_user_config_t userconf[GR1553B_DEVICES] = 
{
	{
		.operating_mode = GR1553BC_MODE_RT,
		.msg_timeout = 0,
		.retry_mode = 1,
		.rtaddress = 10,
		.modecode = 0x55555555,  	/* legalize ALL mode codes */
		.enabled_subs = 0xFFFFFFFF, /* legalize ALL subaddresses*/
		.databufs_per_sub = 12,
		.time_res = 10
	}
};

static bc_command_t command_list[COMMAND_LIST_SIZE] = 
{
	{	
		.ccw = CCW_RT_TO_BC_A | 0x5,
		.rtaddr[0] = 10,
		.rtaddr[1] = 0,
		.subaddr[0] = 20,
		.subaddr[1] = 0,
		.branch_offset = 0,
		.time_slot = 500000
	},
	{	
		.ccw = CCW_BC_TO_RT_A | 0x5,
		.rtaddr[0] = 10,
		.rtaddr[1] = 0,
		.subaddr[0] = 15,
		.subaddr[1] = 0,
		.branch_offset = 0,
		.time_slot = 500000
	},
	{
		.ccw = CCW_RT_TO_BC_A | 0x5,
		.rtaddr[0] = 10,
		.rtaddr[1] = 0,
		.subaddr[0] = 20,
		.subaddr[1] = 0,
		.branch_offset = 0,
		.time_slot = 500000
	},
	{
		.ccw = TB_BIT,
		.rtaddr[0] = 0,
		.rtaddr[1] = 0,
		.subaddr[0] = 0,
		.subaddr[1] = 0,
		.branch_offset = 0,
		.time_slot = 1000
	}
};

bc_command_t *iop_milstd_get_command_list(){
	return &command_list[0];
}

int iop_milstd_get_command_list_size(){
	return COMMAND_LIST_SIZE;
}

grb_user_config_t *iop_grb_get_user_config(unsigned int minor){
	
	/* Return value */
	grb_user_config_t *rc;
	
	/* Verify if the requested minor is available */
	if(minor > GR1553B_DEVICES){
	
		/* There is no such device */
		rc = NULL;
		
	} else{
	
		/* return user config */
		rc = &userconf[minor];
	}
	
	return rc;
}

grb_priv *iop_grb_get_priv_mem(void){
	return &bdevs[0];
}

int iop_get_number_grb_cores(void){
	return GR1553B_DEVICES;
}

void *iop_get_grb_bc_mem(void){

	return (void *)&gr1553bcmem[0];
}

void *iop_get_grb_rt_mem(void){

	return (void *)&gr1553rtmem[0];
}
