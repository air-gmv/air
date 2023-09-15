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

#include <air.h>
#include <bsp.h>
#include <IOPgrspw_router.h>
#include <spw_support.h>

#ifdef DEBUG_SPWRTR
#define ROUTER_DBG(fmt, args...)    do { { iop_debug(" : %03d @ %18s()]:" fmt , __LINE__,__FUNCTION__,## args); }} while(0)
#define ROUTER_DBG2(fmt)            do { { iop_debug(" : %03d @ %18s()]:" fmt , __LINE__,__FUNCTION__); }} while(0)
#else
#define ROUTER_DBG(fmt, args...)
#define ROUTER_DBG2(fmt)
#endif

#define REG_READ(adr) (*(volatile unsigned int *)(adr))
#define REG_WRITE(adr, value) (*(volatile unsigned int *)(adr) = (value))

static void router_hwinfo(router_priv_t *priv,	struct router_hw_info *hwinfo);
static int router_config_set(router_priv_t *priv, router_config_t *cfg);
//static int router_config_read(router_priv_t *priv, router_config_t *cfg);
static int router_routes_set(router_priv_t *priv, router_routes *routes);
//static int router_routes_read(router_priv_t *priv, router_routes *routes);
static int router_ps_set(router_priv_t *priv, router_ps *ps);
//static int router_ps_read(router_priv_t *priv, router_ps *ps);
static int router_we_set(router_priv_t *priv, int we);
static int router_port_ctrl(router_priv_t *priv, router_port *port);
//static int router_cfgsts_set(router_priv_t *priv, unsigned int cfgsts);
//static int router_cfgsts_read(router_priv_t *priv, unsigned int *cfgsts);
//static int router_tc_read(router_priv_t *priv, unsigned int *tc);

void router_print_hwinfo(struct router_hw_info *hwinfo)
{
	iop_debug(" Hardware Configuration of SpaceWire Router:\n");
	iop_debug("  Number of SpW ports:           %d\n", hwinfo->nports_spw);
	iop_debug("  Number of AMBA ports:          %d\n", hwinfo->nports_amba);
	iop_debug("  Number of FIFO ports:          %d\n", hwinfo->nports_fifo);
	iop_debug("  Timers available:              %s\n", hwinfo->timers_avail ? "YES" : "NO");
	iop_debug("  Plug and Play available:       %s\n", hwinfo->pnp_avail ? "YES" : "NO");
	iop_debug("  MAJOR Version:                 %d\n", hwinfo->ver_major);
	iop_debug("  MINOR Version:                 %d\n", hwinfo->ver_minor);
	iop_debug("  PATCH Version:                 %d\n", hwinfo->ver_patch);
	iop_debug("  Current Instance ID:           %d\n", hwinfo->iid);
}

