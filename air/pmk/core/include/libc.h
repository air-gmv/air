/*
 * Copyright (C) 2013-2020  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * https://spass-git-ext.gmv.com/AIR/AIR/raw/master/air/LICENSE
 */
/**
 * @file libc.h
 * @author dtms
 * @brief Defines functionality of LIBC needed by AIR
 */

#ifndef __LIBC_H__
#define __LIBC_H__

#include <stddef.h>
#include <string.h>
#include <stdint.h>

/**
 * @brief constant definitions 
 */
#define AIR_INT128SIZE          ((uint32_t)sizeof (int32_t) << 2U)
#define AIR_INT32SIZE           ((uint32_t)sizeof (int32_t))
#define AIR_UINTPTSIZE        ((uint32_t) sizeof(uintptr_t))

/**
 * @brief macro definitions 
 */
#define AIR_CHECK_LEN128(LEN)  ((LEN) < AIR_INT128SIZE)
#define AIR_CHECK_LENUINTPT(LEN)  ((LEN) < AIR_UINTPTSIZE)
#define AIR_MEM_ALIGN(X, Y) \
  (((uint32_t)X & (sizeof (uint32_t) - 1U)) | ((uint32_t)Y & (sizeof (uint32_t) - 1U)))
#define AIR_ALIGN_UINT(X) ((uintptr_t)X & (AIR_UINTPTSIZE - 1))

void *memcpy(void *destination , const void *source , size_t length);
int strncmp(const char *string1 , const char *string2 , size_t n);
void *memset(void *mem_pointer , int value , size_t length);
  
#endif /* __LIBC_H__ */
