/*
 * GRSPW ROUTER APB-Register Driver.
 * 
 * COPYRIGHT (c) 2010.
 * Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef __GRSPW_ROUTER_H__
#define __GRSPW_ROUTER_H__

#include <iop.h>
#include <ambapp.h>

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

struct router_config {
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
};

/* Logical routing table */
struct router_routes {
	unsigned int route[224];
};

/* Port Setup, see register definitions for "Port setup register" */
struct router_ps {
	unsigned int ps[31]; /* Port Setup for ports 1-31 */
	unsigned int ps_logical[224]; /* Port setup for locgical addresses 32-255 */
};

/* Physical ports timeout */
struct port_timer {
	unsigned int timeout[32];
};

/* Set/Get Port Control/Status */
#define ROUTER_PORTFLG_SET_CTRL	0x01
#define ROUTER_PORTFLG_GET_CTRL	0x02
#define ROUTER_PORTFLG_SET_STS	0x04
#define ROUTER_PORTFLG_GET_STS	0x08
struct router_port {
	unsigned int flag;
	int port;
	unsigned int ctrl;
	unsigned int sts;
};

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

struct router_priv {
	char devName[32];
	struct router_regs *regs;
	int minor;
	int open;
	struct router_hw_info hwinfo;
	int nports;
	struct router_routes *routes;
	struct router_ps *ps;
	struct port_timer *timer_reload;
};

/* Get Hardware support/information available */
#define GRSPWR_IOCTL_HWINFO	0x01	/* OUT:    struct router_hw_info */

/* Router Configuration */
#define GRSPWR_IOCTL_CFG_SET	0x02	/* IN:     struct router_config */
#define GRSPWR_IOCTL_CFG_GET	0x03	/* OUT:    struct router_config */

/* Routes */
#define GRSPWR_IOCTL_ROUTES_SET	0x04	/* IN:     struct router_routes */
#define GRSPWR_IOCTL_ROUTES_GET	0x05	/* OUT:    struct router_routes */

/* Port Setup */
#define GRSPWR_IOCTL_PS_SET	0x06	/* IN:     struct router_ps */
#define GRSPWR_IOCTL_PS_GET	0x07	/* OUT:    struct router_ps */

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

static rtems_device_driver router_initialize(iop_device_driver_t *iop_dev, void *arg);

static rtems_device_driver router_open(iop_device_driver_t *iop_dev, void *arg);

static rtems_device_driver router_close(iop_device_driver_t *iop_dev, void *arg);

static rtems_device_driver router_control(iop_device_driver_t *iop_dev, void *arg);

static void router_hwinfo(struct router_priv *priv,	struct router_hw_info *hwinfo);

static int router_config_set(struct router_priv *priv, struct router_config *cfg);

static int router_config_read(struct router_priv *priv, struct router_config *cfg);

static int router_routes_set(struct router_priv *priv, struct router_routes *routes);

static int router_routes_read(struct router_priv *priv, struct router_routes *routes);

static int router_ps_set(struct router_priv *priv, struct router_ps *ps);

static int router_ps_read(struct router_priv *priv, struct router_ps *ps);

static int router_we_set(struct router_priv *priv, int we);

static int router_port_ctrl(struct router_priv *priv, struct router_port *port);

static int router_cfgsts_set(struct router_priv *priv, unsigned int cfgsts);

static int router_cfgsts_read(struct router_priv *priv, unsigned int *cfgsts);

static int router_tc_read(struct router_priv *priv, unsigned int *tc);

#endif