uint32_t spw_router_initialize(iop_device_driver_t *iop_dev, void *arg)
{
	amba_confarea_t * ptrarea = (amba_confarea_t *)air_syscall_get_ambaconf();

	clock_gating_enable(ptrarea, GATE_SPWR);
	int device_found = 0;
	
	amba_ahb_dev_t ahbspwrtr;
	
	iop_spw_router_device_t *device = (iop_spw_router_device_t *)iop_dev;
	router_priv_t *priv = (router_priv_t *)(device->dev.driver);
	
	router_config_t *cfg = &priv->config;
	
	cfg->flags = device->flags;
	cfg->config = device->config;
	cfg->iid = device->iid;
	cfg->idiv = device->idiv;
	cfg->timer_prescaler = device->prescaler;
	
	int j;
	for (j=0; j<32; j++)
		cfg->timer_reload[j] = priv->timer_reload->timeout[j];	
	
	memset(&ahbspwrtr, 0, sizeof(amba_ahb_dev_t));
	
	/* Scan for MAC AHB slave interface */
	device_found = amba_get_ahb_slave(ptrarea, VENDOR_GAISLER, GAISLER_SPWROUTER, 0, &ahbspwrtr);
									
	if (device_found != 1){
	    ROUTER_DBG2("SPWRTR device not found...\n");
		return AIR_DEVICE_NOT_FOUND;
	}
	
	priv->regs = (struct router_regs *)0xFF880000;
	
	ROUTER_DBG("spacewire router at [0x%x]\n", (unsigned int) priv->regs);

	/* Register character device in registered region */
	router_hwinfo(priv, &priv->hwinfo);
	router_print_hwinfo(&priv->hwinfo);
	priv->open = 0;

	priv->nports = priv->hwinfo.nports_spw + priv->hwinfo.nports_amba +
			priv->hwinfo.nports_fifo;
	if ( (priv->nports < 2) || (priv->nports > 32) ) {
		ROUTER_DBG("AIR_INVALID_SIZE at priv->nports = %d\n", priv->nports);
		return AIR_INVALID_SIZE;
	}
	
	if ( router_we_set(priv, 0) ) {
		ROUTER_DBG2("AIR_DEVICE_ERROR at router_we_set\n");
		return AIR_DEVICE_ERROR;
	}
	
	if ( router_config_set(priv, cfg) ) {
		ROUTER_DBG2("AIR_NOT_AVAILABLE at router_config_set\n");
		return AIR_NOT_AVAILABLE;
	}
	iop_debug("   Router config/status reg: 0x%x\n", REG_READ(&priv->regs->cfgsts));
	
	router_port port;
	port.flag = ROUTER_PORTFLG_GET_CTRL | ROUTER_PORTFLG_GET_STS;
	for (j=0; j < priv->nports + 1; j++) {
		port.port = j;
		router_port_ctrl(priv, &port);
		iop_debug(" port[%2d]: CTRL: 0x%08x  STATUS: 0x%08x\n", j, port.ctrl, port.sts);
	}
	
	if ( router_ps_set(priv, priv->ps) ) {
		ROUTER_DBG2("AIR_DEVICE_ERROR at router_ps_set\n");
		return AIR_DEVICE_ERROR;
	}
	
	if ( router_routes_set(priv, priv->routes) ) {
		ROUTER_DBG2("AIR_DEVICE_ERROR at router_routes_set\n");
		return AIR_DEVICE_ERROR;
	}

	return AIR_SUCCESSFUL;
}

uint32_t spw_router_open(iop_device_driver_t *iop_dev, void *arg)
{	
	iop_spw_router_device_t *device = (iop_spw_router_device_t *)iop_dev;
	router_priv_t *priv = (router_priv_t *)(device->dev.driver);
	
	if ( !priv || priv->open ) {
		ROUTER_DBG2("Router failed to initialize\n");
		return AIR_NOT_AVAILABLE;
	}
	
	priv->open = 1;

	return AIR_SUCCESSFUL;
}

uint32_t spw_router_close(iop_device_driver_t *iop_dev, void *arg)
{
	iop_spw_router_device_t *device = (iop_spw_router_device_t *)iop_dev;
	router_priv_t *priv = (router_priv_t *)(device->dev.driver);

	priv->open = 0;

	return AIR_SUCCESSFUL;
}

static void router_hwinfo(
	router_priv_t *priv,
	struct router_hw_info *hwinfo)
{
	unsigned int tmp;
	
	tmp = REG_READ(&priv->regs->cfgsts);
	hwinfo->nports_spw   = (tmp >> 27) & 0x1f;
	hwinfo->nports_amba  = (tmp >> 22) & 0x1f;
	hwinfo->nports_fifo  = (tmp >> 17) & 0x1f;
	hwinfo->timers_avail = (tmp >>  1) & 0x1;
	hwinfo->pnp_avail    = (tmp >>  0) & 0x1;

	tmp = REG_READ(&priv->regs->ver);
	hwinfo->ver_major   = (tmp >> 24) & 0xff;
	hwinfo->ver_minor   = (tmp >> 16) & 0xff;
	hwinfo->ver_patch   = (tmp >>  8) & 0xff;
	hwinfo->iid         = (tmp >>  0) & 0xff;
}

