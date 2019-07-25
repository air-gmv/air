/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file ipc.h
 * @author pfnf
 * @brief Framework for inter-processor communication
 */

#ifndef __IPC_H__
#define __IPC_H__

#ifndef ASM
#include <cpu.h>

/**
 * @brief IPC Message code
 */
typedef enum {

    PMK_IPC_NO_ACTION                                   = 0x00,
    PMK_IPC_BOOT_PARTITION_CORE                         = 0x01,
    PMK_IPC_TRASH_PARTITION_CORE                        = 0x02,
    PMK_IPC_MODULE_SHUTDOWN                             = 0x03,
    PMK_IPC_MODULE_RESTART                              = 0x04,
    PMK_IPC_SET_TBR                                     = 0x05,
    PMK_IPC_PARTITION_RESTART                           = 0x06


} pmk_ipc_message;

#else

#define PMK_IPC_NO_ACTION                                 0x00
#define PMK_IPC_BOOT_PARTITION_CORE                       0x01
#define PMK_IPC_TRASH_PARTITION_CORE                      0x02
#define PMK_IPC_MODULE_SHUTDOWN                           0x03
#define PMK_IPC_MODULE_RESTART                            0x04
#define PMK_IPC_SET_TBR                                   0x05
#define PMK_IPC_PARTITION_RESTART                         0x06


#endif  /* ASM */
#endif  /* __IPC_H__ */

