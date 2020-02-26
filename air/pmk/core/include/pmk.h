/*
 * Copyright (C) 2013-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief Defines the PMK control structures
 */

#ifndef __PMK_H__
#define __PMK_H__

#include <air.h>
#include <cpu.h>

#ifndef ASM


/**
 * @brief List of elements
 */
typedef struct {

    air_sz_t length;                    /**< length of the list             */
    void *elements;                     /**< elements in list               */

} pmk_list_t;

/**
 * @brief Get an element from list
 * @param type type of element
 * @param list list of elements
 * @param index index of the element to get
 * @param pointer to the element
 */
#define pmk_get_from_list(type, list, index)    \
        &((type *)(list)->elements)[index]


void *memset(void *, int, air_sz_t);
void *memcpy(void *, const void *, air_sz_t);
int strncmp(const char *, const char *, air_sz_t);



#else /* ASM */

/** @defgroup os_status_asm AIR-OS Status Code (ASM definitions)
 *  Status code definitions for assembly
 *  @note Must match the status code values defined in air.h
 *  @{
 */
#define AIR_NO_ERROR                                             0x00
#define AIR_SUCCESSFUL                                           0x00
#define AIR_NO_ACTION                                            0x01
#define AIR_NOT_AVAILABLE                                        0x02
#define AIR_INVALID_PARAM                                        0x03
#define AIR_INVALID_CONFIG                                       0x04
#define AIR_INVALID_MODE                                         0x05
#define AIR_TIMED_OUT                                            0x06
#define AIR_INVALID_POINTER                                      0x07
#define AIR_ERROR_MSGQUEUE_FULL                                  0x08
#define AIR_ERROR_MSGQUEUE_EMPTY                                 0x09
#define AIR_ERROR_MAX_PORT_NUM                                   0x0A
#define AIR_INVALID_PORT_TYPE                                    0x0B
#define AIR_INVALID_SIZE                                         0x0C
#define AIR_OUT_OF_MEMORY                                        0x0D
#define AIR_DEVICE_NOT_FOUND                                     0x0E
#define AIR_DEVICE_ERROR                                         0x0F
#endif /* ASM */
#endif /* __PMK_H__ */