static int router_config_set(
	router_priv_t *priv,
	router_config_t *cfg)
{
	int i;

	if ( (cfg->flags & (ROUTER_FLG_TPRES|ROUTER_FLG_TRLD)) &&
	     !priv->hwinfo.timers_avail ) {
		return AIR_NOT_AVAILABLE;
	}

	/* Write only configuration bits in Config register */
	if ( cfg->flags & ROUTER_FLG_CFG ) {
		REG_WRITE(&priv->regs->cfgsts, (cfg->config << 2));
	}

	/* Write Instance ID to Version Register */
	if ( cfg->flags & ROUTER_FLG_IID ) {
		REG_WRITE(&priv->regs->ver, cfg->iid);
	}

	/* Write startup-clock-divisor Register */
	if ( cfg->flags & ROUTER_FLG_IDIV ) {
		REG_WRITE(&priv->regs->idiv, cfg->idiv);
	}

	/* Write Timer Prescaler Register */
	if ( cfg->flags & ROUTER_FLG_TPRES ) {
		REG_WRITE(&priv->regs->tprescaler, cfg->timer_prescaler);
	}

	/* Write Timer Reload Register */
	if ( cfg->flags & ROUTER_FLG_TRLD ) {
		for (i=0; i<=priv->nports; i++)
			REG_WRITE(&priv->regs->treload[i], cfg->timer_reload[i]);
	}

	return 0;
}
/*
static int router_config_read(
	router_priv_t *priv,
	router_config_t *cfg)
{
	int i;

	cfg->config = REG_READ(&priv->regs->cfgsts) & ~0xffff0007;
	cfg->iid = REG_READ(&priv->regs->ver) & 0xff;
	cfg->idiv = REG_READ(&priv->regs->idiv) & 0xff;
	cfg->timer_prescaler = REG_READ(&priv->regs->tprescaler);
	for (i=0; i<=priv->nports; i++)
		priv->timer_reload->timeout[i] = REG_READ(&priv->regs->treload[i]);

	return 0;
}
*/
static int router_routes_set(
	router_priv_t *priv,
	router_routes *routes)
{
	int i;
	for (i=0; i<224; i++)
		REG_WRITE(&priv->regs->routes[i], routes->route[i]);
	return 0;
}
/*
static int router_routes_read(
	router_priv_t *priv,
	router_routes *routes)
{
	int i;
	for (i=0; i<224; i++)
		routes->route[i] = REG_READ(&priv->regs->routes[i]);
	return 0;
}
*/
static int router_ps_set(router_priv_t *priv, router_ps *ps)
{
	int i;
	unsigned int *p = &ps->ps[0];
	for (i=0; i<255; i++,p++) 
		REG_WRITE(&priv->regs->psetup[i], *(p));
	return 0;
}
/*
static int router_ps_read(router_priv_t *priv, router_ps *ps)
{
	int i;
	unsigned int *p = &ps->ps[0];
	for (i=0; i<255; i++,p++) 
		REG_WRITE(&priv->regs->psetup[i], *(p));
	return 0;
}
*/
/* Configuration writer enable */
static int router_we_set(router_priv_t *priv, int we)
{
	REG_WRITE(&priv->regs->cfgwe, we & 0x1);
	return 0;
}

