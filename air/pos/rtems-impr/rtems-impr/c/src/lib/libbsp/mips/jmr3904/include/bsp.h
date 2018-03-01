/**
 *  @file
 *  
 *  This include file contains some definitions specific to the
 *  JMR3904 simulator in gdb.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_MIPS_JMR3904_BSP_H
#define LIBBSP_MIPS_JMR3904_BSP_H

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <libcpu/tx3904.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_FEATURE_IRQ_EXTENSION
#define BSP_SHARED_HANDLER_SUPPORT      1

#ifdef __cplusplus
}
#endif

#endif
