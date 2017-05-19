/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Global BSP variables and functions
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_POWERPC_BSP_H
#define LIBBSP_POWERPC_BSP_H

#include <stdint.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#include <bspopts.h>

#include <bsp/tictac.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_SMALL_MEMORY 1

#ifndef ASM

/** @brief System clock frequency */
extern unsigned int bsp_clock_speed;

/** @brief Time base clicks per micro second */
extern uint32_t bsp_clicks_per_usec;

/** @brief Convert Decrementer ticks to microseconds */
#define BSP_Convert_decrementer( _value ) \
  (((unsigned long long) (_value)) / ((unsigned long long)bsp_clicks_per_usec))

rtems_status_code mpc55xx_sd_card_init( bool mount);

/* Network driver configuration */

struct rtems_bsdnet_ifconfig;

int smsc9218i_attach_detach(
  struct rtems_bsdnet_ifconfig *config,
  int attaching
);

#define RTEMS_BSP_NETWORK_DRIVER_ATTACH smsc9218i_attach_detach

#define RTEMS_BSP_NETWORK_DRIVER_NAME "eth0"

#endif /* ASM */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_POWERPC_BSP_H */