static int router_port_ctrl(router_priv_t *priv, router_port *port)
{
	unsigned int ctrl, sts;

	if ( port->port > priv->nports )
		return AIR_INVALID_PARAM;

	ctrl = port->ctrl;
	if ( port->flag & ROUTER_PORTFLG_GET_CTRL ) {
		ctrl = REG_READ(&priv->regs->pctrl[port->port]);
	}
	sts = port->sts;
	if ( port->flag & ROUTER_PORTFLG_GET_STS ) {
		sts = REG_READ(&priv->regs->psts[port->port]);
	}

	if ( port->flag & ROUTER_PORTFLG_SET_CTRL ) {
		REG_WRITE(&priv->regs->pctrl[port->port], port->ctrl);
	}
	if ( port->flag & ROUTER_PORTFLG_SET_STS ) {
		REG_WRITE(&priv->regs->psts[port->port], port->sts);
	}

	port->ctrl = ctrl;
	port->sts = sts;
	return 0;
}
/*
static int router_cfgsts_set(router_priv_t *priv, unsigned int cfgsts)
{
	REG_WRITE(&priv->regs->cfgsts, cfgsts);
	return 0;
}

static int router_cfgsts_read(router_priv_t *priv, unsigned int *cfgsts)
{
	*cfgsts = REG_READ(&priv->regs->cfgsts);
	return 0;
}

static int router_tc_read(router_priv_t *priv, unsigned int *tc)
{
	*tc = REG_READ(&priv->regs->timecode);
	return 0;
}
*/
//air_status_code_e router_control(
//	rtems_device_major_number major,
//	rtems_device_minor_number minor,
//	void                    * arg
//	)
//{
//	router_priv_t *priv;
//	struct drvmgr_dev *dev;
//	rtems_libio_ioctl_args_t *ioarg = (rtems_libio_ioctl_args_t *) arg;
//	void *argp = (void *)ioarg->buffer;
//
//	if ( drvmgr_get_dev(&router_drv_info.general, minor, &dev) ) {
//		ROUTER_DBG("Wrong minor %d\n", minor);
//		return AIR_INVALID_PARAM;
//	}
//	priv = (router_priv_t *)dev->priv;
//
//	ioarg->ioctl_return = 0;
//	switch (ioarg->command) {
//
//	/* Get Hardware support/information available */
//	case GRSPWR_IOCTL_HWINFO:
//	{
//		struct router_hw_info *hwinfo = argp;
//		router_hwinfo(priv, hwinfo);
//		break;
//	}
//
//	/* Set Router Configuration */
//	case GRSPWR_IOCTL_CFG_SET:
//	{
//		router_config_t *cfg = argp;
//		return router_config_set(priv, cfg);
//	}
//
//	/* Read Router Configuration */
//	case GRSPWR_IOCTL_CFG_GET:
//	{
//		router_config_t *cfg = argp;
//		router_config_read(priv, cfg);
//		break;
//	}
//
//	/* Routes */
//	case GRSPWR_IOCTL_ROUTES_SET:
//	{
//		router_routes *routes = argp;
//		return router_routes_set(priv, routes);
//	}
//
//	case GRSPWR_IOCTL_ROUTES_GET:
//	{
//		router_routes *routes = argp;
//		router_routes_read(priv, routes);
//		break;
//	}
//
//	/* Port Setup */
//	case GRSPWR_IOCTL_PS_SET:
//	{
//		router_ps *ps = argp;
//		return router_ps_set(priv, ps);
//	}
//
//	case GRSPWR_IOCTL_PS_GET:
//	{
//		router_ps *ps = argp;
//		router_ps_read(priv, ps);
//		break;
//	}
//
//	/* Set configuration write enable */
//	case GRSPWR_IOCTL_WE_SET:
//	{
//		return router_we_set(priv, (int)argp);
//	}
//
//	/* Set/Get Port Control/Status */
//	case GRSPWR_IOCTL_PORT:
//	{
//		router_port *port = argp;
//		int result;
//		if ( (result=router_port_ctrl(priv, port)) )
//			return result;
//		break;
//	}
//
//	/* Set Router Configuration/Status Register */
//	case GRSPWR_IOCTL_CFGSTS_SET:
//	{
//		return router_cfgsts_set(priv, (int)argp);
//	}
//
//	/* Get Router Configuration/Status Register */
//	case GRSPWR_IOCTL_CFGSTS_GET:
//	{
//		unsigned int *cfgsts = argp;
//		router_cfgsts_read(priv, cfgsts);
//		break;
//	}
//
//	/* Get Current Time-Code Register */
//	case GRSPWR_IOCTL_TC_GET:
//	{
//		unsigned int *tc = argp;
//		router_tc_read(priv, tc);
//		break;
//	}
//
//	default: return AIR_NOT_AVAILABLE;
//	}
//
//	return 0;
//}

uint32_t spw_router_write(iop_device_driver_t *iop_dev, void *arg){return AIR_SUCCESSFUL;}
uint32_t spw_router_read(iop_device_driver_t *iop_dev, void *arg){return AIR_SUCCESSFUL;}
