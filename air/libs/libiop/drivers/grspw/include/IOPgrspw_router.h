/*
 * GRSPW ROUTER APB-Register Driver.
 *
 * COPYRIGHT (c) 2010  Cobham Gaisler AB
 * Copyright (C) 2011-2017  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef __GRSPW_ROUTER_H__
#define __GRSPW_ROUTER_H__

#include <iop.h>

/* SpaceWire registry fields */
struct router_regs {
	unsigned int resv1;		/* 0x000 */
	unsigned int psetup[255];	/* 0x004 */
	unsigned int resv2[32];		/* 0x400 */
	unsigned int routes[224];	/* 0x480 */
	unsigned int pctrl[32];		/* 0x800 */
	unsigned int psts[32];		/* 0x880 */
	unsigned int treload[32];	/* 0x900 */
	unsigned int resv3[32];		/* 0x980 */
	unsigned int cfgsts;		/* 0xA00 */
	unsigned int timecode;		/* 0xA04 */
	unsigned int ver;		/* 0xA08 */
	unsigned int idiv;		/* 0xA0C */
	unsigned int cfgwe;		/* 0xA10 */
	unsigned int tprescaler;	/* 0xA14 */
	unsigned int resv4[123];	/* 0xA18 */
	unsigned int charo[31];		/* 0xC04 */
	unsigned int resv5;		/* 0xC80 */
	unsigned int chari[31];		/* 0xC84 */
	unsigned int resv6;		/* 0xD00 */
	unsigned int pkto[31];		/* 0xD04 */
	unsigned int resv7;		/* 0xD80 */
	unsigned int pkti[31];		/* 0xD84 */
};

/* Hardware Information */
struct router_hw_info {
	unsigned char nports_spw;
	unsigned char nports_amba;
	unsigned char nports_fifo;
	char timers_avail;
	char pnp_avail;
	unsigned char ver_major;
	unsigned char ver_minor;
	unsigned char ver_patch;
	unsigned char iid;
};

#define ROUTER_FLG_CFG		0x01
#define ROUTER_FLG_IID		0x02
#define ROUTER_FLG_IDIV		0x04
#define ROUTER_FLG_TPRES	0x08
#define ROUTER_FLG_TRLD		0x10
#define ROUTER_FLG_ALL		0x1f	/* All Above Flags */

typedef struct {
	unsigned int flags; /* Determine what configuration should be updated */

	/* Router Configuration Register */
	unsigned int config;

	/* Set Instance ID */
	unsigned char iid;

	/* SpaceWire Link Initialization Clock Divisor */
	unsigned char idiv;

	/* Timer Prescaler and Reload */
	unsigned int timer_prescaler;
	unsigned int timer_reload[32];
} router_config_t;

/* Logical routing table */
typedef struct {
	unsigned int route[224];
} router_routes;

/* Port Setup, see register definitions for "Port setup register" */
typedef struct {
	unsigned int ps[31]; /* Port Setup for ports 1-31 */
	unsigned int ps_logical[224]; /* Port setup for locgical addresses 32-255 */
} router_ps;

/* Physical ports timeout */
typedef struct{
	unsigned int timeout[32];
} port_timer;

/* Set/Get Port Control/Status */
#define ROUTER_PORTFLG_SET_CTRL	0x01
#define ROUTER_PORTFLG_GET_CTRL	0x02
#define ROUTER_PORTFLG_SET_STS	0x04
#define ROUTER_PORTFLG_GET_STS	0x08
typedef struct{
	unsigned int flag;
	int port;
	unsigned int ctrl;
	unsigned int sts;
} router_port;

typedef struct {
	char devName[32];
	struct router_regs *regs;
	int minor;
	int open;
	struct router_hw_info hwinfo;
	int nports;
	router_routes *routes;
	router_ps *ps;
	port_timer *timer_reload;
	router_config_t config;
} router_priv_t;

/* Get Hardware support/information available */
#define GRSPWR_IOCTL_HWINFO	0x01	/* OUT:    struct router_hw_info */

/* Router Configuration */
#define GRSPWR_IOCTL_CFG_SET	0x02	/* IN:     router_config_t */
#define GRSPWR_IOCTL_CFG_GET	0x03	/* OUT:    router_config_t */

/* Routes */
#define GRSPWR_IOCTL_ROUTES_SET	0x04	/* IN:     router_routes */
#define GRSPWR_IOCTL_ROUTES_GET	0x05	/* OUT:    router_routes */

/* Port Setup */
#define GRSPWR_IOCTL_PS_SET	0x06	/* IN:     router_ps */
#define GRSPWR_IOCTL_PS_GET	0x07	/* OUT:    router_ps */

/* Set configuration write enable */
#define GRSPWR_IOCTL_WE_SET	0x08	/* INT:    int */

/* Set/Get Port Control/Status */
#define GRSPWR_IOCTL_PORT	0x09	/* IN/OUT: struct router_port */

/* Set Router Configuration/Status Register */
#define GRSPWR_IOCTL_CFGSTS_SET	0x0a	/* IN:     unsigned int */
/* Get Router Configuration/Status Register */
#define GRSPWR_IOCTL_CFGSTS_GET	0x0b	/* OUT:    unsigned int */

/* Get Current Time-Code Register */
#define GRSPWR_IOCTL_TC_GET	0x0c	/* OUT:    unsigned int */

uint32_t spw_router_initialize(iop_device_driver_t *iop_dev, void *arg);

uint32_t spw_router_open(iop_device_driver_t *iop_dev, void *arg);

uint32_t spw_router_close(iop_device_driver_t *iop_dev, void *arg);

uint32_t spw_router_write(iop_device_driver_t *iop_dev, void *arg);

uint32_t spw_router_read(iop_device_driver_t *iop_dev, void *arg);

#endif
