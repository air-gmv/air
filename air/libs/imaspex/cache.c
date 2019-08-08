/*
 * Copyright (C) 2008-2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/AIR/raw/master/air/LICENSE
 */
/** 
 * @file cache.c
 * @author cdcs
 * @brief  IMASPEX's cache services implementation
 */

#include <imaspex.h>

extern int imaspex_tsal_init;

void FLUSH_CACHE(CACHE_TYPE CACHE, RETURN_CODE_TYPE *RETURN_CODE){
	
    /* check TSAL init */
    if (imaspex_tsal_init == 0){
        *RETURN_CODE = INVALID_MODE;
        return;
    }

	/* syscall to flush cache */
	*RETURN_CODE = air_syscall_flush_cache(CACHE);
}

void ACTIVATE_CACHE(CACHE_TYPE CACHE, RETURN_CODE_TYPE *RETURN_CODE){
	
    /* check TSAL init */
    if (imaspex_tsal_init == 0){
        *RETURN_CODE = INVALID_MODE;
        return;
    }
	
	/* syscall to flush cache */
	*RETURN_CODE = air_syscall_enable_cache(CACHE);

}

void DEACTIVATE_CACHE(CACHE_TYPE CACHE, RETURN_CODE_TYPE *RETURN_CODE){
	
    /* check TSAL init */
    if (imaspex_tsal_init == 0){
        *RETURN_CODE = INVALID_MODE;
        return;
    }
	
	/* syscall to flush cache */
	*RETURN_CODE = air_syscall_disable_cache(CACHE);

}

void FREEZE_CACHE(CACHE_TYPE CACHE, RETURN_CODE_TYPE *RETURN_CODE){
	
    /* check TSAL init */
    if (imaspex_tsal_init == 0){
        *RETURN_CODE = INVALID_MODE;
        return;
    }
	
	/* syscall to flush cache */
	*RETURN_CODE = air_syscall_freeze_cache(CACHE);
}
