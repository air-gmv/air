/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2015
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file 
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE. 
 * ==========================================================================*/
/**
 * @file
 * @author pfnf
 * @brief Defines the PMK control structures
 */

#ifndef __PMK_H__
#define __PMK_H__

#include <xky.h>
#include <cpu.h>

#ifndef ASM


/**
 * @brief List of elements
 */
typedef struct {

    xky_sz_t length;                      /**< length of the list             */
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


void *memset(void *, int, xky_sz_t);
void *memcpy(void *, const void *, xky_sz_t);
int strncmp(const char *, const char *, xky_sz_t);



#else /* ASM */

/** @defgroup os_status_asm XKY-OS Status Code (ASM definitions)
 *  Status code definitions for assembly
 *  @note The must match the status code values defined in xky.h
 *  @{
 */
#define XKY_NO_ERROR                                             0x00
#define XKY_SUCCESSFUL                                           0x00
#define XKY_NO_ACTION                                            0x01
#define XKY_NOT_AVAILABLE                                        0x02
#define XKY_INVALID_PARAM                                        0x03
#define XKY_INVALID_CONFIG                                       0x04
#define XKY_INVALID_MODE                                         0x05
#define XKY_TIMED_OUT                                            0x06
#define XKY_INVALID_POINTER                                      0x07
#define XKY_ERROR_MSGQUEUE_FULL                                  0x08
#define XKY_ERROR_MSGQUEUE_EMPTY                                 0x09
#define XKY_ERROR_MAX_PORT_NUM                                   0x0A
#define XKY_INVALID_PORT_TYPE                                    0x0B
#define XKY_UNSUCCESSFUL                                         0x0C
#define XKY_INTERNAL_ERROR                                       0x0D
#define XKY_INVALID_SIZE                                         0x0E
#define XKY_INVALID_ADDRESS                                      0x0F
#define XKY_OUT_OF_MEMORY                                        0x10
#define XKY_DEVICE_NOT_FOUND                                     0x11
#define XKY_DEVICE_ERROR                                         0x12
#endif /* ASM */
#endif /* __PMK_H__ */
