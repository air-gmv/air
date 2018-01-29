/**
 *  @file 
 *  @brief This file contains the user configuration interface for the gr1553b device
 *
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT
 *
 *  @author Cl�udio Silva
 *	
 */

#ifndef __GR1553B_CONFIG_H__
#define __GR1553B_CONFIG_H__
 
#include <stdint.h>
#include <IOPgr1553b.h>
#include <IOPmilstd_config.h>

#define GR1553BC_MODE_BC 0
#define GR1553BC_MODE_RT 1
#define GR1553BC_MODE_BM 2

grb_user_config_t *iop_grb_get_user_config(unsigned int minor);

grb_priv *iop_grb_get_priv_mem(void);

int iop_get_number_grb_cores(void);

void *iop_get_grb_bc_mem(void);

void *iop_get_grb_rt_mem(void);

#endif
