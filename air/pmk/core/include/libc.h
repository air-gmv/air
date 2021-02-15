/*
 * Copyright (C) 2013-2020  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file libc.h
 * @author $(AIR_RUNNER_USER)
 * @brief Defines functionality of LIBC needed by AIR
 */

#ifndef __LIBC_H__
#define __LIBC_H__

#ifndef ASM
#ifdef  __cplusplus
extern "C" {
#endif
    
#include <air.h>

/**
 * @brief constant definitions 
 */
#define AIR_INT128SIZE          ((air_u32_t)sizeof (air_i32_t) << 2U)
#define AIR_INT32SIZE           ((air_u32_t)sizeof (air_i32_t))
#define AIR_UINTPTSIZE        ((air_u32_t) sizeof(air_uptr_t))

/**
 * @brief macro definitions 
 */
#define AIR_CHECK_LEN128(LEN)  ((LEN) < AIR_INT128SIZE)
#define AIR_CHECK_LENUINTPT(LEN)  ((LEN) < AIR_UINTPTSIZE)
#define AIR_MEM_ALIGN(X, Y) \
  (((air_u32_t)X & (sizeof (air_u32_t) - 1U)) | ((air_u32_t)Y & (sizeof (air_u32_t) - 1U)))
#define AIR_ALIGN_UINT(X) ((air_uptr_t)X & (AIR_UINTPTSIZE - 1))

void *memcpy(void *destination , const void *source , air_sz_t length);
int strncmp(const char *string1 , const char *string2 , air_sz_t n);
void *memset(void *mem_pointer , int value , air_sz_t length);
  
#endif /* ASM */
#endif /* __LIBC_H__ */
