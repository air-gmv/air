/** 
 * 	@file IOPdriverconfig_interface.h
 * 
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT 
 *
 * 	@author Cl�udio Silva
 * 
 *  The license and distribution terms for this file may be 
 *  found in the file LICENSE at: 
 *  http://www.rtems.com/license/LICENSE.
 * 
 * 	@brief Contains driver configuration options
 * 
 * 	Contains configuration options needed by the several drivers.
 *	Declares the "getter" functions for those configurations.
 *	To be auto generated by a configuration tool
 */ 

#ifndef _USERCONFIG_H
#define _USERCONFIG_H

#include <IOPlibio.h>

#ifdef	__SPW_H__
#include <IOPgrspw.h>

/**
 * @brief gets SpW internal driver structure 
 * @return pointer to SpW internal per core driver structure
 */
SPW_DEV *get_spw_devs();

/**
 * @brief gets number of SpW v1 cores
 * @return number of SpW v1 cores
 */
int get_number_spw_cores();

/**
 * @brief gets number of SpW v2 cores
 * @return number of SpW v2 cores
 */
int get_number_spw2_cores();

/**
 * @brief gets SpW user configuration
 * @return pointer to SpW configuration structure#spw_user_config
 */
spw_user_config * get_spw_config();

/**
 * @brief gets SpW default configuration
 * @return pointer to SpW configuration structure #spw_user_config
 */
spw_user_config * get_spw_defconfig();

/**
 * @brief gets poiter to SpW memory are
 * @return pointer to the beggining of the SpW allocated memory area
 */
void * get_spw_mem();

/**
 * @brief gets pointer to SpW memory area
 * @return pointer to the end of the SpW allocated memory area
 */
void * get_spw_memend();

/**
 * @brief gets the size of SpW allocated memory
 * @return SpW alocated memory size
 */
int get_spw_memsize();

#endif

#ifdef __MIL1553BRM_H__
#include <IOPmil1553brm.h>

/**
 * @brief gets BRM internal driver structure 
 * @return pointer to BRM internal per core driver structure
 */
brm_priv *get_milstd_brm_priv();

/**
 * @brief gets number of milstd cores
 * @return number of milstd cores
 */
int get_number_milstd_cores();

/**
 * @brief gets pointer to BRM memory are
 * @return pointer to the beggining of the BRM allocated memory area
 */
void * get_milstd_mem();

/**
 * @brief gets pointer to BRM memory area
 * @return pointer to the end of the BRM allocated memory area
 */
void * get_milstd_memend();

/**
 * @brief gets BRM user configuration
 * @return pointer to BRM configuration structure #milstd_config_t
 */
milstd_config_t * get_milstd_config();

#endif

#ifdef __GR1553B_H__
#include <IOPgr1553b.h>

void *iop_get_grb_rt_mem();

grb_priv *iop_grb_get_priv_mem(void);

#endif

#endif
