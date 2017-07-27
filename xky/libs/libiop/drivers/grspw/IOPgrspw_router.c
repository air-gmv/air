/* GRSPW ROUTER APB-Register Driver.
 *
 * COPYRIGHT (c) 2010.
 * Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <xky.h>
#include <rtems.h>

#include <amba.h>
#include <ambapp.h>

#include <IOPgrspw_router.h>
#include <spw_support.h>

#define ROUTER_DBG(args...)

#define REG_READ(adr) (*(volatile unsigned int *)(adr))
#define REG_WRITE(adr, value) (*(volatile unsigned int *)(adr) = (value))

router_priv_t router_priv;

static rtems_device_driver router_initialize(iop_device_driver_t *iop_dev, void *arg)
{
	int device_found = 0;
	
	amba_apb_device apbgreth;
	
	iop_spw_router_device_t *device = (iop_spw_router_device_t *)iop_dev;
	struct router_priv *priv = (struct router_priv *)(device->dev.driver);
	
	char prefix[32];
	rtems_status_code status;

	struct router_config *cfg = (struct router_config *) &device->flags;
	
	memset(&apbgreth, 0, sizeof(amba_apb_device));
	
	/* Scan for MAC AHB slave interface */
	device_found = amba_find_apbslv(&amba_conf, VENDOR_GAISLER, GAISLER_GRSPW_ROUTER, &apbgreth);
									
	if (device_found != 1){
	    iop_debug("    GRETH device not found...\n");
		return RTEMS_INTERNAL_ERROR;
	}
	
	priv->regs = (struct router_regs *)apbgreth.start;

	/* Register character device in registered region */
	router_hwinfo(priv, &priv->hwinfo);
	priv->open = 0;

	if ( router_we_set(priv, 1) ) {
		return RTEMS_INTERNAL_ERROR;
	}
	
	if ( router_config_set(priv, cfg) ) {
		return RTEMS_INTERNAL_ERROR;
	}
	
	if ( router_ps_set(priv, priv->ps) ) {
		return RTEMS_INTERNAL_ERROR;
	}
	
	if ( router_routes_set(priv, priv->routes) ) {
		return RTEMS_INTERNAL_ERROR;
	}
	
	priv->nports = priv->hwinfo.nports_spw + priv->hwinfo.nports_amba +
			priv->hwinfo.nports_fifo;
	if ( (priv->nports < 2) || (priv->nports > 32) )
		return RTEMS_INTERNAL_ERROR;

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver router_open(iop_device_driver_t *iop_dev, void *arg)
{	
	iop_spw_router_device_t *device = (iop_spw_router_device_t *)iop_dev;
	struct router_priv *priv = (struct router_priv *)(device->dev.driver);

	if ( !priv || priv->open ) {
		return RTEMS_RESOURCE_IN_USE;
	}
	
	priv->open = 1;

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver router_close(iop_device_driver_t *iop_dev, void *arg)
{
	iop_spw_router_device_t *device = (iop_spw_router_device_t *)iop_dev;
	struct router_priv *priv = (struct router_priv *)(device->dev.driver);

	priv->open = 0;

	return RTEMS_SUCCESSFUL;
}

static void router_hwinfo(
	struct router_priv *priv,
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
	struct router_priv *priv,
	struct router_config *cfg)
{
	int i;

	if ( (cfg->flags & (ROUTER_FLG_TPRES|ROUTER_FLG_TRLD)) &&
	     !priv->hwinfo.timers_avail ) {
		return RTEMS_NOT_IMPLEMENTED;
	}

	/* Write only configuration bits in Config register */
	if ( cfg->flags & ROUTER_FLG_CFG ) {
		REG_WRITE(&priv->regs->cfgsts, cfg->config & ~0x4);
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
			REG_WRITE(&priv->regs->treload[i], priv->timer_reload->timeout[i]);
	}

	return 0;
}

static int router_config_read(
	struct router_priv *priv,
	struct router_config *cfg)
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

static int router_routes_set(
	struct router_priv *priv,
	struct router_routes *routes)
{
	int i;
	for (i=0; i<224; i++)
		REG_WRITE(&priv->regs->routes[i], routes->route[i]);
	return 0;
}

static int router_routes_read(
	struct router_priv *priv,
	struct router_routes *routes)
{
	int i;
	for (i=0; i<224; i++)
		routes->route[i] = REG_READ(&priv->regs->routes[i]);
	return 0;
}

static int router_ps_set(struct router_priv *priv, struct router_ps *ps)
{
	int i;
	unsigned int *p = &ps->ps[0];
	for (i=0; i<255; i++,p++) 
		REG_WRITE(&priv->regs->psetup[i], *p);
	return 0;
}

static int router_ps_read(struct router_priv *priv, struct router_ps *ps)
{
	int i;
	unsigned int *p = &ps->ps[0];
	for (i=0; i<255; i++,p++) 
		REG_WRITE(&priv->regs->psetup[i], *p);
	return 0;
}

/* Configuration writer enable */
static int router_we_set(struct router_priv *priv, int we)
{
	REG_WRITE(&priv->regs->cfgwe, we & 0x1);
	return 0;
}

static int router_port_ctrl(struct router_priv *priv, struct router_port *port)
{
	unsigned int ctrl, sts;

	if ( port->port > priv->nports )
		return RTEMS_INVALID_NAME;

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

static int router_cfgsts_set(struct router_priv *priv, unsigned int cfgsts)
{
	REG_WRITE(&priv->regs->cfgsts, cfgsts);
	return 0;
}

static int router_cfgsts_read(struct router_priv *priv, unsigned int *cfgsts)
{
	*cfgsts = REG_READ(&priv->regs->cfgsts);
	return 0;
}

static int router_tc_read(struct router_priv *priv, unsigned int *tc)
{
	*tc = REG_READ(&priv->regs->timecode);
	return 0;
}

//static rtems_device_driver router_control(
//	rtems_device_major_number major,
//	rtems_device_minor_number minor,
//	void                    * arg
//	)
//{
//	struct router_priv *priv;
//	struct drvmgr_dev *dev;
//	rtems_libio_ioctl_args_t *ioarg = (rtems_libio_ioctl_args_t *) arg;
//	void *argp = (void *)ioarg->buffer;
//
//	if ( drvmgr_get_dev(&router_drv_info.general, minor, &dev) ) {
//		ROUTER_DBG("Wrong minor %d\n", minor);
//		return RTEMS_INVALID_NAME;
//	}
//	priv = (struct router_priv *)dev->priv;
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
//		struct router_config *cfg = argp;
//		return router_config_set(priv, cfg);
//	}
//
//	/* Read Router Configuration */
//	case GRSPWR_IOCTL_CFG_GET:
//	{
//		struct router_config *cfg = argp;
//		router_config_read(priv, cfg);
//		break;
//	}
//
//	/* Routes */
//	case GRSPWR_IOCTL_ROUTES_SET:
//	{
//		struct router_routes *routes = argp;
//		return router_routes_set(priv, routes);
//	}
//
//	case GRSPWR_IOCTL_ROUTES_GET:
//	{
//		struct router_routes *routes = argp;
//		router_routes_read(priv, routes);
//		break;
//	}
//
//	/* Port Setup */
//	case GRSPWR_IOCTL_PS_SET:
//	{
//		struct router_ps *ps = argp;
//		return router_ps_set(priv, ps);
//	}
//
//	case GRSPWR_IOCTL_PS_GET:
//	{
//		struct router_ps *ps = argp;
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
//		struct router_port *port = argp;
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
//	default: return RTEMS_NOT_IMPLEMENTED;
//	}
//
//	return 0;
//}
